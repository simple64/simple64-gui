#include "settingclasses.h"
#include "core_interface.h"
#include "plugin.h"
#include "controllerdialog.h"
#include "keyselect.h"
#include "cheat.h"
#include "common.h"
#include <SDL.h>
#include <QLabel>
#include <QVBoxLayout>

CustomSlider::CustomSlider()
{
    m_myLabel = new QLabel;
    setOrientation(Qt::Horizontal);
    setMinimum(0);
    setMaximum(1024);
    connect(this, &QSlider::valueChanged, [=](){
        int i_value = this->value() * 32;
        m_myLabel->setStyleSheet("font-weight: bold");
        m_myLabel->setText("  " + QString::number(i_value / 32768.0 * 100.0, 'f', 2) + "%");
        QString output = QString::number(i_value);
        output += ",";
        output += QString::number(i_value);
        (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.c_str(), m_ParamType, output.toLatin1().data());
        (*ConfigSaveFile)();
    });
}

CustomLineEdit::CustomLineEdit()
{
    connect(this, &QLineEdit::editingFinished, [=](){
        int i_value = this->text().toInt();
        float f_value = this->text().toFloat();
        switch (m_ParamType) {
        case M64TYPE_INT:
            (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.c_str(), m_ParamType, &i_value);
            break;
        case M64TYPE_FLOAT:
            (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.c_str(), m_ParamType, &f_value);
            break;
        case M64TYPE_STRING:
            (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.c_str(), m_ParamType, this->text().toLatin1().data());
            break;
        default:
            break;
        }
        (*ConfigSaveFile)();
    });
}

CustomCheckBox::CustomCheckBox()
{
    connect(this, &QCheckBox::stateChanged, [=](int state){
        int value = state == Qt::Checked ? 1 : 0;
        (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.c_str(), m_ParamType, &value);
        (*ConfigSaveFile)();
    });
}

CheatCheckBox::CheatCheckBox()
{
    m_Checked = false;
    connect(this, &QAbstractButton::clicked, [=](bool checked){
        sCheatInfo *pCheat;
        pCheat = CheatFindCode(m_Number);
        if (checked && !m_Checked) {
            if (pCheat == NULL)
               DebugMessage(M64MSG_WARNING, "invalid cheat code number %i", m_Number);
            else
            {
                if (pCheat->VariableLine != -1 && pCheat->Count > pCheat->VariableLine && m_Option < pCheat->Codes[pCheat->VariableLine].var_count)
                    pCheat->Codes[pCheat->VariableLine].var_to_use = m_Option;
                CheatActivate(pCheat);
                pCheat->active = true;
            }
            m_Checked = true;
        }
        else {
            (*CoreCheatEnabled)(pCheat->Name, 0);
            pCheat->active = false;
            if (m_ButtonGroup != nullptr)
                m_ButtonGroup->setChecked(true);
        }
    });
    connect(this, &QCheckBox::stateChanged, [=](int state){
        int value = state == Qt::Checked ? 1 : 0;
        if (!value)
            m_Checked = false;
    });
}

CustomComboBox::CustomComboBox()
{
    m_Auto = nullptr;
    connect(this, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [=](int index) {
        int temp;
        if (m_ParamName == "device") {
            if (index == this->count() - 2) {
                temp = -1;
                (*ConfigSetParameter)(m_CurrentHandle, "name", M64TYPE_STRING, "Keyboard");
            } else if (index == this->count() - 1) {
                temp = -1;
                (*ConfigSetParameter)(m_CurrentHandle, "name", M64TYPE_STRING, "");
            } else {
                temp = index;
                (*ConfigSetParameter)(m_CurrentHandle, "name", M64TYPE_STRING, SDL_JoystickName(SDL_JoystickOpen(index)));
            }
        } else if (m_ParamName == "mode") {
            if (index)
                *m_Auto = true;
            else
                *m_Auto = false;
            temp = index;
        } else if (m_ParamName == "plugin") {
            switch (index) {
            case 0:
                temp = 1;
                break;
            case 1:
                temp = 2;
                break;
            case 2:
                temp = 4;
                break;
            case 3:
                temp = 5;
                break;
            default:
                break;
            }
        }
        (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.c_str(), m_ParamType, &temp);
        if (m_ParamName == "mode" || m_ParamName == "device") {
            (*ConfigSaveFile)();
            int value;
            (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &value);
            if (value == M64EMU_STOPPED) {
                PluginUnload();
                PluginSearchLoad();
                PluginUnload();
            }

            p1Row = 0;
            p2Row = 0;
            p3Row = 0;
            p4Row = 0;
            needBindAllButton = true;
            last = nullptr;
            (*ConfigListParameters)(p1Handle, (char*)"Input-SDL-Control1", controllerListCallback);
            (*ConfigListParameters)(p2Handle, (char*)"Input-SDL-Control2", controllerListCallback);
            (*ConfigListParameters)(p3Handle, (char*)"Input-SDL-Control3", controllerListCallback);
            (*ConfigListParameters)(p4Handle, (char*)"Input-SDL-Control4", controllerListCallback);
        }
        (*ConfigSaveFile)();
    });
}

BindAllButton::BindAllButton()
{
    connect(this, &QAbstractButton::clicked, [=](){
        m_First->fromBindAll();
    });
}

CustomPushButton::CustomPushButton()
{
    m_Next = nullptr;

    connect(this, &QAbstractButton::clicked, [=](){
        KeySelect* keyselect = new KeySelect;
        if (m_bindAll)
            keyselect->setBindAll(m_Next);
        keyselect->setJoystick(m_Joystick);
        keyselect->setParamName(m_ParamName.c_str());
        keyselect->setConfigHandle(m_CurrentHandle);
        keyselect->setButton(this);
        keyselect->setIndex(m_index);
        keyselect->setString(m_string);
        QHBoxLayout* layout = new QHBoxLayout;
        QString myString;
        if (m_ParamName == "X Axis") {
            keyselect->setAxis(true);
            myString.append("Press the key/button you would like to bind to <b>");
            myString.append(QString::fromStdString(m_ParamName));
            myString.append(" LEFT</b> followed by <b>");
            myString.append(QString::fromStdString(m_ParamName));
            myString.append(" RIGHT");
        } else if (m_ParamName == "Y Axis") {
            keyselect->setAxis(true);
            myString.append("Press the key/button you would like to bind to <b>");
            myString.append(QString::fromStdString(m_ParamName));
            myString.append(" UP</b> followed by <b>");
            myString.append(QString::fromStdString(m_ParamName));
            myString.append(" DOWN");
        } else {
            keyselect->setAxis(false);
            myString.append("Press the key/button you would like to bind to <b>");
            myString.append(QString::fromStdString(m_ParamName));
        }
        myString.append("</b>");
        QLabel* label = new QLabel(myString);
        layout->addWidget(label);
        keyselect->setLayout(layout);
        keyselect->show();
    });
}
