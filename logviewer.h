#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>
#include <QScrollArea>
#include <QLabel>
#include <QVBoxLayout>
#include <QTemporaryFile>
#include <QTextStream>

class LogViewer : public QDialog
{
    Q_OBJECT
public:
    LogViewer();
    ~LogViewer();
    void addLog(QString text);
    void clearLog();
protected:
    void showEvent(QShowEvent *event);
private:
    QTemporaryFile *file = nullptr;
    QScrollArea *area = nullptr;
    QLabel *label = nullptr;
};

extern LogViewer *logViewer;

#endif // LOGVIEWER_H
