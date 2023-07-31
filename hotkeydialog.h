#ifndef HOTKEYDIALOG_H
#define HOTKEYDIALOG_H

#include <QDialog>

class HotkeyDialog : public QDialog
{
    Q_OBJECT
public:
    HotkeyDialog(QWidget *parent = nullptr);
private slots:
    void handleResetButton();
};

#endif // HOTKEYDIALOG_H
