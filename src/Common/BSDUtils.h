/*
===========================================================================
    Copyright (C) 2018-2025 Adriano Di Dio.
    
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

#ifndef __BSD_UTILS_H_
#define __BSD_UTILS_H_

#include "Common.h"
#include "VAO.h"
#include "VRAM.h"

#define BSD_HEADER_SIZE 2048
#define BSD_ANIMATED_LIGHTS_TABLE_SIZE 40
#define MOH_RENDER_OBJECT_SIZE 256
#define MOH_UNDERGROUND_RENDER_OBJECT_SIZE 276
#define BSD_ANIMATION_FRAME_DATA_SIZE 20
#define BSD_ANIMATED_LIGHTS_TABLE_SIZE 40
#define BSD_SKY_MAX_STARS_NUMBER 255
#define BSD_SKY_MAX_STAR_COLORS_NUMBER 8

#define BSD_MOON_VRAM_PAGE 15
#define BSD_MOON_WIDTH 32
#define BSD_MOON_HEIGHT 32
#define BSD_MOON_TEXTURE_X 192
#define BSD_MOON_TEXTURE_Y 96

//Position is defined as if the BSD header was removed.
//Real file position is + sizeof(BSD_HEADER_t).

#define BSD_TSP_INFO_BLOCK_POSITION 0x0
#define BSD_SCENE_INFO_BLOCK_POSITION 0x90
#define BSD_HANDLER_REG_TABLE_POSITION 0x59C
#define BSD_PROPERTY_SET_FILE_POSITION 0x598
#define BSD_ANIMATED_LIGHTS_FILE_POSITION 0xD8
#define BSD_RENDER_OBJECT_STARTING_OFFSET 0x5A4
#define BSD_MOH_UNDERGROUND_RENDER_OBJECT_STARTING_OFFSET 0x5B4
#define BSD_ENTRY_TABLE_FILE_POSITION 0x53C
#define BSD_SKY_DATA_FILE_POSITION 0x58C

typedef enum {
    BSD_COLLISION_VOLUME_TYPE_SPHERE,
    BSD_COLLISION_VOLUME_TYPE_CYLINDER,
    BSD_COLLISION_VOLUME_TYPE_BOX,
    BSD_COLLISION_VOLUME_MAX_VALUE
} BSDCollisionVolumeType_t;

typedef enum  {
    //NOTE(Adriano): Spawn node has an additional attribute at 0x34(52) indicating if it is the first or the second.
    BSD_PLAYER_SPAWN = 2289546822,
    BSD_TSP_LOAD_TRIGGER = 1292341027,
    BSD_ENEMY_SPAWN = 1328096051,
    BSD_DOOR = 34299, //Hm Interactive objects maybe?
    BSD_LADDER = 806467859,
    BSD_PICKUP_OBJECT = 3172110974, //needs doublecheck
    BSD_NODE_SCRIPT = 3088847075, // Needs doublecheck
    BSD_ANIMATED_OBJECT = 376079867,
    BSD_DESTRUCTIBLE_WINDOW = 3542063242
    //Object Id 8499 could either be Audio Source / Timers / Random entity spawner ...
} BSDNodeId_t;

typedef enum {
    BSD_NODE_TABLE,
    BSD_UNKNOWN
} BSDEntryType_t;

typedef struct BSDPosition_s {
    short x;
    short y;
    short z;
    short Pad; // Always zero but doublecheck!
} BSDPosition_t;

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
    
} BSDAnimationFrame_t;

typedef struct BSDAnimation_s
{
    BSDAnimationFrame_t *Frame;
    int NumFrames;
} BSDAnimation_t;

typedef struct BSDFace_s {
    BSDUv_t UV0;
    short CBA;
    BSDUv_t UV1;
    short TexInfo;
    BSDUv_t UV2;
    short Pad;
    
    unsigned int Vert0;
    unsigned int Vert1;
    unsigned int Vert2;
} BSDFace_t;

typedef struct BSDNode_s {
    int Id;
    int Size;
    int u2;
    int Type;
    BSDPosition_t Position;
    BSDPosition_t Rotation;
    char Pad[8];
    short CollisionVolumeType;
    short CollisionInfo0; // Radius or BoxWidth
    short CollisionInfo1; // CylinderBottom or BoxHeight
    short CollisionInfo2; // CylinderTop or BoxDepth
//     char Pad2[8];
    int MessageData; //Offset to a list of Message Ids
    short SpawnIndex;
    Byte DynamicBlockIndex;
    //TODO:At 0x5c in every pickup object is stored the amount of ammo...
    int Visited;
    
    int FilePosition; // Position inside the BSD file
} BSDNode_t;

typedef struct BSDNodeTableEntry_s {
    int Pointer;
    int Offset;
} BSDNodeTableEntry_t;

typedef struct BSDNodeHeader_s {
    int NumNodes;
    int TableSize; //Should be the node table size...
    unsigned short u2;
    unsigned short u3;
    unsigned short u4;
    unsigned short u5;
} BSDNodeHeader_t;

typedef struct BSDNodeInfo_s {
    BSDNodeHeader_t Header;
    //
    // Each Entry in the nodetable is the offset to read all the nodes...
    // So each one can be found at PositionInFileOfNode0 + NodeTable[i]
    // All the entry of the table are spaced out with (NumNodes - 1) 0 pad or 4 Bytes of pad.
    // 
    BSDNodeTableEntry_t *Table;
    // Each node has an entry that tells what size this particular node is
    // Check if the size difference depends from the vertex maybe.
    // Also check that the size of the header is the same for all the node type.
    BSDNode_t *Node;
} BSDNodeInfo_t;

typedef struct BSDProperty_s {
    Byte NumNodes;
    unsigned short *NodeList;
} BSDProperty_t;

typedef struct BSDPropertySetFile_s {
    int  NumProperties;
    BSDProperty_t *Property;
} BSDPropertySetFile_t;

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
    // Required by MOH:Underground
    int             FaceV2Offset;
    short           NumV2Faces;
    short           NumV2AnimatedFaces;
    int             AnimatedV2FaceOffset;
    int             Unknown1;
    int             Unknown2;
    
} BSDRenderObjectElement_t;

typedef struct BSDRenderObjectTable_s {
    int NumRenderObject;
    BSDRenderObjectElement_t *RenderObject;
} BSDRenderObjectTable_t;

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

typedef struct BSDAnimatedLight_s {
    int NumColors;
    int StartingColorOffset;
    int ColorIndex;
    int CurrentColor;
    int Delay;
    
    Color1i_t *ColorList;
    
    int LastUpdateTime;
} BSDAnimatedLight_t;

typedef struct BSDAnimatedLightTable_s {
    int NumAnimatedLights;
    BSDAnimatedLight_t AnimatedLightsList[BSD_ANIMATED_LIGHTS_TABLE_SIZE];
} BSDAnimatedLightTable_t;

typedef struct BSDTSPInfo_s {
    char TSPPattern[128];
    int  NumTSP;
    int  TargetInitialCompartment; // This should indicate the number of compartment we have to draw at start.
    int  StartingComparment; // This should indicate the first compartment to render...from StartingComparment to TargetInitialCompartment
    int  u3; // Always 0...Maybe PAD
} BSDTSPInfo_t;

typedef struct BSDSky_s {
    Byte U0;
    Byte U1;
    Byte U2;
    Byte StarRadius;
    int U3;
    short MoonZ;
    short MoonY;
    int U4;
    int U5;
    int U6;
    
    Color1i_t StarsColors[BSD_SKY_MAX_STARS_NUMBER];
    VAO_t *MoonVAO;
    VAO_t *StarsVAO;
} BSDSky_t;

typedef struct BSDSceneInfo_s {
    char        Unknown[60];
    short       FogNear;
    Color3b_t   ClearColor;
    char        Unknown2[7];
} BSDSceneInfo_t;

typedef struct RenderObject_s RenderObject_t;

bool                        BSDReadTSPInfoBlock(FILE *BSDFile, BSDTSPInfo_t *TSPInfo);
bool                        BSDReadSceneInfoBlock(FILE *BSDFile, BSDSceneInfo_t *SceneInfo);
bool                        BSDReadAnimatedLightTableBlock(FILE *BSDFile, BSDAnimatedLightTable_t *AnimatedLightTable);
bool                        BSDReadEntryTableBlock(FILE *BSDFile,BSDEntryTable_t *EntryTable);
bool                        BSDReadSkyBlock(FILE *BSDFile,BSDSky_t *Sky);
bool                        BSDReadRenderObjectTable(FILE *BSDFile,int GameEngine, BSDRenderObjectTable_t *RenderObjectTable);
bool                        BSDReadNodeInfoBlock(FILE *BSDFile,int NodeInfoOffset,BSDNodeInfo_t *NodeInfo);
bool                        BSDReadPropertySetFileBlock(FILE *BSDFile, BSDPropertySetFile_t *PropertySetFile);

BSDRenderObjectElement_t    *BSDGetRenderObjectById(const BSDRenderObjectTable_t *RenderObjectTable,int RenderObjectId);
int                         BSDGetRenderObjectIndexById(const BSDRenderObjectTable_t *RenderObjectTable,int RenderObjectId);
bool                        BSDIsRenderObjectPresent(BSDRenderObjectTable_t RenderObjectTable,int RenderObjectId);
int                         BSDMPNodeIdToRenderObjectId(int NodeId);
int                         BSDNodeIdToRenderObjectId(int NodeId);


void                        BSDUpdateAnimatedLights(BSDAnimatedLightTable_t *AnimatedLightsTable);
void                        BSDUpdateStarsColors(BSDSky_t *SkyData);
void                        BSDClearNodesFlag(BSDNodeInfo_t *NodeData);
void                        BSDWriteRenderObjectToPlyFile(RenderObject_t *RenderObject,VRAM_t *VRAM,int GameEngine,const char *OutDirectory);

int                         BSDGetRenderObjectTableOffset(int GameEngine);
int                         BSDGetRealOffset(int RelativeOffset);
bool                        BSDIsMoonEnabled(BSDSky_t SkyData);
bool                        BSDAreStarsEnabled(BSDSky_t SkyData);
void                        BSDGetNodeColorById(int NodeId,vec3 OutColor);
int                         BSDGetCurrentAnimatedLightColorByIndex(BSDAnimatedLightTable_t AnimatedLightTable,int Index);
void                        BSDPositionToGLMVec3(BSDPosition_t In,vec3 Out);
void                        BSDGetProperty(BSDPropertySetFile_t PropertySetFile,int PropertyIndex);
int                         BSDGetNodeDynamicDataFromPosition(BSDNodeInfo_t *NodeData,vec3 Position);
bool                        BSDPointInNode(vec3 Position,const BSDNode_t *Node);
bool                        BSDPointInBox(vec3 Point,BSDPosition_t Center,BSDPosition_t NodeRotation,float Width,float Height,float Depth);
bool                        BSDPointInCylinder(vec3 Point,BSDPosition_t Center,float Radius,float MinY,float MaxY);
bool                        BSDPointInSphere(vec3 Point,BSDPosition_t Center,float Radius);
const char                  *BSDGetCollisionVolumeStringFromType(int CollisionVolumeType);
const char                  *BSDNodeGetEnumStringFromNodeId(int NodeId);
void                        BSDGetPlayerSpawn(BSDNodeInfo_t NodeData,int SpawnIndex,vec3 Position,vec3 Rotation);


extern Color3b_t BSDStarsColors[BSD_SKY_MAX_STAR_COLORS_NUMBER];

#endif //__BSD_UTILS_H_
