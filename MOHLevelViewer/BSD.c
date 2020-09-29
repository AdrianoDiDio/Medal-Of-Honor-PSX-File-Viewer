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

#include "MOHLevelViewer.h"

/*
    1_1.BSD Compartment Trigger => 3246.604492;9.330523;-8456.515625
    673.832092;22.795897;-3504.162842
    
    FIXME:RenderObjectList,RenderObjectRealList,RenderObjectShowCaseList CLEANUP!
*/

void BSDRenderObjectListCleanUp(BSDRenderObject_t *RenderObjectList)
{
    BSDRenderObject_t *Temp;
    
    while( RenderObjectList ) {
        Temp = RenderObjectList;
        if( RenderObjectList->Face != NULL ) {
            free(RenderObjectList->Face);
        }
        if( RenderObjectList->Vertex != NULL ) {
            free(RenderObjectList->Vertex);
        }
        VaoFree(RenderObjectList->Vao);
        VaoFree(RenderObjectList->FaceVao);
        RenderObjectList = RenderObjectList->Next;
        free(Temp);
    }
}

void BSDFree(BSD_t *BSD)
{
    int i;
    
    BSDTSPStreamNode_t *Temp;
    
    free(BSD->NodeData.Table);
    free(BSD->NodeData.Node);

    for( i = 0; i < BSD->PropertySetFile.NumProperties; i++ ) {
        free(BSD->PropertySetFile.Property[i].Data);
    }
    free(BSD->PropertySetFile.Property);
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        free(BSD->RenderObjectList[i].Face);            
        free(BSD->RenderObjectList[i].Vertex);            
    }
    free(BSD->RenderObjectTable.RenderObjectList);
    free(BSD->RenderObjectList);
    BSDRenderObjectListCleanUp(BSD->RenderObjectRealList);
    BSDRenderObjectListCleanUp(BSD->RenderObjectShowCaseList);
    VaoFree(BSD->NodeVao);
    VaoFree(BSD->NodeBoxVao);
    VaoFree(BSD->RenderObjectPointVao);
    
    while( BSD->TSPStreamNodeList ) {
        Temp = BSD->TSPStreamNodeList;
        BSD->TSPStreamNodeList = BSD->TSPStreamNodeList->Next;
        free(Temp);
    }
    free(BSD);
}


void BSDVAOPointList(BSD_t *BSD)
{
    int NumSkip;
    float *NodeData;
    int    NodeDataPointer;
    int    Stride;
    int    NodeDataSize;
    int    i;
    
    Stride = (3 + 3) * sizeof(float);
    NodeDataSize = Stride * BSD->NodeData.Header.NumNodes;
    NodeData = malloc(NodeDataSize);
    NodeDataPointer = 0;
    NumSkip = 0;
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
//         if( BSD->NodeData.Node[i].Id != BSD_PICKUP_OBJECT ) {
//             NumSkip++;
//             continue;
//         }
        NodeData[NodeDataPointer] =   BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer+1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer+2] = BSD->NodeData.Node[i].Position.z;
//         NodeData[NodeDataPointer] =   BSD->NodeData.Node[i].Position.x - (BSD->NodeData.Node[i].PositionMax.x / 2);
//         NodeData[NodeDataPointer+1] = BSD->NodeData.Node[i].Position.y - (BSD->NodeData.Node[i].PositionMax.y / 2);
//         NodeData[NodeDataPointer+2] = BSD->NodeData.Node[i].Position.z - (BSD->NodeData.Node[i].PositionMax.z / 2);
        if( BSD->NodeData.Node[i].Id == BSD_TSP_LOAD_TRIGGER ) {
            // BLUE
            NodeData[NodeDataPointer+3] = 0.f;
            NodeData[NodeDataPointer+4] = 0.f;
            NodeData[NodeDataPointer+5] = 1.f;
        } else if (BSD->NodeData.Node[i].Id == BSD_PLAYER_SPAWN ) {
            // GREEN
            NodeData[NodeDataPointer+3] = 0.f;
            NodeData[NodeDataPointer+4] = 1.f;
            NodeData[NodeDataPointer+5] = 0.f;
        } else if( BSD->NodeData.Node[i].Id == BSD_NODE_SCRIPT ) {
            // Yellow
            NodeData[NodeDataPointer+3] = 1.f;
            NodeData[NodeDataPointer+4] = 1.f;
            NodeData[NodeDataPointer+5] = 0.f;
        } else if( BSD->NodeData.Node[i].Id == BSD_ANIMATED_OBJECT ) {
            // Fuchsia -- Interactive Objects (Door,Objectives,MG42).
            NodeData[NodeDataPointer+3] = 1.f;
            NodeData[NodeDataPointer+4] = 0.f;
            NodeData[NodeDataPointer+5] = 1.f;
        } else if ( BSD->NodeData.Node[i].Id == BSD_LADDER ) {
            // Orange
            NodeData[NodeDataPointer+3] = 1.f;
            NodeData[NodeDataPointer+4] = 0.64f;
            NodeData[NodeDataPointer+5] = 0.f;
        } else {
            // RED
            NodeData[NodeDataPointer+3] = 1.f;
            NodeData[NodeDataPointer+4] = 0.f;
            NodeData[NodeDataPointer+5] = 0.f;
        }
        NodeDataPointer += 6;
    }
    BSD->NodeVao = VaoInitXYZRGB(NodeData,NodeDataSize - (Stride * NumSkip),Stride,0,3);            
    free(NodeData);
}
void BSDVAORenderObjectPointList(BSD_t *BSD)
{
    BSDRenderObject_t *Iterator;
    int NumSkip;
    float *RenderObjectData;
    int    RenderObjectDataPointer;
    int    Stride;
    int    RenderObjectDataSize;
    int    i;
    
    Stride = (3 + 3) * sizeof(float);
    RenderObjectDataSize = Stride * BSD->NumRenderObjectPoint;
    RenderObjectData = malloc(RenderObjectDataSize);
    RenderObjectDataPointer = 0;
    NumSkip = 0;
    i = 0;
    for( Iterator = BSD->RenderObjectRealList; Iterator; Iterator = Iterator->Next ) {

        RenderObjectData[RenderObjectDataPointer] =   Iterator->Position.x;
        RenderObjectData[RenderObjectDataPointer+1] = Iterator->Position.y;
        RenderObjectData[RenderObjectDataPointer+2] = Iterator->Position.z;
        if( Iterator->Type == BSD_RENDER_OBJECT_ENEMY ) {
            // BLUE
            RenderObjectData[RenderObjectDataPointer+3] = 0.f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        } else if (Iterator->Type == BSD_RENDER_OBJECT_DOOR ) {
            // GREEN
            RenderObjectData[RenderObjectDataPointer+3] = 0.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.f;
        } else if( Iterator->Type == BSD_RENDER_OBJECT_MG42 ) {
            // Yellow
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.f;
        } else if( Iterator->Type == BSD_RENDER_OBJECT_PLANE ) {
            // Fuchsia -- Plane.
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        } else if ( Iterator->Type == BSD_RENDER_OBJECT_UNKNOWN1 ) {
            // Maroon
            RenderObjectData[RenderObjectDataPointer+3] = 0.5f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.f;
        } else if( Iterator->Type == BSD_RENDER_OBJECT_DESTRUCTIBLE_WINDOW ) {
            // saddlebrown -- 
            RenderObjectData[RenderObjectDataPointer+3] = 0.54f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.27f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.07f;
        } else if( Iterator->Type == BSD_RENDER_OBJECT_UNKNOWN3 ) {
            // White -- 
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        } else if( Iterator->Type == BSD_RENDER_OBJECT_EXPLOSIVE_CHARGE ) {
            // Pink
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.5f;
        } else {
            // RED
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        }
        RenderObjectDataPointer += 6;
        i++;
    }
    BSD->RenderObjectPointVao = VaoInitXYZRGB(RenderObjectData,RenderObjectDataSize - (Stride * NumSkip),Stride,0,3);            
    free(RenderObjectData);
}
#if 0
void BSDVAOBoxList(BSD_t *BSD)
{
    int NumSkip;
    float *NodeData;
    int    NodeDataPointer;
    int    Stride;
    int    NodeDataSize;
    int    i;
    
    Stride = (48 + 3) * sizeof(float);
    NodeDataSize = Stride * BSD->NodeData.Header.NumNodes;
    NodeData = malloc(NodeDataSize);
    NodeDataPointer = 0;
    NumSkip = 0;
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
//         if( BSD->NodeData.Node[i].Id != BSD_PICKUP_OBJECT ) {
//             NumSkip++;
//             continue;
//         }
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].Position.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].PositionMax.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].PositionMax.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].PositionMax.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].PositionMax.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].Position.z;
        NodeDataPointer += 3;
        
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].PositionMax.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].Position.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].PositionMax.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].PositionMax.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].PositionMax.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].PositionMax.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].PositionMax.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].PositionMax.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].PositionMax.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].Position.z;
        NodeDataPointer += 3;
        
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].Position.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].PositionMax.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].Position.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].PositionMax.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].PositionMax.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].PositionMax.z;
        NodeDataPointer += 3;
        
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].PositionMax.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].Position.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].PositionMax.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].PositionMax.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].Position.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].PositionMax.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].PositionMax.z;
        NodeDataPointer += 3;
        NodeData[NodeDataPointer] = BSD->NodeData.Node[i].PositionMax.x;
        NodeData[NodeDataPointer + 1] = BSD->NodeData.Node[i].PositionMax.y;
        NodeData[NodeDataPointer + 2] = BSD->NodeData.Node[i].PositionMax.z;
        NodeDataPointer += 3;
        if( BSD->NodeData.Node[i].Id == BSD_TSP_LOAD_TRIGGER ) {
            NodeData[NodeDataPointer] = 0.f;
            NodeData[NodeDataPointer+1] = 0.f;
            NodeData[NodeDataPointer+2] = 1.f;
        }
        NodeDataPointer += 3;
    }
    BSD->NodeBoxVao = VaoInitXYZRGB(NodeData,NodeDataSize - (Stride * NumSkip),Stride,0,3);            
    free(NodeData);
}
#endif

