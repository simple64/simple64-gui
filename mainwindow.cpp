#include <QString>
#include <QLibrary>
#include <QFileDialog>
#include <QMessageBox>
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
#include "logviewer.h"

#define RECENT_SIZE 10
OGLWindow *my_window = nullptr;
WorkerThread *workerThread = nullptr;
LogViewer *logViewer = nullptr;
QSettings *settings = nullptr;

void MainWindow::resetTitle()
{
    this->setWindowTitle(m_title);
}

void MainWindow::updatePlugins()
{
    QDir *PluginDir = new QDir(qtPluginDir);
    PluginDir->setFilter(QDir::Files);
    QStringList Filter;
    Filter.append("");
    QStringList current;
    QString default_value;
    if (!settings->contains("videoPlugin")) {
        Filter.replace(0,"mupen64plus-video*");
        current = PluginDir->entryList(Filter);
        default_value = "mupen64plus-video-GLideN64";
        default_value += OSAL_DLL_EXTENSION;
        if (current.isEmpty())
            settings->setValue("videoPlugin", "dummy");
        else if (current.indexOf(default_value) != -1)
            settings->setValue("videoPlugin", default_value);
        else
            settings->setValue("videoPlugin", current.at(0));
    }
    if (!settings->contains("audioPlugin")) {
        Filter.replace(0,"mupen64plus-audio*");
        current = PluginDir->entryList(Filter);
        if (current.isEmpty())
            settings->setValue("audioPlugin", "dummy");
        else
            settings->setValue("audioPlugin", current.at(0));
    }
    if (!settings->contains("rspPlugin")) {
        Filter.replace(0,"mupen64plus-rsp*");
        current = PluginDir->entryList(Filter);
        default_value = "mupen64plus-rsp-hle";
        default_value += OSAL_DLL_EXTENSION;
        if (current.isEmpty())
            settings->setValue("rspPlugin", "dummy");
        else if (current.indexOf(default_value) != -1)
            settings->setValue("rspPlugin", default_value);
        else
            settings->setValue("rspPlugin", current.at(0));
    }
    if (!settings->contains("inputPlugin")) {
        Filter.replace(0,"mupen64plus-input*");
        current = PluginDir->entryList(Filter);
        if (current.isEmpty())
            settings->setValue("inputPlugin", "dummy");
        else
            settings->setValue("inputPlugin", current.at(0));
    }
    qtGfxPlugin = settings->value("videoPlugin").toString();
    qtAudioPlugin = settings->value("audioPlugin").toString();
    qtRspPlugin = settings->value("rspPlugin").toString();
    qtInputPlugin = settings->value("inputPlugin").toString();
}

