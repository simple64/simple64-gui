#ifndef CHEATS_H
#define CHEATS_H

#include <QDialog>
#include <QGridLayout>
#include <QCheckBox>
#include <QButtonGroup>

class CheatsCheckBox : public QCheckBox
{
public:
    explicit CheatsCheckBox(QWidget *parent = 0);
    void setGroup(QButtonGroup* group) {
        m_group = group;
    }
private:
    QButtonGroup* m_group = nullptr;
};

class CheatsDialog : public QDialog
{
    Q_OBJECT
public:
    CheatsDialog(QWidget *parent = nullptr);
private:
    QGridLayout *m_layout;
};

#endif
