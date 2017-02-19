#include "oglwindow.h"
#include "common.h"
OGLWindow::OGLWindow()
{

}

OGLWindow::~OGLWindow()
{

}

void OGLWindow::paintGL()
{
    if (game_thread != NULL) {
        co_switch(game_thread);
        if (!emuRunning) {
            co_delete(game_thread);
            game_thread = NULL;
        }
        update();
    }
}
