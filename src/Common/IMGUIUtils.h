/*
===========================================================================
    Copyright (C) 2018-2023 Adriano Di Dio.
    
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

#define FILE_DIALOG_DEFAULT_OPEN_DIR "."
typedef struct FileDialog_s FileDialog_t;

typedef void (*FileDialogSelectCallback_t)(FileDialog_t *FileDialog,const char *Directory,const char *File,void *UserData);
typedef void (*FileDialogCancelCallback_t)(FileDialog_t *FileDialog);

typedef struct FileDialog_s {
    char                        *WindowTitle;
    char                        *Key;
    char                        *Filters;
    ImGuiFileDialog             *Window;
    char                        *PreviousFolder;
    FileDialogSelectCallback_t  OnElementSelected;
    FileDialogCancelCallback_t  OnDialogCancelled;    
    struct FileDialog_s         *Next;
} FileDialog_t;

typedef struct ProgressBar_s {
    ImGuiContext    *Context;
    ImGuiContext    *OldContext;
    float           CurrentPercentage;
    int             IsOpen;
    char            *DialogTitle;
} ProgressBar_t;

typedef struct ErrorMessageDialog_s {
    char            *Message;
    int             Handle;
} ErrorMessageDialog_t;

typedef struct VSyncSettings_s {
    char        *DisplayValue;
    int         Value;
} VSyncSettings_t;


extern Config_t *GUIFont;
extern Config_t *GUIFontSize;
extern Config_t *GUIShowFPS;

void                    GUILoadCommonSettings();
void                    GUIContextInit(ImGuiContext *Context,VideoSystem_t *VideoSystem,const char *ConfigFilePath);
void                    GUIReleaseContext(ImGuiContext *Context);
void                    GUIBeginFrame();
void                    GUIEndFrame();
ErrorMessageDialog_t    *ErrorMessageDialogInit();
void                    ErrorMessageDialogSet(ErrorMessageDialog_t *ErrorMessageDialog,const char *Message);
void                    ErrorMessageDialogDraw(ErrorMessageDialog_t *ErrorMessageDialog);
void                    ErrorMessageDialogFree(ErrorMessageDialog_t *ErrorMessageDialog);
void                    GUIPrepareModalWindow();
bool                    GUICheckBoxWithTooltip(char *Label,bool *Value,char *DescriptionFormat,...);
void                    GUIDrawVideoSettingsWindow(bool *WindowHandle,VideoSystem_t *VideoSystem);
int                     GUIDrawTitleBar(const char *Title,float ContentWidth);

ProgressBar_t           *ProgressBarInitialize(VideoSystem_t *VideoSystem);
void                    ProgressBarDestroy(ProgressBar_t *ProgressBar);
void                    ProgressBarBegin(ProgressBar_t *GUIProgressBar,const char *Title);
void                    ProgressBarEnd(ProgressBar_t *GUIProgressBar,VideoSystem_t *VideoSystem);
void                    ProgressBarReset(ProgressBar_t *GUIProgressBar);
void                    ProgressBarSetDialogTitle(ProgressBar_t *GUIProgressBar,const char *Title);
void                    ProgressBarIncrement(ProgressBar_t *GUIProgressBar,VideoSystem_t *VideoSystem,float Increment,const char *Message);

FileDialog_t            *FileDialogRegister(const char *WindowTitle,const char *Filters,FileDialogSelectCallback_t OnElementSelected,
                                       FileDialogCancelCallback_t OnDialogCancelled);
void                    FileDialogSetTitle(FileDialog_t *FileDialog,const char *Title);
void                    FileDialogSetOnElementSelectedCallback(FileDialog_t *FileDialog,FileDialogSelectCallback_t OnElementSelected);
void                    FileDialogSetOnDialogCancelledCallback(FileDialog_t *FileDialog,FileDialogCancelCallback_t OnDialogCancelled);
int                     FileDialogIsOpen(FileDialog_t *FileDialog);
void                    FileDialogOpenWithUserData(FileDialog_t *FileDialog,const char *Path,void *UserData);
void                    FileDialogOpen(FileDialog_t *FileDialog,void *UserData);
void                    *FileDialogGetUserData(FileDialog_t *FileDialog);
void                    FileDialogRenderList();
void                    FileDialogClose(FileDialog_t *FileDialog);
void                    FileDialogListFree();

#endif//__IMGUI_UTILS_H_
