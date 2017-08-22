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
#include "cheatdialog.h"
#include "controllerdialog.h"

OGLWindow *my_window = nullptr;
WorkerThread *workerThread = nullptr;

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
        QAction *temp_slot = my_slots[i];
        connect(temp_slot, &QAction::triggered,[=](bool checked){
            if (checked) {
                int slot = temp_slot->text().remove("Slot ").toInt();
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
        findRecursion("/usr/lib", QString("mupen64plus-rsp-cxd4-sse2") + OSAL_DLL_EXTENSION, &files2);
        findRecursion("/usr/local/lib", QString("mupen64plus-rsp-cxd4-sse2") + OSAL_DLL_EXTENSION, &files2);
        findRecursion("/usr/lib", QString("mupen64plus-rsp-hle") + OSAL_DLL_EXTENSION, &files2);
        findRecursion("/usr/local/lib", QString("mupen64plus-rsp-hle") + OSAL_DLL_EXTENSION, &files2);

        if (files2.size() > 0) {
            QFileInfo pluginPath(files2.at(0));
            settings.setValue("pluginDirPath", pluginPath.absolutePath());
        } else
            settings.setValue("pluginDirPath", QString(".") + QDir::separator());
    }
    if (!settings.value("coreLibPath").isNull())
        qtCoreDirPath = settings.value("coreLibPath").toString();
    if (!settings.value("pluginDirPath").isNull())
        qtPluginDir = settings.value("pluginDirPath").toString();

    QDir *PluginDir = new QDir(qtPluginDir);
    QStringList Filter;
    Filter.append("");
    QStringList current;
    QString default_value;
    if (!settings.contains("videoPlugin")) {
        Filter.replace(0,"mupen64plus-video*");
        current = PluginDir->entryList(Filter);
        default_value = "mupen64plus-video-GLideN64";
        default_value += OSAL_DLL_EXTENSION;
        if (current.isEmpty())
            settings.setValue("videoPlugin", default_value);
        else if (current.indexOf(default_value) != -1)
            settings.setValue("videoPlugin", default_value);
        else
            settings.setValue("videoPlugin", current.at(0));
    }
    if (!settings.contains("audioPlugin")) {
        Filter.replace(0,"mupen64plus-audio*");
        current = PluginDir->entryList(Filter);
        default_value = "mupen64plus-audio-sdl";
        default_value += OSAL_DLL_EXTENSION;
        if (current.isEmpty())
            settings.setValue("audioPlugin", default_value);
        else
            settings.setValue("audioPlugin", current.at(0));
    }
    if (!settings.contains("rspPlugin")) {
        Filter.replace(0,"mupen64plus-rsp*");
        current = PluginDir->entryList(Filter);
        default_value = "mupen64plus-rsp-cxd4-sse2";
        default_value += OSAL_DLL_EXTENSION;
        if (current.isEmpty())
            settings.setValue("rspPlugin", default_value);
        else if (current.indexOf(default_value) != -1)
            settings.setValue("rspPlugin", default_value);
        else
            settings.setValue("rspPlugin", current.at(0));
    }
    if (!settings.contains("inputPlugin")) {
        Filter.replace(0,"mupen64plus-input*");
        current = PluginDir->entryList(Filter);
        default_value = "mupen64plus-input-sdl";
        default_value += OSAL_DLL_EXTENSION;
        if (current.isEmpty())
            settings.setValue("inputPlugin", default_value);
        else
            settings.setValue("inputPlugin", current.at(0));
    }
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

void MainWindow::resizeMainWindow(int Width, int Height)
{
    resize(Width, Height + (menuBar()->isNativeMenuBar() ? 0 : ui->menuBar->height()) + ui->statusBar->height());
}

void MainWindow::toggleFS(int force)
{
    int response = M64VIDEO_NONE;
    if (force == M64VIDEO_NONE)
        (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_VIDEO_MODE, &response);
    if (response == M64VIDEO_WINDOWED || force == M64VIDEO_FULLSCREEN) {
        if (!menuBar()->isNativeMenuBar())
            menuBar()->hide();
        statusBar()->hide();
        showFullScreen();
    } else if (response == M64VIDEO_FULLSCREEN || force == M64VIDEO_WINDOWED) {
        if (!menuBar()->isNativeMenuBar())
            menuBar()->show();
        statusBar()->show();
        showNormal();
    }
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
    if (QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_STOP, 0, NULL);
    if (workerThread != nullptr) {
        while (workerThread->isRunning())
            QCoreApplication::processEvents();
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
        QAction *temp_recent = recent[i];
        connect(temp_recent, &QAction::triggered,[=](){
                    openROM(temp_recent->text());
                });
    }
}

void MainWindow::setTitle(std::string title)
{
    QString _title = QString::fromStdString(title);
    _title.prepend("mupen64plus: ");
    this->setWindowTitle(_title);
}

void MainWindow::pluginWarning(QString name)
{
    QMessageBox msgBox;
    msgBox.setText("No " + name + " plugin attached. Please go to \"Settings->Plugin Selection\" and choose one.");
    msgBox.exec();
}

void MainWindow::createOGLWindow(QSurfaceFormat format)
{
    my_window = new OGLWindow();
    QWidget *container = QWidget::createWindowContainer(my_window);
    container->setFocusPolicy(Qt::StrongFocus);

    my_window->setCursor(Qt::BlankCursor);
    my_window->setFormat(format);

    setCentralWidget(container);
}

void MainWindow::deleteOGLWindow()
{
    my_window->doneCurrent();
    my_window->destroy();
    my_window = nullptr;
}

void MainWindow::openROM(QString filename)
{
    if (QtAttachCoreLib()) {
        (*CoreDoCommand)(M64CMD_STOP, 0, NULL);
        if (workerThread != nullptr) {
            while (workerThread->isRunning())
                QCoreApplication::processEvents();
        }
        workerThread = new WorkerThread();
        workerThread->setFileName(filename);
        workerThread->start();

        QSettings settings("mupen64plus", "gui");
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

void MainWindow::on_actionOpen_ROM_triggered()
{
    QSettings settings("mupen64plus", "gui");
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open ROM"), settings.value("ROMdir").toString(), tr("ROM Files (*.n64 *.z64 *.v64)"));
    if (!filename.isNull()) {
        QFileInfo info(filename);
        settings.setValue("ROMdir", info.absoluteDir().absolutePath());
        openROM(filename);
    }
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
    if (QtAttachCoreLib()) {
        PluginDialog *settings = new PluginDialog();
        settings->show();
    }
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
        tr("Open Save State"), NULL, tr("State Files (*.st* *.pj*)"));
    if (!filename.isNull()) {
        if (QtAttachCoreLib()) {
            (*CoreDoCommand)(M64CMD_STATE_LOAD, 1, filename.toLatin1().data());
        }
    }
}

void MainWindow::on_actionCheats_triggered()
{
    CheatDialog *cheats = new CheatDialog();
    cheats->show();
}

void MainWindow::on_actionController_Configuration_triggered()
{
    if (QtAttachCoreLib()) {
        ControllerDialog *controller = new ControllerDialog();
        controller->show();
    }
}

void MainWindow::on_actionToggle_Speed_Limiter_triggered()
{
    if (QtAttachCoreLib()) {
        int value;
        (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_SPEED_LIMITER, &value);
        value = !value;
        (*CoreDoCommand)(M64CMD_CORE_STATE_SET, M64CORE_SPEED_LIMITER, &value);
    }
}
