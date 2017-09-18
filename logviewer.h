#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>
#include <QScrollArea>
#include <QLabel>
#include <QVBoxLayout>

class LogViewer : public QDialog
{
    Q_OBJECT
public:
    LogViewer();
    void addLog(QString text);
    void clearLog();
protected:
    void showEvent(QShowEvent *event);
private:
    QString data;
    QScrollArea *area;
    QLabel *label;
};

extern LogViewer *logViewer;

#endif // LOGVIEWER_H
