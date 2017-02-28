#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QCheckBox>
#include <QLineEdit>
#include "m64p_types.h"

class CustomLineEdit : public QLineEdit
{
public:
    CustomLineEdit();
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
    std::string m_ParamName;
    m64p_handle m_CurrentHandle;
};

class CustomCheckBox : public QCheckBox
{
public:
    CustomCheckBox();
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
    std::string m_ParamName;
    m64p_handle m_CurrentHandle;
};

#endif // CUSTOMLINEEDIT_H
