#ifndef VIDEXT_H
#define VIDEXT_H

#include "m64p_types.h"

#ifdef __cplusplus

#include <QWidget>
#include <QOpenGLWindow>
extern QWidget *container;
extern QOpenGLWindow *my_window;

extern "C" {
#endif
m64p_error qtVidExtFuncInit(void);
m64p_error qtVidExtFuncQuit(void);
m64p_error qtVidExtFuncListModes(m64p_2d_size *SizeArray, int *NumSize);
m64p_error qtVidExtFuncSetMode(int Width, int Height, int BitsPerPixel, int ScreenMode, int Flags);
void *     qtVidExtFuncGLGetProc(const char*Proc);
m64p_error qtVidExtFuncGLSetAttr(m64p_GLattr Attr, int Value);
m64p_error qtVidExtFuncGLGetAttr(m64p_GLattr Attr, int *pValue);
m64p_error qtVidExtFuncGLSwapBuf(void);
m64p_error qtVidExtFuncSetCaption(const char *Title);
m64p_error qtVidExtFuncToggleFS(void);
m64p_error qtVidExtFuncResizeWindow(int Width, int Height);
#ifdef __cplusplus
}
#endif

#endif // VIDEXT_H
