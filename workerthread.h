#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QApplication>
#include <QString>
#include <QSurfaceFormat>
#include "common.h"

class WorkerThread : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE;
public:
    void setFileName(QString filename);

signals:
    void resizeMainWindow(int Width, int Height);
    void toggleFS(int force);
    void createOGLWindow(QSurfaceFormat* format);
    void deleteOGLWindow();
    void setTitle(std::string title);
    void pluginWarning(QString name);

private:
    QString m_fileName;
};

#endif // WORKERTHREAD_H
