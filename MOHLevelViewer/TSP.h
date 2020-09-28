/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
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

typedef struct TSPVec3_s {
    short x;
    short y;
    short z;
} TSPVec3_t;

typedef struct TSPColor_s {
    Byte r;
    Byte g;
    Byte b;
    Byte a;
} TSPColor_t;

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

typedef struct BBox_s {
    Vec3_t Min;
    Vec3_t Max;
} BBox_t;
//16 Bytes.
typedef struct TSPFace_s {
    unsigned short V0;
    unsigned short V1;
    unsigned short V2;
    TSPUv_t UV0;
    ShortByteUnion Unk0;
    TSPUv_t UV1;
    ShortByteUnion TSB;
    TSPUv_t UV2;
} TSPFace_t;
//36 Bytes.
typedef struct TSPNodeFileLookUp_s {
    int Offset;
    int Child1Offset;
    int Child2Offset;
} TSPNodeFileLookUp_t;

typedef struct TSPNode_s {
    //Mine
    
    TSPNodeFileLookUp_t FileOffset;
    //
    TSPBBox_t BBox; // 12
    /*unsigned */int NumFaces; // 16 This should be an offset relative to the face offset...
    IntShortUnion U2; // 20 //Starting index
    IntShortUnion U3; // 24
    int BaseData; // This is the Node Dimension...BaseData / sizeof(TSPFace_t) If NumFaces != 0 or BaseData / sizeof(TSPNode_t) If NumFaces == 0
    
    Vao_t *BBoxVao;
    Vao_t *LeafFaceListVao;
    Vao_t *LeafCollisionFaceListVao;
    struct TSPNode_s *Child1;
    struct TSPNode_s *Child2;
    struct TSPNode_s *Child[2];
    struct TSPNode_s *Next;
//     IntShortUnion Child1Offset;
//     IntShortUnion Child2Offset;
} TSPNode_t;

typedef struct TSPD_s {
    int Size;
    char Pad[8];
    int U4;
    short NumData;
    short U5;
    short U6;
    short *Data;
} TSPD_t;

typedef struct TSPCollisionHeader_s {
    unsigned short U0;
    unsigned short U1;
    unsigned short U2;
    unsigned short U3;
    unsigned short NumGs;
    unsigned short NumHs;
    unsigned short NumVertices;
    unsigned short NumNormals;
    unsigned short NumFaces;
} TSPCollisionHeader_t;

// typedef struct TSPCollisionG_s {
//     short U0;
//     short U1;
//     short U2;
//     short U3;
// } TSPCollisionG_t;

typedef struct TSPCollisionFace_s {
    unsigned short V0;
    unsigned short V1;
    unsigned short V2;
    unsigned short NormalIndex;
    unsigned short Flags;
} TSPCollisionFace_t;

typedef struct TSPCollision_s {
    TSPCollisionHeader_t Header;
    TSPVert_t *G;
    short *H; //Hm?
    TSPVert_t *Vertex;
    TSPVert_t *Normal;
    TSPCollisionFace_t *Face;
} TSPCollision_t;

typedef struct TSPHeader_s {
    unsigned short ID;
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
    
    int NumD;
    int DOffset;
    
    int CollisionOffset;
} TSPHeader_t;



typedef struct TSP_s {
    char FName[256];
    TSPHeader_t Header;
    TSPNode_t   *Node;
    TSPFace_t   *Face;
    TSPVert_t   *Vertex;
    TSPColor_t  *Color;
    TSPD_t      *DBlock;
    TSPCollision_t *CollisionData;
    TSPNode_t  *BSDTree;
    //
    int          Number;
    Vao_t       *VaoList;
    Vao_t       *CollisionVaoList;
    Vao_t       *WorldBBoxVao;
    struct TSP_s *Next;
} TSP_t;

struct Level_s;
typedef struct Level_s Level_t;

TSP_t  *TSPLoad(char *FName,int TSPNumber);
void    DrawTSPList(Level_t *Level);
void    TSPCreateVAO(TSP_t *TSP);
void TSPCreateNodeBBoxVAO(TSP_t *TSPList);
void TSPCreateCollisionVAO(TSP_t *TSPList);
#endif //__TSPVIEWER_H_
