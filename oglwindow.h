#ifndef OGLWINDOW_H
#define OGLWINDOW_H

#include <QOpenGLWindow>

class OGLWindow : public QOpenGLWindow
{
public:
    OGLWindow();
    ~OGLWindow();
    void paintGL();
};

#endif // OGLWINDOW_H
