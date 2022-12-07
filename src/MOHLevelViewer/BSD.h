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

#ifndef __BSD_H_
#define __BSD_H_

#include "../Common/Common.h"
#include "../Common/VAO.h"
#include "../Common/VRAM.h"
#include "../Common/ShaderManager.h"

#define MOH_BSD_NUM_ENTRY 26

#define MOH_RENDER_OBJECT_SIZE 256
#define MOH_UNDERGROUND_RENDER_OBJECT_SIZE 276
//Position is defined as if the BSD header was removed.
//Real file position is + sizeof(BSD_HEADER_t).
#define BSD_HANDLER_REG_TABLE_POSITION 0x59C
#define BSD_PROPERTY_SET_FILE_POSITION 0x598

#define BSD_ANIMATED_LIGHTS_FILE_POSITION 0xD8
#define BSD_ANIMATED_LIGHTS_TABLE_SIZE 40

#define BSD_ENTRY_TABLE_FILE_POSITION 0x53C

#define BSD_SKY_DATA_FILE_POSITION 0x58C
#define BSD_SKY_MAX_STARS_NUMBER 255
#define BSD_MOON_VRAM_PAGE 15
#define BSD_MOON_WIDTH 32
#define BSD_MOON_HEIGHT 32
#define BSD_MOON_TEXTURE_X 192
#define BSD_MOON_TEXTURE_Y 96

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
    BSD_RENDER_OBJECT_CARRY_AUX_ELEMENTS = 0, //Contains all the RenderObject that can be carried such as enemy helmet,grenade,rocket, etc.
    BSD_RENDER_OBJECT_ENEMY = 5122,
    BSD_RENDER_OBJECT_PICKUP_AND_EXPLOSIVE = 6000, //Contains Barrels,Boxes and also MedPacks!
    BSD_RENDER_OBJECT_PLANE = 6001,
    BSD_RENDER_OBJECT_MG42 = 6002,
    BSD_RENDER_OBJECT_DOOR = 6006,
    BSD_RENDER_OBJECT_UNKNOWN1 = 5125,
    BSD_RENDER_OBJECT_DESTRUCTIBLE_WINDOW = 6007,
    BSD_RENDER_OBJECT_VALVE = 6005,
    BSD_RENDER_OBJECT_EXPLOSIVE_CHARGE = 6008,
    BSD_RENDER_OBJECT_RADIO = 6009,
    BSD_RENDER_OBJECT_V2_ROCKET = 6004
} BSDRenderObjectType_t;

typedef enum {
    BSD_RENDER_OBJECT_WEAPON_PISTOL_TYPE_1 = 1878462241,
    BSD_RENDER_OBJECT_WEAPON_SMG_TYPE_1 = 1631105660,
    BSD_RENDER_OBJECT_WEAPON_BAZOOKA = 509069799,
    BSD_RENDER_OBJECT_WEAPON_AMERICAN_GRENADE = 424281247,
    BSD_RENDER_OBJECT_WEAPON_SHOTGUN = 2634331343,
    BSD_RENDER_OBJECT_WEAPON_SNIPER_RIFLE = 4284575011,
    BSD_RENDER_OBJECT_WEAPON_SMG_TYPE_2 = 2621329551,
    BSD_RENDER_OBJECT_WEAPON_DOCUMENT_PAPERS = 3147228851,
    BSD_RENDER_OBJECT_WEAPON_PISTOL_TYPE_2 = 860498661,
    BSD_RENDER_OBJECT_WEAPON_PISTOL_TYPE_3 = 1609048829,
    BSD_RENDER_OBJECT_WEAPON_GERMAN_GRENADE = 3097846808,
    BSD_RENDER_OBJECT_WEAPON_SMG_TYPE_3 = 2691923848,
    BSD_RENDER_OBJECT_WEAPON_M1_GARAND = 1326598003
} BSDRenderObjectWeaponId_t;

typedef enum {
    BSD_NODE_TABLE,
    BSD_UNKNOWN
} BSDEntryType_t;

typedef enum {
    BSD_COLLISION_VOLUME_TYPE_SPHERE,
    BSD_COLLISION_VOLUME_TYPE_CYLINDER,
    BSD_COLLISION_VOLUME_TYPE_BOX,
    BSD_COLLISION_VOLUME_MAX_VALUE
} BSDCollisionVolumeType_t;

typedef struct BSDPosition_s {
    short x;
    short y;
    short z;
    short Pad; // Always zero but doublecheck!
} BSDPosition_t;

typedef struct BSDUv_s {
    Byte u;
    Byte v;
} BSDUv_t;

