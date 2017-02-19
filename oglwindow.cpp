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
        update();
    }
}
