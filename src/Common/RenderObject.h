/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
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

#ifndef __RENDER_OBJECT_H_
#define __RENDER_OBJECT_H_ 

#include "BSDUtils.h"
#include "ShaderManager.h"

typedef enum {
    RENDER_OBJECT_CARRY_AUX_ELEMENTS = 0, //Contains all the RenderObject that can be carried such as enemy helmet,grenade,rocket, etc.
    RENDER_OBJECT_ENEMY = 5122,
    RENDER_OBJECT_PICKUP_AND_EXPLOSIVE = 6000, //Contains Barrels,Boxes and also MedPacks!
    RENDER_OBJECT_PLANE = 6001,
    RENDER_OBJECT_MG42 = 6002,
    RENDER_OBJECT_DOOR = 6006,
    RENDER_OBJECT_UNKNOWN1 = 5125,
    RENDER_OBJECT_DESTRUCTIBLE_WINDOW = 6007,
    RENDER_OBJECT_VALVE = 6005,
    RENDER_OBJECT_EXPLOSIVE_CHARGE = 6008,
    RENDER_OBJECT_RADIO = 6009,
    RENDER_OBJECT_V2_ROCKET = 6004
} RenderObjectType_t;

typedef enum {
    RENDER_OBJECT_WEAPON_PISTOL_TYPE_1 = 1878462241,
    RENDER_OBJECT_WEAPON_SMG_TYPE_1 = 1631105660,
    RENDER_OBJECT_WEAPON_BAZOOKA = 509069799,
    RENDER_OBJECT_WEAPON_AMERICAN_GRENADE = 424281247,
    RENDER_OBJECT_WEAPON_SHOTGUN = 2634331343,
    RENDER_OBJECT_WEAPON_SNIPER_RIFLE = 4284575011,
    RENDER_OBJECT_WEAPON_SMG_TYPE_2 = 2621329551,
    RENDER_OBJECT_WEAPON_DOCUMENT_PAPERS = 3147228851,
    RENDER_OBJECT_WEAPON_PISTOL_TYPE_2 = 860498661,
    RENDER_OBJECT_WEAPON_PISTOL_TYPE_3 = 1609048829,
    RENDER_OBJECT_WEAPON_GERMAN_GRENADE = 3097846808,
    RENDER_OBJECT_WEAPON_SMG_TYPE_3 = 2691923848,
    RENDER_OBJECT_WEAPON_M1_GARAND = 1326598003
} RenderObjectWeaponId_t;

typedef struct RenderObject_s {
    int                         Id;
    int                         ReferencedRenderObjectId;
    int                         Type;
    BSDVertexTable_t            *VertexTable;
    BSDVertexTable_t            *CurrentVertexTable;
    int                         NumVertexTables;
    BSDAnimatedModelFace_t      *FaceList;
    int                         NumFaces;
    BSDHierarchyBone_t          *HierarchyDataRoot;
    BSDAnimation_t              *AnimationList;
    int                         NumAnimations;
    int                         CurrentAnimationIndex;
    int                         CurrentFrameIndex;
    
    BSDRenderObjectElement_t    *Data; // Pointer to the original data parsed from the BSD file.

    BSDVertex_t                 *VertexList;
    BSDColor_t                  *ColorList;
    BSDFace_t                   *StaticFaceList;

    vec3                        Position;
    vec3                        Scale;
    vec3                        Rotation;
    vec3                        Center;
    VAO_t                       *VAO;
    
    bool                        IsStatic;

    struct RenderObject_s *Next;
} RenderObject_t;


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

RenderObject_t              *RenderObjectLoadAllFromTable(BSDEntryTable_t EntryTable,BSDRenderObjectTable_t RenderObjectTable,FILE *BSDFile,
                                             int GameVersion,bool KeepTableOrder);

RenderObject_t              *RenderObjectLoad(BSDRenderObjectElement_t RenderObjectElement,BSDEntryTable_t EntryTable,
                                              BSDRenderObjectTable_t RenderObjectTable,FILE *BSDFile,int GameVersion);
void                        RenderObjectBeginDraw(const VRAM_t *VRAM,const RenderObjectShader_t *RenderObjectShader,
                                                  bool EnableAmbientLight,bool EnableWireFrameMode,bool EnableFog);
void                        RenderObjectDraw(RenderObject_t *RenderObject,const RenderObjectShader_t *RenderObjectShader,
                                             mat4 ModelMatrix,mat4 ViewMatrix,mat4 ProjectionMatrix);
void                        RenderObjectEndDraw(bool EnableWireFrameMode);



void                        RenderObjectResetFrameQuaternionList(BSDAnimationFrame_t *Frame);
bool                        RenderObjectSetAnimationPose(RenderObject_t *RenderObject,int AnimationIndex,int FrameIndex,int Override);

BSDAnimationFrame_t         *RenderObjectGetCurrentFrame(const RenderObject_t *RenderObject);
RenderObject_t              *GetRenderObjectByIdFromList(const RenderObject_t *RenderObjectList,unsigned int RenderObjectId);


RenderObjectShader_t        *RenderObjectInitShader();
void                        RenderObjectUpdateShader(RenderObjectShader_t *RenderObjectShader, short FogNear, Color3b_t ClearColor);
void                        RenderObjectFreeShader(RenderObjectShader_t *RenderObjectShader);


void                        RenderObjectGenerateVAO(RenderObject_t *RenderObject);

void                        RenderObjectExportCurrentPoseToPly(RenderObject_t *RenderObject,VRAM_t *VRAM,FILE *OutFile);
void                        RenderObjectExportCurrentAnimationToPly(RenderObject_t *RenderObject,VRAM_t *VRAM,const char *Directory,
                                                                    const char *EngineName);


void                        RenderObjectFreeList(RenderObject_t *RenderObjectList);
void                        RenderObjectFree(RenderObject_t *RenderObject);

RenderObject_t              *RenderObjectGetByIdFromList(const RenderObject_t *RenderObjectList,int RenderObjectId);

const char                  *RenderObjectGetWeaponNameFromId(int RenderObjectId);
const char                  *RenderObjectGetStringFromType(RenderObjectType_t RenderObjectType);
#endif // __RENDER_OBJECT_H_
