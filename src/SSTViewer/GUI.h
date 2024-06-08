/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    SSTViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SSTViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SSTViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#ifndef __GUI_H_
#define __GUI_H_

#include "../Common/Common.h"
#include "../Common/Config.h"
#include "../Common/Video.h"
#include "../Common/IMGUIUtils.h"

typedef struct GUI_s {
    ImGuiContext            *DefaultContext;
    char                    *ConfigFilePath;
    bool                    DebugWindowHandle;
    bool                    VideoSettingsWindowHandle;
    bool                    ScriptSelectWindowHandle;
    int                     NumActiveWindows;
    ProgressBar_t           *ProgressBar;
    ErrorMessageDialog_t    *ErrorMessageDialog;  

} GUI_t;

typedef struct SSTManager_s SSTManager_t;
typedef struct Camera_s Camera_t;
typedef struct ComTimeInfo_s ComTimeInfo_t;
typedef struct SoundSystem_s SoundSystem_t;

GUI_t           *GUIInit(VideoSystem_t *VideoSystem);
void            GUIPushWindow(GUI_t *GUI);
void            GUIPopWindow(GUI_t *GUI);
int             GUIIsActive(GUI_t *GUI);
void            GUIToggleDebugWindow(GUI_t *GUI);
void            GUIToggleVideoSettingsWindow(GUI_t *GUI);
void            GUIToggleScriptSelectWindow(GUI_t *GUI);
int             GUIProcessEvent(GUI_t *GUI,SDL_Event *Event);
void            GUIDraw(GUI_t *GUI,SSTManager_t *SSTManager,Camera_t *Camera,VideoSystem_t *VideoSystem,ComTimeInfo_t *TimeInfo);
void            GUIFree(GUI_t *GUI);
#endif//__GUI_H_
