#ifndef HOTKEYDIALOG_H
#define HOTKEYDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include "settingclasses.h"

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
