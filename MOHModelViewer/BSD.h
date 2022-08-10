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

#ifndef __BSD_H_
#define __BSD_H_

#include "Common.h"

#define BSD_HEADER_SIZE 2048
#define BSD_ANIMATED_LIGHTS_TABLE_SIZE 40
#define MOH_RENDER_OBJECT_SIZE 256
#define MOH_UNDERGROUND_RENDER_OBJECT_SIZE 276

#define BSD_ANIMATED_LIGHTS_FILE_POSITION 0xD8
#define BSD_RENDER_OBJECT_STARTING_OFFSET 0x5A4
#define BSD_VERTEX_TABLE_INDEX_OFFSET 0x574
#define BSD_ENTRY_TABLE_FILE_POSITION 0x53C

typedef struct BSDVertex_s {
    short x;
    short y;
    short z;
    short Pad;
} BSDVertex_t;

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

typedef struct BSDRenderObjectElement_s {
    unsigned int    Id;
    int             UnknownOffset0;
    int             AnimationDataOffset;
    char            U0[32];
    int             FaceOffset;
    char            U[8];
    int             FaceTableOffset; // => References 0x564 BSDFile/EntryTable.
    int             UnknownOffset4;
    char            Pad[68];
    int             VertexTableIndexOffset; // => References 0x574 BSDFile/EntryTable.
    int             VertOffset;
    unsigned short  NumVertex;
    char            RemainingU[14];
    int             RootBoneOffset; // => References 0x55C BSDFile/EntryTable.
    int             Unknown;
    int             ScaleX;
    int             ScaleY;
    int             ScaleZ;
    char            Pad2[20];
    int             ColorOffset;
    char            LastU[32];
    unsigned int    ReferencedRenderObjectId;
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
    BSDVertexTable_t        *VertexTable;
    int                     NumVertexTables;
    BSDAnimatedModelFace_t  *FaceList;
    
    struct BSDRenderObject_s *Next;
} BSDRenderObject_t;

typedef struct BSD_s {
    BSDEntryTable_t         EntryTable;
    BSDRenderObjectBlock_t  RenderObjectTable;
} BSD_t;

BSDRenderObject_t   *BSDLoadAllAnimatedRenderObjects(const char *FName);
void                BSDFree(BSD_t *BSD);
void                BSDFreeRenderObjectList(BSDRenderObject_t *RenderObjectList);


#endif //__BSD_H_
