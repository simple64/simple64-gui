#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>

class PluginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PluginDialog(QWidget *parent = 0);
    ~PluginDialog(){}
};

#endif // PLUGINDIALOG_H
