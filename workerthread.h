#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QApplication>
#include <QString>
#include "common.h"
#include "vidext.h"

class WorkerThread : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE {
        openROM(m_fileName);
        my_window->context()->moveToThread(QApplication::instance()->thread());
    }
public:
    void setFileName(QString filename) {
        m_fileName = filename;
    }

private:
    QString m_fileName;
};

#endif // WORKERTHREAD_H
