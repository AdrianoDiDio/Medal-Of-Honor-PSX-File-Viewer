/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
    Medal-Of-Honor-PSX-File-Viewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Medal-Of-Honor-PSX-File-Viewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Medal-Of-Honor-PSX-File-Viewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 

#ifndef __IMGUI_UTILS_H_
#define __IMGUI_UTILS_H_

#include "Common.h"
#include "Video.h"

typedef struct ProgressBar_s {
    ImGuiContext    *Context;
    ImGuiContext    *OldContext;
    float           CurrentPercentage;
    int             IsOpen;
    char            *DialogTitle;
} ProgressBar_t;

extern Config_t *GUIFont;
extern Config_t *GUIFontSize;
extern Config_t *GUIShowFPS;

void            GUILoadCommonSettings();
void            GUIContextInit(ImGuiContext *Context,VideoSystem_t *VideoSystem,const char *ConfigFilePath);
void            GUIReleaseContext(ImGuiContext *Context);
void            GUIBeginFrame();
void            GUIEndFrame();
ProgressBar_t   *ProgressBarInitialize(VideoSystem_t *VideoSystem);
void            ProgressBarDestroy(ProgressBar_t *ProgressBar);
void            ProgressBarBegin(ProgressBar_t *GUIProgressBar,const char *Title);
void            ProgressBarEnd(ProgressBar_t *GUIProgressBar,VideoSystem_t *VideoSystem);
void            ProgressBarReset(ProgressBar_t *GUIProgressBar);
void            ProgressBarSetDialogTitle(ProgressBar_t *GUIProgressBar,const char *Title);
void            ProgressBarIncrement(ProgressBar_t *GUIProgressBar,VideoSystem_t *VideoSystem,float Increment,const char *Message);

#endif//__IMGUI_UTILS_H_
