
#include "workerthread.h"
#include "vidext.h"
#include "mainwindow.h"

void WorkerThread::run()
{
    w->setGameRunning(1);
    connect(this, SIGNAL(resizeMainWindow(int,int)), w, SLOT(resizeMainWindow(int, int)), Qt::QueuedConnection);
    openROM(m_fileName);
    (*ConfigSaveFile)();
    my_window->doneCurrent();
    my_window->context()->moveToThread(QApplication::instance()->thread());
    w->setGameRunning(0);
}

void WorkerThread::setFileName(QString filename)
{
    m_fileName = filename;
}