void BSDVAOObjectList(BSD_t *BSD)
{
    float  *NodeData;
    int     NodeDataPointer;
    int     Stride;
    int     NodeDataSize;
    int     i;
    int     j;
    

    NodeDataPointer = 0;
    
    Stride = (3 + 3) * sizeof(float);
    NodeDataSize = 0;

    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObjectList[i].VertOffset == 0 ) {
            continue;
        }
        NodeDataSize = BSD->RenderObjectTable.RenderObjectList[i].NumVertex * Stride;
        NodeData = malloc(NodeDataSize);
        NodeDataPointer = 0;
        for( j = 0; j < BSD->RenderObjectTable.RenderObjectList[i].NumVertex; j++ ) {
            NodeData[NodeDataPointer] =   BSD->RenderObjectList[i].Vertex[j].x;
            NodeData[NodeDataPointer+1] = BSD->RenderObjectList[i].Vertex[j].y;
            NodeData[NodeDataPointer+2] = BSD->RenderObjectList[i].Vertex[j].z;
            // BLUE
            NodeData[NodeDataPointer+3] = 0.f;
            NodeData[NodeDataPointer+4] = 0.f;
            NodeData[NodeDataPointer+5] = 1.f;
            NodeDataPointer += 6;
        }
        BSD->RenderObjectList[i].Vao = VaoInitXYZRGB(NodeData,NodeDataSize/* - (Stride * NumSkip)*/,Stride,0,3);
        free(NodeData);
    }

}

void BSDVAOTexturedObjectList(BSD_t *BSD)
{
    float Width = 255.f;
    float Height = 255.f;
    int i;
    int j;
    float *VertexData;
    int VertexSize;
    int VertexPointer;
    int Stride;
    int VertexOffset;
    int TextureOffset;

    for( i = 0; i <BSD->RenderObjectTable.NumRenderObject; i++  ) {
        if( BSD->RenderObjectTable.RenderObjectList[i].FaceOffset == 0 ) {
            continue;
        }
        for( j = 0; j < BSD->RenderObjectList[i].NumFaces; j++ ) {
            Vao_t *Vao;
            unsigned short Vert0;
            unsigned short Vert1;
            unsigned short Vert2;
            Vert0 = (BSD->RenderObjectList[i].Face[j].VData & 0xFF);
            Vert1 = (BSD->RenderObjectList[i].Face[j].VData & 0x3fc00) >> 10;
            Vert2 = (BSD->RenderObjectList[i].Face[j].VData & 0xFF00000 ) >> 20;

            float U0 = (((float)BSD->RenderObjectList[i].Face[j].UV0.u)/Width);
            float V0 = /*255 -*/(((float)BSD->RenderObjectList[i].Face[j].UV0.v) / Height);
            float U1 = (((float)BSD->RenderObjectList[i].Face[j].UV1.u) / Width);
            float V1 = /*255 -*/(((float)BSD->RenderObjectList[i].Face[j].UV1.v) /Height);
            float U2 = (((float)BSD->RenderObjectList[i].Face[j].UV2.u) /Width);
            float V2 = /*255 -*/(((float)BSD->RenderObjectList[i].Face[j].UV2.v) / Height);
            
            Stride = (3 + 2) * sizeof(float);
    
            VertexOffset = 0;
            TextureOffset = 3;
        
            VertexSize = /*BSD->RenderObjectList[i].NumFaces **/ Stride;
            VertexData = malloc(VertexSize * 3/** sizeof(float)*/);
            VertexPointer = 0;
            
            VertexData[VertexPointer] =   BSD->RenderObjectList[i].Vertex[Vert0].x;
            VertexData[VertexPointer+1] = BSD->RenderObjectList[i].Vertex[Vert0].y;
            VertexData[VertexPointer+2] = BSD->RenderObjectList[i].Vertex[Vert0].z;
            VertexData[VertexPointer+3] = U1;
            VertexData[VertexPointer+4] = V1;
            VertexPointer += 5;
            
            VertexData[VertexPointer] =   BSD->RenderObjectList[i].Vertex[Vert1].x;
            VertexData[VertexPointer+1] = BSD->RenderObjectList[i].Vertex[Vert1].y;
            VertexData[VertexPointer+2] = BSD->RenderObjectList[i].Vertex[Vert1].z;
            VertexData[VertexPointer+3] = U0;
            VertexData[VertexPointer+4] = V0;
            VertexPointer += 5;
            
            VertexData[VertexPointer] =   BSD->RenderObjectList[i].Vertex[Vert2].x;
            VertexData[VertexPointer+1] = BSD->RenderObjectList[i].Vertex[Vert2].y;
            VertexData[VertexPointer+2] = BSD->RenderObjectList[i].Vertex[Vert2].z;
            VertexData[VertexPointer+3] = U2;
            VertexData[VertexPointer+4] = V2;
            VertexPointer += 5;
            
            Vao = VaoInitXYZUV(VertexData,VertexSize * 3,Stride,VertexOffset,TextureOffset,BSD->RenderObjectList[i].Face[j].TexInfo,-1);
            Vao->Next = BSD->RenderObjectList[i].FaceVao;
            BSD->RenderObjectList[i].FaceVao = Vao;
            free(VertexData);
        }
    }
}

void BSDAddNodeToRenderObjectList(BSD_t *BSD,int MissionNumber,unsigned int NodeID,Vec3_t Position,Vec3_t Rotation)
{
//         float Width = 255.f;
//     float Height = 255.f;
//     BSDUBlockElement_t *Element;
//     BSDUObject_t *ElementData;
    BSDRenderObject_t *Object;
//     int UBlockIndex;
    unsigned int RenderObjectID;
    int RenderObjectIndex;
//     float *VertexData;
//     int VertexSize;
//     int VertexPointer;
//     int Stride;
//     int VertexOffset;
//     int TextureOffset;
//     int i;
//     
//     RenderBlockID = BSDNodeIDToRenderBlockID(NodeID);
//     
//     if( RenderBlockID != -1 ) {
//         UBlockIndex = BSDGetUBlockIndexByID(BSD,RenderBlockID);
//     } else {
//         UBlockIndex = BSDGetUBlockIndexByID(BSD,NodeID);
//     }
//     if( UBlockIndex == -1 ) {
//         DPrintf("Failed adding new object...UBlockIndex not found.\n");
//         return;
//     }
//     Element = &BSD->UBlock.UList[UBlockIndex];
//     
//     ElementData = &BSD->UObjectList[UBlockIndex];
//     DPrintf("For Node ID %u using RenderBlockID %u\n",NodeID,RenderBlockID);
//     if( ElementData->NumFaces == 0 ) {
//         DPrintf("Failed adding new object...Invalid NumFace %i\n",ElementData->NumFaces);
//         return;
//     }
//     DPrintf("We have %i faces for block Index:%i ID:%u\n",ElementData->NumFaces,UBlockIndex,RenderBlockID);
//     for( i = 0; i < ElementData->NumFaces; i++ ) {
//         Vao_t *Vao;
//         unsigned short Vert0;
//         unsigned short Vert1;
//         unsigned short Vert2;
//         DPrintf("Element faceVData is %u\n",ElementData->Face[i].VData);
//         Vert0 = (ElementData->Face[i].VData & 0xFF);
//         Vert1 = (ElementData->Face[i].VData & 0x3fc00) >> 10;
//         Vert2 = (ElementData->Face[i].VData & 0xFF00000 ) >> 20;
// 
//         float U0 = (((float)ElementData->Face[i].UV0.u)/Width);
//         float V0 = /*255 -*/(((float)ElementData->Face[i].UV0.v) / Height);
//         float U1 = (((float)ElementData->Face[i].UV1.u) / Width);
//         float V1 = /*255 -*/(((float)ElementData->Face[i].UV1.v) /Height);
//         float U2 = (((float)ElementData->Face[i].UV2.u) /Width);
//         float V2 = /*255 -*/(((float)ElementData->Face[i].UV2.v) / Height);
//             
//         Stride = (3 + 2) * sizeof(float);
//     
//         VertexOffset = 0;
//         TextureOffset = 3;
//     
//         VertexSize = /*BSD->UObjectList[i].NumFaces **/ Stride;
//         VertexData = malloc(VertexSize * 3/** sizeof(float)*/);
//         VertexPointer = 0;
//             
//         //ORDER IS U1V1 U0V0 U2V2 for most
//         
//         VertexData[VertexPointer] =   ElementData->Vertex[Vert0].x;
//         VertexData[VertexPointer+1] = ElementData->Vertex[Vert0].y;
//         VertexData[VertexPointer+2] = ElementData->Vertex[Vert0].z;
//         VertexData[VertexPointer+3] = U0;
//         VertexData[VertexPointer+4] = V0;
//         VertexPointer += 5;
//             
//         VertexData[VertexPointer] =   ElementData->Vertex[Vert1].x;
//         VertexData[VertexPointer+1] = ElementData->Vertex[Vert1].y;
//         VertexData[VertexPointer+2] = ElementData->Vertex[Vert1].z;
//         VertexData[VertexPointer+3] = U1;
//         VertexData[VertexPointer+4] = V1;
//         VertexPointer += 5;
//             
//         VertexData[VertexPointer] =   ElementData->Vertex[Vert2].x;
//         VertexData[VertexPointer+1] = ElementData->Vertex[Vert2].y;
//         VertexData[VertexPointer+2] = ElementData->Vertex[Vert2].z;
//         VertexData[VertexPointer+3] = U2;
//         VertexData[VertexPointer+4] = V2;
//         VertexPointer += 5;
//         Vao = VaoInitXYZUV(VertexData,VertexSize * 3,Stride,VertexOffset,TextureOffset,ElementData->Face[i].TexInfo,-1);
//         Vao->Next = Object->FaceVao;
//         Object->FaceVao = Vao;
//         free(VertexData);
//     }
//     RenderBlockID = BSDNodeIDToRenderBlockID(NodeID);
    if( MissionNumber == 12 ) {
        RenderObjectID = BSDMPNodeIDToRenderObjectID(NodeID);
    } else {
        RenderObjectID = BSDNodeIDToRenderObjectID(NodeID);
    }

    RenderObjectIndex = BSDGetRenderObjectIndexByID(BSD,RenderObjectID);

    if( RenderObjectIndex == -1 ) {
        DPrintf("Failed adding new object...ID %u doesn't match any.\n",NodeID);
        return;
    }
    DPrintf("RenderObjectID %u for node %u Index %i\n",RenderObjectID,NodeID,RenderObjectIndex);

    Object = malloc(sizeof(BSDRenderObject_t));
    Object->Type = BSD->RenderObjectTable.RenderObjectList[RenderObjectIndex].Type;
    Object->Position = Position;
    Object->Face = NULL;
    Object->Vertex = NULL;
    //PSX GTE Uses 4096 as unit value only when dealing with fixed math operation.
    //When dealing with rotation then 4096 = 360 degrees.
    //We need to map it back to OpenGL standard format [0;360]. 
    Object->Rotation.x = (Rotation.x  / 4096) * 360.f;
    Object->Rotation.y = (Rotation.y  / 4096) * 360.f;
    Object->Rotation.z = (Rotation.z  / 4096) * 360.f;

    Object->RenderObjectID = RenderObjectID;
    Object->FaceVao = NULL;
    Object->Vao = NULL;
    Object->Next = BSD->RenderObjectRealList;
    BSD->RenderObjectRealList = Object;
    BSD->NumRenderObjectPoint++;
}

