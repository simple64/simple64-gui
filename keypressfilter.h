#ifndef KEYPRESSFILTER_H
#define KEYPRESSFILTER_H

#include <QObject>

class KeyPressFilter : public QObject
{
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif
