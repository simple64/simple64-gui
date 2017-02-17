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
    co_switch(game_thread);
    update();
}
