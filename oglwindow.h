#ifndef OGLWINDOW_H
#define OGLWINDOW_H

#include <QCloseEvent>
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include "common.h"
#include "core_interface.h"

class OGLWindow : public QOpenGLWindow
{
public:
    int isInit() { return init; }
protected:
    void exposeEvent(QExposeEvent *) Q_DECL_OVERRIDE {}

    void initializeGL() Q_DECL_OVERRIDE;

    void resizeEvent(QResizeEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);

    void timerEvent(QTimerEvent *te);
private:
    int m_width;
    int m_height;
    int timerId = 0;
    int init = 0;
};
#endif // OGLWINDOW_H
