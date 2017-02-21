#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLabel>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog(){}
private slots:
    void handleCoreButton();
    void handlePluginButton();
private:
    QLabel *corePath;
    QLabel *pluginPath;
};

#endif // SETTINGSDIALOG_H
