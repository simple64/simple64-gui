#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>

class PluginDialog : public QDialog
{
    Q_OBJECT
public:
    PluginDialog(QWidget *parent = nullptr);
private slots:
    void handleResetButton();
};

#endif // PLUGINDIALOG_H
