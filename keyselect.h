#ifndef KEYSELECT_H
#define KEYSELECT_H

#include <SDL2/SDL.h>
#include <QDialog>
#include <QKeyEvent>
#include "m64p_types.h"
#include "controllerdialog.h"
#include "settingclasses.h"

class KeySelect : public QDialog
{
    Q_OBJECT
public:
    KeySelect();
    void setJoystick (int joystick) {
        m_Joystick = joystick;
        if (joystick >= 0) {
            initSDL();
            m_JoystickPointer = SDL_JoystickOpen(m_Joystick);
        }
    }
    void setParamName(const char* ParamName) {
        m_ParamName = ParamName;
    }
    void setConfigHandle(m64p_handle CurrentHandle) {
        m_CurrentHandle = CurrentHandle;
    }
    void setButton(CustomPushButton * button) {
        m_Button = button;
    }
    void setAxis (bool axis) {
        m_Axis = axis;
    }
    void setBindAll(CustomPushButton* next) {
        m_Next = next;
    }

protected:
    void keyReleaseEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *te);
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
private:
    int m_timer = 0;
    int m_Joystick;
    SDL_Joystick* m_JoystickPointer;
    std::string m_ParamName;
    m64p_handle m_CurrentHandle;
    CustomPushButton *m_Button;
    CustomPushButton *m_Next;
    bool m_Axis;
    int m_Number;
    QString m_Text;
    std::string m_Value;
    std::string m_PrevValue;
    int m_joyBlacklist[256];
};

#endif // KEYSELECT_H
