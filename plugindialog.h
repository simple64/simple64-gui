#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>

class PluginDialog : public QDialog
{
    Q_OBJECT
public:
    PluginDialog();
private slots:
    void handleResetButton();
private:
    QString RSPName;
    QString AudioName;
    QString VideoName;
};

#endif // PLUGINDIALOG_H
