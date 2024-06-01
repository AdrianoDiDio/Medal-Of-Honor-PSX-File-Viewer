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

#ifndef __SSTMANAGER_H_
#define __SSTMANAGER_H_

#include "GUI.h"
#include "../Common/Sound.h"
#include "../Common/Config.h"
#include "../Common/ShaderManager.h"
#include "../Common/RSC.h"
#include "SST.h"

typedef enum {
    LEVEL_MANAGER_EXPORT_FORMAT_OBJ,
    LEVEL_MANAGER_EXPORT_FORMAT_PLY,
    LEVEL_MANAGER_EXPORT_FORMAT_WAV,
    LEVEL_MANAGER_EXPORT_FORMAT_UNKNOWN
} SSTManagerExportFormats_t;

typedef struct MissionLevel_s {
    char *LevelName;
    int LevelNumber;
} MissionLevel_t;

typedef struct Mission_s {
    char *MissionName;
    int   MissionNumber;
    int   NumLevels;
    MissionLevel_t *Levels;
} Mission_t;

typedef struct RenderObjectShader_s {
    int             MVPMatrixId;
    int             MVMatrixId;
    int             EnableLightingId;
    int             EnableFogId;
    int             FogNearId;
    int             FogColorId;
    int             PaletteTextureId;
    int             TextureIndexId;
    Shader_t        *Shader;
} RenderObjectShader_t;

typedef struct SSTManager_s {
    char                    *BasePath;
    char                    MissionPath[256];
    char                    EngineName[256];
    FileDialog_t            *FileDialog;
    int                     HasToSpawnCamera;
    int                     IsPathSet;
    int                     GameEngine;
    SoundSystem_t           *SoundSystem;
    RenderObjectShader_t    *RenderObjectShader;
    RSC_t                   *GlobalRSCList;
    SST_t                   *ScriptList;
    SST_t                   *ActiveScript;

} SSTManager_t;

typedef struct SSTManagerDialogData_s {
    SSTManager_t *SSTManager;
    VideoSystem_t  *VideoSystem;
    GUI_t          *GUI;
    int             OutputFormat;
} SSTManagerDialogData_t;

SSTManager_t    *SSTManagerInit(GUI_t *GUI,VideoSystem_t *VideoSystem);
int             SSTManagerLoadLevel(SSTManager_t *SSTManager,GUI_t *GUI,VideoSystem_t *VideoSystem,
                                      int MissionNumber,int LevelNumber);
void            SSTManagerDrawString(const SSTManager_t *SSTManager,const char *String,float x,float y,Color4f_t Color);
void            SSTManagerUpdateSoundSettings(SSTManager_t *SSTManager,int SoundValue);
int             SSTManagerIsLevelLoaded(const SSTManager_t *SSTManager);
int             SSTManagerGetGameEngine(SSTManager_t *SSTManager);
int             SSTManagerInitWithPath(SSTManager_t *SSTManager,GUI_t *GUI,VideoSystem_t *VideoSystem,
                                         const char *Path);
void            SSTManagerUpdateRenderObjectShaderFog(SSTManager_t *SSTManager);
void            SSTManagerUpdate(SSTManager_t *SSTManager,Camera_t *Camera);
void            SSTManagerDraw(SSTManager_t *SSTManager,Camera_t *Camera);
void            SSTManagerToggleFileDialog(SSTManager_t *SSTManager,GUI_t *GUI,VideoSystem_t *VideoSystem);
void            SSTManagerExport(SSTManager_t* SSTManager,GUI_t *GUI,VideoSystem_t  *VideoSystem,int OutputFormat);
void            SSTManagerCleanUp(SSTManager_t *SSTManager);

extern const Mission_t MOHMissionsList[];
extern int NumMOHMissions;
extern const Mission_t MOHUMissionsList[];
extern int NumMOHUMissions;

extern Config_t *SSTManagerBasePath;
#endif//__SSTMANAGER_H_
