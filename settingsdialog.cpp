#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "core_interface.h"
#include "plugin.h"

#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <QFileDialog>

void SettingsDialog::handleCoreButton()
{
    QSettings settings("mupen64plus", "gui");
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Locate Core Library"), NULL, tr("Shares Libraries (*.dynlib *.so* *.dll)"));
    if (!fileName.isNull()) {
        corePath->setText(fileName);
        settings.setValue("coreLibPath", fileName);
        qtCoreDirPath = settings.value("coreLibPath").toString();
    }
}

void SettingsDialog::handlePluginButton()
{
    QSettings settings("mupen64plus", "gui");
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Locate Plugin Directory"),
                                                         NULL,
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
    if (!fileName.isNull()) {
        pluginPath->setText(fileName);
        settings.setValue("pluginDirPath", fileName);
        qtPluginDir = settings.value("pluginDirPath").toString();
    }
}

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    QSettings settings("mupen64plus", "gui");

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *row1 = new QHBoxLayout;
    row1->setSpacing(50);
    QHBoxLayout *row2 = new QHBoxLayout;
    row2->setSpacing(50);

    QLabel *coreLabel = new QLabel("Core Library Path");
    corePath = new QLabel;
    corePath->setText(settings.value("coreLibPath").toString());
    QPushButton *coreButton = new QPushButton("Set Path");
    connect(coreButton, SIGNAL (released()),this, SLOT (handleCoreButton()));
    corePath->setStyleSheet("border: 1px solid black; background: white");
    row1->addWidget(coreLabel);
    row1->addWidget(corePath);
    row1->addWidget(coreButton);
    QLabel *pluginLabel = new QLabel("Plugin Dir Path");
    pluginPath = new QLabel;
    pluginPath->setText(settings.value("pluginDirPath").toString());
    QPushButton *pluginButton = new QPushButton("Set Path");
    connect(pluginButton, SIGNAL (released()),this, SLOT (handlePluginButton()));
    pluginPath->setStyleSheet("border: 1px solid black; background: white");
    row2->addWidget(pluginLabel);
    row2->addWidget(pluginPath);
    row2->addWidget(pluginButton);
    layout->addLayout(row1);
    layout->addLayout(row2);

    setLayout(layout);
}

SettingsDialog::~SettingsDialog()
{
}