void MainWindow::updateGB(Ui::MainWindow *ui)
{
    QMenu *GB = new QMenu;
    GB->setTitle("Game Boy Cartridges");
    ui->menuFile->insertMenu(ui->actionTake_Screenshot, GB);

    QAction *fileSelect = new QAction(this);
    QString current = settings->value("Player1GBROM").toString();
    fileSelect->setText("Player 1 ROM: " + current);
    GB->addAction(fileSelect);
    connect(fileSelect, &QAction::triggered,[=](){
        QString filename = QFileDialog::getOpenFileName(this,
            tr("GB ROM File"), NULL, tr("GB ROM Files (*.gb)"));
        if (!filename.isNull()) {
            settings->setValue("Player1GBROM", filename);
            QString current = filename;
            fileSelect->setText("Player 1 ROM: " + current);
        }
    });

    QAction *fileSelect2 = new QAction(this);
    current = settings->value("Player1GBRAM").toString();
    fileSelect2->setText("Player 1 RAM: " + current);
    GB->addAction(fileSelect2);
    connect(fileSelect2, &QAction::triggered,[=](){
        QString filename = QFileDialog::getOpenFileName(this,
            tr("GB RAM File"), NULL, tr("GB RAM Files (*.sav)"));
        if (!filename.isNull()) {
            settings->setValue("Player1GBRAM", filename);
            QString current = filename;
            fileSelect2->setText("Player 1 RAM: " + current);
        }
    });

    QAction *clearSelect = new QAction(this);
    clearSelect->setText("Clear Player 1 Selections");
    GB->addAction(clearSelect);
    connect(clearSelect, &QAction::triggered,[=](){
        settings->remove("Player1GBROM");
        settings->remove("Player1GBRAM");
        fileSelect->setText("Player 1 ROM: ");
        fileSelect2->setText("Player 1 RAM: ");
    });
    GB->addSeparator();

    fileSelect = new QAction(this);
    current = settings->value("Player2GBROM").toString();
    fileSelect->setText("Player 2 ROM: " + current);
    GB->addAction(fileSelect);
    connect(fileSelect, &QAction::triggered,[=](){
        QString filename = QFileDialog::getOpenFileName(this,
            tr("GB ROM File"), NULL, tr("GB ROM Files (*.gb)"));
        if (!filename.isNull()) {
            settings->setValue("Player2GBROM", filename);
            QString current = filename;
            fileSelect->setText("Player 2 ROM: " + current);
        }
    });

    fileSelect2 = new QAction(this);
    current = settings->value("Player2GBRAM").toString();
    fileSelect2->setText("Player 2 RAM: " + current);
    GB->addAction(fileSelect2);
    connect(fileSelect2, &QAction::triggered,[=](){
        QString filename = QFileDialog::getOpenFileName(this,
            tr("GB RAM File"), NULL, tr("GB RAM Files (*.sav)"));
        if (!filename.isNull()) {
            settings->setValue("Player2GBRAM", filename);
            QString current = filename;
            fileSelect2->setText("Player 2 RAM: " + current);
        }
    });

    clearSelect = new QAction(this);
    clearSelect->setText("Clear Player 2 Selections");
    GB->addAction(clearSelect);
    connect(clearSelect, &QAction::triggered,[=](){
        settings->remove("Player2GBROM");
        settings->remove("Player2GBRAM");
        fileSelect->setText("Player 2 ROM: ");
        fileSelect2->setText("Player 2 RAM: ");
    });
    GB->addSeparator();

    fileSelect = new QAction(this);
    current = settings->value("Player3GBROM").toString();
    fileSelect->setText("Player 3 ROM: " + current);
    GB->addAction(fileSelect);
    connect(fileSelect, &QAction::triggered,[=](){
        QString filename = QFileDialog::getOpenFileName(this,
            tr("GB ROM File"), NULL, tr("GB ROM Files (*.gb)"));
        if (!filename.isNull()) {
            settings->setValue("Player3GBROM", filename);
            QString current = filename;
            fileSelect->setText("Player 3 ROM: " + current);
        }
    });

    fileSelect2 = new QAction(this);
    current = settings->value("Player3GBRAM").toString();
    fileSelect2->setText("Player 3 RAM: " + current);
    GB->addAction(fileSelect2);
    connect(fileSelect2, &QAction::triggered,[=](){
        QString filename = QFileDialog::getOpenFileName(this,
            tr("GB RAM File"), NULL, tr("GB RAM Files (*.sav)"));
        if (!filename.isNull()) {
            settings->setValue("Player3GBRAM", filename);
            QString current = filename;
            fileSelect2->setText("Player 3 RAM: " + current);
        }
    });

    clearSelect = new QAction(this);
    clearSelect->setText("Clear Player 3 Selections");
    GB->addAction(clearSelect);
    connect(clearSelect, &QAction::triggered,[=](){
        settings->remove("Player3GBROM");
        settings->remove("Player3GBRAM");
        fileSelect->setText("Player 3 ROM: ");
        fileSelect2->setText("Player 3 RAM: ");
    });
    GB->addSeparator();

    fileSelect = new QAction(this);
    current = settings->value("Player4GBROM").toString();
    fileSelect->setText("Player 4 ROM: " + current);
    GB->addAction(fileSelect);
    connect(fileSelect, &QAction::triggered,[=](){
        QString filename = QFileDialog::getOpenFileName(this,
            tr("GB ROM File"), NULL, tr("GB ROM Files (*.gb)"));
        if (!filename.isNull()) {
            settings->setValue("Player4GBROM", filename);
            QString current = filename;
            fileSelect->setText("Player 4 ROM: " + current);
        }
    });

    fileSelect2 = new QAction(this);
    current = settings->value("Player4GBRAM").toString();
    fileSelect2->setText("Player 4 RAM: " + current);
    GB->addAction(fileSelect2);
    connect(fileSelect2, &QAction::triggered,[=](){
        QString filename = QFileDialog::getOpenFileName(this,
            tr("GB RAM File"), NULL, tr("GB RAM Files (*.sav)"));
        if (!filename.isNull()) {
            settings->setValue("Player4GBRAM", filename);
            QString current = filename;
            fileSelect2->setText("Player 4 RAM: " + current);
        }
    });

    clearSelect = new QAction(this);
    clearSelect->setText("Clear Player 4 Selections");
    GB->addAction(clearSelect);
    connect(clearSelect, &QAction::triggered,[=](){
        settings->remove("Player4GBROM");
        settings->remove("Player4GBRAM");
        fileSelect->setText("Player 4 ROM: ");
        fileSelect2->setText("Player 4 RAM: ");
    });
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    verbose = 0;
    nogui = 0;
    gles = 0;
    ui->setupUi(this);

    m_title = "mupen64plus-gui    Build Date: ";
    m_title += __DATE__;
    resetTitle();

    QString ini_path = QDir(QCoreApplication::applicationDirPath()).filePath("mupen64plus-gui.ini");
    settings = new QSettings(ini_path, QSettings::IniFormat);

    if (!settings->isWritable())
        settings = new QSettings("mupen64plus", "gui");

    restoreGeometry(settings->value("geometry").toByteArray());
    restoreState(settings->value("windowState").toByteArray());

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
    updateGB(ui);

    if (!settings->contains("coreLibPath")) {
        QStringList files;
        findRecursion("/usr/lib", OSAL_DEFAULT_DYNLIB_FILENAME, &files);
        findRecursion("/usr/local/lib", OSAL_DEFAULT_DYNLIB_FILENAME, &files);
        if (files.size() > 0)
            settings->setValue("coreLibPath", files.at(0));
        else
            settings->setValue("coreLibPath", QDir(QCoreApplication::applicationDirPath()).filePath(OSAL_DEFAULT_DYNLIB_FILENAME));
    }
    if (!settings->contains("pluginDirPath")) {
        QStringList files2;
        findRecursion("/usr/lib", QString("mupen64plus-rsp-cxd4-sse2") + OSAL_DLL_EXTENSION, &files2);
        findRecursion("/usr/local/lib", QString("mupen64plus-rsp-cxd4-sse2") + OSAL_DLL_EXTENSION, &files2);
        findRecursion("/usr/lib", QString("mupen64plus-rsp-hle") + OSAL_DLL_EXTENSION, &files2);
        findRecursion("/usr/local/lib", QString("mupen64plus-rsp-hle") + OSAL_DLL_EXTENSION, &files2);

        if (files2.size() > 0) {
            QFileInfo pluginPath(files2.at(0));
            settings->setValue("pluginDirPath", pluginPath.absolutePath());
        } else
            settings->setValue("pluginDirPath", QCoreApplication::applicationDirPath());
    }
    if (!settings->value("coreLibPath").isNull())
        qtCoreDirPath = settings->value("coreLibPath").toString();
    if (!settings->value("pluginDirPath").isNull())
        qtPluginDir = settings->value("pluginDirPath").toString();
    if (!settings->value("configDirPath").isNull())
        qtConfigDir = settings->value("configDirPath").toString();

    updatePlugins();

    logViewer = new LogViewer();
}

