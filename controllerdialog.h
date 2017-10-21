#ifndef CONTROLLERDIALOG_H
#define CONTROLLERDIALOG_H

#include <QDialog>
#include "m64p_types.h"
#include "settingclasses.h"

extern m64p_handle p1Handle;
extern m64p_handle p2Handle;
extern m64p_handle p3Handle;
extern m64p_handle p4Handle;

extern int p1Row;
extern int p2Row;
extern int p3Row;
extern int p4Row;
extern bool needBindAllButton;
extern CustomPushButton* last;
void initSDL();
void controllerListCallback(void * context, const char *ParamName, m64p_type ParamType);
QString convertName(const char *ParamName, QString l_ParamString);

class ControllerDialog : public QDialog
{
    Q_OBJECT
public:
    ControllerDialog();
private slots:
    void handleResetButton();
};

#endif // CONTROLLERDIALOG_H
