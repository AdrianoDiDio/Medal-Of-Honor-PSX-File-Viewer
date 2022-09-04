// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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
#ifndef __RENDER_OBJECT_MANAGER_H_
#define __RENDER_OBJECT_MANAGER_H_

#include "GUI.h"
#include "BSD.h"
#include "../Common/VRAM.h"
#include "../Common/TIM.h"
#include "Camera.h"

typedef enum {
    RENDER_OBJECT_MANAGER_BSD_NO_ERRORS = 1,
    RENDER_OBJECT_MANAGER_BSD_ERROR_GENERIC = 0,
    RENDER_OBJECT_MANAGER_BSD_ERROR_INVALID_TAF_FILE = -1,
    RENDER_OBJECT_MANAGER_BSD_ERROR_NO_ANIMATED_RENDEROBJECTS = -2,
    RENDER_OBJECT_MANAGER_BSD_ERROR_ALREADY_LOADED = -3,
    RENDER_OBJECT_MANAGER_BSD_ERROR_VRAM_INITIALIZATION = -4 
} RenderObjectManagerBSDErrorCode;

typedef enum {
    RENDER_OBJECT_MANAGER_EXPORT_FORMAT_PLY,
    RENDER_OBJECT_MANAGER_EXPORT_FORMAT_UNKNOWN
} LevelManagerExportFormats_t;

//NOTE(Adriano):A single BSD file that gets loaded together with his corresponding TAF goes
//              here...this allows for multiple BSD files to be loaded without overlapping VRAMs.
typedef struct BSDRenderObjectPack_s {
    char                            *Name;
    int                             GameVersion;
    VRAM_t                          *VRAM;
    TIMImage_t                      *ImageList;
    BSDRenderObject_t               *RenderObjectList;
    BSDRenderObject_t               *SelectedRenderObject;
    int                             LastUpdateTime;
    struct BSDRenderObjectPack_s    *Next;
} BSDRenderObjectPack_t;

typedef struct RenderObjectManager_s {
    BSDRenderObjectPack_t   *BSDList;
    
    BSDRenderObjectPack_t   *SelectedBSDPack;
    BSDRenderObject_t       *SelectedRenderObject;
    
    FileDialog_t            *BSDFileDialog;
    FileDialog_t            *ExportFileDialog;
    
    int                     PlayAnimation;
} RenderObjectManager_t;

typedef struct RenderObjectManagerDialogData_s {
    RenderObjectManager_t           *RenderObjectManager;
    VideoSystem_t                   *VideoSystem;
    GUI_t                           *GUI;
    int                             OutputFormat;
} RenderObjectManagerDialogData_t;

extern Config_t *EnableWireFrameMode;
extern Config_t *EnableAmbientLight;

RenderObjectManager_t   *RenderObjectManagerInit(GUI_t *GUI);
int                     RenderObjectManagerDeleteBSDPack(RenderObjectManager_t *RenderObjectManager,const char *BSDPackName,int GameVersion);
void                    RenderObjectManagerOpenFileDialog(RenderObjectManager_t *RenderObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem);
void                    RenderObjectManagerExportCurrentPose(RenderObjectManager_t *RenderObjectManager,
                                                             GUI_t *GUI,VideoSystem_t *VideoSystem,int OutputFormat);
void                    RenderObjectManagerUpdate(RenderObjectManager_t *RenderObjectManager);
void                    RenderObjectManagerDraw(RenderObjectManager_t *RenderObjectManager,Camera_t *Camera);
void                    RenderObjectManagerCleanUp(RenderObjectManager_t *RenderObjectManager);
int                     RenderObjectManagerLoadPack(RenderObjectManager_t *RenderObjectManager,GUI_t *GUI,
                                                    VideoSystem_t *VideoSystem,const char *File);
void                    RenderObjectManagerSetAnimationPlay(RenderObjectManager_t *RenderObjectManager,int Play);
int                     RenderObjectManagerIsAnimationPlaying(RenderObjectManager_t *RenderObjectManager);
BSDRenderObjectPack_t   *RenderObjectManagerGetSelectedBSDPack(RenderObjectManager_t *RenderObjectManager);
BSDRenderObject_t       *RenderObjectManagerGetSelectedRenderObject(RenderObjectManager_t *RenderObjectManager);
void                    RenderObjectManagerSetSelectedRenderObject(RenderObjectManager_t *RenderObjectManager,BSDRenderObjectPack_t *SelectedBSDPack,
                                                BSDRenderObject_t *SelectedRenderObject);
#endif//__RENDER_OBJECT_MANAGER_H_
