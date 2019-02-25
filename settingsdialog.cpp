#include "settingsdialog.h"
#include "core_interface.h"
#include "mainwindow.h"
#include "plugin.h"

#include <QPushButton>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

void SettingsDialog::handleCoreButton()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Locate Core Library"), NULL, tr("Shared Libraries (*.dylib *.so* *.dll)"));
    if (!fileName.isNull()) {
        corePath->setText(fileName);
        settings->setValue("coreLibPath", fileName);
        qtCoreDirPath = settings->value("coreLibPath").toString();
    }
}

void SettingsDialog::handlePluginButton()
{
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Locate Plugin Directory"),
                                                         NULL,
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
    if (!fileName.isNull()) {
        pluginPath->setText(fileName);
        settings->setValue("pluginDirPath", fileName);
        qtPluginDir = settings->value("pluginDirPath").toString();

        settings->remove("inputPlugin");
        settings->remove("videoPlugin");
        settings->remove("audioPlugin");
        settings->remove("rspPlugin");
        w->updatePlugins();
        initStuff();
    }
}

void SettingsDialog::handleConfigButton()
{
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Set Config Directory"),
                                                         NULL,
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
    if (!fileName.isNull()) {
        configPath->setText(fileName);
        settings->setValue("configDirPath", fileName);
        qtConfigDir = settings->value("configDirPath").toString();
    }
}

void SettingsDialog::handleClearConfigButton()
{
    configPath->setText("");
    settings->remove("configDirPath");
    qtConfigDir = QString();
}

void SettingsDialog::handleCoreEdit()
{
    settings->setValue("coreLibPath", corePath->text());
    qtCoreDirPath = settings->value("coreLibPath").toString();
}

void SettingsDialog::handlePluginEdit()
{
    settings->setValue("pluginDirPath", pluginPath->text());
    qtPluginDir = settings->value("pluginDirPath").toString();

    settings->remove("inputPlugin");
    settings->remove("videoPlugin");
    settings->remove("audioPlugin");
    settings->remove("rspPlugin");
    w->updatePlugins();
    initStuff();
}

void SettingsDialog::handleConfigEdit()
{
    settings->setValue("configDirPath", configPath->text());
    qtConfigDir = settings->value("configDirPath").toString();
}

void SettingsDialog::initStuff()
{
    if (layout != nullptr)
        delete layout;
    layout = new QGridLayout;

    QLabel *coreLabel = new QLabel("Core Library Path");
    corePath = new QLineEdit;
    corePath->setText(settings->value("coreLibPath").toString());
    QPushButton *coreButton = new QPushButton("Set Path");
    connect(coreButton, SIGNAL (released()),this, SLOT (handleCoreButton()));
    connect(corePath, SIGNAL (editingFinished()),this, SLOT (handleCoreEdit()));
    corePath->setStyleSheet("border: 1px solid; padding: 10px");
    layout->addWidget(coreLabel,0,0);
    layout->addWidget(corePath,0,1);
    layout->addWidget(coreButton,0,2);

    QLabel *pluginLabel = new QLabel("Plugin Dir Path");
    pluginPath = new QLineEdit;
    pluginPath->setText(settings->value("pluginDirPath").toString());
    QPushButton *pluginButton = new QPushButton("Set Path");
    connect(pluginButton, SIGNAL (released()),this, SLOT (handlePluginButton()));
    connect(pluginPath, SIGNAL (editingFinished()),this, SLOT (handlePluginEdit()));
    pluginPath->setStyleSheet("border: 1px solid; padding: 10px");
    layout->addWidget(pluginLabel,1,0);
    layout->addWidget(pluginPath,1,1);
    layout->addWidget(pluginButton,1,2);

    QLabel *note = new QLabel("Note: If you change the Config Path, you need to close and re-open mupen64plus-gui before it will take effect.");
    QLabel *configLabel = new QLabel("Config Dir Path");
    configPath = new QLineEdit;
    configPath->setText(settings->value("configDirPath").toString());
    QPushButton *configButton = new QPushButton("Set Path");
    connect(configButton, SIGNAL (released()),this, SLOT (handleConfigButton()));
    connect(configPath, SIGNAL (editingFinished()),this, SLOT (handleConfigEdit()));
    QPushButton *clearConfigButton = new QPushButton("Clear");
    connect(clearConfigButton, SIGNAL (released()),this, SLOT (handleClearConfigButton()));
    configPath->setStyleSheet("border: 1px solid; padding: 10px");
    layout->addWidget(note,2,0,1,-1);
    layout->addWidget(configLabel,3,0);
    layout->addWidget(configPath,3,1);
    layout->addWidget(configButton,3,2);
    layout->addWidget(clearConfigButton,3,3);

    QDir *PluginDir = new QDir(qtPluginDir);
    QStringList Filter;
    Filter.append("");
    QStringList current;

    QLabel *audioLabel = new QLabel("Audio Plugin");
    layout->addWidget(audioLabel,5,0);
    QComboBox *audioChoice = new QComboBox();
    Filter.replace(0,"mupen64plus-audio*");
    current = PluginDir->entryList(Filter);
    audioChoice->addItems(current);
    int my_index = audioChoice->findText(qtAudioPlugin);
    if (my_index == -1) {
        audioChoice->addItem(qtAudioPlugin);
        my_index = audioChoice->findText(qtAudioPlugin);
    }
    audioChoice->setCurrentIndex(my_index);
    connect(audioChoice, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated),
        [=](const QString &text) {
            settings->setValue("audioPlugin", text);
            qtAudioPlugin = settings->value("audioPlugin").toString();
    });
    layout->addWidget(audioChoice,5,1);

    QLabel *inputLabel = new QLabel("Input Plugin");
    layout->addWidget(inputLabel,7,0);
    QComboBox *inputChoice = new QComboBox();
    Filter.replace(0,"mupen64plus-input*");
    current = PluginDir->entryList(Filter);
    inputChoice->addItems(current);
    my_index = inputChoice->findText(qtInputPlugin);
    if (my_index == -1) {
        inputChoice->addItem(qtInputPlugin);
        my_index = inputChoice->findText(qtInputPlugin);
    }
    inputChoice->setCurrentIndex(my_index);
    connect(inputChoice, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated),
        [=](const QString &text) {
            settings->setValue("inputPlugin", text);
            qtInputPlugin = settings->value("inputPlugin").toString();
    });
    layout->addWidget(inputChoice,7,1);

    setLayout(layout);
}

SettingsDialog::SettingsDialog()
{
    initStuff();
}
