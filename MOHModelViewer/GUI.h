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

#include "Common.h"
#include "Config.h"
#include "Video.h"

typedef struct GUI_s GUI_t;
typedef struct GUIFileDialog_s GUIFileDialog_t;

typedef void (*FileDialogSelectCallback_t)(GUIFileDialog_t *FileDialog,GUI_t *GUI,const char *Directory,const char *File,void *UserData);
typedef void (*FileDialogCancelCallback_t)(GUIFileDialog_t *FileDialog,GUI_t *GUI);

typedef struct GUIProgressBar_s {
    ImGuiContext    *Context;
    float           CurrentPercentage;
    int             IsOpen;
    char            *DialogTitle;
} GUIProgressBar_t;

typedef struct GUIFileDialog_s {
    char                        *WindowTitle;
    char                        *Key;
    char                        *Filters;
    ImGuiFileDialog             *Window;
    FileDialogSelectCallback_t  OnElementSelected;
    FileDialogCancelCallback_t  OnDialogCancelled;    
    struct GUIFileDialog_s      *Next;
} GUIFileDialog_t;
typedef struct GUI_s {
    ImGuiContext        *DefaultContext;
    
    GUIFileDialog_t     *FileDialogList;
    int                 NumRegisteredFileDialog;
    
    char                *ConfigFilePath;
    bool                DebugWindowHandle;
    bool                VideoSettingsWindowHandle;
    int                 NumActiveWindows;
    GUIProgressBar_t    *ProgressBar;
    char                *ErrorMessage;
    int                 ErrorDialogHandle;    
} GUI_t;

typedef struct VSyncSettings_s {
    char *DisplayValue;
    int Value;
} VSyncSettings_t;

typedef struct Camera_s Camera_t;
typedef struct ComTimeInfo_s ComTimeInfo_t;
typedef struct RenderObjectManager_s RenderObjectManager_t;
typedef struct Engine_s Engine_t;

extern Config_t *GUIFont;
extern Config_t *GUIFontSize;
extern Config_t *GUIShowFPS;

GUI_t               *GUIInit(VideoSystem_t *VideoSystem);
int                 GUIIsActive(GUI_t *GUI);
void                GUIToggleDebugWindow(GUI_t *GUI);
void                GUIToggleVideoSettingsWindow(GUI_t *GUI);
void                GUIToggleLevelSelectWindow(GUI_t *GUI);
void                GUISetErrorMessage(GUI_t *GUI,const char *Message);
GUIFileDialog_t     *GUIFileDialogRegister(GUI_t *GUI,const char *WindowTitle,const char *Filters,FileDialogSelectCallback_t OnElementSelected,
                                       FileDialogCancelCallback_t OnDialogCancelled);
void                GUIFileDialogSetTitle(GUIFileDialog_t *FileDialog,const char *Title);
void                GUIFileDialogSetOnElementSelectedCallback(GUIFileDialog_t *FileDialog,FileDialogSelectCallback_t OnElementSelected);
void                GUIFileDialogSetOnDialogCancelledCallback(GUIFileDialog_t *FileDialog,FileDialogCancelCallback_t OnDialogCancelled);
int                 GUIFileDialogIsOpen(GUIFileDialog_t *FileDialog);
void                GUIFileDialogOpen(GUIFileDialog_t *FileDialog);
void                GUIFileDialogOpenWithUserData(GUIFileDialog_t *FileDialog,void *UserData);
void                *GUIFileDialogGetUserData(GUIFileDialog_t *FileDialog);
void                GUIFileDialogClose(GUI_t *GUI,GUIFileDialog_t *FileDialog);
void                GUIProcessEvent(GUI_t *GUI,SDL_Event *Event);
void                GUIProgressBarBegin(GUI_t *GUI,const char *Title);
void                GUIProgressBarEnd(GUI_t *GUI,VideoSystem_t *VideoSystem);
void                GUIProgressBarReset(GUI_t *GUI);
void                GUISetProgressBarDialogTitle(GUI_t *GUI,const char *Title);
void                GUIProgressBarIncrement(GUI_t *GUI,VideoSystem_t *VideoSystem,float Increment,const char *Message);
void                GUIDraw(Engine_t *Engine);
void                GUIFree(GUI_t *GUI);
#endif//__GUI_H_
