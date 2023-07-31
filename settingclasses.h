#ifndef SETTINGCLASSES_H
#define SETTINGCLASSES_H

#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include "m64p_types.h"

class CustomLineEdit : public QLineEdit
{
public:
    explicit CustomLineEdit(QWidget *parent = 0);
    void setParamName(const char* ParamName) {
        m_ParamName = ParamName;
    }
    void setParamType(m64p_type ParamType) {
        m_ParamType = ParamType;
    }
    void setConfigHandle(m64p_handle CurrentHandle) {
        m_CurrentHandle = CurrentHandle;
    }

private:
    m64p_type m_ParamType;
    QString m_ParamName;
    m64p_handle m_CurrentHandle;
};

class CustomCheckBox : public QCheckBox
{
public:
    explicit CustomCheckBox(QWidget *parent = 0);
    void setParamName(const char* ParamName) {
        m_ParamName = ParamName;
    }
    void setParamType(m64p_type ParamType) {
        m_ParamType = ParamType;
    }
    void setConfigHandle(m64p_handle CurrentHandle) {
        m_CurrentHandle = CurrentHandle;
    }

private:
    m64p_type m_ParamType;
    QString m_ParamName;
    m64p_handle m_CurrentHandle;
};

class CustomButton : public QPushButton
{
public:
    explicit CustomButton(QWidget *parent = 0);
    void setParamName(const char* ParamName) {
        m_ParamName = ParamName;
    }
    void setParamType(m64p_type ParamType) {
        m_ParamType = ParamType;
    }
    void setConfigHandle(m64p_handle CurrentHandle) {
        m_CurrentHandle = CurrentHandle;
    }

private:
    m64p_type m_ParamType;
    QString m_ParamName;
    m64p_handle m_CurrentHandle;
};

class ClearButton : public QPushButton
{
public:
    explicit ClearButton(QWidget *parent = 0);
    void setParamName(const char* ParamName) {
        m_ParamName = ParamName;
    }
    void setParamType(m64p_type ParamType) {
        m_ParamType = ParamType;
    }
    void setConfigHandle(m64p_handle CurrentHandle) {
        m_CurrentHandle = CurrentHandle;
    }
    void setMainButton(CustomButton* MainButton) {
        m_MainButton = MainButton;
    }

private:
    m64p_type m_ParamType;
    QString m_ParamName;
    m64p_handle m_CurrentHandle;
    CustomButton* m_MainButton;
};

#endif // SETTINGCLASSES_H
