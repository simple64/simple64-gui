
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
    int keyValue = QT2SDL2(event->key());
    if (keyValue != -1 && QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_SEND_SDL_KEYDOWN, keyValue, NULL);
}

void OGLWindow::keyReleaseEvent(QKeyEvent *event)
{
    int keyValue = QT2SDL2(event->key());
    if (keyValue != -1 && QtAttachCoreLib())
        (*CoreDoCommand)(M64CMD_SEND_SDL_KEYUP, keyValue, NULL);
}
