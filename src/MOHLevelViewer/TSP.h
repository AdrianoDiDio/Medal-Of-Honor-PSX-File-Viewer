/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
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
#ifndef __TSPVIEWER_H_
#define __TSPVIEWER_H_

#include "../Common/Common.h"
#include "../Common/VAO.h"
#include "../Common/VRAM.h"

typedef enum {
    TSP_FX_NONE = 1,
    TSP_FX_TRANSPARENT_FACE = 2,
    TSP_FX_ANIMATED_LIGHT_FACE = 4,
    TSP_FX_DYNAMIC_FACE = 8
} TSPRenderingFaceFlags_t;

typedef enum {
    TSP_DYNAMIC_FACE_EFFECT_PLAY_AND_STOP_TO_LAST,
    TSP_DYNAMIC_FACE_EFFECT_JUMP_TO_LAST,
    TSP_DYNAMIC_FACE_EFFECT_CYCLE,
    TSP_DYNAMIC_FACE_EFFECT_PULSE
} TSPDynamicFaceEffectType_t;

typedef enum {
    TSP_BLENDING_MODE_HALF_BACKGROUND_PLUS_HALF_FOREGROUND,
    TSP_BLENDING_MODE_BACKGROUND_PLUS_FOREGROUND,
    TSP_BLENDING_MODE_BACKGROUND_MINUS_FOREGROUND,
    TSP_BLENDING_MODE_BACKGROUND_PLUS_QUARTER_FOREGROUND,
} TSPBlendingMode;

typedef struct TSPVec3_s {
    short x;
    short y;
    short z;
} TSPVec3_t;

typedef struct TSPUv_s {
    Byte u;
    Byte v;
} TSPUv_t;

typedef struct TSPVert_s {
    TSPVec3_t Position;
    short Pad; //Should always be 104...in case there is an assert to warn if the value changes.
} TSPVert_t;

typedef struct TSPBBox_s {
    TSPVec3_t Min;
    TSPVec3_t Max;
} TSPBBox_t;

typedef struct TSPTextureInfo_s {
    TSPUv_t UV0;
    short   CBA;
    TSPUv_t UV1;
    short   TSB;
    TSPUv_t UV2;
    short   Padding;
} TSPTextureInfo_t;

typedef struct TSPRenderingFace_s {
    int VAOBufferOffset;
    int BlendingMode;
    TSPVert_t Vert0;
    TSPVert_t Vert1;
    TSPVert_t Vert2;
    Color1i_t Colors[3];
    int ColorIndex[3];
    int DynamicDataIndex;
    int Flags;
    struct TSPRenderingFace_s *Next;
} TSPRenderingFace_t;

//16 Bytes.
typedef struct TSPFace_s {
    unsigned short V0;
    unsigned short V1;
    unsigned short V2;
    TSPUv_t UV0;
    unsigned short CBA;
    TSPUv_t UV1;
    unsigned short TSB;
    TSPUv_t UV2;
} TSPFace_t;

typedef struct TSPFaceV3_s {
    int V0V1;
    unsigned short V2;
    unsigned short TextureDataIndex;
    
    unsigned int Vert0;
    unsigned int Vert1;
    unsigned int Vert2;
    int SwapV1V2;
    int FileOffset;
} TSPFaceV3_t;

//36 Bytes.
typedef struct TSPNodeFileLookUp_s {
    int Offset;
    int Child1Offset;
    int Child2Offset;
} TSPNodeFileLookUp_t;

typedef struct TSPNode_s {
    TSPBBox_t BBox;
    int NumFaces; // 16 This should be an offset relative to the face offset...
    int U2; // 20 //Starting index
    int U3; // 24
    int BaseData; // This is the Node Dimension...BaseData / sizeof(TSPFace_t) If NumFaces != 0 or BaseData / sizeof(TSPNode_t) If NumFaces == 0
    
    TSPNodeFileLookUp_t FileOffset;
    TSPFaceV3_t *FaceList;
    VAO_t *BBoxVAO;
    VAO_t *OpaqueFacesVAO;
    VAO_t *LeafCollisionFaceListVAO;
    TSPRenderingFace_t *OpaqueFaceList;
    int    NumTransparentFaces;
    struct TSPNode_s *Child[2];
    struct TSPNode_s *Next;
} TSPNode_t;



typedef struct TSPDynamicFaceData_s
{
    TSPUv_t UV0;
    unsigned short   CBA;
    TSPUv_t UV1;
    unsigned short   TSB;
    TSPUv_t UV2;
} TSPDynamicFaceData_t;

