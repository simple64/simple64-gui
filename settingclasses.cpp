#include "settingclasses.h"
#include "interface/common.h"
#include "interface/core_commands.h"
#include <SDL.h>
#include <QLabel>
#include <QVBoxLayout>

CustomLineEdit::CustomLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    connect(this, &QLineEdit::editingFinished, [=](){
        int i_value = this->text().toInt();
        float f_value = this->text().toFloat();
        switch (m_ParamType) {
        case M64TYPE_INT:
            (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.toUtf8().constData(), m_ParamType, &i_value);
            break;
        case M64TYPE_FLOAT:
            (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.toUtf8().constData(), m_ParamType, &f_value);
            break;
        case M64TYPE_STRING:
            (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.toUtf8().constData(), m_ParamType, this->text().toUtf8().constData());
            break;
        default:
            break;
        }
        (*ConfigSaveFile)();
    });
}

CustomCheckBox::CustomCheckBox(QWidget *parent)
    : QCheckBox(parent)
{
    connect(this, &QCheckBox::stateChanged, [=](int state){
        int value = state == Qt::Checked ? 1 : 0;
        (*ConfigSetParameter)(m_CurrentHandle, m_ParamName.toUtf8().constData(), m_ParamType, &value);
        (*ConfigSaveFile)();
    });
}

CustomButton::CustomButton(QWidget *parent)
    : QPushButton(parent)
{
    connect(this, &QPushButton::released, [=]{
        this->setDisabled(1);
    });
}
