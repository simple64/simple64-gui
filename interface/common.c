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

/* Version number for UI-Console config section parameters */
#define CONFIG_PARAM_VERSION     1.00

/** global variables **/
int    g_Verbose = 0;

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

  DebugCallback("UI-Console", level, msgbuf);

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

static m64p_handle l_ConfigCore = NULL;
static m64p_handle l_ConfigVideo = NULL;
static m64p_handle l_ConfigUI = NULL;

/*********************************************************************************************************
 *  Configuration handling
 */

static m64p_error OpenConfigurationHandles(void)
{
    float fConfigParamsVersion;
    int bSaveConfig = 0;
    m64p_error rval;

    /* Open Configuration sections for core library and console User Interface */
    rval = (*ConfigOpenSection)("Core", &l_ConfigCore);
    if (rval != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "failed to open 'Core' configuration section");
        return rval;
    }

    rval = (*ConfigOpenSection)("Video-General", &l_ConfigVideo);
    if (rval != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "failed to open 'Video-General' configuration section");
        return rval;
    }

    rval = (*ConfigOpenSection)("UI-Console", &l_ConfigUI);
    if (rval != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "failed to open 'UI-Console' configuration section");
        return rval;
    }

    if ((*ConfigGetParameter)(l_ConfigUI, "Version", M64TYPE_FLOAT, &fConfigParamsVersion, sizeof(float)) != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_WARNING, "No version number in 'UI-Console' config section. Setting defaults.");
        (*ConfigDeleteSection)("UI-Console");
        (*ConfigOpenSection)("UI-Console", &l_ConfigUI);
        bSaveConfig = 1;
    }
    else if (((int) fConfigParamsVersion) != ((int) CONFIG_PARAM_VERSION))
    {
        DebugMessage(M64MSG_WARNING, "Incompatible version %.2f in 'UI-Console' config section: current is %.2f. Setting defaults.", fConfigParamsVersion, (float) CONFIG_PARAM_VERSION);
        (*ConfigDeleteSection)("UI-Console");
        (*ConfigOpenSection)("UI-Console", &l_ConfigUI);
        bSaveConfig = 1;
    }
    else if ((CONFIG_PARAM_VERSION - fConfigParamsVersion) >= 0.0001f)
    {
        /* handle upgrades */
        float fVersion = CONFIG_PARAM_VERSION;
        ConfigSetParameter(l_ConfigUI, "Version", M64TYPE_FLOAT, &fVersion);
        DebugMessage(M64MSG_INFO, "Updating parameter set version in 'UI-Console' config section to %.2f", fVersion);
        bSaveConfig = 1;
    }

    /* Set default values for my Config parameters */
    (*ConfigSetDefaultFloat)(l_ConfigUI, "Version", CONFIG_PARAM_VERSION,  "Mupen64Plus UI-Console config parameter set version number.  Please don't change this version number.");
    (*ConfigSetDefaultString)(l_ConfigUI, "PluginDir", OSAL_CURRENT_DIR, "Directory in which to search for plugins");
    (*ConfigSetDefaultString)(l_ConfigUI, "VideoPlugin", "mupen64plus-video-rice" OSAL_DLL_EXTENSION, "Filename of video plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "AudioPlugin", "mupen64plus-audio-sdl" OSAL_DLL_EXTENSION, "Filename of audio plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "InputPlugin", "mupen64plus-input-sdl" OSAL_DLL_EXTENSION, "Filename of input plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "RspPlugin", "mupen64plus-rsp-hle" OSAL_DLL_EXTENSION, "Filename of RSP plugin");

    if (bSaveConfig && ConfigSaveSection != NULL) /* ConfigSaveSection was added in Config API v2.1.0 */
        (*ConfigSaveSection)("UI-Console");

    return M64ERR_SUCCESS;
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
    /* load the Mupen64Plus core library */
    if (AttachCoreLib("/home/loganmc10/mupen64plus-GLideN64/mupen64plus/libmupen64plus.so.2") != M64ERR_SUCCESS)
        return;

    /* start the Mupen64Plus core library, load the configuration file */
    m64p_error rval = (*CoreStartup)(CORE_API_VERSION, NULL, "/usr/share/mupen64plus", "Core", DebugCallback, NULL, NULL);
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

    /* Open configuration sections */
    rval = OpenConfigurationHandles();
    if (rval != M64ERR_SUCCESS)
    {
        (*CoreShutdown)();
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
    rval = PluginSearchLoad(l_ConfigUI);
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
}
