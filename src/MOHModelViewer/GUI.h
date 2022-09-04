/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
    MOHModelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MOHModelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MOHModelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#ifndef __GUI_H_
#define __GUI_H_

#include "../Common/Common.h"
#include "../Common/Config.h"
#include "../Common/Video.h"
#include "../Common/IMGUIUtils.h"

typedef struct GUI_s {
    ImGuiContext        *DefaultContext;
    char                *ConfigFilePath;
    bool                DebugWindowHandle;
    bool                VideoSettingsWindowHandle;
    int                 NumActiveWindows;
    ProgressBar_t       *ProgressBar;
    char                *ErrorMessage;
    int                 ErrorDialogHandle;    
} GUI_t;

typedef struct Camera_s Camera_t;
typedef struct ComTimeInfo_s ComTimeInfo_t;
typedef struct RenderObjectManager_s RenderObjectManager_t;
typedef struct Engine_s Engine_t;

GUI_t               *GUIInit(VideoSystem_t *VideoSystem);
bool                GUIIsMouseFree();
bool                GUIIsKeyboardFree();
void                GUIToggleDebugWindow(GUI_t *GUI);
void                GUIToggleVideoSettingsWindow(GUI_t *GUI);
void                GUIToggleLevelSelectWindow(GUI_t *GUI);
void                GUISetErrorMessage(GUI_t *GUI,const char *Message);
void                GUIProcessEvent(GUI_t *GUI,SDL_Event *Event);
void                GUIDraw(Engine_t *Engine);
void                GUIFree(GUI_t *GUI);
#endif//__GUI_H_