MainWindow::~MainWindow()
{
    if (coreStarted)
        (*CoreShutdown)();
    DetachCoreLib();
    delete ui;
}

void MainWindow::setVerbose()
{
    verbose = 1;
}

void MainWindow::setNoGUI()
{
    nogui = 1;
    if (!menuBar()->isNativeMenuBar())
        menuBar()->hide();
    statusBar()->hide();
}

void MainWindow::setGLES()
{
    gles = 1;
}

int MainWindow::getNoGUI()
{
    return nogui;
}

int MainWindow::getVerbose()
{
    return verbose;
}

int MainWindow::getGLES()
{
    return gles;
}

void MainWindow::resizeMainWindow(int Width, int Height)
{
    QSize size = this->size();
    Height += (menuBar()->isNativeMenuBar() ? 0 : ui->menuBar->height()) + ui->statusBar->height();
    if (size.width() != Width || size.height() != Height)
        resize(Width, Height);
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
        if (!nogui) {
            if (!menuBar()->isNativeMenuBar())
                menuBar()->show();
            statusBar()->show();
        }
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

    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());

    event->accept();
}

void MainWindow::updateOpenRecent()
{
    OpenRecent->clear();
    QAction *recent[RECENT_SIZE];
    QStringList list = settings->value("RecentROMs").toString().split(";");
    if (list.first() == "") {
        OpenRecent->setEnabled(false);
        return;
    }

    OpenRecent->setEnabled(true);
    for (int i = 0; i < list.size() && i < RECENT_SIZE; ++i) {
        recent[i] = new QAction(this);
        recent[i]->setText(list.at(i));
        OpenRecent->addAction(recent[i]);
        QAction *temp_recent = recent[i];
        connect(temp_recent, &QAction::triggered,[=](){
                    openROM(temp_recent->text());
                });
    }
    OpenRecent->addSeparator();

    QAction *clearRecent = new QAction(this);
    clearRecent->setText("Clear List");
    OpenRecent->addAction(clearRecent);
    connect(clearRecent, &QAction::triggered,[=](){
        settings->remove("RecentROMs");
        updateOpenRecent();
    });
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

void MainWindow::createOGLWindow(QSurfaceFormat* format)
{
    my_window = new OGLWindow();
    QWidget *container = QWidget::createWindowContainer(my_window);
    container->setFocusPolicy(Qt::StrongFocus);

    my_window->setCursor(Qt::BlankCursor);
    my_window->setFormat(*format);

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
    if (QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_STOP, 0, NULL);

    if (workerThread != nullptr) {
        while (workerThread->isRunning())
            QCoreApplication::processEvents();
    }
    logViewer->clearLog();
    workerThread = new WorkerThread();
    workerThread->setFileName(filename);
    workerThread->start();

    QStringList list;
    if (settings->contains("RecentROMs"))
        list = settings->value("RecentROMs").toString().split(";");
    list.removeAll(filename);
    list.prepend(filename);
    if (list.size() > RECENT_SIZE)
        list.removeLast();
    settings->setValue("RecentROMs",list.join(";"));
    updateOpenRecent();
}

void MainWindow::on_actionOpen_ROM_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open ROM"), settings->value("ROMdir").toString(), tr("ROM Files (*.n64 *.N64 *.z64 *.Z64 *.v64 *.V64 *.zip *.ZIP *.7z)"));
    if (!filename.isNull()) {
        QFileInfo info(filename);
        settings->setValue("ROMdir", info.absoluteDir().absolutePath());
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
        tr("Save State File"), NULL, NULL);
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
        if (qtInputPlugin.contains("input-sdl")) {
            ControllerDialog *controller = new ControllerDialog();
            controller->show();
        } else {
            QMessageBox msgBox;
            msgBox.setText("This dialog is only compatible with the Input-SDL plugin.");
            msgBox.exec();
        }
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

void MainWindow::on_actionView_Log_triggered()
{
    logViewer->show();
}

void MainWindow::on_actionVideo_Settings_triggered()
{
    if (QtAttachCoreLib()) {
        PluginSearchLoad();
        QString lib_location = qtPluginDir + "/" + qtGfxPlugin;
        QLibrary myLib(lib_location);
        typedef void (*Config_Func)();
        Config_Func Config_DoConfig = (Config_Func) myLib.resolve("Config_DoConfig");
        if (Config_DoConfig)
            Config_DoConfig();
        else
            printf("%s", lib_location.toStdString().c_str());
    }
}
