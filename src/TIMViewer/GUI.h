/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
    TIMViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TIMViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TIMViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#ifndef __GUI_H_
#define __GUI_H_

#include "../Common/Common.h"
#include "../Common/Video.h"
#include "../Common/IMGUIUtils.h"

typedef struct GUI_s {
    ImGuiContext        *DefaultContext;
    char                *ConfigFilePath;
    bool                VideoSettingsWindowHandle;
    ProgressBar_t       *ProgressBar;
    char                *ErrorMessage;
    int                 ErrorDialogHandle;
    int                 ImageSelectionChanged;
} GUI_t;

typedef struct Application_s Application_t;

GUI_t               *GUIInit(VideoSystem_t *VideoSystem);
void                GUIProcessEvent(GUI_t *GUI,SDL_Event *Event);
void                GUIDraw(Application_t *Application);
void                GUIFree(GUI_t *GUI);
#endif//__GUI_H_
 
