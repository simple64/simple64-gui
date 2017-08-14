#include "plugindialog.h"
#include "core_interface.h"
#include "osal_preproc.h"
#include "plugin.h"

#include <QLabel>
#include <QScrollArea>
#include <QSettings>
#include <QTabWidget>
#include <QGridLayout>
#include <QValidator>
#include <QMessageBox>
#include "settingclasses.h"

m64p_handle coreConfigHandle;
m64p_handle videoGenConfigHandle;
m64p_handle audioConfigHandle;
m64p_handle rspConfigHandle;
m64p_handle videoConfigHandle;
QGridLayout *coreLayout;
int coreLayoutRow;
QGridLayout *videoGenLayout;
int videoGenRow;
QGridLayout *audioLayout;
int audioRow;
QGridLayout *rspLayout;
int rspRow;
QGridLayout *videoLayout;
int videoRow;

void paramListCallback(void * context, const char *ParamName, m64p_type ParamType)
{
    QGridLayout *my_layout = nullptr;
    int * my_row = nullptr;
    m64p_handle current_handle = nullptr;
    if (strcmp((char*)context, "Core") == 0) {
        my_layout = coreLayout;
        my_row = &coreLayoutRow;
        current_handle = coreConfigHandle;
    } else if (strcmp((char*)context, "Video-General") == 0) {
        my_layout = videoGenLayout;
        my_row = &videoGenRow;
        current_handle = videoGenConfigHandle;
    } else if (strcmp((char*)context, "Audio") == 0) {
        my_layout = audioLayout;
        my_row = &audioRow;
        current_handle = audioConfigHandle;
    } else if (strcmp((char*)context, "Video") == 0) {
        my_layout = videoLayout;
        my_row = &videoRow;
        current_handle = videoConfigHandle;
    } else if (strcmp((char*)context, "RSP") == 0) {
        my_layout = rspLayout;
        my_row = &rspRow;
        current_handle = rspConfigHandle;
    }
    int l_ParamInt;
    bool l_ParamBool;
    float l_ParamFloat;
    QString l_ParamString;
    QString helper = (*ConfigGetParameterHelp)(current_handle, ParamName);
    QLabel *desc = new QLabel(ParamName);
    if (!helper.isEmpty()) {
       helper.prepend("<span style=\"color:black;\">");
       helper.append("</span>");
       desc->setToolTip(helper);
    }
    desc->setStyleSheet("border: 1px solid; padding: 10px");
    my_layout->addWidget(desc, *my_row, 0);
    void *my_Widget = nullptr;
    switch (ParamType) {
    case M64TYPE_INT:
        my_Widget = new CustomLineEdit;
        ((CustomLineEdit*)my_Widget)->setConfigHandle(current_handle);
        ((CustomLineEdit*)my_Widget)->setParamType(ParamType);
        ((CustomLineEdit*)my_Widget)->setParamName(ParamName);
        ((CustomLineEdit*)my_Widget)->setStyleSheet("border: 1px solid; padding: 10px");
        l_ParamInt = (*ConfigGetParamInt)(current_handle, ParamName);
        ((CustomLineEdit*)my_Widget)->setText(QString::number(l_ParamInt));
        ((CustomLineEdit*)my_Widget)->setValidator(new QIntValidator());
        break;
    case M64TYPE_FLOAT:
        my_Widget = new CustomLineEdit;
        ((CustomLineEdit*)my_Widget)->setConfigHandle(current_handle);
        ((CustomLineEdit*)my_Widget)->setParamType(ParamType);
        ((CustomLineEdit*)my_Widget)->setParamName(ParamName);
        ((CustomLineEdit*)my_Widget)->setStyleSheet("border: 1px solid; padding: 10px");
        l_ParamFloat = (*ConfigGetParamFloat)(current_handle, ParamName);
        ((CustomLineEdit*)my_Widget)->setText(QString::number(l_ParamFloat));
        ((CustomLineEdit*)my_Widget)->setValidator(new QDoubleValidator());
        break;
    case M64TYPE_BOOL:
        my_Widget = new CustomCheckBox;
        ((CustomCheckBox*)my_Widget)->setConfigHandle(current_handle);
        ((CustomCheckBox*)my_Widget)->setParamType(ParamType);
        ((CustomCheckBox*)my_Widget)->setParamName(ParamName);
        ((CustomCheckBox*)my_Widget)->setStyleSheet("border: 1px solid; padding: 10px");
        l_ParamBool = (*ConfigGetParamBool)(current_handle, ParamName);
        ((CustomCheckBox*)my_Widget)->setCheckState(l_ParamBool ? Qt::Checked : Qt::Unchecked);
        break;
    case M64TYPE_STRING:
        my_Widget = new CustomLineEdit;
        ((CustomLineEdit*)my_Widget)->setConfigHandle(current_handle);
        ((CustomLineEdit*)my_Widget)->setParamType(ParamType);
        ((CustomLineEdit*)my_Widget)->setParamName(ParamName);
        ((CustomLineEdit*)my_Widget)->setStyleSheet("border: 1px solid; padding: 10px");
        l_ParamString = (*ConfigGetParamString)(current_handle, ParamName);
        ((CustomLineEdit*)my_Widget)->setText(l_ParamString);
        break;
    }

    my_layout->addWidget((QWidget*)my_Widget, *my_row, 1);
    ++*my_row;
}

