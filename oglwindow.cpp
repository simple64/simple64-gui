
#include "oglwindow.h"
#include "workerthread.h"
extern WorkerThread* workerThread;

void OGLWindow::initializeGL() {
    doneCurrent();
    context()->moveToThread(workerThread);
    workerThread->start();
}

void OGLWindow::keyPressEvent(QKeyEvent *event)
{
    int modValue = QT2SDL2MOD(event->modifiers());
    int keyValue = QT2SDL2(event->key());
    if (keyValue != -1 && QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_SEND_SDL_KEYDOWN, (modValue << 16) + keyValue, NULL);
}

void OGLWindow::keyReleaseEvent(QKeyEvent *event)
{
    int modValue = QT2SDL2MOD(event->modifiers());
    int keyValue = QT2SDL2(event->key());
    if (keyValue != -1 && QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_SEND_SDL_KEYUP, (modValue << 16) + keyValue, NULL);
}

void OGLWindow::resizeEvent(QResizeEvent *event) {
    QOpenGLWindow::resizeEvent(event);
    int size = (event->size().width() << 16) + event->size().height();
    if (QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_CORE_STATE_SET, M64CORE_VIDEO_SIZE, &size);
}