typedef struct TSPDynamicDataHeader_s
{
    int Size;
    int Unk0;
    int EffectType;
    int DynamicDataIndex;
    short FaceDataSizeMultiplier; //Maybe it tells how many face data we have (2*NumFaces)?
    short NumFacesIndex;
    short FaceIndexOffset;
    short FaceDataOffset;
} TSPDynamicDataHeader_t;

typedef struct TSPDynamicData_s {
    TSPDynamicDataHeader_t Header;
    short *FaceIndexList;
    TSPDynamicFaceData_t *FaceDataList;
    short *FaceDataListV3;
    int CurrentStride;
    int IncrementOffset;
    int LastUpdateTime;
} TSPDynamicData_t;

typedef struct TSPCollisionHeader_s {
    short CollisionBoundMinX;
    short CollisionBoundMinZ;
    short CollisionBoundMaxX;
    short CollisionBoundMaxZ;
    unsigned short NumCollisionKDTreeNodes;
    unsigned short NumCollisionFaceIndex;
    unsigned short NumVertices;
    unsigned short NumNormals;
    unsigned short NumFaces;
} TSPCollisionHeader_t;


//Bounding Volume Hierarchy
typedef struct TSPCollisionKDTreeNode_s {
    short Child0; // If Child0 < 0 => Leaf and it represents ~NumFaces, Else Child Plane Index Number
    short Child1; // Child Plane Index Number
    short MaxX; // Axis extent, If Child1 < 0 Z axis else XAxis
    short PropertySetFileIndex; // Unknown, this should be a pointer to the Property-Set-File
} TSPCollisionKDTreeNode_t;

typedef struct TSPCollisionFace_s {
    unsigned short V0;
    unsigned short V1;
    unsigned short V2;
    unsigned short NormalIndex;
    short PlaneDistance;
} TSPCollisionFace_t;

typedef struct TSPCollision_s {
    TSPCollisionHeader_t Header;
    TSPCollisionKDTreeNode_t *KDTree;
    short *FaceIndexList; // Index to the collided face...
    TSPVert_t *Vertex;
    TSPVert_t *Normal;
    TSPCollisionFace_t *Face;
} TSPCollision_t;

typedef struct TSPHeader_s {
    unsigned short Id;
    unsigned short Version;
    
    int NumNodes;
    int NodeOffset;
    
    int NumFaces;
    int FaceOffset;
    
    int NumVertices;
    int VertexOffset;
    
    int NumB;
    int BOffset;
    
    int NumColors;
    int ColorOffset;
    
    int NumC;
    int COffset;
    
    int NumDynamicDataBlock;
    int DynamicDataOffset;
    
    int CollisionOffset;
    
    int NumTextureInfo;
    int TextureInfoOffset;
} TSPHeader_t;



typedef struct TSP_s {
    char *FName;
    TSPHeader_t Header;
    TSPNode_t   *Node;
    TSPFace_t   *Face;
    TSPVert_t   *Vertex;
    Color1i_t     *Color;
    TSPDynamicData_t  *DynamicData;
    TSPTextureInfo_t *TextureData;
    TSPCollision_t *CollisionData;
    //
    int          Number;
    VAO_t       *VAOList;
    VAO_t       *TransparentVAO;
    TSPRenderingFace_t *TransparentFaceList;
    VAO_t       *CollisionVAOList;
    struct TSP_s *Next;
} TSP_t;

typedef struct Camera_s Camera_t;
typedef struct BSD_s BSD_t;

TSP_t  *TSPLoad(const char *FName,int TSPNumber);
void    TSPDrawList(TSP_t *TSPList,VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix);
void    TSPUpdateAnimatedFaces(TSP_t *TSPList,BSD_t *BSD,Camera_t *Camera,int Reset);
void    TSPUpdateDynamicFaces(TSP_t *TSPList,Camera_t *Camera,int DynamicDataIndex);
void    TSPCreateVAOs(TSP_t *TSPList);
int     TSPGetPointYComponentFromKDTree(vec3 Point,TSP_t *TSPList,int *PropertySetFileIndex,int *OutY);
bool    TSPIsVersion3(TSP_t *TSP);
void    TSPDumpDataToObjFile(TSP_t *TSPList,VRAM_t *VRAM,FILE* OutFile);
void    TSPDumpDataToPlyFile(TSP_t *TSPList,VRAM_t *VRAM,FILE* OutFile);
void    TSPFree(TSP_t *TSP);
void    TSPFreeList(TSP_t *List);
#endif //__TSPVIEWER_H_