void BSDShowCaseRenderObject(BSD_t *BSD)
{
    BSDRenderObject_t *Object;
    int i;
    Vec3_t PSpawn;
    
    PSpawn = BSDGetPlayerSpawn(BSD);
    
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
//         if( /*BSD->RenderObjectTable.RenderObjectList[i].ID != 1342027320 ||*/
//             BSD->RenderObjectTable.RenderObjectList[i].Type != 5125 ) {
//             continue;
//         }
        Object = malloc(sizeof(BSDRenderObject_t));
        Object->Type = BSD->RenderObjectTable.RenderObjectList[i].Type;
        Object->Position.x = PSpawn.x - (i * 200.f);
        Object->Position.y = PSpawn.y;
        Object->Position.z = -PSpawn.z;
        //PSX GTE Uses 4096 as unit value only when dealing with fixed math operation.
        //When dealing with rotation then 4096 = 360 degrees.
        //We need to map it back to OpenGL standard format [0;360]. 
        Object->Rotation.x = 0;
        Object->Rotation.y = 0;
        Object->Rotation.z = 0;

        Object->RenderObjectID = BSD->RenderObjectTable.RenderObjectList[i].ID;
        Object->Vertex = NULL;
        Object->Face = NULL;
        Object->FaceVao = NULL;
        Object->Vao = NULL;
        Object->Next = BSD->RenderObjectShowCaseList;
        BSD->RenderObjectShowCaseList = Object;
    }
}

void BSDSetNodeVao(BSD_t *BSD,BSDRenderObject_t *Object)
{
    float Width = 255.f;
    float Height = 255.f;
    BSDRenderObject_t *RenderObjectData;
    int RenderObjectIndex;
    float *VertexData;
    int VertexSize;
    int VertexPointer;
    int Stride;
    int VertexOffset;
    int TextureOffset;
    int i;
    

    RenderObjectIndex = BSDGetRenderObjectIndexByID(BSD,Object->RenderObjectID);
    if( RenderObjectIndex == -1 ) {
        DPrintf("Failed setting vao...RenderObjectID %u not found.\n",Object->RenderObjectID);
        return;
    }    
    RenderObjectData = &BSD->RenderObjectList[RenderObjectIndex];

    if( RenderObjectData->NumFaces == 0 ) {
        DPrintf("Failed setting vao...Invalid NumFace %i\n",RenderObjectData->NumFaces);
        return;
    }
    for( i = 0; i < RenderObjectData->NumFaces; i++ ) {
        Vao_t *Vao;
        unsigned short Vert0;
        unsigned short Vert1;
        unsigned short Vert2;

        Vert0 = (RenderObjectData->Face[i].VData & 0xFF);
        Vert1 = (RenderObjectData->Face[i].VData & 0x3fc00) >> 10;
        Vert2 = (RenderObjectData->Face[i].VData & 0xFF00000 ) >> 20;

        float U0 = (((float)RenderObjectData->Face[i].UV0.u)/Width);
        float V0 = /*255 -*/(((float)RenderObjectData->Face[i].UV0.v) / Height);
        float U1 = (((float)RenderObjectData->Face[i].UV1.u) / Width);
        float V1 = /*255 -*/(((float)RenderObjectData->Face[i].UV1.v) /Height);
        float U2 = (((float)RenderObjectData->Face[i].UV2.u) /Width);
        float V2 = /*255 -*/(((float)RenderObjectData->Face[i].UV2.v) / Height);
            
        Stride = (3 + 2) * sizeof(float);
    
        VertexOffset = 0;
        TextureOffset = 3;
    
        VertexSize = Stride;
        VertexData = malloc(VertexSize * 3/** sizeof(float)*/);
        VertexPointer = 0;
                    
        VertexData[VertexPointer] =   RenderObjectData->Vertex[Vert0].x;
        VertexData[VertexPointer+1] = RenderObjectData->Vertex[Vert0].y;
        VertexData[VertexPointer+2] = RenderObjectData->Vertex[Vert0].z;
        VertexData[VertexPointer+3] = U0;
        VertexData[VertexPointer+4] = V0;
        VertexPointer += 5;
            
        VertexData[VertexPointer] =   RenderObjectData->Vertex[Vert1].x;
        VertexData[VertexPointer+1] = RenderObjectData->Vertex[Vert1].y;
        VertexData[VertexPointer+2] = RenderObjectData->Vertex[Vert1].z;
        VertexData[VertexPointer+3] = U1;
        VertexData[VertexPointer+4] = V1;
        VertexPointer += 5;
            
        VertexData[VertexPointer] =   RenderObjectData->Vertex[Vert2].x;
        VertexData[VertexPointer+1] = RenderObjectData->Vertex[Vert2].y;
        VertexData[VertexPointer+2] = RenderObjectData->Vertex[Vert2].z;
        VertexData[VertexPointer+3] = U2;
        VertexData[VertexPointer+4] = V2;
        VertexPointer += 5;
        Vao = VaoInitXYZUV(VertexData,VertexSize * 3,Stride,VertexOffset,TextureOffset,RenderObjectData->Face[i].TexInfo,-1);
        Vao->Next = Object->FaceVao;
        Object->FaceVao = Vao;
        free(VertexData);
    }
}

void BSDSpawnNodes(BSD_t *BSD)
{
    BSDRenderObject_t *Iterator;
//     Vec3_t NodePosition;
//     int RenderBlockID;
//     int i;
    if( !BSD ) {
        DPrintf("BSDSpawnNodes:Invalid BSD\n");
        return;
    }
    
    for( Iterator = BSD->RenderObjectRealList; Iterator; Iterator = Iterator->Next ) {
        DPrintf("Spawning it.\n");
        BSDSetNodeVao(BSD,Iterator);
    }
    BSDVAORenderObjectPointList(BSD);
}

void BSDSpawnShowCase(BSD_t *BSD)
{
    BSDRenderObject_t *Iterator;
//     Vec3_t NodePosition;
//     int RenderBlockID;
//     int i;
    if( !BSD ) {
        DPrintf("BSDSpawnShowCase:Invalid BSD\n");
        return;
    }
    
    for( Iterator = BSD->RenderObjectShowCaseList; Iterator; Iterator = Iterator->Next ) {
        DPrintf("BSDSpawnShowCase:Spawning it.\n");
        BSDSetNodeVao(BSD,Iterator);
    }
}
Vec3_t BSDGetPlayerSpawn(BSD_t *BSD)
{
    Vec3_t PlayerSpawn;
    int i;
    
    PlayerSpawn = Vec3_Build(0,0,0);
    
    if( !BSD ) {
        DPrintf("BSDGetPlayerSpawn:Invalid BSD\n");
        return PlayerSpawn;
    }
    
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        if( BSD->NodeData.Node[i].Id != BSD_PLAYER_SPAWN ) {
            continue;
        }
        PlayerSpawn = Vec3_Build(BSD->NodeData.Node[i].Position.x,BSD->NodeData.Node[i].Position.y,BSD->NodeData.Node[i].Position.z);
        break;
    }
    Vec_RotateXAxis(DEGTORAD(180.f),&PlayerSpawn);
