#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QActionGroup>
#include "oglwindow.h"
#include "settingsdialog.h"
#include "plugindialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include "core_interface.h"
#include "plugin.h"
#include "workerthread.h"

OGLWindow *my_window;
QWidget *container;
WorkerThread *workerThread;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSettings settings("mupen64plus", "gui");
    QActionGroup *my_slots_group = new QActionGroup(this);
    QAction *my_slots[10];
    OpenRecent = new QMenu;
    QMenu * SaveSlot = new QMenu;
    OpenRecent->setTitle("Open Recent");
    SaveSlot->setTitle("Change Save Slot");
    ui->menuFile->insertMenu(ui->actionSave_State, OpenRecent);
    ui->menuFile->insertSeparator(ui->actionSave_State);
    ui->menuFile->insertMenu(ui->actionSave_State_To, SaveSlot);
    ui->menuFile->insertSeparator(ui->actionSave_State_To);
    for (int i = 0; i < 10; ++i) {
        my_slots[i] = new QAction(this);
        my_slots[i]->setCheckable(true);
        my_slots[i]->setText("Slot " + QString::number(i));
        my_slots[i]->setActionGroup(my_slots_group);
        SaveSlot->addAction(my_slots[i]);
        connect(my_slots[i], &QAction::triggered,[=](bool checked){
            if (checked) {
                int slot = my_slots[i]->text().remove("Slot ").toInt();
                if (QtAttachCoreLib())
                    (*CoreDoCommand)(M64CMD_STATE_SET_SLOT, slot, NULL);
            }
        });
    }
    my_slots[0]->setChecked(true);

    updateOpenRecent();

    if (!settings.contains("coreLibPath")) {
        QStringList files;
        findRecursion("/usr/lib", OSAL_DEFAULT_DYNLIB_FILENAME, &files);
        findRecursion("/usr/local/lib", OSAL_DEFAULT_DYNLIB_FILENAME, &files);
        if (files.size() > 0)
            settings.setValue("coreLibPath", files.at(0));
        else
            settings.setValue("coreLibPath", QString(".") + QDir::separator() + OSAL_DEFAULT_DYNLIB_FILENAME);
    }
    if (!settings.contains("pluginDirPath")) {
        QStringList files2;
        findRecursion("/usr/lib", QString("mupen64plus-rsp-hle") + OSAL_DLL_EXTENSION, &files2);
        findRecursion("/usr/local/lib", QString("mupen64plus-rsp-hle") + OSAL_DLL_EXTENSION, &files2);
        if (files2.size() > 0) {
            QFileInfo pluginPath(files2.at(0));
            settings.setValue("pluginDirPath", pluginPath.absolutePath());
        } else
            settings.setValue("pluginDirPath", QString(".") + QDir::separator());
    }
    QString path;
    if (!settings.contains("videoPlugin")) {
        path = "mupen64plus-video-GLideN64";
        settings.setValue("videoPlugin", path + OSAL_DLL_EXTENSION);
    }
    if (!settings.contains("audioPlugin")) {
        path = "mupen64plus-audio-sdl";
        settings.setValue("audioPlugin", path + OSAL_DLL_EXTENSION);
    }
    if (!settings.contains("rspPlugin")) {
        path = "mupen64plus-rsp-hle";
        settings.setValue("rspPlugin", path + OSAL_DLL_EXTENSION);
    }
    if (!settings.contains("inputPlugin")) {
        path = "mupen64plus-input-sdl";
        settings.setValue("inputPlugin", path + OSAL_DLL_EXTENSION);
    }
    if (!settings.value("coreLibPath").isNull())
        qtCoreDirPath = settings.value("coreLibPath").toString();
    if (!settings.value("pluginDirPath").isNull())
        qtPluginDir = settings.value("pluginDirPath").toString();
    if (!settings.value("videoPlugin").isNull())
        qtGfxPlugin = settings.value("videoPlugin").toString();
    if (!settings.value("audioPlugin").isNull())
        qtAudioPlugin = settings.value("audioPlugin").toString();
    if (!settings.value("rspPlugin").isNull())
        qtRspPlugin = settings.value("rspPlugin").toString();
    if (!settings.value("inputPlugin").isNull())
        qtInputPlugin = settings.value("inputPlugin").toString();
}

MainWindow::~MainWindow()
{
    if (coreStarted)
        (*CoreShutdown)();
    DetachCoreLib();
    delete ui;
}

