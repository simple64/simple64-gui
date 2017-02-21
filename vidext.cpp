#include "vidext.h"
#include "common.h"
#include <stdio.h>

m64p_error qtVidExtFuncInit(void)
{
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

m64p_error qtVidExtFuncSetMode(int Width, int Height, int, int, int)
{
    container->setFixedSize(Width, Height);
    my_window->makeCurrent();
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
        *pValue = M64P_GL_CONTEXT_PROFILE_CORE;
    }
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncGLSwapBuf(void)
{
    my_window->context()->swapBuffers(my_window);
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncSetCaption(const char * Title)
{
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncToggleFS(void)
{
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncResizeWindow(int, int)
{
    return M64ERR_SUCCESS;
}
