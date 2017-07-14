
#include "workerthread.h"
#include "vidext.h"
#include "mainwindow.h"

void WorkerThread::run()
{
    connect(this, SIGNAL(resizeMainWindow(int,int)), w, SLOT(resizeMainWindow(int, int)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(toggleFS(int)), w, SLOT(toggleFS(int)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(createOGLWindow(QSurfaceFormat)), w, SLOT(createOGLWindow(QSurfaceFormat)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(setTitle(std::string)), w, SLOT(setTitle(std::string)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(pluginWarning(QString)), w, SLOT(pluginWarning(QString)), Qt::BlockingQueuedConnection);
    openROM(m_fileName.toStdString());
    (*ConfigSaveFile)();
    my_window->doneCurrent();
    my_window->context()->moveToThread(QApplication::instance()->thread());
}

void WorkerThread::setFileName(QString filename)
{
    m_fileName = filename;
}
