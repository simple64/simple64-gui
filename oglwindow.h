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
    void exposeEvent(QExposeEvent *) Q_DECL_OVERRIDE {
        if (!exposed) {
            context()->functions()->glViewport(0, 0, width(), height());
            context()->swapBuffers(this);
            exposed = 1;
        }
    }

    void keyPressEvent(QKeyEvent *event)
    {
        int keyValue = QT2SDL2(event->key());
        if (keyValue != -1 && QtAttachCoreLib())
            (*CoreDoCommand)(M64CMD_SEND_SDL_KEYDOWN, keyValue, NULL);
    }

    void keyReleaseEvent(QKeyEvent *event)
    {
        int keyValue = QT2SDL2(event->key());
        if (keyValue != -1 && QtAttachCoreLib())
            (*CoreDoCommand)(M64CMD_SEND_SDL_KEYUP, keyValue, NULL);
    }
private:
    int exposed = 0;
};

#endif // OGLWINDOW_H