//     Vec3_t Temp;
//     Temp.x = Temp.y = Temp.z = 0.f;
//     return Temp;
    return PlayerSpawn;
}
int BSDGetRenderObjectIndexByID(BSD_t *BSD,int ID)
{
    int i;
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObjectList[i].ID == ID ) {
            return i;
        }
    }
    return -1;
}

BSDRenderObject_t *BSDGetRenderObjectByID(BSD_t *BSD,int ID)
{
    int i;
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObjectList[i].ID == ID ) {
            return &BSD->RenderObjectList[i];
        }
    }
    return NULL;
}
bool isPointInsideAABB(Vec3_t Position, Vec3_t BoxMin,Vec3_t BoxMax)
{
  return (Position.x >= BoxMin.x && Position.x <= BoxMax.x) &&
         (Position.y >= BoxMin.y && Position.y <= BoxMax.y) &&
         (Position.z >= BoxMin.z && Position.z <= BoxMax.z);
}
void BSDCheckCompartmentTrigger(Level_t *Level,Vec3_t CameraPosition)
{
//     int i;
//     for( i = 0; i < Level->BSD->NodeData.Header.NumNodes; i++ ) {
//         Vec3_t Min;
//         Vec3_t Max;
//         
//         Min.x = Level->BSD->NodeData.Node[i].Position.x;
//         Min.y = Level->BSD->NodeData.Node[i].Position.y;
//         Min.z = Level->BSD->NodeData.Node[i].Position.z;
//         
//         Max.x = Min.x + -Level->BSD->NodeData.Node[i].PositionMax.x;
//         Max.y = Min.y + -Level->BSD->NodeData.Node[i].PositionMax.y;
//         Max.z = Min.z + Level->BSD->NodeData.Node[i].PositionMax.z;
//         if( isPointInsideAABB(CameraPosition,Min,Max) ) {
//             DPrintf("Camera touching node %s\n",BSDNodeGetEnumStringFromValue(Level->BSD->NodeData.Node[i].Id));
//             return;
//         }
//     }
}

unsigned int BSDNodeIDToRenderObjectID(unsigned int NodeID)
{
    unsigned int RenderObjectID;
// TODO:Look at the assembly to find any other RenderBlockID/NodeRenderBlockID.
//  1821346472 -> 519275822
//  935865828 -> 2278882431
//  149114796 -> 1686008476
//  4262665218 -> 1771441436
//  2622993903 -> 1771441436
//  3832727321 -> 1335595487
//  3815705185 -> 137063914
//  1730263601 -> 1771441436
//  3363099888 -> 1771441436
//  648959524 -> 1771441436
//  427407355 -> 1771441436
//  2966651809 -> 1771441436
//  1444155995 -> 137063914
//  1088149327 -> 1771441436
//  1696717421 -> 1771441436
//  633670612 -> 3487282118
//  2470021088 -> 1909080654
//  2522430033 -> 2018764808
//  1247503060 -> 4294967295
//  3545379694 -> 4294967295
    switch ( NodeID ) {
        case 1821346472:
            RenderObjectID = 519275822;
            break;
        case 935865828:
            RenderObjectID = 2278882431;
            break;
        case 149114796:
            RenderObjectID = 1686008476;
            break;
        case 4262665218:
            RenderObjectID = 1771441436;
            break;
        case 2622993903:
            RenderObjectID = 1771441436;
            break;
        case 3832727321:
            RenderObjectID = 1335595487;
            break;
        case 3815705185:
            RenderObjectID = 137063914;
            break;
        case 1730263601:
            RenderObjectID = 1771441436;
            break;
        case 3363099888:
            RenderObjectID = 1771441436;
            break;
        case 648959524:
            RenderObjectID = 1771441436;
            break;
        case 427407355:
            RenderObjectID = 1771441436;
            break;
        case 2966651809:
            RenderObjectID = 1771441436;
            break;
        case 1444155995:
            RenderObjectID = 137063914;
            break;
        case 1088149327:
            RenderObjectID = 1771441436;
            break;
        case 1696717421:
            RenderObjectID = 1771441436;
            break;
        case 633670612:
            RenderObjectID = 3487282118;
            break;
        case 2470021088:
            RenderObjectID = 1909080654;
            break;
        case 2522430033:
            RenderObjectID = 2018764808;
            break;
        case 1247503060:
            RenderObjectID = 4294967295;
            break;
        case 3545379694:
            RenderObjectID = 4294967295;
            break;
        case 0:
            RenderObjectID = 0;
            break;
        default:
            DPrintf("RenderObjectID was not in the mapping table...assuming it is the NodeID itself.\n");
            RenderObjectID = NodeID;
            break;
    }
    return RenderObjectID;
}

unsigned int BSDMPNodeIDToRenderObjectID(unsigned int NodeID)
{
    unsigned int RenderObjectID;
    switch( NodeID ) {
        //NEW
        case 3815705185:
            //Bazooka Ammo.
            RenderObjectID = 3139577012;
            break;
        case 3832727321:
            //SMG Ammo.
            RenderObjectID = 3158305228;
            break;
        case 2622993903:
            //Pistol Ammo.
            RenderObjectID = 523223373;
            break;
        case 1730263601:
            //Grenade Ammo.
            RenderObjectID = 1064080612;
            break;
        case 3363099888:
            //Shotgun Ammo.
            RenderObjectID = 1322820526;
            break;
        case 648959524:
            //Rifle Ammo.
            RenderObjectID = 414830070;
            break;
        case 1444155995:
            //Silencer Ammo.
            RenderObjectID = 1479327691;
            break;
        case 1088149327:
            //German Grenade Ammo.
            RenderObjectID = 1529087242;
            break;
        case 1696717421:
            //BAR Ammo.
            RenderObjectID = 1857331800;
            break;
        case 427407355:
            //Salute Ammo.
            RenderObjectID = 1008432070;
            break;
        //Undefined but found....
        case 1656540076:
            RenderObjectID = 1771441436;
            break;
        case 475277581:
            RenderObjectID = 1771441436;
            break;
        case 1587751891:
            RenderObjectID = 1771441436;
            break;
        case 2044083958:
            RenderObjectID = 1771441436;
            break;
        case 1271113291:
            RenderObjectID = 1771441436;
            break;
        case 2774162518:
            RenderObjectID = 1771441436;
            break;
        case 586892869:
            RenderObjectID = 1771441436;
            break;
        case 807017850:
            //HEALTH
            RenderObjectID = 2278882431;
            break;
        case 254902066:
            RenderObjectID = 1686008476;
            break;
        case 1801304630:
            RenderObjectID = 519275822;
            break;
        case 3482129947:
            RenderObjectID = 1771441436;
            break;
        case 3538476007:
            RenderObjectID = 1771441436;
            break;
        case 3364936547:
            RenderObjectID = 1771441436;
            break;
        default:
            RenderObjectID = NodeID;
            break;
    }
    return RenderObjectID;
}

bool BSDIsRenderObjectPresent(BSD_t *BSD,unsigned int RenderObjectID) {
    if( BSDGetRenderObjectIndexByID(BSD,RenderObjectID) == -1 ) {
        DPrintf("Render Object ID %u not found..\n",RenderObjectID);
        return false;
    }
    return true;
}
char *BSDNodeGetEnumStringFromNodeID(unsigned int NodeID)
{
    switch( NodeID ) {
        case BSD_PLAYER_SPAWN:
            return "Player Spawn";
        case BSD_TSP_LOAD_TRIGGER:
            return "TSP Trigger";
        case BSD_ENEMY_SPAWN:
            return "Enemy Spawn";
        case BSD_DOOR:
            return "Door";
        case BSD_LADDER:
            return "Ladder";
        case BSD_PICKUP_OBJECT:
            return "Pickup Object";
        case BSD_NODE_SCRIPT:
            return "Scripted Node";
        case BSD_ANIMATED_OBJECT:
            return "Animated Object";
        case BSD_DESTRUCTIBLE_WINDOW:
            return "Destructible Window";
        default:
            return "Unknown";
    }
}

char *BSDRenderObjectGetEnumStringFromType(int RenderObjectType)
{
    switch( RenderObjectType ) {
        case BSD_RENDER_OBJECT_CARRY_AUX_ELEMENTS:
            return "Carry Aux Elements";
        case BSD_RENDER_OBJECT_PICKUP_AND_EXPLOSIVE:
            return "Pickup And Explosive";
        case BSD_RENDER_OBJECT_ENEMY:
            return "Enemy Render Object";
        case BSD_RENDER_OBJECT_PLANE:
            return "Airplane";
        case BSD_RENDER_OBJECT_MG42:
            return "MG42";
        case BSD_RENDER_OBJECT_DOOR:
            return "Door";
        case BSD_RENDER_OBJECT_UNKNOWN1:
            return "Unknown1";
        case BSD_RENDER_OBJECT_DESTRUCTIBLE_WINDOW:
            return "Destructible Window";
        case BSD_RENDER_OBJECT_UNKNOWN3:
            return "Unknown3";
        case BSD_RENDER_OBJECT_EXPLOSIVE_CHARGE:
            return "Explosive Charge";
        default:
            return "Unknown";
    }
}

