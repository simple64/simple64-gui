#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QLibrary>
#include "oglwindow.h"
#include "settingsdialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include "core_interface.h"
#include "plugin.h"

QString filename;
QOpenGLContext *my_context;
QWidget *container;
OGLWindow *my_window;

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
    my_context = my_window->context();

    QSettings settings("mupen64plus", "gui");
    if (!settings.contains("coreLibPath")) {
        QLibrary myLib("mupen64plus");
        if (myLib.load()) {
            settings.setValue("coreLibPath", myLib.fileName());
            myLib.unload();
        }
    }
    if (!settings.value("coreLibPath").isNull())
        qtCoreDirPath = settings.value("coreLibPath").toString();
    if (!settings.value("pluginDirPath").isNull())
        qtPluginDir = settings.value("pluginDirPath").toString();
}

MainWindow::~MainWindow()
{
    if (emuRunning) {
        (*CoreDoCommand)(M64CMD_STOP, 0, NULL);
        co_switch(game_thread);
    }
    if (coreStarted)
        (*CoreShutdown)();
    DetachCoreLib();
    delete ui;
}

void MainWindow::on_actionOpen_ROM_triggered()
{
    filename = QFileDialog::getOpenFileName(this,
        tr("Open ROM"), NULL, tr("ROM Files (*.n64 *.z64 *.v64)"));
    if (!filename.isNull() && !emuRunning) {
        if (QtAttachCoreLib()) {
            main_thread = co_active();
            game_thread = co_create(65536 * sizeof(void*) * 16, openROM);
            my_window->update();
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
