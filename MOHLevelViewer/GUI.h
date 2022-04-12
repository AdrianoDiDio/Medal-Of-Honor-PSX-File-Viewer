/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
    MOHLevelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MOHLevelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MOHLevelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#ifndef __GUI_H_
#define __GUI_H_

typedef struct GUI_s {
    ImGuiContext *Context;
    ImGuiFileDialog *DirSelectFileDialog;
    bool DebugWindowHandle;
    bool SettingsWindowHandle;
    int NumActiveWindows;
} GUI_t;

struct LevelManager_s;
typedef struct LevelManager_s LevelManager_t;
GUI_t *GUIInit(SDL_Window *Window,SDL_GLContext *GLContext);

void GUIToggleDebugWindow(GUI_t *GUI);
void GUIToggleSettingsWindow(GUI_t *GUI);
int GUIProcessEvent(GUI_t *GUI,SDL_Event *Event);
void GUIDraw(GUI_t *GUI,LevelManager_t *LevelManager);
void GUIFree(GUI_t *GUI);
#endif//__GUI_H_