//TODO:Spawn the RenderObject when loading node data!
//     Some nodes don't have a corresponding RenderObject like the PlayerSpawn.
//     BSDSpawnEntity(int UBlockID,Vec3_t NodePos) => Store into a list and transform to vao.
void BSDDraw(Level_t *Level)
{
    GL_Shader_t *Shader;
    BSDRenderObject_t *RenderObjectIterator;
    Vao_t *VaoIterator;
    int MVPMatrixID;
    
    if( Level->Settings.ShowBSDNodes ) {    
        Shader = Shader_Cache("BSDShader","Shaders/BSDVertexShader.glsl","Shaders/BSDFragmentShader.glsl");
        glUseProgram(Shader->ProgramID);

        MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
        glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
        glBindVertexArray(Level->BSD->NodeVao->VaoID[0]);
        glPointSize(10.f);
        glDrawArrays(GL_POINTS, 0, Level->BSD->NodeData.Header.NumNodes);
        glBindVertexArray(0);
        glUseProgram(0);
        
//         glUseProgram(Shader->ProgramID);
// 
//         MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
//         glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
//         glBindVertexArray(Level->BSD->NodeBoxVao->VaoID[0]);
//         glDrawArrays(GL_LINES, 0, Level->BSD->NodeData.Header.NumNodes);
//         glBindVertexArray(0);
//         glUseProgram(0);
    }
    
    if( Level->Settings.ShowBSDRenderObject ) {    
        Shader = Shader_Cache("BSDShader","Shaders/BSDVertexShader.glsl","Shaders/BSDFragmentShader.glsl");
        glUseProgram(Shader->ProgramID);

        MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
        glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
        glBindVertexArray(Level->BSD->RenderObjectPointVao->VaoID[0]);
        glPointSize(10.f);
        glDrawArrays(GL_POINTS, 0, Level->BSD->NumRenderObjectPoint);
        glBindVertexArray(0);
        glUseProgram(0);
        
//         glUseProgram(Shader->ProgramID);
// 
//         MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
//         glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
//         glBindVertexArray(Level->BSD->NodeBoxVao->VaoID[0]);
//         glDrawArrays(GL_LINES, 0, Level->BSD->NodeData.Header.NumNodes);
//         glBindVertexArray(0);
//         glUseProgram(0);
    }
#if 0
    Shader = Shader_Cache("BSDShader","Shaders/BSDVertexShader.glsl","Shaders/BSDFragmentShader.glsl");
    glUseProgram(Shader->ProgramID);
    MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
    glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);

    for( i = 0; i < Level->BSD->UBlock.NumU; i++ ) {
        if( Level->BSD->UBlock.UList[i].VertOffset == 0 ) {
            continue;
        }
        glBindVertexArray(Level->BSD->UObjectList[i].Vao->VaoID[0]);
        glPointSize(15.f);
        glDrawArrays(GL_POINTS, 0, Level->BSD->UBlock.UList[i].NumVertex);
        glBindVertexArray(0);
    }
    glBindVertexArray(0);
    glUseProgram(0);
#endif
    
    
    if( Level->Settings.DrawBSDRenderObjects ) {
            Shader = Shader_Cache("BSDObjectShader","Shaders/BSDObjectVertexShader.glsl","Shaders/BSDObjectFragmentShader.glsl");
            glUseProgram(Shader->ProgramID);
            MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
        for( RenderObjectIterator = Level->BSD->RenderObjectRealList; RenderObjectIterator; 
            RenderObjectIterator = RenderObjectIterator->Next ) {
            vec3 temp;
            glm_mat4_identity(VidConf.ModelViewMatrix);
            temp[0] = 1;
            temp[1] = 0;
            temp[2] = 0;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera.Angle.x), temp);
            temp[0] = 0;
            temp[1] = 1;
            temp[2] = 0;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera.Angle.y), temp);
            temp[0] = 0;
            temp[1] = 0;
            temp[2] = 1;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera.Angle.z), temp);
            temp[0] = -(Camera.Position.x - RenderObjectIterator->Position.x);
            temp[1] = -(Camera.Position.y + RenderObjectIterator->Position.y);
            temp[2] = -(Camera.Position.z + RenderObjectIterator->Position.z);

            glm_translate(VidConf.ModelViewMatrix,temp);
            temp[0] = 0;
            temp[1] = -1;
            temp[2] = 0;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(RenderObjectIterator->Rotation.y), temp);
            temp[0] = 1;
            temp[1] = 0;
            temp[2] = 0;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(RenderObjectIterator->Rotation.x), temp);
            temp[0] = 0;
            temp[1] = 0;
            temp[2] = 1;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(RenderObjectIterator->Rotation.z), temp);
            glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
            
            //Emulate PSX Coordinate system...
            glm_rotate_x(VidConf.MVPMatrix,glm_rad(180.f), VidConf.MVPMatrix);
            glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);

            for( VaoIterator = RenderObjectIterator->FaceVao; VaoIterator; VaoIterator = VaoIterator->Next ) {
                int VRamPage = VaoIterator->TSB & 0x3F;
                int ColorMode = (VaoIterator->TSB & 0xC0) >> 7;
                
                if( ColorMode == 1 ) {
                    glBindTexture(GL_TEXTURE_2D,Level->VRam->Page8Bit[VRamPage].TextureID);
                } else {
                    glBindTexture(GL_TEXTURE_2D,Level->VRam->Page4Bit[VRamPage].TextureID);
                }
                glBindVertexArray(VaoIterator->VaoID[0]);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D,0);
            }
        }
        glUseProgram(0);
    }
    
    if( Level->Settings.DrawBSDShowCaseRenderObject ) {
        Shader = Shader_Cache("BSDObjectShader","Shaders/BSDObjectVertexShader.glsl","Shaders/BSDObjectFragmentShader.glsl");
        glUseProgram(Shader->ProgramID);
        MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
        for( RenderObjectIterator = Level->BSD->RenderObjectShowCaseList; RenderObjectIterator; 
                RenderObjectIterator = RenderObjectIterator->Next ) {
            vec3 temp;
            glm_mat4_identity(VidConf.ModelViewMatrix);
            temp[0] = 1;
            temp[1] = 0;
            temp[2] = 0;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera.Angle.x), temp);
            temp[0] = 0;
            temp[1] = 1;
            temp[2] = 0;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera.Angle.y), temp);
            temp[0] = 0;
            temp[1] = 0;
            temp[2] = 1;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera.Angle.z), temp);
            temp[0] = -(Camera.Position.x - RenderObjectIterator->Position.x);
            temp[1] = -(Camera.Position.y + RenderObjectIterator->Position.y);
            temp[2] = -(Camera.Position.z + RenderObjectIterator->Position.z);

            glm_translate(VidConf.ModelViewMatrix,temp);
            temp[0] = 0;
            temp[1] = -1;
            temp[2] = 0;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(RenderObjectIterator->Rotation.y), temp);
            temp[0] = 1;
            temp[1] = 0;
            temp[2] = 0;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(RenderObjectIterator->Rotation.x), temp);
            temp[0] = 0;
            temp[1] = 0;
            temp[2] = 1;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(RenderObjectIterator->Rotation.z), temp);
            glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
            
            //Emulate PSX Coordinate system...
            glm_rotate_x(VidConf.MVPMatrix,glm_rad(180.f), VidConf.MVPMatrix);
            glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);

            for( VaoIterator = RenderObjectIterator->FaceVao; VaoIterator; VaoIterator = VaoIterator->Next ) {
                int VRamPage = VaoIterator->TSB & 0x3F;
                int ColorMode = (VaoIterator->TSB & 0xC0) >> 7;
                
                if( ColorMode == 1 ) {
                    glBindTexture(GL_TEXTURE_2D,Level->VRam->Page8Bit[VRamPage].TextureID);
                } else {
                    glBindTexture(GL_TEXTURE_2D,Level->VRam->Page4Bit[VRamPage].TextureID);
                }
                glBindVertexArray(VaoIterator->VaoID[0]);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D,0);
            }
        }
    }
    
    glBindVertexArray(0);
    glUseProgram(0);
}

void ParseUVertexData(BSD_t *BSD,FILE *BSDFile)
{
    int i;
    int j;
    
    BSD->RenderObjectList = malloc(BSD->RenderObjectTable.NumRenderObject * sizeof(BSDRenderObject_t));
    memset(BSD->RenderObjectList,0,BSD->RenderObjectTable.NumRenderObject * sizeof(BSDRenderObject_t));
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObjectList[i].VertOffset == 0 ) {
            continue;
        }

        int UVertSize = sizeof(BSDPosition_t) * BSD->RenderObjectTable.RenderObjectList[i].NumVertex;
//         DPrintf("Element Size is %i\n",UVertSize); 
        BSD->RenderObjectList[i].Vertex = malloc(UVertSize);
        memset(BSD->RenderObjectList[i].Vertex,0,UVertSize);
        fseek(BSDFile,BSD->RenderObjectTable.RenderObjectList[i].VertOffset + 2048,SEEK_SET);
        DPrintf("Reading Vertex definition at %i (Current:%i)\n",
                BSD->RenderObjectTable.RenderObjectList[i].VertOffset + 2048,GetCurrentFilePosition(BSDFile)); 
        for( j = 0; j < BSD->RenderObjectTable.RenderObjectList[i].NumVertex; j++ ) {
            DPrintf("Reading Vertex at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
            fread(&BSD->RenderObjectList[i].Vertex[j],sizeof(BSDPosition_t),1,BSDFile);
            DPrintf("Vertex %i;%i;%i %i\n",BSD->RenderObjectList[i].Vertex[j].x,
                    BSD->RenderObjectList[i].Vertex[j].y,BSD->RenderObjectList[i].Vertex[j].z,
                    BSD->RenderObjectList[i].Vertex[j].Pad
            );
            
        }
    }
}

