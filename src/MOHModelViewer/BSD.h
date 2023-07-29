/*
===========================================================================
    Copyright (C) 2018-2023 Adriano Di Dio.
    
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

#ifndef __BSD_H_
#define __BSD_H_

#include "../Common/Common.h"
#include "../Common/VAO.h"
#include "../Common/VRAM.h"

#define BSD_HEADER_SIZE 2048
#define BSD_ANIMATED_LIGHTS_TABLE_SIZE 40
#define MOH_RENDER_OBJECT_SIZE 256
#define MOH_UNDERGROUND_RENDER_OBJECT_SIZE 276
#define BSD_ANIMATION_FRAME_DATA_SIZE 20
#define BSD_ANIMATED_LIGHTS_FILE_POSITION 0xD8
#define BSD_RENDER_OBJECT_STARTING_OFFSET 0x5A4
#define BSD_ENTRY_TABLE_FILE_POSITION 0x53C

typedef struct BSDVertex_s {
    short x;
    short y;
    short z;
    short Pad;
} BSDVertex_t;

typedef struct BSDQuaternion_s {
    short x;
    short y;
    short z;
    short w;
} BSDQuaternion_t;

typedef struct BSDColor_s {
    Byte r;
    Byte g;
    Byte b;
    Byte Pad;
} BSDColor_t;

typedef struct BSDUv_s {
    Byte u;
    Byte v;
} BSDUv_t;

typedef struct BSDVertexTable_s {
    int Offset;
    int NumVertex;
    
    BSDVertex_t *VertexList;
} BSDVertexTable_t;

typedef struct BSDAnimatedModelFace_s {
    BSDColor_t RGB0;
    BSDColor_t RGB1;
    BSDColor_t RGB2;
    BSDUv_t UV0;
    short CLUT;
    BSDUv_t UV1;
    short TexInfo;
    BSDUv_t UV2;
    Byte VertexTableDataIndex0;
    Byte VertexTableIndex0;
    Byte VertexTableDataIndex1;
    Byte VertexTableIndex1;
    Byte VertexTableDataIndex2;
    Byte VertexTableIndex2;
} BSDAnimatedModelFace_t;

typedef struct BSDHierarchyBone_s
{
    unsigned short VertexTableIndex;
    BSDVertex_t Position;
    short Pad;
    struct BSDHierarchyBone_s *Child1;
    struct BSDHierarchyBone_s *Child2;
} BSDHierarchyBone_t;

typedef struct BSDAnimationTableEntry_s
{
//     unsigned int Value;
//     unsigned short Value;
    Byte NumFrames;
    Byte NumAffectedVertex;
    unsigned short Pad;
    int Offset;
} BSDAnimationTableEntry_t;

typedef struct BSDAnimationFrame_s
{
    short           U0;
    short           U4;
    int             EncodedVector;
    BSDVertex_t     Vector;
    short           U1;
    short           U2;
    Byte            U3;
    Byte            U5;
    Byte            FrameInterpolationIndex;
    Byte            NumQuaternions;
    
    int             *EncodedQuaternionList;
    BSDQuaternion_t *QuaternionList;
    BSDQuaternion_t *CurrentQuaternionList;
    
//     short V2;
//     short V3;
} BSDAnimationFrame_t;

typedef struct BSDAnimation_s
{
    BSDAnimationFrame_t *Frame;
//     BSDAnimationEncodedVector_t *VectorList;
    int NumFrames;
//     int NumVector;
} BSDAnimation_t;


typedef struct BSDRenderObjectElement_s {
    int             Id;
    int             UnknownOffset0;
    int             AnimationDataOffset;
    int             UnknownOffset1;
    char            U0[28];
    int             FaceOffset;
    char            U[8];
    int             FaceTableOffset;
    int             UnknownOffset4;
    char            Pad[68];
    int             VertexTableIndexOffset;
    int             VertexOffset;
    unsigned short  NumVertex;
    char            RemainingU[14];
    int             HierarchyDataRootOffset;
    int             Unknown;
    int             ScaleX;
    int             ScaleY;
    int             ScaleZ;
    char            Pad2[20];
    int             ColorOffset;
    char            LastU[32];
    int             ReferencedRenderObjectId;
    char            Pad3[16];
    int             Type;
} BSDRenderObjectElement_t;

typedef struct BSDRenderObjectBlock_s {
    int NumRenderObject;
    BSDRenderObjectElement_t *RenderObject;
} BSDRenderObjectBlock_t;

typedef struct BSDEntryTable_s {
    int NodeTableOffset;
    int UnknownDataOffset;
    
    int AnimationTableOffset;
    int NumAnimationTableEntries;
    
    int AnimationDataOffset;
    int NumAnimationData;
    
    int AnimationQuaternionDataOffset;
    int NumAnimationQuaternionData;
        
    int AnimationHierarchyDataOffset;
    int NumAnimationHierarchyData;
        
    int AnimationFaceTableOffset;
    int NumAnimationFaceTables;
    
    int AnimationFaceDataOffset;
    int NumAnimationFaces;
        
    int AnimationVertexTableIndexOffset;
    int NumAnimationVertexTableIndex;

    int AnimationVertexTableOffset;
    int NumAnimationVertexTableEntry;
    
    int AnimationVertexDataOffset;
    int NumAnimationVertex;
    
} BSDEntryTable_t;

typedef struct BSDRenderObject_s {
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
    vec3                        Scale;
    vec3                        Center;
    VAO_t                       *VAO;

    struct BSDRenderObject_s *Next;
} BSDRenderObject_t;

typedef struct BSD_s {
    BSDEntryTable_t         EntryTable;
    BSDRenderObjectBlock_t  RenderObjectTable;
} BSD_t;

typedef struct Camera_s Camera_t;

BSDRenderObject_t       *BSDLoadAllAnimatedRenderObjects(const char *FName,int *GameVersion);
void                    BSDDrawRenderObjectList(BSDRenderObject_t *RenderObjectList,const VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix);
void                    BSDDrawRenderObject(BSDRenderObject_t *RenderObject,const VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix);
void                    BSDRecursivelyApplyHierachyData(const BSDHierarchyBone_t *Bone,const BSDQuaternion_t *QuaternionList,
                                                    BSDVertexTable_t *VertexTable,mat4 TransformMatrix);
int                     BSDRenderObjectSetAnimationPose(BSDRenderObject_t *RenderObject,int AnimationIndex,int FrameIndex,int Override);
BSDAnimationFrame_t     *BSDRenderObjectGetCurrentFrame(BSDRenderObject_t *RenderObject);
void                    BSDRenderObjectResetFrameQuaternionList(BSDAnimationFrame_t *Frame);

void                    BSDRenderObjectGenerateVAO(BSDRenderObject_t *RenderObject);
void                    BSDRenderObjectExportCurrentPoseToPly(BSDRenderObject_t *RenderObject,VRAM_t *VRAM,FILE *OutFile);
void                    BSDRenderObjectExportCurrentAnimationToPly(BSDRenderObject_t *RenderObject,VRAM_t *VRAM,const char *Directory,
                                                                   const char *EngineName);
void                    BSDFree(BSD_t *BSD);
void                    BSDFreeRenderObjectList(BSDRenderObject_t *RenderObjectList);


#endif //__BSD_H_
