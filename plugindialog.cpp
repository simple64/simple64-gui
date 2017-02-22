#include "plugindialog.h"
#include "core_interface.h"
#include "osal_preproc.h"

#include <QLabel>
#include <QScrollArea>
#include <QSettings>
#include <QTabWidget>
#include <QGridLayout>
#include "customlineedit.h"

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
    QString my_context = (char*)context;
    QGridLayout *my_layout;
    int * my_row;
    m64p_handle current_handle;
    if (my_context == "Core") {
        my_layout = coreLayout;
        my_row = &coreLayoutRow;
        current_handle = coreConfigHandle;
    } else if (my_context == "Video-General") {
        my_layout = videoGenLayout;
        my_row = &videoGenRow;
        current_handle = videoGenConfigHandle;
    } else if (my_context == "Audio") {
        my_layout = audioLayout;
        my_row = &audioRow;
        current_handle = audioConfigHandle;
    } else if (my_context == "Video") {
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
    helper.prepend("<span style=\"color:black;\">");
    helper.append("</span>");
    desc->setToolTip(helper);
    desc->setStyleSheet("border: 1px solid; padding: 10px");
    my_layout->addWidget(desc, *my_row, 0);
    CustomLineEdit *my_value = new CustomLineEdit;
    switch (ParamType) {
    case M64TYPE_INT:
        l_ParamInt = (*ConfigGetParamInt)(current_handle, ParamName);
        my_value->setText(QString::number(l_ParamInt));
        break;
    case M64TYPE_FLOAT:
        l_ParamFloat = (*ConfigGetParamFloat)(current_handle, ParamName);
        my_value->setText(QString::number(l_ParamFloat));
        break;
    case M64TYPE_BOOL:
        l_ParamBool = (*ConfigGetParamBool)(current_handle, ParamName);
        my_value->setText(QString::number(l_ParamBool));
        break;
    case M64TYPE_STRING:
        l_ParamString = (*ConfigGetParamString)(current_handle, ParamName);
        my_value->setText(l_ParamString);
        break;
    }
    my_value->setConfigHandle(current_handle);
    my_value->setParamType(ParamType);
    my_value->setParamName(ParamName);
    my_value->setStyleSheet("border: 1px solid; padding: 10px");
    my_layout->addWidget(my_value, *my_row, 1);
    ++*my_row;
}

PluginDialog::PluginDialog(QWidget *parent) :
    QDialog(parent)
{
    if (!QtAttachCoreLib())
        return;
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
    tabWidget->addTab(coreScroll, tr("Core"));

    QWidget *videoGenSettings = new QWidget;
    videoGenLayout = new QGridLayout;
    videoGenSettings->setLayout(videoGenLayout);
    (*ConfigOpenSection)("Video-General", &videoGenConfigHandle);
    (*ConfigListParameters)(videoGenConfigHandle, (char*)"Video-General", paramListCallback);
    QScrollArea *videoGenScroll = new QScrollArea;
    videoGenScroll->setWidget(videoGenSettings);
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
    tabWidget->addTab(videoScroll, tr("Video Plugin"));

    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
}