void ParseRenderObjectFaceData(BSD_t *BSD,FILE *BSDFile)
{
    int FaceListSize;
    int i;
    int j;
    
    if( !BSD->RenderObjectList ) {
        DPrintf("ParseUFaceData:Object list is empty...\n");
        return;
    }
    assert(sizeof(BSDFace_t) == 16);
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObjectList[i].FaceOffset == 0 ) {
            continue;
        }
        //Also Grabs the position...
        short x;
        short y;
        short z;
        fseek(BSDFile,BSD->RenderObjectTable.RenderObjectList[i].MatrixOffset + 2048,SEEK_SET);
        fread(&x,sizeof(x),1,BSDFile);
        fread(&y,sizeof(y),1,BSDFile);
        fread(&z,sizeof(z),1,BSDFile);
        BSD->RenderObjectList[i].Position.x = x;
        BSD->RenderObjectList[i].Position.y = y;
        BSD->RenderObjectList[i].Position.z = z;
        Vec_RotateXAxis(DEGTORAD(180.f),&BSD->RenderObjectList[i].Position);
        DPrintf("ParseRenderObjectFaceData:RenderObject ID %u\n",BSD->RenderObjectTable.RenderObjectList[i].ID);
        DPrintf("Face is at %u;%u;%u\n",x,y,z); 
        fseek(BSDFile,BSD->RenderObjectTable.RenderObjectList[i].FaceOffset + 2048,SEEK_SET);
        fread(&BSD->RenderObjectList[i].NumFaces,sizeof(int),1,BSDFile);
        DPrintf("Reading %i faces\n",BSD->RenderObjectList[i].NumFaces);
        FaceListSize = BSD->RenderObjectList[i].NumFaces * sizeof(BSDFace_t);
        BSD->RenderObjectList[i].Face = malloc(FaceListSize);
        memset(BSD->RenderObjectList[i].Face,0,FaceListSize);
        DPrintf("Reading Face definition at %i (Current:%i)\n",BSD->RenderObjectTable.RenderObjectList[i].FaceOffset
            + 2048,GetCurrentFilePosition(BSDFile)); 
        for( j = 0; j < BSD->RenderObjectList[i].NumFaces; j++ ) {
            DPrintf("Reading Face at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
            fread(&BSD->RenderObjectList[i].Face[j],sizeof(BSDFace_t),1,BSDFile);
            DPrintf(" -- FACE %i --\n",j);
            DPrintf("Tex info %i | Color mode %i | Texture Page %i\n",BSD->RenderObjectList[i].Face[j].TexInfo,
                    (BSD->RenderObjectList[i].Face[j].TexInfo & 0xC0) >> 7,BSD->RenderObjectList[i].Face[j].TexInfo & 0x3f);
            DPrintf("TSB is %i %ix%i\n",BSD->RenderObjectList[i].Face[j].TSB,
                    ((BSD->RenderObjectList[i].Face[j].TSB  & 0x3F ) << 4),((BSD->RenderObjectList[i].Face[j].TSB & 0x7FC0) >> 6));
            DPrintf("UV0:(%i;%i)\n",BSD->RenderObjectList[i].Face[j].UV0.u,BSD->RenderObjectList[i].Face[j].UV0.v);
            DPrintf("UV1:(%i;%i)\n",BSD->RenderObjectList[i].Face[j].UV1.u,BSD->RenderObjectList[i].Face[j].UV1.v);
            DPrintf("UV2:(%i;%i)\n",BSD->RenderObjectList[i].Face[j].UV2.u,BSD->RenderObjectList[i].Face[j].UV2.v);
            DPrintf("Pad is %i\n",BSD->RenderObjectList[i].Face[j].Pad);
            unsigned short LV0;
            unsigned short LV1;
            unsigned short LV2;
            LV0 = (BSD->RenderObjectList[i].Face[j].VData & 0xFF);
            LV1 = (BSD->RenderObjectList[i].Face[j].VData & 0x3fc00) >> 10;
            LV2 = (BSD->RenderObjectList[i].Face[j].VData & 0xFF00000 ) >> 20;
            DPrintf("V0|V1|V2:%u;%u;%u\n",LV0,LV1,LV2);
            DPrintf("V0|V1|V2:(%i;%i;%i)|(%i;%i;%i)|(%i;%i;%i)\n",
                    BSD->RenderObjectList[i].Vertex[LV0].x,BSD->RenderObjectList[i].Vertex[LV0].y,BSD->RenderObjectList[i].Vertex[LV0].z,
                    BSD->RenderObjectList[i].Vertex[LV1].x,BSD->RenderObjectList[i].Vertex[LV1].y,BSD->RenderObjectList[i].Vertex[LV1].z,
                    BSD->RenderObjectList[i].Vertex[LV2].x,BSD->RenderObjectList[i].Vertex[LV2].y,BSD->RenderObjectList[i].Vertex[LV2].z);

        }
    }

}

/*
    NOTE(Adriano): 
    BSD File can roughly be divided in 2 sections:
    The Header which has a fixed size of 2048 and the actual BSD File.
    So all the offset which are found below the header needs to be fixed since they are off by 2048.
    This means that the file is not meant to be read sequentially but rather there are offset that tells you
    which part of the file you need to read...only the TSP info section seems to be static.
    
    
    
    YUnknown in Node table info should be the size of the node table!!!
*/

