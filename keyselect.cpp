#include "keyselect.h"
#include "core_interface.h"
#include "common.h"
#include "sdl_key_converter.h"

KeySelect::KeySelect()
{
    m_Number = 0;
}

void KeySelect::keyReleaseEvent(QKeyEvent *event)
{
    if (m_Joystick == -1) {
        int keyValue = QT2SDL2(event->key());
        if (keyValue != 0) {
            if (m_Number == 0)
                m_Value = "key(";
            m_Value += std::to_string(sdl_scancode2keysym(keyValue));
            if (m_Axis) {
                m_Value += ",";
                m_Axis = false;
                ++m_Number;
            } else {
                m_Value += ")";
                (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.c_str(), M64TYPE_STRING, m_Value.c_str());
                m_Button->setText(QString::fromStdString(m_Value));
                (*ConfigSaveFile)();
                this->close();
            }
        }
    }
}

void KeySelect::timerEvent(QTimerEvent *te)
{
    SDL_Joystick* joystick = SDL_JoystickOpen(m_Joystick);
    SDL_JoystickUpdate();
    int i;
    std::string newValue = "";
    bool found = false;
    for (i = 0; i < SDL_JoystickNumButtons(joystick); ++i) {
        if (SDL_JoystickGetButton(joystick, i)) {
            if (m_Number == 0)
                m_Value = "button(";
            newValue = std::to_string(i);
        }
    }
    for (i = 0; i < SDL_JoystickNumAxes(joystick); ++i) {
        Sint16 axis = SDL_JoystickGetAxis(joystick, i);
        if (axis > 16384 || (axis < -16384 && m_joyBlacklist[i] == 0)) {
            if (m_Number == 0)
                m_Value = "axis(";
            if (axis > 0)
                newValue = std::to_string(i) + "+";
            else
                newValue = std::to_string(i) + "-";
        }
    }
    for (i = 0; i < SDL_JoystickNumHats(joystick); ++i) {
        Uint8 hat;
        hat = SDL_JoystickGetHat(joystick, i);
        if (hat & SDL_HAT_UP) {
            if (m_Number == 0)
                m_Value = "hat(";
            newValue = std::to_string(i) + " Up";
        } else if (hat & SDL_HAT_DOWN) {
            if (m_Number == 0)
                m_Value = "hat(";
            newValue = std::to_string(i) + " Down";
        } else if (hat & SDL_HAT_LEFT) {
            if (m_Number == 0)
                m_Value = "hat(";
            newValue = std::to_string(i) + " Left";
        } else if (hat & SDL_HAT_RIGHT) {
            if (m_Number == 0)
                m_Value = "hat(";
            newValue = std::to_string(i) + " Right";
        }
    }
    if (newValue != "") {
        if (newValue != m_PrevValue) {
            found = true;
            m_Value += newValue;
            m_PrevValue = newValue;
        }
    }
    if (m_Axis && found) {
        m_Value += ",";
        m_Axis = false;
        ++m_Number;
    } else if (!m_Axis && found) {
        m_Value += ")";
        (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.c_str(), M64TYPE_STRING, m_Value.c_str());
        m_Button->setText(QString::fromStdString(m_Value));
        (*ConfigSaveFile)();
        killTimer(te->timerId());
        m_timer = 0;
        this->close();
    }
}

void KeySelect::closeEvent(QCloseEvent *)
{
    if (m_timer)
        killTimer(m_timer);
    m_timer = 0;
}

void KeySelect::showEvent(QShowEvent *)
{
    SDL_Joystick* joystick = SDL_JoystickOpen(m_Joystick);
    SDL_JoystickUpdate();
    int i;
    for (i = 0; i < SDL_JoystickNumAxes(joystick); ++i) {
        Sint16 axis = SDL_JoystickGetAxis(joystick, i);
        if (axis < -32000 )
            m_joyBlacklist[i] = 1;
        else
            m_joyBlacklist[i] = 0;
    }
    if (m_Joystick != -1)
        m_timer = startTimer(100);
}
