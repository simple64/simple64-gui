#include "vidext.h"
#include "common.h"
#include "workerthread.h"
#include <stdio.h>

static int init;

m64p_error qtVidExtFuncInit(void)
{
    init = 0;
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncQuit(void)
{
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncListModes(m64p_2d_size *, int *)
{
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncSetMode(int Width, int Height, int, int ScreenMode, int)
{
    if (!init) {
        workerThread->resizeMainWindow(Width, Height);
        my_window->makeCurrent();
        init = 1;
        workerThread->toggleFS(ScreenMode);
    }
    return M64ERR_SUCCESS;
}

void *qtVidExtFuncGLGetProc(const char* Proc)
{
    return (void*)my_window->context()->getProcAddress(Proc);
}

m64p_error qtVidExtFuncGLSetAttr(m64p_GLattr Attr, int Value)
{
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncGLGetAttr(m64p_GLattr Attr, int *pValue)
{
    if (Attr == M64P_GL_CONTEXT_PROFILE_MASK)
    {
        switch(my_window->format().profile()) {
        case QSurfaceFormat::CoreProfile:
            *pValue = M64P_GL_CONTEXT_PROFILE_CORE;
            break;
        case QSurfaceFormat::CompatibilityProfile:
            *pValue = M64P_GL_CONTEXT_PROFILE_COMPATIBILITY;
            break;
        case QSurfaceFormat::NoProfile:
            *pValue = M64P_GL_CONTEXT_PROFILE_COMPATIBILITY;
            break;
        }
    }
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncGLSwapBuf(void)
{
    my_window->context()->swapBuffers(my_window);
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncSetCaption(const char *)
{
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncToggleFS(void)
{
    workerThread->toggleFS(M64VIDEO_NONE);
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncResizeWindow(int, int)
{
    return M64ERR_SUCCESS;
}