typedef struct BSDNode_s {
    //
    // TODO:Map all the known Id.
    //
    // This is actually an unsigned int Id
//     unsigned short Id;
//     unsigned short u1;
    unsigned int Id;
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
} BSDNode_t;

typedef struct BSDTSPStreamNode_s {
    short TSPNumberRenderList[4];
    vec3 Position;
    struct BSDTSPStreamNode_s *Next;
} BSDTSPStreamNode_t;

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

/*
* Eventually dump as file.
*/

typedef struct BSDRenderObjectElement_s {
unsigned int Id;
    char    U0[40];
    int     FaceOffset;
    char    U[8];
    int     UnknownOffset1; // => References 0x564 BSDFile/EntryTable.
    int     UnknownOffset4;
    char    Pad[68];
    int     UnknownOffset2; // => References 0x574 BSDFile/EntryTable.
    int     VertOffset;
unsigned short NumVertex;
    char    RemainingU[14];
    int     RootBoneOffset; // => References 0x55C BSDFile/EntryTable.
    int     Unknown;
    int     ScaleX;
    int     ScaleY;
    int     ScaleZ;
    char    Pad2[20];
//     int     UOffset1;
    int     ColorOffset;
    char    LastU[32];
    unsigned int     ReferencedRenderObject;
    char    Pad3[16];
    int     Type;
} BSDRenderObjectElement_t;

typedef struct BSDRenderObjectBlock_s {
    int NumRenderObject;
    BSDRenderObjectElement_t *RenderObject;
} BSDRenderObjectBlock_t;

typedef struct BSDEntry_s {
    int Offset;
} BSDEntry_t;

typedef struct BSDEntryTable_s {
//     BSDEntry_t List[MOH_BSD_NUM_ENTRY];
    int NodeTableOffset;
    int UnknownDataOffset;
    
    int Off0;
    int Num0;
    
    int Off1;
    int Num1;
    
    int Off2;
    int Num2;
        
    int Off3;
    int Num3;
        
    int Off4;
    int Num4;
    
        
    int Off5;
    int Num5;
        
    int Off6;
    int Num6;

    int Off7;
    int Num7;
    
    int Off8;
    int Num8;
    
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

typedef struct BSDHeader_s {
    int NumHeadElements;
    int Sector[511];
} BSDHeader_t;

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

typedef struct BSDProperty_s {
    Byte NumNodes;
    unsigned short *NodeList;
} BSDProperty_t;

typedef struct BSDPropertySetFile_s {
    int  NumProperties;
    BSDProperty_t *Property;
} BSDPropertySetFile_t;

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
typedef struct BSD_s {
    BSDHeader_t                 Header;
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
const char              *BSDNodeGetEnumStringFromNodeId(unsigned int NodeId);
const char              *BSDRenderObjectGetEnumStringFromType(int RenderObjectType);
void                    BSDGetPlayerSpawn(BSD_t *BSD,int SpawnIndex,vec3 Position,vec3 Rotation);
void                    BSDCreateVAOs(BSD_t *BSD,int GameEngine,VRAM_t *VRAM);
void                    BSDDraw(BSD_t *BSD,VRAM_t *VRAM,Camera_t *Camera,RenderObjectShader_t *RenderObjectShader,mat4 ProjectionMatrix);
void                    BSDDrawSky(BSD_t *BSD,VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix);
void                    BSDClearNodesFlag(BSD_t *BSD);
int                     BSDGetCurrentCameraNodeDynamicData(BSD_t *BSD,Camera_t *Camera);
unsigned int            BSDNodeIdToRenderObjectId(unsigned int NodeId);
unsigned int            BSDMPNodeIdToRenderObjectId(unsigned int NodeId);
int                     BSDGetRenderObjectIndexById(BSD_t *BSD,int Id);
bool                    BSDIsRenderObjectPresent(BSD_t *BSD,unsigned int RenderObjectId);
void                    BSDFixRenderObjectPosition(Level_t *Level);
int                     BSDGetCurrentAnimatedLightColorByIndex(BSD_t *BSD,int Index);
void                    BSDDumpDataToObjFile(BSD_t *BSD,VRAM_t *VRAM,int GameEngine,FILE *OutFile);
void                    BSDDumpDataToPlyFile(BSD_t *BSD,VRAM_t *VRAM,int GameEngine,FILE *OutFile);
void                    BSDUpdateAnimatedLights(BSD_t *BSD);
void                    BSDUpdateStarsColors(BSD_t *BSD);
int                     BSDIsMoonEnabled(BSD_t *BSD);
int                     BSDAreStarsEnabled(BSD_t *BSD);
void                    BSDFree(BSD_t *BSD);

#endif //__BSDVIEWER_H_
