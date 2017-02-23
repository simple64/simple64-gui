#ifndef OGLWINDOW_H
#define OGLWINDOW_H

#include <QCloseEvent>
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include "common.h"
#include "core_interface.h"

class OGLWindow : public QOpenGLWindow
{
protected:
    void exposeEvent(QExposeEvent *) Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);
private:
    int exposed = 0;
};
#endif // OGLWINDOW_H
