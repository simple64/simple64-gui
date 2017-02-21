#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QLibrary>
#include <QCloseEvent>
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    my_window = new OGLWindow();
    container = QWidget::createWindowContainer(my_window);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    my_window->setFormat(format);

    setCentralWidget(container);

    QSettings settings("mupen64plus", "gui");
    if (!settings.contains("coreLibPath")) {
        QLibrary myLib("mupen64plus");
        if (myLib.load()) {
            settings.setValue("coreLibPath", myLib.fileName());
            myLib.unload();
        }
    }
    if (!settings.contains("pluginDirPath")) {
        QLibrary myLib("mupen64plus-rsp-hle");
        if (myLib.load()) {
            QFileInfo pluginPath(myLib.fileName());
            settings.setValue("pluginDirPath", pluginPath.absolutePath());
            myLib.unload();
        }
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


void MainWindow::closeEvent (QCloseEvent *event)
{
    if (QtAttachCoreLib()) {
        (*CoreDoCommand)(M64CMD_STOP, 0, NULL);
        int response = 0;
        do {
            (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &response);
        } while (response > 1);
    }
    event->accept();
}

void MainWindow::on_actionOpen_ROM_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open ROM"), NULL, tr("ROM Files (*.n64 *.z64 *.v64)"));
    if (!filename.isNull()) {
        if (QtAttachCoreLib()) {
            int response;
            (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &response);
            if (response < 2) {
                WorkerThread *workerThread = new WorkerThread();
                connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
                my_window->context()->moveToThread(workerThread);
                workerThread->setFileName(filename);
                workerThread->start();
            }
        }
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
    PluginDialog *settings = new PluginDialog();
    settings->show();
}
