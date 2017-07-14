#include "settingsdialog.h"
#include "core_interface.h"
#include "plugin.h"

#include <QPushButton>
#include <QSettings>
#include <QGridLayout>
#include <QFileDialog>
#include <QComboBox>
#include <QMessageBox>

void SettingsDialog::handleCoreButton()
{
    QSettings settings("mupen64plus", "gui");
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Locate Core Library"), NULL, tr("Shared Libraries (*.dylib *.so* *.dll)"));
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

SettingsDialog::SettingsDialog()
{
    QSettings settings("mupen64plus", "gui");

    QGridLayout *layout = new QGridLayout;

    QLabel *coreLabel = new QLabel("Core Library Path");
    corePath = new QLabel;
    corePath->setText(settings.value("coreLibPath").toString());
    QPushButton *coreButton = new QPushButton("Set Path");
    connect(coreButton, SIGNAL (released()),this, SLOT (handleCoreButton()));
    corePath->setStyleSheet("border: 1px solid black; background: white");
    layout->addWidget(coreLabel,0,0);
    layout->addWidget(corePath,0,1);
    layout->addWidget(coreButton,0,2);

    QLabel *pluginLabel = new QLabel("Plugin Dir Path");
    pluginPath = new QLabel;
    pluginPath->setText(settings.value("pluginDirPath").toString());
    QPushButton *pluginButton = new QPushButton("Set Path");
    connect(pluginButton, SIGNAL (released()),this, SLOT (handlePluginButton()));
    pluginPath->setStyleSheet("border: 1px solid black; background: white");
    layout->addWidget(pluginLabel,1,0);
    layout->addWidget(pluginPath,1,1);
    layout->addWidget(pluginButton,1,2);

    QDir *PluginDir = new QDir(qtPluginDir);
    QStringList Filter;
    Filter.append("");
    QStringList current;

    QLabel *videoLabel = new QLabel("Video Plugin");
    layout->addWidget(videoLabel,2,0);
    QComboBox *videoChoice = new QComboBox();
    Filter.replace(0,"mupen64plus-video*");
    current = PluginDir->entryList(Filter);
    videoChoice->addItems(current);
    videoChoice->addItem("dummy");
    int my_index = videoChoice->findText(qtGfxPlugin);
    if (my_index != -1) {
        videoChoice->setCurrentIndex(my_index);
    } else {
        settings.setValue("videoPlugin", videoChoice->currentText());
        qtGfxPlugin = settings.value("videoPlugin").toString();
    }
    connect(videoChoice, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated),
        [=](const QString &text) {
            QSettings temp_settings("mupen64plus", "gui");
            temp_settings.setValue("videoPlugin", text);
            qtGfxPlugin = temp_settings.value("videoPlugin").toString();

    });
    layout->addWidget(videoChoice,2,1);

    QLabel *audioLabel = new QLabel("Audio Plugin");
    layout->addWidget(audioLabel,3,0);
    QComboBox *audioChoice = new QComboBox();
    Filter.replace(0,"mupen64plus-audio*");
    current = PluginDir->entryList(Filter);
    audioChoice->addItems(current);
    audioChoice->addItem("dummy");
    my_index = audioChoice->findText(qtAudioPlugin);
    if (my_index != -1) {
        audioChoice->setCurrentIndex(my_index);
    } else {
        settings.setValue("audioPlugin", audioChoice->currentText());
        qtAudioPlugin = settings.value("audioPlugin").toString();
    }
    connect(audioChoice, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated),
        [=](const QString &text) {
            QSettings temp_settings("mupen64plus", "gui");
            temp_settings.setValue("audioPlugin", text);
            qtAudioPlugin = temp_settings.value("audioPlugin").toString();
    });
    layout->addWidget(audioChoice,3,1);

    QLabel *rspLabel = new QLabel("RSP Plugin");
    layout->addWidget(rspLabel,4,0);
    QComboBox *rspChoice = new QComboBox();
    Filter.replace(0,"mupen64plus-rsp*");
    current = PluginDir->entryList(Filter);
    rspChoice->addItems(current);
    rspChoice->addItem("dummy");
    my_index = rspChoice->findText(qtRspPlugin);
    if (my_index != -1) {
        rspChoice->setCurrentIndex(my_index);
    } else {
        settings.setValue("rspPlugin", rspChoice->currentText());
        qtRspPlugin = settings.value("rspPlugin").toString();
    }
    connect(rspChoice, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated),
        [=](const QString &text) {
            QSettings temp_settings("mupen64plus", "gui");
            temp_settings.setValue("rspPlugin", text);
            qtRspPlugin = temp_settings.value("rspPlugin").toString();
    });
    layout->addWidget(rspChoice,4,1);

    setLayout(layout);
}
