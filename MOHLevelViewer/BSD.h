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

#ifndef __BSD_H_
#define __BSD_H_

#define MOH_BSD_NUM_ENTRY 26

//Position is defined as if the BSD header was removed.
//Real file position is + sizeof(BSD_HEADER_t).
#define BSD_HANDLER_REG_TABLE_POSITION 0x59C
#define BSD_PROPERTY_SET_FILE_POSITION 0x598

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
    //Object ID 8499 could either be Audio Source / Timers / Random entity spawner ...
} BSDNodeID;

typedef enum {
    BSD_RENDER_OBJECT_CARRY_AUX_ELEMENTS = 0, //Contains all the RenderObject that can be carried such as enemy hat,grenade,rocket, etc.
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
} BSDRenderObjectType;

typedef enum {
    BSD_NODE_TABLE,
    BSD_UNKNOWN
} BSDEntry;


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
    // TODO:Map all the known ID.
    //
    // This is actually an unsigned int ID
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
    int MessageData; //Offset to a list of Message IDs
} BSDNode_t;

typedef struct BSDTSPStreamNode_s {
    short TSPNumberRenderList[4];
    Vec3_t Position;
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
unsigned int ID;
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
    int     MatrixOffset;
    char    LastU[32];
    unsigned int     ReferencedRenderObject;
    char    Pad3[16];
    int     Type;
} BSDRenderObjectElement_t;

typedef struct BSDRenderObjectBlock_s {
    int NumRenderObject;
    BSDRenderObjectElement_t *RenderObjectList;
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
    
    char UnkBytes[24];
    
} BSDEntryTable_t;

typedef struct BSDTableElement_s {
    int Size;
    int Offset;
    int u1;
    int u2;
    int u3;
} BSDTableElement_t;

typedef struct BSDTable_s {
    int NumElements;
    BSDTableElement_t ElementList[40];
} BSDTable_t;

typedef struct BSDTSPInfo_s {
    char TSPPattern[128];
    int  NumTSP;
    int  TargetInitialCompartment; // This should indicate the number of compartment we have to draw at start.
    int  StartingComparment; // This should indicate the first compartment to render...from StartingComparment to TargetInitialCompartment
    int  u3; // Always 0...Maybe PAD
} BSDTSPInfo_t;

typedef struct BSD_Header_Element_s {
    int Value;
} BSD_Header_Element_t;

typedef struct BSD_HEADER_s {
    int NumHeadElements;
    int Sector[511];
} BSD_HEADER_t;

typedef struct BSDFace_s {
    BSDUv_t UV0;
    short TSB;
    BSDUv_t UV1;
    short TexInfo;
    BSDUv_t UV2;
    short Pad;
    unsigned int VData;
} BSDFace_t;

//TODO: CLEANUP
typedef struct BSDRenderObject_s {
    int             Type;
    BSDPosition_t  *Vertex;
    BSDFace_t      *Face;
    int             NumFaces;
    Vao_t          *Vao;
    Vao_t          *FaceVao;
    Vec3_t          Position;
    Vec3_t          Rotation;

    unsigned int    RenderObjectID;
    struct BSDRenderObject_s *Next;
} BSDRenderObject_t;

typedef struct BSDProperty_s {
    Byte NumNodes;
    unsigned short *NodeList;
} BSDProperty_t;
typedef struct BSDPropertySetFile_s {
    int  NumProperties;
    BSDProperty_t *Property;
} BSDPropertySetFile_t;

typedef struct BSD_s {
    BSD_HEADER_t Header;
    BSDTSPInfo_t TSPInfo;
    char Unknown[72];
    BSDTable_t   PTable;
    BSDEntryTable_t EntryTable;
    BSDRenderObjectBlock_t RenderObjectTable;
    BSDNodeInfo_t NodeData;
    BSDPropertySetFile_t PropertySetFile;
    BSDRenderObject_t *RenderObjectList;
    
    BSDRenderObject_t *RenderObjectRealList;
    BSDRenderObject_t *RenderObjectShowCaseList;
    //
    Vao_t       *NodeVao;
    Vao_t       *RenderObjectPointVao;
    int          NumRenderObjectPoint;
    //Easy lookup to see if the camera is in that node position so that
    //we can update the TSPNumberRenderList.
    BSDTSPStreamNode_t *TSPStreamNodeList;
} BSD_t;

struct Level_s;
typedef struct Level_s Level_t;

BSD_t  *BSDLoad(char *FName,int MissionNumber);
void    BSDCheckCompartmentTrigger(Level_t *Level,Vec3_t CameraPosition);
char   *BSDNodeGetEnumStringFromNodeID(unsigned int NodeID);
char   *BSDRenderObjectGetEnumStringFromType(int RenderObjectType);
Vec3_t  BSDGetPlayerSpawn(BSD_t *BSD);
void    BSDVAOBoxList(BSD_t *BSD);
void    BSDVAOPointList(BSD_t *BSD);
void    BSDVAOObjectList(BSD_t *BSD);
void    BSDVAOTexturedObjectList(BSD_t *BSD);
void    BSDSpawnNodes(BSD_t *BSD);
void    BSDSpawnShowCase(BSD_t *BSD);
void    BSDDraw(Level_t *Level);
unsigned int BSDNodeIDToRenderObjectID(unsigned int NodeID);
unsigned int BSDMPNodeIDToRenderObjectID(unsigned int NodeID);
int     BSDGetRenderObjectIndexByID(BSD_t *BSD,int ID);
bool    BSDIsRenderObjectPresent(BSD_t *BSD,unsigned int RenderObjectID);
void    BSDFixRenderObjectPosition(Level_t *Level);
void    BSDFree(BSD_t *BSD);

#endif //__BSDVIEWER_H_
