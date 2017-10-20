#include "controllerdialog.h"
#include "core_interface.h"
#include "plugin.h"
#include "sdl_key_converter.h"
#include "common.h"

#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>
#include <SDL.h>

m64p_handle p1Handle;
m64p_handle p2Handle;
m64p_handle p3Handle;
m64p_handle p4Handle;
QGridLayout* p1Layout;
QGridLayout* p2Layout;
QGridLayout* p3Layout;
QGridLayout* p4Layout;
QStringList joyNames;
int p1Row;
int p2Row;
int p3Row;
int p4Row;
bool needBindAllButton;
CustomPushButton *last;
bool p1Auto;
bool p2Auto;
bool p3Auto;
bool p4Auto;

void initSDL()
{
    if (!SDL_WasInit(SDL_INIT_JOYSTICK)) {
        if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == -1)
        {
            DebugMessage(M64MSG_ERROR, "Couldn't init SDL joystick subsystem: %s", SDL_GetError() );
        }
    }
}

void deleteItem(QGridLayout* my_layout, int row, int column)
{
    QLayoutItem *deleteItem = my_layout->itemAtPosition(row, column);
    if (deleteItem != 0) {
        QWidget *deleteWidget = deleteItem->widget();
        if (deleteWidget != 0)
            delete deleteWidget;
    }
}

