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
#include "plugin.h"
extern "C" {
#include "core_interface.h"
}

char* filename;
QOpenGLContext *my_context;
QWidget *container;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSettings settings("mupen64plus", "gui");
    if (!settings.contains("coreLibPath")) {
        QLibrary myLib("mupen64plus");
        if (myLib.load()) {
            settings.setValue("coreLibPath", myLib.fileName());
            myLib.unload();
        }
    }
    if (!settings.value("coreLibPath").isNull())
        qt_CoreDirPath = settings.value("coreLibPath").toString().toLatin1().data();

    if (AttachCoreLib(qt_CoreDirPath) != M64ERR_SUCCESS) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't load core library, please set the path in settings.");
        msgBox.exec();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_ROM_triggered()
{
    QSettings settings("mupen64plus", "gui");
    if (!settings.value("pluginDirPath").isNull())
        qt_PluginDir = strdup(settings.value("pluginDirPath").toString().toLatin1().data());
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open ROM"), NULL, tr("ROM Files (*.n64 *.z64 *.v64)"));
    if (!fileName.isNull()) {
        main_thread = co_active();
        filename = strdup(fileName.toLatin1().data());
        game_thread = co_create(65536 * sizeof(void*) * 16, openROM);

        OGLWindow *my_window = new OGLWindow();
        container = QWidget::createWindowContainer(my_window);

        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        my_window->setFormat(format);

        setCentralWidget(container);
        my_context = my_window->context();
    }
}

void MainWindow::on_actionPlugin_Paths_triggered()
{
    SettingsDialog *settings = new SettingsDialog();
    settings->show();
}
