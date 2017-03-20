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
#include "settingclasses.h"

m64p_handle coreConfigHandle;
m64p_handle videoGenConfigHandle;
m64p_handle audioConfigHandle;
m64p_handle videoConfigHandle;
QGridLayout *coreLayout;
int coreLayoutRow;
QGridLayout *videoGenLayout;
int videoGenRow;
QGridLayout *audioLayout;
int audioRow;
QGridLayout *videoLayout;
int videoRow;

void paramListCallback(void * context, const char *ParamName, m64p_type ParamType)
{
    QGridLayout *my_layout;
    int * my_row;
    m64p_handle current_handle;
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
    void *my_Widget;
    switch (ParamType) {
    case M64TYPE_INT:
        my_Widget = new CustomLineEdit;
        l_ParamInt = (*ConfigGetParamInt)(current_handle, ParamName);
        ((CustomLineEdit*)my_Widget)->setText(QString::number(l_ParamInt));
        ((CustomLineEdit*)my_Widget)->setValidator(new QIntValidator());
        break;
    case M64TYPE_FLOAT:
        my_Widget = new CustomLineEdit;
        l_ParamFloat = (*ConfigGetParamFloat)(current_handle, ParamName);
        ((CustomLineEdit*)my_Widget)->setText(QString::number(l_ParamFloat));
        ((CustomLineEdit*)my_Widget)->setValidator(new QDoubleValidator());
        break;
    case M64TYPE_BOOL:
        my_Widget = new CustomCheckBox;
        l_ParamBool = (*ConfigGetParamBool)(current_handle, ParamName);
        ((CustomCheckBox*)my_Widget)->setCheckState(l_ParamBool ? Qt::Checked : Qt::Unchecked);
        break;
    case M64TYPE_STRING:
        my_Widget = new CustomLineEdit;
        l_ParamString = (*ConfigGetParamString)(current_handle, ParamName);
        ((CustomLineEdit*)my_Widget)->setText(l_ParamString);
        break;
    }
    if (ParamType == M64TYPE_BOOL) {
        ((CustomCheckBox*)my_Widget)->setConfigHandle(current_handle);
        ((CustomCheckBox*)my_Widget)->setParamType(ParamType);
        ((CustomCheckBox*)my_Widget)->setParamName(ParamName);
        ((CustomCheckBox*)my_Widget)->setStyleSheet("border: 1px solid; padding: 10px");
    } else {
        ((CustomLineEdit*)my_Widget)->setConfigHandle(current_handle);
        ((CustomLineEdit*)my_Widget)->setParamType(ParamType);
        ((CustomLineEdit*)my_Widget)->setParamName(ParamName);
        ((CustomLineEdit*)my_Widget)->setStyleSheet("border: 1px solid; padding: 10px");
    }
    my_layout->addWidget((QWidget*)my_Widget, *my_row, 1);
    ++*my_row;
}

PluginDialog::PluginDialog()
{
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
    (*ConfigOpenSection)("Core", &coreConfigHandle);
    (*ConfigListParameters)(coreConfigHandle, (char*)"Core", paramListCallback);
    QScrollArea *coreScroll = new QScrollArea;
    coreScroll->setWidget(coreSettings);
    coreScroll->setMinimumWidth(coreSettings->sizeHint().width() + 20);
    coreScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(coreScroll, tr("Core"));

    QWidget *videoGenSettings = new QWidget;
    videoGenLayout = new QGridLayout;
    videoGenSettings->setLayout(videoGenLayout);
    (*ConfigOpenSection)("Video-General", &videoGenConfigHandle);
    (*ConfigListParameters)(videoGenConfigHandle, (char*)"Video-General", paramListCallback);
    QScrollArea *videoGenScroll = new QScrollArea;
    videoGenScroll->setWidget(videoGenSettings);
    videoGenScroll->setMinimumWidth(videoGenSettings->sizeHint().width() + 20);
    videoGenScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(videoGenScroll, tr("Video-General"));

    QWidget *audioSettings = new QWidget;
    audioLayout = new QGridLayout;
    audioSettings->setLayout(audioLayout);
    QString name = settings.value("audioPlugin").toString();
    name.remove("mupen64plus-");
    name.remove(OSAL_DLL_EXTENSION);
    (*ConfigOpenSection)(name.toLatin1().data(), &audioConfigHandle);
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
    name.remove("mupen64plus-");
    name.remove(OSAL_DLL_EXTENSION);
    (*ConfigOpenSection)(name.toLatin1().data(), &videoConfigHandle);
    (*ConfigListParameters)(videoConfigHandle, (char*)"Video", paramListCallback);
    QScrollArea *videoScroll = new QScrollArea;
    videoScroll->setWidget(videoSettings);
    videoScroll->setMinimumWidth(videoSettings->sizeHint().width() + 20);
    videoScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(videoScroll, tr("Video Plugin"));

    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
}
