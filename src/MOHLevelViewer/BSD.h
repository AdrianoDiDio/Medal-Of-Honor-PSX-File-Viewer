/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
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

#ifndef __BSD_H_
#define __BSD_H_

#include "../Common/Common.h"
#include "../Common/VAO.h"
#include "../Common/VRAM.h"
#include "../Common/ShaderManager.h"
#include "../Common/BSDUtils.h"
#include "../Common/RenderObject.h"

typedef struct BSDRenderObjectDrawable_s {
    RenderObject_t                      *RenderObject;
    vec3                                Position;
    vec3                                Rotation;
    vec3                                Scale;
    struct BSDRenderObjectDrawable_s    *Next;
} BSDRenderObjectDrawable_t;


typedef struct BSD_s {
    BSDTSPInfo_t                TSPInfo;
    BSDSceneInfo_t              SceneInfo;
    BSDAnimatedLightTable_t     AnimatedLightsTable;
    BSDEntryTable_t             EntryTable;
    BSDSky_t                    SkyData;
    BSDRenderObjectTable_t      RenderObjectTable;
    BSDNodeInfo_t               NodeData;
    BSDPropertySetFile_t        PropertySetFile;
    
    RenderObject_t              *RenderObjectList;
    BSDRenderObjectDrawable_t   *RenderObjectDrawableList;

    VAO_t                       *NodeVAO;
    VAO_t                       *RenderObjectPointVAO;
    VAO_t                       *CollisionVolumeVAO[BSD_COLLISION_VOLUME_MAX_VALUE];
    int                         NumRenderObjectPoint;
    
} BSD_t;

typedef struct Level_s Level_t;
typedef struct Camera_s Camera_t;
typedef struct RenderObjectShader_s RenderObjectShader_t;

FILE                    *BSDEarlyInit(BSD_t **BSD,const char *MissionPath,int MissionNumber,int LevelNumber);
int                     BSDLoad(BSD_t *BSD,int GameEngine,int Multiplayer,FILE *BSDFile);
void                    BSDCreateVAOs(BSD_t *BSD,int GameEngine,VRAM_t *VRAM);
void                    BSDDraw(BSD_t *BSD,VRAM_t *VRAM,Camera_t *Camera,RenderObjectShader_t *RenderObjectShader,mat4 ProjectionMatrix);
void                    BSDDrawSky(BSD_t *BSD,VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix);
void                    BSDFixRenderObjectPosition(Level_t *Level);
void                    BSDDumpDataToObjFile(BSD_t *BSD,VRAM_t *VRAM,int GameEngine,FILE *OutFile);
void                    BSDDumpDataToPlyFile(BSD_t *BSD,VRAM_t *VRAM,int GameEngine,FILE *OutFile);
void                    BSDFree(BSD_t *BSD);

#endif //__BSDVIEWER_H_
