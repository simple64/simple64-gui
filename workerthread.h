#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QApplication>
#include <QString>
#include "common.h"

class WorkerThread : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE;
public:
    void setFileName(QString filename);

signals:
    void resizeMainWindow(int Width, int Height);

private:
    QString m_fileName;
};

extern WorkerThread* workerThread;
#endif // WORKERTHREAD_H