void PluginDialog::handleResetButton()
{
    int value;
    (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &value);
    if (value == M64EMU_STOPPED) {
        (*ConfigDeleteSection)("Core");
        (*ConfigDeleteSection)("Video-General");
        (*ConfigDeleteSection)(RSPName.toLatin1().data());
        (*ConfigDeleteSection)(AudioName.toLatin1().data());
        (*ConfigDeleteSection)(VideoName.toLatin1().data());
        (*ConfigSaveFile)();
        (*CoreShutdown)();
        (*DetachCoreLib)();
        this->close();
    }
    else {
        QMessageBox msgBox;
        msgBox.setText("Emulator must be stopped.");
        msgBox.exec();
    }
}

PluginDialog::PluginDialog()
{
    m64p_error res;
    int value;
    (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &value);
    if (value == M64EMU_STOPPED) {
        PluginSearchLoad();
        PluginUnload();
    }

    coreLayoutRow = 0;
    videoGenRow = 0;
    audioRow = 0;
    videoRow = 0;
    QSettings settings("mupen64plus", "gui");
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->setUsesScrollButtons(false);

    QWidget *coreSettings = new QWidget;
    coreLayout = new QGridLayout;
    coreSettings->setLayout(coreLayout);
    res = (*ConfigOpenSection)("Core", &coreConfigHandle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(coreConfigHandle, (char*)"Core", paramListCallback);
    QScrollArea *coreScroll = new QScrollArea;
    coreScroll->setWidget(coreSettings);
    coreScroll->setMinimumWidth(coreSettings->sizeHint().width() + 20);
    coreScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(coreScroll, tr("Core"));

    QWidget *videoGenSettings = new QWidget;
    videoGenLayout = new QGridLayout;
    videoGenSettings->setLayout(videoGenLayout);
    res = (*ConfigOpenSection)("Video-General", &videoGenConfigHandle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(videoGenConfigHandle, (char*)"Video-General", paramListCallback);
    QScrollArea *videoGenScroll = new QScrollArea;
    videoGenScroll->setWidget(videoGenSettings);
    videoGenScroll->setMinimumWidth(videoGenSettings->sizeHint().width() + 20);
    videoGenScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(videoGenScroll, tr("Video-General"));

    QWidget *rspSettings = new QWidget;
    rspLayout = new QGridLayout;
    rspSettings->setLayout(rspLayout);
    QString name = settings.value("rspPlugin").toString();
    name.remove(OSAL_DLL_EXTENSION);
    QStringList name2 = name.split("-");
    if (name2.size() > 2)
        RSPName = name2.at(1) + "-" + name2.at(2);
    res = (*ConfigOpenSection)(RSPName.toLatin1().data(), &rspConfigHandle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(rspConfigHandle, (char*)"RSP", paramListCallback);
    QScrollArea *rspScroll = new QScrollArea;
    rspScroll->setWidget(rspSettings);
    rspScroll->setMinimumWidth(rspSettings->sizeHint().width() + 20);
    rspScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(rspScroll, tr("RSP Plugin"));

    QWidget *audioSettings = new QWidget;
    audioLayout = new QGridLayout;
    audioSettings->setLayout(audioLayout);
    name = settings.value("audioPlugin").toString();
    name.remove(OSAL_DLL_EXTENSION);
    name2 = name.split("-");
    if (name2.size() > 2)
        AudioName = name2.at(1) + "-" + name2.at(2);
    res = (*ConfigOpenSection)(AudioName.toLatin1().data(), &audioConfigHandle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(audioConfigHandle, (char*)"Audio", paramListCallback);
    QScrollArea *audioScroll = new QScrollArea;
    audioScroll->setWidget(audioSettings);
    audioScroll->setMinimumWidth(audioSettings->sizeHint().width() + 20);
    audioScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(audioScroll, tr("Audio Plugin"));

    QWidget *videoSettings = new QWidget;
    videoLayout = new QGridLayout;
    videoSettings->setLayout(videoLayout);
    name = settings.value("videoPlugin").toString();
    name.remove(OSAL_DLL_EXTENSION);
    name2 = name.split("-");
    if (name2.size() > 2)
        VideoName = name2.at(1) + "-" + name2.at(2);
    res = (*ConfigOpenSection)(VideoName.toLatin1().data(), &videoConfigHandle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(videoConfigHandle, (char*)"Video", paramListCallback);
    QScrollArea *videoScroll = new QScrollArea;
    videoScroll->setWidget(videoSettings);
    videoScroll->setMinimumWidth(videoSettings->sizeHint().width() + 20);
    videoScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(videoScroll, tr("Video Plugin"));

    mainLayout->addWidget(tabWidget);
    QPushButton *resetButton = new QPushButton("Reset All Settings");
    connect(resetButton, SIGNAL (released()),this, SLOT (handleResetButton()));
    mainLayout->addWidget(resetButton);
    setLayout(mainLayout);
}