void controllerListCallback(void * context, const char *ParamName, m64p_type ParamType)
{
    if (strcmp(ParamName, "version") == 0 || strcmp(ParamName, "name") == 0)
        return;

    int *myRow = nullptr;
    bool *pAuto = nullptr;
    QGridLayout *my_layout = nullptr;
    m64p_handle current_handle = nullptr;
    if (strcmp((char*)context, "Input-SDL-Control1") == 0) {
        current_handle = p1Handle;
        my_layout = p1Layout;
        myRow = &p1Row;
        pAuto = &p1Auto;
    } else if (strcmp((char*)context, "Input-SDL-Control2") == 0) {
        current_handle = p2Handle;
        my_layout = p2Layout;
        myRow = &p2Row;
        pAuto = &p2Auto;
    } else if (strcmp((char*)context, "Input-SDL-Control3") == 0) {
        current_handle = p3Handle;
        my_layout = p3Layout;
        myRow = &p3Row;
        pAuto = &p3Auto;
    } else if (strcmp((char*)context, "Input-SDL-Control4") == 0) {
        current_handle = p4Handle;
        my_layout = p4Layout;
        myRow = &p4Row;
        pAuto = &p4Auto;
    }

    if (strcmp(ParamName, "mode") == 0) {
        if ((*ConfigGetParamInt)(current_handle, ParamName) == 2) {
            int temp = 1;
            (*ConfigSetParameter)(current_handle, ParamName, ParamType, &temp);
        }
    }

    int l_ParamInt = 0;
    bool l_ParamBool = 0;
    QString l_ParamString;
    QString helper = (*ConfigGetParameterHelp)(current_handle, ParamName);
    QLabel *desc = new QLabel(ParamName);
    if (!helper.isEmpty()) {
        helper.prepend("<span style=\"color:black;\">");
        helper.append("</span>");
        desc->setToolTip(helper);
    }
    desc->setStyleSheet("border: 1px solid; padding: 10px");

    switch (ParamType) {
    case M64TYPE_INT:
        l_ParamInt = (*ConfigGetParamInt)(current_handle, ParamName);
        break;
    case M64TYPE_FLOAT:
        break;
    case M64TYPE_BOOL:
        l_ParamBool = (*ConfigGetParamBool)(current_handle, ParamName);
        break;
    case M64TYPE_STRING:
        l_ParamString = (*ConfigGetParamString)(current_handle, ParamName);
        break;
    }
    void *my_Widget = nullptr;
    if (strcmp(ParamName, "mode") == 0) {
        my_Widget = new CustomComboBox;
        ((CustomComboBox*)my_Widget)->setConfigHandle(current_handle);
        ((CustomComboBox*)my_Widget)->setParamType(ParamType);
        ((CustomComboBox*)my_Widget)->setParamName(ParamName);
        ((CustomComboBox*)my_Widget)->setAuto(pAuto);
        ((CustomComboBox*)my_Widget)->insertItem(0, "Manual");
        ((CustomComboBox*)my_Widget)->insertItem(1, "Automatic");
        ((CustomComboBox*)my_Widget)->setCurrentIndex(l_ParamInt);
        if (l_ParamInt)
            *pAuto = true;
        else
            *pAuto = false;
    }
    else if (strcmp(ParamName, "device") == 0) {
        my_Widget = new CustomComboBox;
        ((CustomComboBox*)my_Widget)->setConfigHandle(current_handle);
        ((CustomComboBox*)my_Widget)->setParamType(ParamType);
        ((CustomComboBox*)my_Widget)->setParamName(ParamName);
        int i;
        for (i = 0; i < joyNames.length(); ++i) {
            ((CustomComboBox*)my_Widget)->insertItem(i, joyNames.at(i) + " (" + QString::number(i) + ")");
        }
        ((CustomComboBox*)my_Widget)->insertItem(i, "Keyboard");
        ((CustomComboBox*)my_Widget)->insertItem(i + 1, "None");
        if (l_ParamInt == -1) {
            if (strcmp((*ConfigGetParamString)(current_handle, "name"), "Keyboard") == 0)
                ((CustomComboBox*)my_Widget)->setCurrentIndex(i);
            else
                ((CustomComboBox*)my_Widget)->setCurrentIndex(i + 1);
        } else
            ((CustomComboBox*)my_Widget)->setCurrentIndex(l_ParamInt);
    }
    else if (strcmp(ParamName, "plugged") == 0) {
        my_Widget = new CustomCheckBox;
        ((CustomCheckBox*)my_Widget)->setConfigHandle(current_handle);
        ((CustomCheckBox*)my_Widget)->setParamType(ParamType);
        ((CustomCheckBox*)my_Widget)->setParamName(ParamName);
        ((CustomCheckBox*)my_Widget)->setCheckState(l_ParamBool ? Qt::Checked : Qt::Unchecked);
        ((CustomComboBox*)my_Widget)->setDisabled(*pAuto);
    }
    else if (strcmp(ParamName, "plugin") == 0) {
        my_Widget = new CustomComboBox;
        ((CustomComboBox*)my_Widget)->setConfigHandle(current_handle);
        ((CustomComboBox*)my_Widget)->setParamType(ParamType);
        ((CustomComboBox*)my_Widget)->setParamName(ParamName);
        ((CustomComboBox*)my_Widget)->insertItem(0, "None");
        ((CustomComboBox*)my_Widget)->insertItem(1, "Mem pak");
        ((CustomComboBox*)my_Widget)->insertItem(2, "Transfer pak");
        ((CustomComboBox*)my_Widget)->insertItem(3, "Rumble pak");
        switch (l_ParamInt) {
        case 1:
            ((CustomComboBox*)my_Widget)->setCurrentIndex(0);
            break;
        case 2:
            ((CustomComboBox*)my_Widget)->setCurrentIndex(1);
            break;
        case 4:
            ((CustomComboBox*)my_Widget)->setCurrentIndex(2);
            break;
        case 5:
            ((CustomComboBox*)my_Widget)->setCurrentIndex(3);
            break;
        default:
            break;
        }
    }
    else if (strcmp(ParamName, "mouse") == 0) {
        //mouse not supported
    }
    else if (strcmp(ParamName, "MouseSensitivity") == 0) {
        //mouse not supported
    }
    else if (strcmp(ParamName, "AnalogDeadzone") == 0) {
        my_Widget = new CustomSlider;
        ((CustomSlider*)my_Widget)->setConfigHandle(current_handle);
        ((CustomSlider*)my_Widget)->setParamType(ParamType);
        ((CustomSlider*)my_Widget)->setParamName(ParamName);
        ((CustomSlider*)my_Widget)->setStyleSheet("border: 1px solid; padding: 10px");
        ((CustomSlider*)my_Widget)->setValue(l_ParamString.split(",").at(0).toInt() / 32);
        QLabel *myLabel = ((CustomSlider*)my_Widget)->getLabel();
        deleteItem(my_layout, *myRow, 1);
        deleteItem(my_layout, *myRow, 2);
        my_layout->addWidget(myLabel, *myRow, 2);
    }
    else if (strcmp(ParamName, "AnalogPeak") == 0) {
        my_Widget = new CustomSlider;
        ((CustomSlider*)my_Widget)->setConfigHandle(current_handle);
        ((CustomSlider*)my_Widget)->setParamType(ParamType);
        ((CustomSlider*)my_Widget)->setParamName(ParamName);
        ((CustomSlider*)my_Widget)->setStyleSheet("border: 1px solid; padding: 10px");
        ((CustomSlider*)my_Widget)->setValue(l_ParamString.split(",").at(0).toInt() / 32);
        QLabel *myLabel = ((CustomSlider*)my_Widget)->getLabel();
        deleteItem(my_layout, *myRow, 1);
        deleteItem(my_layout, *myRow, 2);
        my_layout->addWidget(myLabel, *myRow, 2);
    }
    else {
        my_Widget = new CustomPushButton;
        if (needBindAllButton) {
            BindAllButton *button = new BindAllButton;
            button->setText("Bind All");
            button->setDisabled(*pAuto);
            button->setFirst((CustomPushButton*)my_Widget);
            my_layout->addWidget(button, *myRow, 0, 1, 2);
            ++*myRow;
            QLabel *primaryLabel = new QLabel("Primary");
            primaryLabel->setAlignment(Qt::AlignCenter);
            primaryLabel->setStyleSheet("font-weight: bold");
            my_layout->addWidget(primaryLabel, *myRow, 1);
            QLabel *secondaryLabel = new QLabel("Secondary");
            secondaryLabel->setAlignment(Qt::AlignCenter);
            secondaryLabel->setStyleSheet("font-weight: bold");
            my_layout->addWidget(secondaryLabel, *myRow, 2);
            ++*myRow;
            needBindAllButton = false;
        }
        if (strstr(ParamName, "switch") == NULL) {
            if (last != nullptr)
                last->setNext((CustomPushButton*)my_Widget);
            last = (CustomPushButton*)my_Widget;
        }

        CustomPushButton *secondButton = new CustomPushButton;
        QStringList multiple = l_ParamString.split(") ");
        if (multiple.size() > 1) {
            QString first = multiple.at(0);
            first.append(")");
            l_ParamString = first;
            QString second = multiple.at(1);
            secondButton->setText(second);
        }
        secondButton->setConfigHandle(current_handle);
        secondButton->setParamType(ParamType);
        secondButton->setParamName(ParamName);
        secondButton->setString(l_ParamString);
        secondButton->setDisabled(*pAuto);
        secondButton->setIndex(1);
        secondButton->setJoystick((*ConfigGetParamInt)(current_handle, "device"));
        my_layout->addWidget(secondButton, *myRow, 2);

        ((CustomPushButton*)my_Widget)->setConfigHandle(current_handle);
        ((CustomPushButton*)my_Widget)->setParamType(ParamType);
        ((CustomPushButton*)my_Widget)->setParamName(ParamName);
        ((CustomPushButton*)my_Widget)->setString(l_ParamString);
        ((CustomPushButton*)my_Widget)->setIndex(0);
        if (l_ParamString.contains("key(")) {
            QString text;
            if (strstr(ParamName,"Axis") != NULL) {
                int i, j;
                sscanf(l_ParamString.toLatin1().data(),"%*c%*c%*c%*c%d%*c%d%*c", &i, &j);
                text = SDL_GetScancodeName((SDL_Scancode)sdl_keysym2scancode(i));
                text += ", ";
                text += SDL_GetScancodeName((SDL_Scancode)sdl_keysym2scancode(j));
            } else {
                int k;
                sscanf(l_ParamString.toLatin1().data(),"%*c%*c%*c%*c%d%*c", &k);
                text = SDL_GetScancodeName((SDL_Scancode)sdl_keysym2scancode(k));
            }
            ((CustomPushButton*)my_Widget)->setText(text);
        }
        else
            ((CustomPushButton*)my_Widget)->setText(l_ParamString);
        ((CustomPushButton*)my_Widget)->setDisabled(*pAuto);
        ((CustomPushButton*)my_Widget)->setJoystick((*ConfigGetParamInt)(current_handle, "device"));
    }
    if (my_Widget != nullptr) {
        my_layout->addWidget(desc, *myRow, 0);
        my_layout->addWidget((QWidget*)my_Widget, *myRow, 1);
        ++*myRow;
    }
}

