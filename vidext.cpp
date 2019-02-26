#include "vidext.h"
#include "common.h"
#include "workerthread.h"
#include "mainwindow.h"
#include <stdio.h>
#include <QDesktopWidget>

static int init;
static int needs_toggle;
QSurfaceFormat* format;

m64p_error qtVidExtFuncInit(void)
{
    init = 0;
    format = new QSurfaceFormat;
    format->setOption(QSurfaceFormat::DeprecatedFunctions, 1);
    format->setDepthBufferSize(24);
    format->setProfile(QSurfaceFormat::CompatibilityProfile);
    format->setMajorVersion(2);
    format->setMinorVersion(1);
    if (w->getGLES())
        format->setRenderableType(QSurfaceFormat::OpenGLES);
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncQuit(void)
{
    init = 0;
    workerThread->toggleFS(M64VIDEO_WINDOWED);
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncListModes(m64p_2d_size *SizeArray, int *NumSizes)
{
    QRect size = QApplication::desktop()->screenGeometry();
    SizeArray[0].uiWidth = size.width();
    SizeArray[0].uiHeight = size.height();
    *NumSizes = 1;
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncSetMode(int Width, int Height, int, int ScreenMode, int)
{
    if (!init) {
        workerThread->createOGLWindow(format);
        while (!my_window->isValid()) {}
        workerThread->resizeMainWindow(Width, Height);
        my_window->makeCurrent();
        init = 1;
        needs_toggle = ScreenMode;
    }
    return M64ERR_SUCCESS;
}

void *qtVidExtFuncGLGetProc(const char* Proc)
{
    if (!init) return NULL;
    return (void*)my_window->context()->getProcAddress(Proc);
}

m64p_error qtVidExtFuncGLSetAttr(m64p_GLattr Attr, int Value)
{
    switch (Attr) {
    case M64P_GL_DOUBLEBUFFER:
        if (Value == 1)
            format->setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        else if (Value == 0)
            format->setSwapBehavior(QSurfaceFormat::SingleBuffer);
        break;
    case M64P_GL_BUFFER_SIZE:
        break;
    case M64P_GL_DEPTH_SIZE:
        format->setDepthBufferSize(Value);
        break;
    case M64P_GL_RED_SIZE:
        format->setRedBufferSize(Value);
        break;
    case M64P_GL_GREEN_SIZE:
        format->setGreenBufferSize(Value);
        break;
    case M64P_GL_BLUE_SIZE:
        format->setBlueBufferSize(Value);
        break;
    case M64P_GL_ALPHA_SIZE:
        format->setAlphaBufferSize(Value);
        break;
    case M64P_GL_SWAP_CONTROL:
        format->setSwapInterval(Value);
        break;
    case M64P_GL_MULTISAMPLEBUFFERS:
        break;
    case M64P_GL_MULTISAMPLESAMPLES:
        format->setSamples(Value);
        break;
    case M64P_GL_CONTEXT_MAJOR_VERSION:
        format->setMajorVersion(Value);
        break;
    case M64P_GL_CONTEXT_MINOR_VERSION:
        format->setMinorVersion(Value);
        break;
    case M64P_GL_CONTEXT_PROFILE_MASK:
        switch (Value) {
        case M64P_GL_CONTEXT_PROFILE_CORE:
            format->setProfile(QSurfaceFormat::CoreProfile);
            break;
        case M64P_GL_CONTEXT_PROFILE_COMPATIBILITY:
            format->setProfile(QSurfaceFormat::CompatibilityProfile);
            break;
        case M64P_GL_CONTEXT_PROFILE_ES:
            format->setRenderableType(QSurfaceFormat::OpenGLES);
            break;
        }

        break;
    }

    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncGLGetAttr(m64p_GLattr Attr, int *pValue)
{
    if (!init) return M64ERR_NOT_INIT;
    QSurfaceFormat::SwapBehavior SB = my_window->format().swapBehavior();
    switch (Attr) {
    case M64P_GL_DOUBLEBUFFER:
        if (SB == QSurfaceFormat::SingleBuffer)
            *pValue = 0;
        else
            *pValue = 1;
        break;
    case M64P_GL_BUFFER_SIZE:
        *pValue = my_window->format().alphaBufferSize() + my_window->format().redBufferSize() + my_window->format().greenBufferSize() + my_window->format().blueBufferSize();
        break;
    case M64P_GL_DEPTH_SIZE:
        *pValue = my_window->format().depthBufferSize();
        break;
    case M64P_GL_RED_SIZE:
        *pValue = my_window->format().redBufferSize();
        break;
    case M64P_GL_GREEN_SIZE:
        *pValue = my_window->format().greenBufferSize();
        break;
    case M64P_GL_BLUE_SIZE:
        *pValue = my_window->format().blueBufferSize();
        break;
    case M64P_GL_ALPHA_SIZE:
        *pValue = my_window->format().alphaBufferSize();
        break;
    case M64P_GL_SWAP_CONTROL:
        *pValue = my_window->format().swapInterval();
        break;
    case M64P_GL_MULTISAMPLEBUFFERS:
        break;
    case M64P_GL_MULTISAMPLESAMPLES:
        *pValue = my_window->format().samples();
        break;
    case M64P_GL_CONTEXT_MAJOR_VERSION:
        *pValue = my_window->format().majorVersion();
        break;
    case M64P_GL_CONTEXT_MINOR_VERSION:
        *pValue = my_window->format().minorVersion();
        break;
    case M64P_GL_CONTEXT_PROFILE_MASK:
        switch (my_window->format().profile()) {
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
        break;
    }
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncGLSwapBuf(void)
{
    if (needs_toggle) {
        int value;
        (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &value);
        if (value > M64EMU_STOPPED) {
            workerThread->toggleFS(needs_toggle);
            needs_toggle = 0;
        }
    }

    my_window->context()->swapBuffers(my_window);
    my_window->context()->makeCurrent(my_window);
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncSetCaption(const char * _title)
{
    std::string title = _title;
    workerThread->setTitle(title);
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncToggleFS(void)
{
    workerThread->toggleFS(M64VIDEO_NONE);
    return M64ERR_SUCCESS;
}

m64p_error qtVidExtFuncResizeWindow(int width, int height)
{
    int response = M64VIDEO_NONE;
    (*CoreDoCommand)(M64CMD_CORE_STATE_QUERY, M64CORE_VIDEO_MODE, &response);
    if (response == M64VIDEO_WINDOWED)
        workerThread->resizeMainWindow(width, height);
    return M64ERR_SUCCESS;
}

uint32_t qtVidExtFuncGLGetDefaultFramebuffer(void)
{
    return 0;
}
