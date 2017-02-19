/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-ui-console - common.c                                     *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2007-2010 Richard42                                     *
 *   Copyright (C) 2008 Ebenblues Nmn Okaygo Tillin9                       *
 *   Copyright (C) 2002 Hacktarux                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"
#include "plugin.h"
#include "m64p_types.h"
#include "core_interface.h"
#include "version.h"
#include "vidext.h"

/** global variables **/
int    g_Verbose = 0;
const char* qt_CoreDirPath = NULL;

cothread_t main_thread = NULL;
cothread_t game_thread = NULL;

/*********************************************************************************************************
 *  Callback functions from the core
 */

void DebugMessage(int level, const char *message, ...)
{
  char msgbuf[1024];
  va_list args;

  va_start(args, message);
  vsnprintf(msgbuf, 1024, message, args);

  DebugCallback("GUI", level, msgbuf);

  va_end(args);
}

void DebugCallback(void *Context, int level, const char *message)
{
#ifdef ANDROID
    if (level == M64MSG_ERROR)
        __android_log_print(ANDROID_LOG_ERROR, (const char *) Context, "%s", message);
    else if (level == M64MSG_WARNING)
        __android_log_print(ANDROID_LOG_WARN, (const char *) Context, "%s", message);
    else if (level == M64MSG_INFO)
        __android_log_print(ANDROID_LOG_INFO, (const char *) Context, "%s", message);
    else if (level == M64MSG_STATUS)
        __android_log_print(ANDROID_LOG_DEBUG, (const char *) Context, "%s", message);
    else if (level == M64MSG_VERBOSE)
    {
        if (g_Verbose)
            __android_log_print(ANDROID_LOG_VERBOSE, (const char *) Context, "%s", message);
    }
    else
        __android_log_print(ANDROID_LOG_ERROR, (const char *) Context, "Unknown: %s", message);
#else
    if (level == M64MSG_ERROR)
        printf("%s Error: %s\n", (const char *) Context, message);
    else if (level == M64MSG_WARNING)
        printf("%s Warning: %s\n", (const char *) Context, message);
    else if (level == M64MSG_INFO)
        printf("%s: %s\n", (const char *) Context, message);
    else if (level == M64MSG_STATUS)
        printf("%s Status: %s\n", (const char *) Context, message);
    else if (level == M64MSG_VERBOSE)
    {
        if (g_Verbose)
            printf("%s: %s\n", (const char *) Context, message);
    }
    else
        printf("%s Unknown: %s\n", (const char *) Context, message);
#endif
}

m64p_video_extension_functions vidExtFunctions = {11,
                                                 qtVidExtFuncInit,
                                                 qtVidExtFuncQuit,
                                                 qtVidExtFuncListModes,
                                                 qtVidExtFuncSetMode,
                                                 qtVidExtFuncGLGetProc,
                                                 qtVidExtFuncGLSetAttr,
                                                 qtVidExtFuncGLGetAttr,
                                                 qtVidExtFuncGLSwapBuf,
                                                 qtVidExtFuncSetCaption,
                                                 qtVidExtFuncToggleFS,
                                                 qtVidExtFuncResizeWindow};

void openROM()
{
    /* start the Mupen64Plus core library, load the configuration file */
    m64p_error rval = (*CoreStartup)(CORE_API_VERSION, NULL /*Config dir*/, NULL /*Data dir*/, "Core", DebugCallback, NULL, NULL);
    if (rval != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "couldn't start Mupen64Plus core library.");
        DetachCoreLib();
        return;
    }

    rval = CoreOverrideVidExt(&vidExtFunctions);
    if (rval != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "couldn't start VidExt library.");
        DetachCoreLib();
        return;
    }

    /* load ROM image */
    FILE *fPtr = fopen(filename, "rb");
    if (fPtr == NULL)
    {
        DebugMessage(M64MSG_ERROR, "couldn't open ROM file '%s' for reading.", filename);
        (*CoreShutdown)();
        DetachCoreLib();
        return;
    }

    /* get the length of the ROM, allocate memory buffer, load it from disk */
    long romlength = 0;
    fseek(fPtr, 0L, SEEK_END);
    romlength = ftell(fPtr);
    fseek(fPtr, 0L, SEEK_SET);
    unsigned char *ROM_buffer = (unsigned char *) malloc(romlength);
    if (ROM_buffer == NULL)
    {
        DebugMessage(M64MSG_ERROR, "couldn't allocate %li-byte buffer for ROM image file '%s'.", romlength, filename);
        fclose(fPtr);
        (*CoreShutdown)();
        DetachCoreLib();
        return;
    }
    else if (fread(ROM_buffer, 1, romlength, fPtr) != romlength)
    {
        DebugMessage(M64MSG_ERROR, "couldn't read %li bytes from ROM image file '%s'.", romlength, filename);
        free(ROM_buffer);
        fclose(fPtr);
        (*CoreShutdown)();
        DetachCoreLib();
        return;
    }
    fclose(fPtr);

    /* Try to load the ROM image into the core */
    if ((*CoreDoCommand)(M64CMD_ROM_OPEN, (int) romlength, ROM_buffer) != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "core failed to open ROM image file '%s'.", filename);
        free(ROM_buffer);
        (*CoreShutdown)();
        DetachCoreLib();
        return;
    }
    free(ROM_buffer); /* the core copies the ROM image, so we can release this buffer immediately */

    /* search for and load plugins */
    rval = PluginSearchLoad();
    if (rval != M64ERR_SUCCESS)
    {
        (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
        (*CoreShutdown)();
        DetachCoreLib();
        return;
    }

    /* attach plugins to core */
    for (int i = 0; i < 4; i++)
    {
        if ((*CoreAttachPlugin)(g_PluginMap[i].type, g_PluginMap[i].handle) != M64ERR_SUCCESS)
        {
            DebugMessage(M64MSG_ERROR, "core error while attaching %s plugin.", g_PluginMap[i].name);
            (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
            (*CoreShutdown)();
            DetachCoreLib();
            return;
        }
    }

    /* run the game */
    (*CoreDoCommand)(M64CMD_EXECUTE, 0, NULL);
    free(filename);
    free(qt_PluginDir);
}
