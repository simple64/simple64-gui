#include "settingsdialog.h"
#include "core_interface.h"
#include "plugin.h"

#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QComboBox>

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
    QHBoxLayout *rowCorePath = new QHBoxLayout;
    rowCorePath->setSpacing(50);
    QHBoxLayout *rowPluginPath = new QHBoxLayout;
    rowPluginPath->setSpacing(50);
    QHBoxLayout *rowVideoPath = new QHBoxLayout;
    rowVideoPath->setSpacing(50);
    QHBoxLayout *rowAudioPath = new QHBoxLayout;
    rowAudioPath->setSpacing(50);
    QHBoxLayout *rowRSPPath = new QHBoxLayout;
    rowRSPPath->setSpacing(50);

    QLabel *coreLabel = new QLabel("Core Library Path");
    corePath = new QLabel;
    corePath->setText(settings.value("coreLibPath").toString());
    QPushButton *coreButton = new QPushButton("Set Path");
    connect(coreButton, SIGNAL (released()),this, SLOT (handleCoreButton()));
    corePath->setStyleSheet("border: 1px solid black; background: white");
    rowCorePath->addWidget(coreLabel);
    rowCorePath->addWidget(corePath);
    rowCorePath->addWidget(coreButton);

    QLabel *pluginLabel = new QLabel("Plugin Dir Path");
    pluginPath = new QLabel;
    pluginPath->setText(settings.value("pluginDirPath").toString());
    QPushButton *pluginButton = new QPushButton("Set Path");
    connect(pluginButton, SIGNAL (released()),this, SLOT (handlePluginButton()));
    pluginPath->setStyleSheet("border: 1px solid black; background: white");
    rowPluginPath->addWidget(pluginLabel);
    rowPluginPath->addWidget(pluginPath);
    rowPluginPath->addWidget(pluginButton);

    QDir *PluginDir = new QDir(qtPluginDir);
    QStringList Filter;
    Filter.append("");
    QStringList current;

    QLabel *videoLabel = new QLabel("Video Plugin");
    rowVideoPath->addWidget(videoLabel);
    QComboBox *videoChoice = new QComboBox();
    Filter.replace(0,"mupen64plus-video*");
    current = PluginDir->entryList(Filter);
    videoChoice->addItems(current);
    int my_index = videoChoice->findText(qtGfxPlugin);
    if (my_index != -1) {
        videoChoice->setCurrentIndex(my_index);
    }
    connect(videoChoice, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated),
        [=](const QString &text) {
            QSettings temp_settings("mupen64plus", "gui");
            temp_settings.setValue("videoPlugin", text);
            qtGfxPlugin = temp_settings.value("videoPlugin").toString();
    });
    rowVideoPath->addWidget(videoChoice);

    QLabel *audioLabel = new QLabel("Audio Plugin");
    rowAudioPath->addWidget(audioLabel);
    QComboBox *audioChoice = new QComboBox();
    Filter.replace(0,"mupen64plus-audio*");
    current = PluginDir->entryList(Filter);
    audioChoice->addItems(current);
    my_index = audioChoice->findText(qtAudioPlugin);
    if (my_index != -1) {
        audioChoice->setCurrentIndex(my_index);
    }
    connect(audioChoice, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated),
        [=](const QString &text) {
            QSettings temp_settings("mupen64plus", "gui");
            temp_settings.setValue("audioPlugin", text);
            qtAudioPlugin = temp_settings.value("audioPlugin").toString();
    });
    rowAudioPath->addWidget(audioChoice);

    QLabel *rspLabel = new QLabel("RSP Plugin");
    rowRSPPath->addWidget(rspLabel);
    QComboBox *rspChoice = new QComboBox();
    Filter.replace(0,"mupen64plus-rsp*");
    current = PluginDir->entryList(Filter);
    rspChoice->addItems(current);
    my_index = rspChoice->findText(qtRspPlugin);
    if (my_index != -1) {
        rspChoice->setCurrentIndex(my_index);
    }
    connect(rspChoice, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated),
        [=](const QString &text) {
            QSettings temp_settings("mupen64plus", "gui");
            temp_settings.setValue("rspPlugin", text);
            qtRspPlugin = temp_settings.value("rspPlugin").toString();
    });
    rowRSPPath->addWidget(rspChoice);

    layout->addLayout(rowCorePath);
    layout->addLayout(rowPluginPath);
    layout->addLayout(rowVideoPath);
    layout->addLayout(rowAudioPath);
    layout->addLayout(rowRSPPath);

    setLayout(layout);
}

SettingsDialog::~SettingsDialog()
{
}
