#include "settingclasses.h"
#include "core_interface.h"

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
