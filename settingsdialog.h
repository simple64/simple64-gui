#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog();
private slots:
    void handleCoreButton();
    void handlePluginButton();
    void handleConfigButton();
    void handleClearConfigButton();
    void handleCoreEdit();
    void handlePluginEdit();
    void handleConfigEdit();
private:
    QLineEdit *corePath;
    QLineEdit *pluginPath;
    QLineEdit *configPath;
};

#endif // SETTINGSDIALOG_H
