#ifndef OGLWINDOW_H
#define OGLWINDOW_H

#include <QOpenGLWindow>

class OGLWindow : public QOpenGLWindow
{
protected:
    void exposeEvent(QExposeEvent *) Q_DECL_OVERRIDE {}
};

#endif // OGLWINDOW_H