void ControllerDialog::handleResetButton()
{
    int value;
    (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &value);
    if (value == M64EMU_STOPPED) {
        (*ConfigDeleteSection)("Input-SDL-Control1");
        (*ConfigDeleteSection)("Input-SDL-Control2");
        (*ConfigDeleteSection)("Input-SDL-Control3");
        (*ConfigDeleteSection)("Input-SDL-Control4");
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

ControllerDialog::ControllerDialog()
{
    m64p_error res;
    int value;
    (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &value);
    if (value == M64EMU_STOPPED) {
        PluginSearchLoad();
        PluginUnload();
    }

    initSDL();
    int numJoysticks = SDL_NumJoysticks();
    joyNames.clear();
    int i;
    for (i = 0; i < numJoysticks; ++i)
        joyNames.append(SDL_JoystickNameForIndex(i));
    p1Row = 0;
    p2Row = 0;
    p3Row = 0;
    p4Row = 0;
    needBindAllButton = true;
    last = nullptr;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->setUsesScrollButtons(false);

    QWidget *p1Settings = new QWidget;
    p1Layout = new QGridLayout;
    p1Layout->setColumnMinimumWidth(1,200);
    p1Layout->setColumnMinimumWidth(2,200);
    p1Settings->setLayout(p1Layout);
    res = (*ConfigOpenSection)("Input-SDL-Control1", &p1Handle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(p1Handle, (char*)"Input-SDL-Control1", controllerListCallback);
    QScrollArea *p1Scroll = new QScrollArea;
    p1Scroll->setWidget(p1Settings);
    p1Scroll->setMinimumWidth(p1Settings->sizeHint().width() + 20);
    p1Scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(p1Scroll, tr("Player 1"));

    QWidget *p2Settings = new QWidget;
    p2Layout = new QGridLayout;
    p2Layout->setColumnMinimumWidth(1,200);
    p2Layout->setColumnMinimumWidth(2,200);
    p2Settings->setLayout(p2Layout);
    res = (*ConfigOpenSection)("Input-SDL-Control2", &p2Handle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(p2Handle, (char*)"Input-SDL-Control2", controllerListCallback);
    QScrollArea *p2Scroll = new QScrollArea;
    p2Scroll->setWidget(p2Settings);
    p2Scroll->setMinimumWidth(p2Settings->sizeHint().width() + 20);
    p2Scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(p2Scroll, tr("Player 2"));

    QWidget *p3Settings = new QWidget;
    p3Layout = new QGridLayout;
    p3Layout->setColumnMinimumWidth(1,200);
    p3Layout->setColumnMinimumWidth(2,200);
    p3Settings->setLayout(p3Layout);
    res = (*ConfigOpenSection)("Input-SDL-Control3", &p3Handle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(p3Handle, (char*)"Input-SDL-Control3", controllerListCallback);
    QScrollArea *p3Scroll = new QScrollArea;
    p3Scroll->setWidget(p3Settings);
    p3Scroll->setMinimumWidth(p3Settings->sizeHint().width() + 20);
    p3Scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(p3Scroll, tr("Player 3"));

    QWidget *p4Settings = new QWidget;
    p4Layout = new QGridLayout;
    p4Layout->setColumnMinimumWidth(1,200);
    p4Layout->setColumnMinimumWidth(2,200);
    p4Settings->setLayout(p4Layout);
    res = (*ConfigOpenSection)("Input-SDL-Control4", &p4Handle);
    if (res == M64ERR_SUCCESS)
        (*ConfigListParameters)(p4Handle, (char*)"Input-SDL-Control4", controllerListCallback);
    QScrollArea *p4Scroll = new QScrollArea;
    p4Scroll->setWidget(p4Settings);
    p4Scroll->setMinimumWidth(p4Settings->sizeHint().width() + 20);
    p4Scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabWidget->addTab(p4Scroll, tr("Player 4"));

    mainLayout->addWidget(tabWidget);
    QPushButton *resetButton = new QPushButton("Reset All Controller Settings");
    connect(resetButton, SIGNAL (released()),this, SLOT (handleResetButton()));
    mainLayout->addWidget(resetButton);
    setLayout(mainLayout);
}
