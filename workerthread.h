#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QString>
#include "common.h"

class WorkerThread : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE {
        openROM(m_fileName);
    }
public:
    void setFileName(QString filename) {
        m_fileName = filename;
    }

private:
    QString m_fileName;
};

#endif // WORKERTHREAD_H
