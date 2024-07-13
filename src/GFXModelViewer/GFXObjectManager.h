/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    GFXModelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GFXModelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GFXModelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#ifndef __GFX_OBJECT_MANAGER_H_
#define __GFX_OBJECT_MANAGER_H_

#include "GUI.h"
#include "../Common/VRAM.h"
#include "../Common/TIM.h"
#include "../Common/GFX.h"
#include "Camera.h"

typedef enum {
    GFX_OBJECT_MANAGER_NO_ERRORS = 1,
    GFX_OBJECT_MANAGER_ERROR_GENERIC = 0,
    GFX_OBJECT_MANAGER_ERROR_INVALID_TIM_FILE = -1,
    GFX_OBJECT_MANAGER_ERROR_INVALID_GFX_FILE = -2,
    GFX_OBJECT_MANAGER_ERROR_VRAM_INITIALIZATION = -3
} GFXObjectManagerErrorCode;

typedef enum {
    GFX_OBJECT_MANAGER_EXPORT_FORMAT_PLY,
    GFX_OBJECT_MANAGER_EXPORT_FORMAT_UNKNOWN
} GFXObjectManagerExportFormats_t;

typedef struct GFXPack_s 
{
    char                    *Name;
    VRAM_t                  *VRAM;
    TIMImage_t              *ImageList;
    GFX_t                   *GFX;
    int                     LastUpdateTime;

} GFXPack_t;
typedef struct GFXObjectManager_s {
    GFXPack_t               *GFXPack;
    FileDialog_t            *GFXFileDialog;
    FileDialog_t            *ExportFileDialog;
    
    int                     PlayAnimation;
} GFXObjectManager_t;

typedef struct GFXObjectManagerDialogData_s {
    GFXObjectManager_t              *GFXObjectManager;
    VideoSystem_t                   *VideoSystem;
    GUI_t                           *GUI;
    int                             OutputFormat;
    bool                            ExportCurrentAnimation; // If not true, it will export only the current pose
} GFXObjectManagerDialogData_t;

extern Config_t *EnableWireFrameMode;
extern Config_t *EnableAmbientLight;

GFXObjectManager_t      *GFXObjectManagerInit(GUI_t *GUI);
void                    GFXObjectManagerOpenFileDialog(GFXObjectManager_t *GFXObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem);
void                    GFXObjectManagerExportSelectedModel(GFXObjectManager_t *GFXObjectManager,
                                                             GUI_t *GUI,VideoSystem_t *VideoSystem,int OutputFormat,bool ExportCurrentAnimation);
void                    GFXObjectManagerUpdate(GFXObjectManager_t *GFXObjectManager);
void                    GFXObjectManagerDraw(GFXObjectManager_t *GFXObjectManager,Camera_t *Camera);
void                    GFXObjectManagerCleanUp(GFXObjectManager_t *GFXObjectManager);
int                     GFXObjectManagerLoadPack(GFXObjectManager_t *GFXObjectManager,GUI_t *GUI,
                                                    VideoSystem_t *VideoSystem,const char *File);
void                    GFXObjectManagerAdvanceCurrentGFXAnimationFrame(GFXObjectManager_t *GFXObjectManager);
void                    GFXObjectManagerAdvanceCurrentGFXAnimationPose(GFXObjectManager_t *GFXObjectManager);
void                    GFXObjectManagerSetAnimationPlay(GFXObjectManager_t *GFXObjectManager,int Play);
int                     GFXObjectManagerIsAnimationPlaying(GFXObjectManager_t *GFXObjectManager);
GFX_t                   *GFXObjectManagerGetCurrentGFX(GFXObjectManager_t *GFXObjectManager);
#endif//__GFX_OBJECT_MANAGER_H_
