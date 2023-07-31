#ifndef HOTKEYDIALOG_H
#define HOTKEYDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include "m64p_types.h"

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
    void setButtonList(QList<CustomButton*>* ButtonList) {
        m_coreEventsButtonList = ButtonList;
    }

private:
    m64p_type m_ParamType;
    QString m_ParamName;
    m64p_handle m_CurrentHandle;
    QList<CustomButton*>* m_coreEventsButtonList;
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

class HotkeyDialog : public QDialog
{
    Q_OBJECT
public:
    HotkeyDialog(QWidget *parent = nullptr);
    QList<CustomButton*>* getButtonList();
    m64p_handle getHandle();
    QGridLayout* getLayout();
    int* getLayoutRow();
private slots:
    void handleResetButton();
private:
    QList<CustomButton*> m_coreEventsButtonList;
    m64p_handle m_configHandle;
    QGridLayout *m_layout;
    int m_layoutRow;
};

#endif // HOTKEYDIALOG_H