void BSDReadPropertySetFile(BSD_t *BSD,FILE *BSDFile)
{
    int PreviousFilePosition;
    int PropertySetFileOffset;
    int i;
    int j;
    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        DPrintf("BSDReadPropertySetFile: Invalid %s\n",InvalidFile ? "file" : "bsd");
        return;
    }
    
    PreviousFilePosition = GetCurrentFilePosition(BSDFile);
    fseek(BSDFile,sizeof(BSD_HEADER_t) + BSD_PROPERTY_SET_FILE_POSITION,SEEK_SET);
    fread(&PropertySetFileOffset,sizeof(PropertySetFileOffset),1,BSDFile);
    if( PropertySetFileOffset == 0 ) {
        DPrintf("BSDReadPropertySetFile:BSD File has no property file set.\n");
        return;
    }
    fseek(BSDFile,sizeof(BSD_HEADER_t) + PropertySetFileOffset,SEEK_SET);
    fread(&BSD->PropertySetFile.NumProperties,sizeof(BSD->PropertySetFile.NumProperties),1,BSDFile);
    DPrintf("BSDReadPropertySetFile:Reading %i properties at %i (%i).\n",BSD->PropertySetFile.NumProperties,
            GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
    BSD->PropertySetFile.NumProperties++;
    BSD->PropertySetFile.Property = malloc(sizeof(BSDProperty_t) * BSD->PropertySetFile.NumProperties);
    for( i = 0; i < BSD->PropertySetFile.NumProperties; i++ ) {
        DPrintf("BSDReadPropertySetFile:Reading property %i at %i (%i)\n",i,GetCurrentFilePosition(BSDFile),
            GetCurrentFilePosition(BSDFile) - 2048
        );
        fread(&BSD->PropertySetFile.Property[i].Size,sizeof(BSD->PropertySetFile.Property[i].Size),1,BSDFile);
        DPrintf("Property Size: %u\n",BSD->PropertySetFile.Property[i].Size);
        BSD->PropertySetFile.Property[i].Size = (255 - BSD->PropertySetFile.Property[i].Size) /*<< 1*/;
        DPrintf("Property Real size: %u %i shorts\n",BSD->PropertySetFile.Property[i].Size,BSD->PropertySetFile.Property[i].Size /*/ 2*/);
        
        SkipFileSection(BSDFile,1);
        DPrintf("BSDReadPropertySetFile:Reading short list at %i (%i)\n",GetCurrentFilePosition(BSDFile),
            GetCurrentFilePosition(BSDFile) - 2048
        );
        BSD->PropertySetFile.Property[i].Data = malloc(BSD->PropertySetFile.Property[i].Size * sizeof(unsigned short));
        DPrintf("BSDReadPropertySetFile:Reading %li bytes.\n",BSD->PropertySetFile.Property[i].Size * sizeof(unsigned short));
        for( j = 0; j <  BSD->PropertySetFile.Property[i].Size; j++ ) {
            fread(&BSD->PropertySetFile.Property[i].Data[j],sizeof(BSD->PropertySetFile.Property[i].Data[j]),1,BSDFile);
            DPrintf("Short %u\n", BSD->PropertySetFile.Property[i].Data[j]);
//             assert(BSD->PropertySetFile.Property[i].Data[j] < 158);
        }
    }
    DPrintf("BSDReadPropertySetFile:Property end at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
    fseek(BSDFile,PreviousFilePosition,SEEK_SET);
}

BSD_t *BSDLoad(char *FName,int MissionNumber)
{
    FILE *BSDFile;
    BSD_t *BSD;
    BSDTSPStreamNode_t *StreamNode;
    int i;
    int j;
    int Jump;
    int NodeTableEnd;
    short Number;
    int MemBegin;
    int MemEnd;
    int NodeFilePosition;
    int Offset;
    int Delta;
    int NodeNumReferencedRenderObjectIDOffset;
    int NumReferencedRenderObjectID;
    BSDTableElement_t *Element;
    unsigned int NodeRenderObjectID;
    Vec3_t NodePosition;
    Vec3_t NodeRotation;
    int PrevPos;
    
    DPrintf("Loading file %s...\n",FName);
    
    BSDFile = fopen(FName,"rb");
    
    if( BSDFile == NULL ) {
        DPrintf("Failed opening BSD File.\n");
        return NULL;
    }
    BSD = malloc(sizeof(BSD_t));
    
    BSD->TSPStreamNodeList = NULL;
    BSD->RenderObjectRealList = NULL;
    BSD->RenderObjectList = NULL;
    BSD->RenderObjectShowCaseList = NULL;
    
    BSD->NodeVao = NULL;
    BSD->NodeBoxVao = NULL;
    BSD->RenderObjectPointVao = NULL;
    BSD->NumRenderObjectPoint = 0;
    
    assert(sizeof(BSD->Header) == 2048);
    fread(&BSD->Header,sizeof(BSD->Header),1,BSDFile);
    DPrintf("Header contains %i(%#02x) element.\n",BSD->Header.NumHeadElements,BSD->Header.NumHeadElements);
    for( i = 0; i < BSD->Header.NumHeadElements; i++ ) {
        printf("Got %i(%#02x)(%i)\n",BSD->Header.Sector[i],BSD->Header.Sector[i],BSD->Header.Sector[i] >> 0xb);
    }
    //At position 152 after the header we have the SPRITE definitions...
    //Maybe hud/ammo...
    fread(&BSD->TSPInfo,sizeof(BSD->TSPInfo),1,BSDFile);
    DPrintf("Compartment pattern: %s\n",BSD->TSPInfo.TSPPattern);
    DPrintf("Number of compartments: %i\n",BSD->TSPInfo.NumTSP);
    DPrintf("TargetInitialCompartment: %i\n",BSD->TSPInfo.TargetInitialCompartment);
    DPrintf("Starting Compartment: %i\n",BSD->TSPInfo.StartingComparment);
    DPrintf("u3: %i\n",BSD->TSPInfo.u3);
    DPrintf("TSP Block ends at %i\n",GetCurrentFilePosition(BSDFile));
    assert(BSD->TSPInfo.u3 == 0);
    fread(&BSD->Unknown,sizeof(BSD->Unknown),1,BSDFile);
    fread(&BSD->PTable.NumElements,sizeof(BSD->PTable.NumElements),1,BSDFile);
    DPrintf("PTable:Reading %i elements\n",BSD->PTable.NumElements);
    fread(&BSD->PTable.ElementList,sizeof(BSD->PTable.ElementList),1,BSDFile);
    //The PTable offset references the q block(Offset+2048) the size seems to be the given by size times 4
    for( i = 0; i < BSD->PTable.NumElements; i++ ) {
        DPrintf("Size:%i\n",BSD->PTable.ElementList[i].Size);
        DPrintf("Offset:%i\n",BSD->PTable.ElementList[i].Offset);
        DPrintf("Offset No Header:%i\n",BSD->PTable.ElementList[i].Offset + 2048);
        DPrintf("U1:%i\n",BSD->PTable.ElementList[i].u1);
        DPrintf("U2:%i\n",BSD->PTable.ElementList[i].u2);
        DPrintf("U3:%i\n",BSD->PTable.ElementList[i].u3);
    }
    DPrintf("PTable end at %i\n",GetCurrentFilePosition(BSDFile));
    //This section seems unused and should be constant in size (320 bytes).
    //TODO:Remove this useless code and just jump 320 bytes...
    MemBegin = GetCurrentFilePosition(BSDFile);
    fread(&Number,sizeof(Number),1,BSDFile);
    while( Number == -1 ) {
        SkipFileSection(BSDFile,14);
        fread(&Number,sizeof(Number),1,BSDFile);
    }
    fseek(BSDFile,-sizeof(Number),SEEK_CUR);
    MemEnd = GetCurrentFilePosition(BSDFile);
    assert( (MemEnd - MemBegin) == 320 );
    DPrintf("Mem end at %i\n",GetCurrentFilePosition(BSDFile));
    //This file section of 104 bytes contains the begin/end offset of some entries.
    //Like the NodeListTableStart/NodeListEnd after the header (+2048).
    //We can have a max of 26 offsets or 13 Begin/End Definitions.
//     SkipFileSection(BSDFile,104);
    assert(sizeof(BSD->EntryTable) == 104);
    DPrintf("Reading EntryTable at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
    fread(&BSD->EntryTable,sizeof(BSD->EntryTable),1,BSDFile);
    DPrintf("Node table is at %i (%i)\n",BSD->EntryTable.NodeTableOffset,BSD->EntryTable.NodeTableOffset + 2048);
    DPrintf("Unknown data is at %i (%i)\n",BSD->EntryTable.UnknownDataOffset,BSD->EntryTable.UnknownDataOffset + 2048);
    //TODO(Adriano): 492544 in 1_1.BSD references data that is 8 bytes.
    DPrintf("Data0 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off0,BSD->EntryTable.Off0 + 2048,BSD->EntryTable.Num0);
    DPrintf("Data1 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off1,BSD->EntryTable.Off1 + 2048,BSD->EntryTable.Num1);
    DPrintf("Data2 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off2,BSD->EntryTable.Off2 + 2048,BSD->EntryTable.Num2);
    DPrintf("Data3 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off3,BSD->EntryTable.Off3 + 2048,BSD->EntryTable.Num3);
    DPrintf("Data4 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off4,BSD->EntryTable.Off4 + 2048,BSD->EntryTable.Num4);
    DPrintf("Data5 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off5,BSD->EntryTable.Off5 + 2048,BSD->EntryTable.Num5);
    DPrintf("Data6 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off6,BSD->EntryTable.Off6 + 2048,BSD->EntryTable.Num6);
    DPrintf("Data7 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off7,BSD->EntryTable.Off7 + 2048,BSD->EntryTable.Num7);
    DPrintf("Data8 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off8,BSD->EntryTable.Off8 + 2048,BSD->EntryTable.Num8);

    DPrintf("Current Position after entries is %i\n",GetCurrentFilePosition(BSDFile));
    fread(&BSD->RenderObjectTable.NumRenderObject,sizeof(BSD->RenderObjectTable.NumRenderObject),1,BSDFile);
    int StartinUAt = GetCurrentFilePosition(BSDFile);
    DPrintf("Reading %i RenderObject Elements...\n",BSD->RenderObjectTable.NumRenderObject);
    assert(sizeof(BSDRenderObjectElement_t) == 256);
    BSD->RenderObjectTable.RenderObjectList = malloc(BSD->RenderObjectTable.NumRenderObject * sizeof(BSDRenderObjectElement_t));
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        assert(GetCurrentFilePosition(BSDFile) == StartinUAt + (i * 256));
        DPrintf("Reading RenderObject %i at %i\n",i,GetCurrentFilePosition(BSDFile));
        fread(&BSD->RenderObjectTable.RenderObjectList[i],sizeof(BSD->RenderObjectTable.RenderObjectList[i]),1,BSDFile);
        DPrintf("RenderObject ID:%u\n",BSD->RenderObjectTable.RenderObjectList[i].ID);
        DPrintf("RenderObject Type:%i | %s\n",BSD->RenderObjectTable.RenderObjectList[i].Type,
                BSDRenderObjectGetEnumStringFromType(BSD->RenderObjectTable.RenderObjectList[i].Type));
        DPrintf("RenderObject Element Vertex Offset: %i (%i)\n",BSD->RenderObjectTable.RenderObjectList[i].VertOffset,
                BSD->RenderObjectTable.RenderObjectList[i].VertOffset + 2048);
        DPrintf("RenderObject Element NumVertex: %i\n",BSD->RenderObjectTable.RenderObjectList[i].NumVertex);
        //Those offset are relative to the EntryTable.
        DPrintf("RenderObject UnknownOffset1: %i (%i)\n",BSD->RenderObjectTable.RenderObjectList[i].UnknownOffset1,
                BSD->RenderObjectTable.RenderObjectList[i].UnknownOffset1 + 2048);
        DPrintf("RenderObject UnknownOffset2: %i (%i)\n",BSD->RenderObjectTable.RenderObjectList[i].UnknownOffset2,
                BSD->RenderObjectTable.RenderObjectList[i].UnknownOffset2 + 2048);
        DPrintf("RenderObject UnknownOffset3: %i (%i)\n",BSD->RenderObjectTable.RenderObjectList[i].UnknownOffset3,
                BSD->RenderObjectTable.RenderObjectList[i].UnknownOffset3 + 2048);
        DPrintf("RenderObject FaceOffset: %i (%i)\n",BSD->RenderObjectTable.RenderObjectList[i].FaceOffset,
                BSD->RenderObjectTable.RenderObjectList[i].FaceOffset + 2048);
        //Grab the UnknownOffset value (Fread(UnknownOffset) Seek_set
        //UnknownOffset1 + ValueFrom(0x564) => Face Table.
        //UnknownOffset2 + ValueFrom(0x574) => Vertex Data
        //UnknownOffset3 + ValueFrom(0x55C)
        //Then update the UnknownOffset%i Value with the final one...
    }
    DPrintf("Current Position after RenderObject Table is: %i\n",GetCurrentFilePosition(BSDFile));
    //PTable is empty only 7_1 BSD!
    if( BSD->PTable.NumElements != 0 ) {
        DPrintf("Skipping block referenced by PTable...\n");
        Element = &BSD->PTable.ElementList[BSD->PTable.NumElements - 1];
        Jump = ((Element->Offset + 2048) + (Element->Size * 4)) - GetCurrentFilePosition(BSDFile);
        DPrintf("Skipping %i Bytes...\n",Jump);
        assert(Jump > 0);
        SkipFileSection(BSDFile,Jump);
    }
    DPrintf("Current Position after Q Block is: %i\n",GetCurrentFilePosition(BSDFile));
    assert( GetCurrentFilePosition(BSDFile) == (BSD->EntryTable.NodeTableOffset + 2048));
    DPrintf("Reading node table at %i...\n",GetCurrentFilePosition(BSDFile));
    fread(&BSD->NodeData.Header,sizeof(BSD->NodeData.Header),1,BSDFile);
    DPrintf("Reading %i entries.\n",BSD->NodeData.Header.NumNodes);
    DPrintf("TableSize: %i\n",BSD->NodeData.Header.TableSize);
    DPrintf("U2: %i\n",BSD->NodeData.Header.u2);
    DPrintf("U3: %i\n",BSD->NodeData.Header.u3);
    DPrintf("U4: %i\n",BSD->NodeData.Header.u4);
    DPrintf("U5: %i\n",BSD->NodeData.Header.u5);
    BSD->NodeData.Table = malloc(BSD->NodeData.Header.NumNodes * sizeof(BSDNodeTableEntry_t));
    DPrintf("Nodetable starts at %i\n",GetCurrentFilePosition(BSDFile));
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        fread(&BSD->NodeData.Table[i],sizeof(BSD->NodeData.Table[i]),1,BSDFile);
        DPrintf("-- NODE %i --\n",i);
        DPrintf("Pointer:%i\n",BSD->NodeData.Table[i].Pointer);
        DPrintf("Offset:%i\n",BSD->NodeData.Table[i].Offset);
    }
    NodeTableEnd = GetCurrentFilePosition(BSDFile);
    DPrintf("Nodetable ends at %i\n",NodeTableEnd);
    //All the node offset are calculated from the 0 node...
    //So all the offset inside a node are Offset+AddressOfFirstNode.
    BSD->NodeData.Node = malloc(BSD->NodeData.Header.NumNodes * sizeof(BSDNode_t));
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        NodeFilePosition = GetCurrentFilePosition(BSDFile);
        DPrintf(" -- NODE %i (Pos %i PosNoHeader %i)-- \n",i,NodeFilePosition,NodeFilePosition - 2048);
        assert(GetCurrentFilePosition(BSDFile) == (BSD->NodeData.Table[i].Offset + NodeTableEnd));
        fread(&BSD->NodeData.Node[i],sizeof(BSDNode_t),1,BSDFile);
        DPrintf("ID:%u | ID:%s\n",BSD->NodeData.Node[i].Id,BSDNodeGetEnumStringFromNodeID(BSD->NodeData.Node[i].Id));
        DPrintf("Size:%i\n",BSD->NodeData.Node[i].Size);
        DPrintf("U2:%i\n",BSD->NodeData.Node[i].u2);
        DPrintf("Type:%i\n",BSD->NodeData.Node[i].Type);
        DPrintf("Position:(%i;%i;%i) Pad %i\n",BSD->NodeData.Node[i].Position.x,BSD->NodeData.Node[i].Position.y,
                BSD->NodeData.Node[i].Position.z,BSD->NodeData.Node[i].Position.Pad);
        DPrintf("Rotation:(%i;%i;%i) Pad %i\n",BSD->NodeData.Node[i].Rotation.x,BSD->NodeData.Node[i].Rotation.y,
                BSD->NodeData.Node[i].Rotation.z,BSD->NodeData.Node[i].Rotation.Pad);
        assert(BSD->NodeData.Node[i].Position.Pad == 0);
        assert(BSD->NodeData.Node[i].Rotation.Pad == 0);
                
        NodePosition.x = BSD->NodeData.Node[i].Position.x;
        NodePosition.y = BSD->NodeData.Node[i].Position.y;
        NodePosition.z = BSD->NodeData.Node[i].Position.z;
        NodeRotation.x = BSD->NodeData.Node[i].Rotation.x;
        NodeRotation.y = BSD->NodeData.Node[i].Rotation.y;
        NodeRotation.z = BSD->NodeData.Node[i].Rotation.z;
        PrevPos = GetCurrentFilePosition(BSDFile);
        int MessageRegIndex;
        fseek(BSDFile,PrevPos + 52,SEEK_SET);
        fread(&MessageRegIndex,sizeof(MessageRegIndex),1,BSDFile);
        DPrintf("Node has message reg index %i\n",MessageRegIndex);
        fseek(BSDFile,PrevPos,SEEK_SET);
        
        if( BSD->NodeData.Node[i].Type == 2 || BSD->NodeData.Node[i].Type == 4 || BSD->NodeData.Node[i].Type == 6 ) {
            //0x74
            Offset = 96;
        } else if ( BSD->NodeData.Node[i].Type == 3 ) {
            //0x60
            Offset = 116;
        } else if( BSD->NodeData.Node[i].Type == 0 && BSD->NodeData.Node[i].Id == BSD_TSP_LOAD_TRIGGER ) {
            Offset = 48;
        } else {
            Offset = 0;
        }
        if( Offset != 0 ) {
            PrevPos = GetCurrentFilePosition(BSDFile);
            Delta = Offset - sizeof(BSDNode_t);
            DPrintf("Node type %i has offset at %i.\n",BSD->NodeData.Node[i].Type,Offset);
            fseek(BSDFile,GetCurrentFilePosition(BSDFile) + Delta,SEEK_SET);
            if( BSD->NodeData.Node[i].Id == BSD_TSP_LOAD_TRIGGER ) {
                DPrintf("Node is a BSD_TSP_LOAD_TRIGGER.\n");
                StreamNode = malloc(sizeof(BSDTSPStreamNode_t));
                StreamNode->Position = NodePosition;
                for( j = 0; j < 4; j++ ) {
                    StreamNode->TSPNumberRenderList[j] = -1;
                    fread(&StreamNode->TSPNumberRenderList[j],sizeof(short),1,BSDFile);
                    assert(StreamNode->TSPNumberRenderList[j] != -1);
                    DPrintf("Node will ask TSP to draw Compartment %i\n",StreamNode->TSPNumberRenderList[j]);
                }
                StreamNode->Next = BSD->TSPStreamNodeList;
                BSD->TSPStreamNodeList = StreamNode;
            } else {
                fread(&NodeNumReferencedRenderObjectIDOffset,sizeof(NodeNumReferencedRenderObjectIDOffset),1,BSDFile);
                DPrintf("Node has RenderObject offset %i.\n",NodeNumReferencedRenderObjectIDOffset);
                if( NodeNumReferencedRenderObjectIDOffset != 0 ) {
                    if( BSD->NodeData.Node[i].Type == 4 ) {
                        DPrintf("Node has Type 4 so the RenderObject ID is %u.\n",NodeNumReferencedRenderObjectIDOffset);
                        BSDAddNodeToRenderObjectList(BSD,MissionNumber,NodeNumReferencedRenderObjectIDOffset,NodePosition,NodeRotation);
                    } else {
                        fseek(BSDFile,NodeFilePosition + NodeNumReferencedRenderObjectIDOffset,SEEK_SET);
                        fread(&NumReferencedRenderObjectID,sizeof(NumReferencedRenderObjectID),1,BSDFile);
                        DPrintf("Node is referencing %i RenderObjects.\n",NumReferencedRenderObjectID);
                        for( j = 0; j < NumReferencedRenderObjectID; j++ ) {
                            fread(&NodeRenderObjectID,sizeof(NodeRenderObjectID),1,BSDFile);
                            if( BSD->NodeData.Node[i].Id == BSD_ENEMY_SPAWN && NodeRenderObjectID != 3817496448 && j == 0 ) {
                                DPrintf("We have a different RenderObject for this enemy spawn...\n");
                            }
                            BSDAddNodeToRenderObjectList(BSD,MissionNumber,NodeRenderObjectID,NodePosition,NodeRotation);
                        }
                    }
                }
            }
            fseek(BSDFile,PrevPos,SEEK_SET);
        } else {
            DPrintf("Zero Offset.\n");
        }
//         int Delta;
//         int Offset;
//             if( BSD->NodeData.Node[i].Size > 116  ) {
//                 if( BSD->NodeData.Node[i].Size <= 124 ) {
//                     Offset = 120;
//                 } else {
//                     Offset = 124;
//                 }
//             } else {
//                 if( BSD->NodeData.Node[i].Type == 3 ) {
//                     //Fread 116
//                     Offset = 116;
//                 } else {
//                     Offset = 96;
//                     //FRead 96
//                 }
//             }
// //         }
//         fseek(BSDFile,GetCurrentFilePosition(BSDFile) + Delta,SEEK_SET);
//         fread(&BSD->NodeData.Node[i].NodeID,sizeof(BSD->NodeData.Node[i].NodeID),1,BSDFile);
//         unsigned int UBlockID;
//         UBlockID = BSDNodeIDToUBlockID(BSD,BSD->NodeData.Node[i].NodeID);
//         if( UBlockID == -1 ) {
//             DPrintf("NodeID %u has not a corresponding UBlock.\n",BSD->NodeData.Node[i].NodeID);
//             MissingNodeID++;
//         }
        Jump = BSD->NodeData.Node[i].Size - sizeof(BSDNode_t);
        DPrintf("Skipping %i bytes of data from %i.\n",Jump,GetCurrentFilePosition(BSDFile));
        SkipFileSection(BSDFile,Jump);
    }
    DPrintf("NodeList ends at %i\n",GetCurrentFilePosition(BSDFile));
    BSDReadPropertySetFile(BSD,BSDFile);
    // Prepare vertices to be rendered!
    ParseUVertexData(BSD,BSDFile);
    ParseRenderObjectFaceData(BSD,BSDFile);
    BSDShowCaseRenderObject(BSD);
    fclose(BSDFile);
    return BSD;
}

#ifdef __STANDALONE
int main(int argc,char **argv) {
    FILE *BSDFile;
    BSD_t BSD;
    BSDTableElement_t *Element;
    int Jump;
    int NodeTableEnd;
    int i;

    if( argc == 1 ) {
        printf("%s <Input.bsd> will extract the content of Input.bsd in the current folder.\n",argv[0]);
        return -1;
    }
    BSDParseFile(argv[1]);
    return 0;
}
#endif
