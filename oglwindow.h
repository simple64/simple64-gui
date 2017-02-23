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
    void exposeEvent(QExposeEvent *) Q_DECL_OVERRIDE {}

    void initializeGL() Q_DECL_OVERRIDE;

    void resizeEvent(QResizeEvent *event) {
        QOpenGLWindow::resizeEvent(event);
        int size = (event->size().width() << 16) + event->size().height();
        if (QtAttachCoreLib())
            (*CoreDoCommand)(M64CMD_CORE_STATE_SET, M64CORE_VIDEO_SIZE, &size);
    }

    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);
private:
    int exposed = 0;
};
#endif // OGLWINDOW_H
