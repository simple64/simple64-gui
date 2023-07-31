#include "hotkeydialog.h"
#include "mainwindow.h"
#include "interface/core_commands.h"
#include "interface/sdl_key_converter.h"

#include <QLabel>
#include <QScrollArea>
#include <QSettings>
#include <QMessageBox>

static void paramListCallback(void *context, const char *ParamName, m64p_type ParamType)
{
    HotkeyDialog* dialog = (HotkeyDialog*)context;

    if (strcmp((char*)ParamName, "Version") == 0) return;
    else if (strncmp((char*)ParamName, "Joy Mapping", 11) == 0) return;

    int l_ParamInt;
    bool l_ParamBool;
    float l_ParamFloat;
    QString l_ParamString;
    QString helper = (*ConfigGetParameterHelp)(dialog->getHandle(), ParamName);
    QLabel *desc = new QLabel(ParamName, dialog->getLayout()->parentWidget());
    if (!helper.isEmpty()) {
       helper.prepend("<span style=\"color:black;\">");
       helper.append("</span>");
       desc->setToolTip(helper);
    }
    desc->setStyleSheet("border: 1px solid; padding: 10px");
    dialog->getLayout()->addWidget(desc, *dialog->getLayoutRow(), 0);
    CustomButton *my_Widget = new CustomButton(dialog->getLayout()->parentWidget());
    dialog->getButtonList()->append(my_Widget);
    my_Widget->setConfigHandle(dialog->getHandle());
    my_Widget->setParamType(ParamType);
    my_Widget->setParamName(ParamName);
    l_ParamInt = (*ConfigGetParamInt)(dialog->getHandle(), ParamName);
    my_Widget->setText(QKeySequence(SDL22QT(sdl_keysym2scancode(l_ParamInt))).toString());

    dialog->getLayout()->addWidget((QWidget*)my_Widget, *dialog->getLayoutRow(), 1);

    ClearButton* clear_Widget = new ClearButton(dialog->getLayout()->parentWidget());
    clear_Widget->setConfigHandle(dialog->getHandle());
    clear_Widget->setParamType(ParamType);
    clear_Widget->setParamName(ParamName);
    clear_Widget->setMainButton(my_Widget);
    l_ParamInt = (*ConfigGetParamInt)(dialog->getHandle(), ParamName);
    clear_Widget->setText("Clear");

    dialog->getLayout()->addWidget((QWidget*)clear_Widget, *dialog->getLayoutRow(), 2);
    ++*dialog->getLayoutRow();
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

    m_layoutRow = 0;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *coreEventsSettings = new QWidget(this);
    m_layout = new QGridLayout(coreEventsSettings);
    coreEventsSettings->setLayout(m_layout);
    res = (*ConfigOpenSection)("CoreEvents", &m_configHandle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(m_configHandle, this, paramListCallback);
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

QList<CustomButton*>* HotkeyDialog::getButtonList()
{
    return &m_coreEventsButtonList;
}

m64p_handle HotkeyDialog::getHandle()
{
    return m_configHandle;
}

QGridLayout* HotkeyDialog::getLayout()
{
    return m_layout;
}

int* HotkeyDialog::getLayoutRow()
{
    return &m_layoutRow;
}
