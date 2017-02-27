
#include "workerthread.h"
#include "vidext.h"
#include "mainwindow.h"

void WorkerThread::run()
{
    connect(this, SIGNAL(resizeMainWindow(int,int)), w, SLOT(resizeMainWindow(int, int)), Qt::QueuedConnection);
    openROM(m_fileName.toStdString());
    (*ConfigSaveFile)();
    my_window->doneCurrent();
    my_window->context()->moveToThread(QApplication::instance()->thread());
}

void WorkerThread::setFileName(QString filename)
{
    m_fileName = filename;
}
