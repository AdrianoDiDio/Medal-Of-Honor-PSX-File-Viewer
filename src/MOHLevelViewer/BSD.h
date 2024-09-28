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

typedef enum {
    BSD_COLLISION_VOLUME_TYPE_SPHERE,
    BSD_COLLISION_VOLUME_TYPE_CYLINDER,
    BSD_COLLISION_VOLUME_TYPE_BOX,
    BSD_COLLISION_VOLUME_MAX_VALUE
} BSDCollisionVolumeType_t;


typedef struct BSDTSPStreamNode_s {
    short TSPNumberRenderList[4];
    vec3 Position;
    struct BSDTSPStreamNode_s *Next;
} BSDTSPStreamNode_t;



//TODO: CLEANUP
typedef struct BSDRenderObject_s {
    BSDRenderObjectElement_t *Data;
    BSDPosition_t  *Vertex;
    Color1i_t      *Color;
    BSDFace_t      *Face;
    int             NumFaces;
    VAO_t          *VAO;
} BSDRenderObject_t;

typedef struct BSDRenderObjectDrawable_s {
    int     RenderObjectIndex;
    vec3    Position;
    vec3    Rotation;
    vec3    Scale;
    struct BSDRenderObjectDrawable_s *Next;
} BSDRenderObjectDrawable_t;


typedef struct BSD_s {
    BSDTSPInfo_t                TSPInfo;
    BSDSceneInfo_t              SceneInfo;
    BSDAnimatedLightTable_t     AnimatedLightsTable;
    BSDEntryTable_t             EntryTable;
    BSDSky_t                    SkyData;
    BSDRenderObjectBlock_t      RenderObjectTable;
    BSDNodeInfo_t               NodeData;
    BSDPropertySetFile_t        PropertySetFile;
    
    BSDRenderObject_t           *RenderObjectList;
    BSDRenderObjectDrawable_t   *RenderObjectDrawableList;

    VAO_t                       *NodeVAO;
    VAO_t                       *RenderObjectPointVAO;
    VAO_t                       *CollisionVolumeVAO[BSD_COLLISION_VOLUME_MAX_VALUE];
    int                         NumRenderObjectPoint;
    //Easy lookup to see if the camera is in that node position so that
    //we can update the TSPNumberRenderList.
    BSDTSPStreamNode_t *TSPStreamNodeList;  
    
} BSD_t;

typedef struct Level_s Level_t;
typedef struct Camera_s Camera_t;
typedef struct RenderObjectShader_s RenderObjectShader_t;

FILE                    *BSDEarlyInit(BSD_t **BSD,const char *MissionPath,int MissionNumber,int LevelNumber);
int                     BSDLoad(BSD_t *BSD,int GameEngine,int Multiplayer,FILE *BSDFile);
const char              *BSDNodeGetEnumStringFromNodeId(int NodeId);
const char              *BSDRenderObjectGetEnumStringFromType(int RenderObjectType);
void                    BSDGetPlayerSpawn(BSD_t *BSD,int SpawnIndex,vec3 Position,vec3 Rotation);
void                    BSDCreateVAOs(BSD_t *BSD,int GameEngine,VRAM_t *VRAM);
void                    BSDDraw(BSD_t *BSD,VRAM_t *VRAM,Camera_t *Camera,RenderObjectShader_t *RenderObjectShader,mat4 ProjectionMatrix);
void                    BSDDrawSky(BSD_t *BSD,VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix);
void                    BSDClearNodesFlag(BSD_t *BSD);
int                     BSDGetCurrentCameraNodeDynamicData(BSD_t *BSD,Camera_t *Camera);
int                     BSDNodeIdToRenderObjectId(int NodeId);
int                     BSDMPNodeIdToRenderObjectId(int NodeId);
int                     BSDGetRenderObjectIndexById(BSD_t *BSD,int Id);
bool                    BSDIsRenderObjectPresent(BSD_t *BSD,int RenderObjectId);
void                    BSDFixRenderObjectPosition(Level_t *Level);
int                     BSDGetCurrentAnimatedLightColorByIndex(BSD_t *BSD,int Index);
void                    BSDDumpDataToObjFile(BSD_t *BSD,VRAM_t *VRAM,int GameEngine,FILE *OutFile);
void                    BSDDumpDataToPlyFile(BSD_t *BSD,VRAM_t *VRAM,int GameEngine,FILE *OutFile);
void                    BSDUpdateAnimatedLights(BSD_t *BSD);
void                    BSDUpdateStarsColors(BSD_t *BSD);
int                     BSDIsMoonEnabled(BSD_t *BSD);
int                     BSDAreStarsEnabled(BSD_t *BSD);
bool                    BSDPointInNode(vec3 Position,const BSDNode_t *Node);
char                    *BSDGetCollisionVolumeStringFromType(int CollisionVolumeType);
void                    BSDFree(BSD_t *BSD);

#endif //__BSDVIEWER_H_
