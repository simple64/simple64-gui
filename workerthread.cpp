
#include "workerthread.h"
#include "vidext.h"

void WorkerThread::run() {
    openROM(m_fileName);
    my_window->doneCurrent();
    my_window->context()->moveToThread(QApplication::instance()->thread());
}

void WorkerThread::setFileName(QString filename) {
    m_fileName = filename;
}
