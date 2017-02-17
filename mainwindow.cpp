#include <QString>
#include <QFileDialog>
#include "oglwindow.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"

char* filename;
QOpenGLContext *my_context;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_ROM_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open ROM"), NULL, tr("ROM Files (*.n64 *.z64 *.v64)"));
    main_thread = co_active();
    filename = strdup(fileName.toStdString().c_str());
    game_thread = co_create(65536 * sizeof(void*) * 16, openROM);

    OGLWindow *my_window = new OGLWindow();
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    my_window->setFormat(format);
    QWidget *container = QWidget::createWindowContainer(my_window);
    setCentralWidget(container);
    my_context = my_window->context();
}
