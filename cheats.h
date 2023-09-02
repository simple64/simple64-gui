#ifndef CHEATS_H
#define CHEATS_H

#include <QDialog>
#include <QGridLayout>

class CheatsDialog : public QDialog
{
    Q_OBJECT
public:
    CheatsDialog(QWidget *parent = nullptr);
private:
    QGridLayout *m_layout;
};

#endif
