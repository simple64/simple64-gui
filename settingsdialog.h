#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLabel>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog();
private slots:
    void handleCoreButton();
    void handlePluginButton();
private:
    QLabel *corePath;
    QLabel *pluginPath;
};

#endif // SETTINGSDIALOG_H
