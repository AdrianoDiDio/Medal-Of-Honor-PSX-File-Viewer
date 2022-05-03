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

#include "Common.h"
#include "Config.h"

typedef struct GUIProgressBar_s {
    ImGuiContext *Context;
    float CurrentPercentage;
    int IsOpen;
    char *DialogTitle;
} GUIProgressBar_t;

typedef struct GUI_s GUI_t;
typedef struct GUIFileDialog_s {
    char *WindowTitle;
    char *Key;
    char *Filters;
    
    ImGuiFileDialog *Window;
    void  (*OnDirSelected)(struct GUIFileDialog_s *FileDialog,GUI_t *GUI,char *Directory);
    void  (*OnDirSelectionCancelled)(struct GUIFileDialog_s *FileDialog,GUI_t *GUI);
    
    struct GUIFileDialog_s *Next;
} GUIFileDialog_t;
typedef struct GUI_s {
    ImGuiContext *DefaultContext;
    
    GUIFileDialog_t *FileDialogList;
    int NumRegisteredFileDialog;
        
    char *ConfigFilePath;
    bool DebugWindowHandle;
    bool SettingsWindowHandle;
    bool LevelSelectWindowHandle;
    int NumActiveWindows;
    GUIProgressBar_t *ProgressBar;
    char *ErrorMessage;
} GUI_t;

typedef struct LevelManager_s LevelManager_t;

extern Config_t *GUIFont;
extern Config_t *GUIFontSize;
extern Config_t *GUIShowFPS;

GUI_t *GUIInit(SDL_Window *Window,SDL_GLContext *GLContext);
void GUIToggleDebugWindow(GUI_t *GUI);
void GUIToggleSettingsWindow(GUI_t *GUI);
void GUIToggleLevelSelectWindow(GUI_t *GUI);
void GUISetErrorMessage(GUI_t *GUI,char *Message);
GUIFileDialog_t *GUIFileDialogRegister(GUI_t *GUI,char *WindowTitle,char *Filters,void (*OnDirSelected)(GUIFileDialog_t *,GUI_t *,char *),
                                       void (*OnDirSelectionCancelled)(GUIFileDialog_t *,GUI_t*));
int GUIFileDialogIsOpen(GUIFileDialog_t *Dialog);
void GUIFileDialogOpen(GUI_t *GUI,GUIFileDialog_t *Dialog);
void GUIFileDialogClose(GUI_t *GUI,GUIFileDialog_t *Dialog);
int GUIProcessEvent(GUI_t *GUI,SDL_Event *Event);
void GUIProgressBarBegin(GUI_t *GUI,char *Title);
void GUIProgressBarEnd(GUI_t *GUI);
void GUIProgressBarReset(GUI_t *GUI);
void GUISetProgressBarDialogTitle(GUI_t *GUI,char *Title);
void GUIProgressBarIncrement(GUI_t *GUI,float Increment,char *Message);
void GUIDraw(GUI_t *GUI,LevelManager_t *LevelManager);
void GUIFree(GUI_t *GUI);
#endif//__GUI_H_