void MainWindow::findRecursion(const QString &path, const QString &pattern, QStringList *result)
{
    QDir currentDir(path);
    const QString prefix = path + QLatin1Char('/');
    foreach (const QString &match, currentDir.entryList(QStringList(pattern), QDir::Files))
        result->append(prefix + match);
    foreach (const QString &dir, currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
        findRecursion(prefix + dir, pattern, result);
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    if (QtAttachCoreLib()) {
        (*CoreDoCommand)(M64CMD_STOP, 0, NULL);
        int response;
        do {
            (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &response);
        } while (response != M64EMU_STOPPED);
    }
    event->accept();
}

void MainWindow::updateOpenRecent()
{
    QSettings settings("mupen64plus", "gui");
    OpenRecent->clear();
    QAction *recent[5];
    QStringList list = settings.value("RecentROMs").toString().split(";");
    for (int i = 0; i < list.size() && i < 5; ++i) {
        recent[i] = new QAction(this);
        recent[i]->setText(list.at(i));
        OpenRecent->addAction(recent[i]);
        connect(recent[i], &QAction::triggered,[=](){
                    openROM(recent[i]->text());
                });
    }
}

void MainWindow::openROM(QString filename)
{
    if (QtAttachCoreLib()) {
        int response;
        (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &response);
        if (response == M64EMU_STOPPED) {
            workerThread = new WorkerThread();
            connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
            workerThread->setFileName(filename);
            QSettings settings("mupen64plus", "gui");
            my_window = new OGLWindow();
            container = QWidget::createWindowContainer(my_window);
            container->setFocusPolicy(Qt::StrongFocus);

            QSurfaceFormat format;
            format.setDepthBufferSize(24);
            if (settings.value("videoPlugin").toString().contains("GLideN64")) {
                format.setVersion(3, 3);
                format.setProfile(QSurfaceFormat::CoreProfile);
            }
            else {
                format.setVersion(2, 1);
                format.setProfile(QSurfaceFormat::CompatibilityProfile);
            }
            my_window->setFormat(format);

            setCentralWidget(container);

            QStringList list;
            if (settings.contains("RecentROMs"))
                    list = settings.value("RecentROMs").toString().split(";");
            list.removeAll(filename);
            list.prepend(filename);
            if (list.size() > 5)
                list.removeLast();
            settings.setValue("RecentROMs",list.join(";"));
            updateOpenRecent();
        }
    }
}

void MainWindow::on_actionOpen_ROM_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open ROM"), NULL, tr("ROM Files (*.n64 *.z64 *.v64)"));
    if (!filename.isNull())
        openROM(filename);
}

void MainWindow::on_actionPlugin_Paths_triggered()
{
    SettingsDialog *settings = new SettingsDialog();
    settings->show();
}

void MainWindow::on_actionStop_Game_triggered()
{
    if (QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_STOP, 0, NULL);
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionPlugin_Settings_triggered()
{
    PluginDialog *settings = new PluginDialog();
    settings->show();
}

void MainWindow::on_actionPause_Game_triggered()
{
    if (QtAttachCoreLib()) {
        int response;
        (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &response);
        if (response == M64EMU_RUNNING)
            (*CoreDoCommand)(M64CMD_PAUSE, 0, NULL);
        else if (response == M64EMU_PAUSED)
            (*CoreDoCommand)(M64CMD_RESUME, 0, NULL);
    }
}

void MainWindow::on_actionMute_triggered()
{
    if (QtAttachCoreLib()) {
        int response;
        (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_AUDIO_MUTE, &response);
        if (response == 0) {
            response = 1;
            (*CoreDoCommand)(M64CMD_CORE_STATE_SET, M64CORE_AUDIO_MUTE, &response);
        } else if (response == 1) {
            response = 0;
            (*CoreDoCommand)(M64CMD_CORE_STATE_SET, M64CORE_AUDIO_MUTE, &response);
        }
    }
}

void MainWindow::on_actionHard_Reset_triggered()
{
    if (QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_RESET, 1, NULL);
}

void MainWindow::on_actionSoft_Reset_triggered()
{
    if (QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_RESET, 0, NULL);
}

void MainWindow::on_actionTake_Screenshot_triggered()
{
    if (QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_TAKE_NEXT_SCREENSHOT, 0, NULL);
}

void MainWindow::on_actionSave_State_triggered()
{
    if (QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_STATE_SAVE, 1, NULL);
}

void MainWindow::on_actionLoad_State_triggered()
{
    if (QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_STATE_LOAD, 1, NULL);
}

void MainWindow::on_actionToggle_Fullscreen_triggered()
{
    if (QtAttachCoreLib()) {
        int response;
        (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_VIDEO_MODE, &response);
        if (response == M64VIDEO_WINDOWED) {
            response = M64VIDEO_FULLSCREEN;
            (*CoreDoCommand)(M64CMD_CORE_STATE_SET, M64CORE_VIDEO_MODE, &response);
        } else if (response == M64VIDEO_FULLSCREEN) {
            response = M64VIDEO_WINDOWED;
            (*CoreDoCommand)(M64CMD_CORE_STATE_SET, M64CORE_VIDEO_MODE, &response);
        }
    }
}

void MainWindow::on_actionSave_State_To_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Save State File"), NULL, tr("State Files (*.st*)"));
    if (!filename.isNull()) {
        if (!filename.contains(".st"))
            filename.append(".state");
        if (QtAttachCoreLib()) {
            (*CoreDoCommand)(M64CMD_STATE_SAVE, 1, filename.toLatin1().data());
        }
    }
}

void MainWindow::on_actionLoad_State_From_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open Save State"), NULL, tr("State Files (*.st*)"));
    if (!filename.isNull()) {
        if (QtAttachCoreLib()) {
            (*CoreDoCommand)(M64CMD_STATE_LOAD, 1, filename.toLatin1().data());
        }
    }
}
