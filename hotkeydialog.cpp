#include "hotkeydialog.h"
#include "mainwindow.h"
#include "interface/core_commands.h"
#include "interface/sdl_key_converter.h"

#include <QLabel>
#include <QScrollArea>
#include <QSettings>
#include <QTabWidget>
#include <QGridLayout>
#include <QValidator>
#include <QMessageBox>
#include "settingclasses.h"

m64p_handle coreEventsConfigHandle;
QGridLayout *coreEventsLayout;
int coreEventsLayoutRow;

static void paramListCallback(void *context, const char *ParamName, m64p_type ParamType)
{
    if (strcmp((char*)ParamName, "Version") == 0) return;
    else if (strncmp((char*)ParamName, "Joy Mapping", 11) == 0) return;

    int l_ParamInt;
    bool l_ParamBool;
    float l_ParamFloat;
    QString l_ParamString;
    QString helper = (*ConfigGetParameterHelp)(coreEventsConfigHandle, ParamName);
    QLabel *desc = new QLabel(ParamName, coreEventsLayout->parentWidget());
    if (!helper.isEmpty()) {
       helper.prepend("<span style=\"color:black;\">");
       helper.append("</span>");
       desc->setToolTip(helper);
    }
    desc->setStyleSheet("border: 1px solid; padding: 10px");
    coreEventsLayout->addWidget(desc, coreEventsLayoutRow, 0);
    void *my_Widget = new CustomButton(coreEventsLayout->parentWidget());
    ((CustomButton*)my_Widget)->setConfigHandle(coreEventsConfigHandle);
    ((CustomButton*)my_Widget)->setParamType(ParamType);
    ((CustomButton*)my_Widget)->setParamName(ParamName);
    l_ParamInt = (*ConfigGetParamInt)(coreEventsConfigHandle, ParamName);
    ((CustomButton*)my_Widget)->setText(QKeySequence(SDL22QT(sdl_keysym2scancode(l_ParamInt))).toString());

    coreEventsLayout->addWidget((QWidget*)my_Widget, coreEventsLayoutRow, 1);
    ++coreEventsLayoutRow;
}

void HotkeyDialog::handleResetButton()
{
    int value;
    (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &value);
    if (value == M64EMU_STOPPED) {
        (*ConfigDeleteSection)("CoreEvents");
        (*ConfigSaveFile)();
        w->resetCore();
        this->close();
    }
    else {
        QMessageBox msgBox;
        msgBox.setText("Emulator must be stopped.");
        msgBox.exec();
    }
}

HotkeyDialog::HotkeyDialog(QWidget *parent)
    : QDialog(parent)
{
    m64p_error res;

    coreEventsLayoutRow = 0;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *coreEventsSettings = new QWidget(this);
    coreEventsLayout = new QGridLayout(coreEventsSettings);
    coreEventsSettings->setLayout(coreEventsLayout);
    res = (*ConfigOpenSection)("CoreEvents", &coreEventsConfigHandle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(coreEventsConfigHandle, (char*)"CoreEvents", paramListCallback);
    QScrollArea *coreEventsScroll = new QScrollArea(this);
    coreEventsScroll->setWidget(coreEventsSettings);
    coreEventsScroll->setMinimumWidth(coreEventsSettings->sizeHint().width() + 20);
    coreEventsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QLabel *myLabel = new QLabel("Hover your mouse over the configuration item name for a description.\n", this);
    myLabel->setStyleSheet("font-weight: bold");
    mainLayout->addWidget(myLabel);
    mainLayout->addWidget(coreEventsScroll);
    QPushButton *resetButton = new QPushButton("Reset All Settings", this);
    resetButton->setAutoDefault(false);
    connect(resetButton, SIGNAL (released()),this, SLOT (handleResetButton()));
    mainLayout->addWidget(resetButton);
    setLayout(mainLayout);
}
