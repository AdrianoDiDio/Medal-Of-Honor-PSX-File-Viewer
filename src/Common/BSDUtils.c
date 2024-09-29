// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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

#include "BSDUtils.h"

void BSDRenderObjectResetFrameQuaternionList(BSDAnimationFrame_t *Frame)
{
    if( !Frame ) {
        return;
    }
    memcpy(Frame->CurrentQuaternionList,Frame->QuaternionList,Frame->NumQuaternions * sizeof(BSDQuaternion_t));
}

void BSDRecursivelyApplyHierachyData(const BSDHierarchyBone_t *Bone,const BSDQuaternion_t *QuaternionList,BSDVertexTable_t *VertexTable,
                                     mat4 TransformMatrix)
{
    versor Quaternion;
    mat4 LocalRotationMatrix;
    mat4 LocalTranslationMatrix;
    mat4 LocalTransformMatrix;
    vec3 TransformedBonePosition;
    vec3 TransformedVertexPosition;
    vec3 Temp;
    int i;
    
    if( !Bone ) {
        DPrintf("BSDRecursivelyApplyHierachyData:NULL Bone.\n");
        return;
    }
    
    if( !QuaternionList ) {
        DPrintf("BSDRecursivelyApplyHierachyData:Invalid Quaternion List.\n");
        return;
    }
    if( !VertexTable ) {
        DPrintf("BSDRecursivelyApplyHierachyData:Invalid Vertex Table.\n");
        return;
    }

    glm_mat4_identity(LocalRotationMatrix);
    glm_mat4_identity(LocalTranslationMatrix);
    glm_mat4_identity(LocalTransformMatrix);
    
    Quaternion[0] = QuaternionList[Bone->VertexTableIndex].x / 4096.f;
    Quaternion[1] = QuaternionList[Bone->VertexTableIndex].y / 4096.f;
    Quaternion[2] = QuaternionList[Bone->VertexTableIndex].z / 4096.f;
    Quaternion[3] = QuaternionList[Bone->VertexTableIndex].w / 4096.f;
    
    glm_quat_mat4t(Quaternion,LocalRotationMatrix);
    
    Temp[0] = Bone->Position.x;
    Temp[1] = Bone->Position.y;
    Temp[2] = Bone->Position.z;
    
    glm_mat4_mulv3(TransformMatrix,Temp,1.f,TransformedBonePosition);
    glm_translate_make(LocalTranslationMatrix,TransformedBonePosition);
    glm_mat4_mul(LocalTranslationMatrix,LocalRotationMatrix,LocalTransformMatrix);

    if( VertexTable[Bone->VertexTableIndex].Offset != -1 && VertexTable[Bone->VertexTableIndex].NumVertex != 0 ) {
        for( i = 0; i < VertexTable[Bone->VertexTableIndex].NumVertex; i++ ) {
            Temp[0] = VertexTable[Bone->VertexTableIndex].VertexList[i].x;
            Temp[1] = VertexTable[Bone->VertexTableIndex].VertexList[i].y;
            Temp[2] = VertexTable[Bone->VertexTableIndex].VertexList[i].z;
            glm_mat4_mulv3(LocalTransformMatrix,Temp,1.f,TransformedVertexPosition);
            VertexTable[Bone->VertexTableIndex].VertexList[i].x = TransformedVertexPosition[0];
            VertexTable[Bone->VertexTableIndex].VertexList[i].y = TransformedVertexPosition[1];
            VertexTable[Bone->VertexTableIndex].VertexList[i].z = TransformedVertexPosition[2];
        }
    }

    if( Bone->Child2 ) {
        BSDRecursivelyApplyHierachyData(Bone->Child2,QuaternionList,VertexTable,TransformMatrix);
    }
    if( Bone->Child1 ) {
        BSDRecursivelyApplyHierachyData(Bone->Child1,QuaternionList,VertexTable,LocalTransformMatrix);
    }
}

int BSDGetRealOffset(int RelativeOffset)
{
    return RelativeOffset + BSD_HEADER_SIZE;
}

int BSDGetRenderObjectTableOffset(int GameEngine)
{
    return GameEngine == MOH_GAME_STANDARD ? BSD_RENDER_OBJECT_STARTING_OFFSET : BSD_MOH_UNDERGROUND_RENDER_OBJECT_STARTING_OFFSET;
}

BSDRenderObjectElement_t *BSDGetRenderObjectById(const BSDRenderObjectTable_t *RenderObjectTable,unsigned int RenderObjectId)
{
    int i;
    
    if( !RenderObjectTable ) {
        return NULL;
    }
    
    for( i = 0; i < RenderObjectTable->NumRenderObject; i++ ) {
        if( RenderObjectTable->RenderObject[i].Id == RenderObjectId ) {
            return &RenderObjectTable->RenderObject[i];
        }
    }
    return NULL;
}

int BSDGetRenderObjectIndexById(const BSDRenderObjectTable_t *RenderObjectTable,unsigned int RenderObjectId)
{
    int i;
    
    if( !RenderObjectTable ) {
        return -1;
    }
    for( i = 0; i < RenderObjectTable->NumRenderObject; i++ ) {
        if( RenderObjectTable->RenderObject[i].Id == RenderObjectId ) {
            return i;
        }
    }
    return -1;
}

const char *BSDRenderObjectGetStringFromType(int RenderObjectType)
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
        case BSD_RENDER_OBJECT_VALVE:
            return "Valve";
        case BSD_RENDER_OBJECT_RADIO:
            return "Radio";
        case BSD_RENDER_OBJECT_EXPLOSIVE_CHARGE:
            return "Explosive Charge";
        default:
            return "Unknown";
    }
}

const char *BSDRenderObjectGetWeaponNameFromId(int RenderObjectId)
{
    switch( RenderObjectId ) {
        case BSD_RENDER_OBJECT_WEAPON_PISTOL_TYPE_1:
            return "Pistol Type 1";
        case BSD_RENDER_OBJECT_WEAPON_SMG_TYPE_1:
            return "SubMachineGun Type 1";
        case BSD_RENDER_OBJECT_WEAPON_BAZOOKA:
            return "Bazooka";
        case BSD_RENDER_OBJECT_WEAPON_AMERICAN_GRENADE:
            return "American Grenade";
        case BSD_RENDER_OBJECT_WEAPON_SHOTGUN:
            return "Shotgun";
        case BSD_RENDER_OBJECT_WEAPON_SNIPER_RIFLE:
            return "Sniper Rifle";
        case BSD_RENDER_OBJECT_WEAPON_SMG_TYPE_2:
            return "SubMachineGun Type 2";
        case BSD_RENDER_OBJECT_WEAPON_DOCUMENT_PAPERS:
            return "Document Papers";
        case BSD_RENDER_OBJECT_WEAPON_PISTOL_TYPE_2:
            return "Pistol Type 2";
        case BSD_RENDER_OBJECT_WEAPON_PISTOL_TYPE_3:
            return "Pistol Type 3";
        case BSD_RENDER_OBJECT_WEAPON_GERMAN_GRENADE:
            return "German Grenade";
        case BSD_RENDER_OBJECT_WEAPON_SMG_TYPE_3:
            return "SubMachineGun Type 3";
        case BSD_RENDER_OBJECT_WEAPON_M1_GARAND:
            return "M1 Garand";
        default:
            //Should never happen!
            return "Unknown";
    }
}

/*
 * NOTE(Adriano):
 * Some RenderObjects uses the 'ReferencedRenderObjectId' field to reference a RenderObject that contains common
 * informations shared by multiple RenderObjects.
 * In order to correctly parse these entry we need to copy the field from the 'ReferencedRenderObjectId' to the RenderObject that
 * requested it.
 * NOTE(Adriano):Make sure to update the data when new fields are added.
 */
void BSDPatchRenderObjects(FILE *BSDFile,BSDRenderObjectTable_t *RenderObjectTable)
{
    BSDRenderObjectElement_t *CurrentRenderObject;
    BSDRenderObjectElement_t *ReferencedRenderObject;
    int i;
        
    for( i = 0; i < RenderObjectTable->NumRenderObject; i++ ) {
        if( RenderObjectTable->RenderObject[i].ReferencedRenderObjectId == -1 ) {
            continue;
        }
        ReferencedRenderObject = BSDGetRenderObjectById(RenderObjectTable,RenderObjectTable->RenderObject[i].ReferencedRenderObjectId);
        CurrentRenderObject = &RenderObjectTable->RenderObject[i];
        if( !ReferencedRenderObject ) {
            DPrintf("BSDPatchRenderObjects:RenderObject Id %i not found\n",RenderObjectTable->RenderObject[i].ReferencedRenderObjectId);
            continue;
        }
        DPrintf("BSDPatchRenderObjects:Patching up RenderObject Id %i using Id %i\n",RenderObjectTable->RenderObject[i].Id,
            RenderObjectTable->RenderObject[i].ReferencedRenderObjectId
        );
        if(CurrentRenderObject->AnimationDataOffset == -1 ) {
            CurrentRenderObject->AnimationDataOffset = ReferencedRenderObject->AnimationDataOffset;
        }
        if(CurrentRenderObject->FaceOffset == -1 ) {
            CurrentRenderObject->FaceOffset = ReferencedRenderObject->FaceOffset;
        }
        if(CurrentRenderObject->FaceTableOffset == -1 ) {
            CurrentRenderObject->FaceTableOffset = ReferencedRenderObject->FaceTableOffset;
        }
        if(CurrentRenderObject->VertexTableIndexOffset == -1 ) {
            CurrentRenderObject->VertexTableIndexOffset = ReferencedRenderObject->VertexTableIndexOffset;
        }
        if(CurrentRenderObject->UnknownOffset4 == -1 ) {
            CurrentRenderObject->UnknownOffset4 = ReferencedRenderObject->UnknownOffset4;
        }
        if(CurrentRenderObject->VertexOffset == -1 ) {
            CurrentRenderObject->VertexOffset = ReferencedRenderObject->VertexOffset;
            CurrentRenderObject->NumVertex = ReferencedRenderObject->NumVertex;
        }
        if(CurrentRenderObject->HierarchyDataRootOffset == -1 ) {
            CurrentRenderObject->HierarchyDataRootOffset = ReferencedRenderObject->HierarchyDataRootOffset;
        }
        if(CurrentRenderObject->ColorOffset == -1 ) {
            CurrentRenderObject->ColorOffset = ReferencedRenderObject->ColorOffset;
        }
        if(CurrentRenderObject->AnimationDataOffset == -1 ) {
            CurrentRenderObject->AnimationDataOffset = ReferencedRenderObject->AnimationDataOffset;
        }
        if( CurrentRenderObject->ScaleX == 0 && CurrentRenderObject->ScaleY == 0 && CurrentRenderObject->ScaleZ == 0 ) {
            CurrentRenderObject->ScaleX = ReferencedRenderObject->ScaleX;
            CurrentRenderObject->ScaleY = ReferencedRenderObject->ScaleY;
            CurrentRenderObject->ScaleZ = ReferencedRenderObject->ScaleZ;

        }
        if( CurrentRenderObject->Type == -1 ) {
            CurrentRenderObject->Type = ReferencedRenderObject->Type;
        }
    }
}

bool BSDReadPropertySetFileBlock(FILE *BSDFile, BSDPropertySetFile_t *PropertySetFile)
{
    int PropertySetFileOffset;
    int i;
    int j;
    
    if( !BSDFile ) {
        DPrintf("BSDReadPropertySetFileBlock: Invalid file\n");
        return false;
    }
    
    if( !PropertySetFile ) {
        DPrintf("BSDReadPropertySetFileBlock: Invalid data\n");
        return false;
    }
    
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_PROPERTY_SET_FILE_POSITION)) {
        fseek(BSDFile, BSDGetRealOffset(BSD_PROPERTY_SET_FILE_POSITION), SEEK_SET);
    }

    fread(&PropertySetFileOffset,sizeof(PropertySetFileOffset),1,BSDFile);
    
    if( PropertySetFileOffset == 0 ) {
        DPrintf("BSDReadPropertySetFileBlock:BSD File has no property file set.\n");
        return false;
    }
    
    fseek(BSDFile, BSDGetRealOffset(PropertySetFileOffset), SEEK_SET);
    fread(&PropertySetFile->NumProperties,sizeof(PropertySetFile->NumProperties),1,BSDFile);
    DPrintf("BSDReadPropertySetFile:Reading %i properties at %i (%i).\n",PropertySetFile->NumProperties,
            GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - BSD_HEADER_SIZE);
    PropertySetFile->NumProperties++;
    PropertySetFile->Property = malloc(sizeof(BSDProperty_t) * PropertySetFile->NumProperties);
    if( !PropertySetFile->Property ) {
        DPrintf("BSDReadPropertySetFileBlock:Couldn't allocate memory for Property array\n");
        return false;
    }
    for( i = 0; i < PropertySetFile->NumProperties; i++ ) {
        DPrintf("BSDReadPropertySetFileBlock:Reading property %i at %i (%i)\n",i,GetCurrentFilePosition(BSDFile),
            GetCurrentFilePosition(BSDFile) - BSD_HEADER_SIZE
        );
        fread(&PropertySetFile->Property[i].NumNodes,sizeof(PropertySetFile->Property[i].NumNodes),1,BSDFile);
        PropertySetFile->Property[i].NumNodes = (255 - PropertySetFile->Property[i].NumNodes) /*<< 1*/;
        DPrintf("Property contains %i nodes\n",PropertySetFile->Property[i].NumNodes);
        
        SkipFileSection(1,BSDFile);
        PropertySetFile->Property[i].NodeList = malloc(PropertySetFile->Property[i].NumNodes * sizeof(unsigned short));
        if( !PropertySetFile->Property[i].NodeList ) {
            DPrintf("BSDReadPropertySetFileBlock:Couldn't allocate memory for Property node array\n");
            return false;
        }
        DPrintf("BSDReadPropertySetFileBlock:Reading %li bytes.\n",PropertySetFile->Property[i].NumNodes * sizeof(unsigned short));
        for( j = 0; j <  PropertySetFile->Property[i].NumNodes; j++ ) {
            fread(&PropertySetFile->Property[i].NodeList[j],sizeof(PropertySetFile->Property[i].NodeList[j]),1,BSDFile);
            DPrintf("Short %u\n", PropertySetFile->Property[i].NodeList[j]);
        }
    }
    DPrintf("BSDReadPropertySetFileBlock:Property end at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - BSD_HEADER_SIZE);
    return true;
}

bool BSDReadNodeInfoBlock(FILE *BSDFile,int NodeInfoOffset, BSDNodeInfo_t *NodeInfo)
{
    int NodeFilePosition;
    int NodeTableEnd;
    int NextNodeOffset;
    int i;
    
    if( !BSDFile ) {
        DPrintf("BSDReadNodeInfoBlock: Invalid file\n");
        return false;
    }
    
    if( !NodeInfo ) {
        DPrintf("BSDReadNodeInfoBlock: Invalid data\n");
        return false;
    }
    
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(NodeInfoOffset)) {
        fseek(BSDFile, BSDGetRealOffset(NodeInfoOffset), SEEK_SET);
    }

    DPrintf("BSDReadNodeInfoBlock:Reading node table at %i...\n",GetCurrentFilePosition(BSDFile));
    fread(&NodeInfo->Header,sizeof(NodeInfo->Header),1,BSDFile);
    DPrintf("BSDReadNodeInfoBlock:Reading %i entries.\n",NodeInfo->Header.NumNodes);
    DPrintf("TableSize: %i\n",NodeInfo->Header.TableSize);
    DPrintf("U2: %i\n",NodeInfo->Header.u2);
    DPrintf("U3: %i\n",NodeInfo->Header.u3);
    DPrintf("U4: %i\n",NodeInfo->Header.u4);
    DPrintf("U5: %i\n",NodeInfo->Header.u5);
    NodeInfo->Table = malloc(NodeInfo->Header.NumNodes * sizeof(BSDNodeTableEntry_t));
    if( !NodeInfo->Table ) {
        DPrintf("BSDReadNodeChunk:Failed to allocate memory for node table\n");
        return false;
    }
    DPrintf("BSDReadNodeInfoBlock:Nodetable starts at %i\n",GetCurrentFilePosition(BSDFile));
    for( i = 0; i < NodeInfo->Header.NumNodes; i++ ) {
        fread(&NodeInfo->Table[i],sizeof(NodeInfo->Table[i]),1,BSDFile);
        DPrintf("-- NODE %i --\n",i);
        DPrintf("Pointer:%i\n",NodeInfo->Table[i].Pointer);
        DPrintf("Offset:%i\n",NodeInfo->Table[i].Offset);
    }
    NodeTableEnd = GetCurrentFilePosition(BSDFile);
    DPrintf("BSDReadNodeInfoBlock:Nodetable ends at %i\n",NodeTableEnd);
    //All the node offset are calculated from the 0 node...
    //So all the offset inside a node are Offset+AddressOfFirstNode.
    //TODO:Load each node entry one by one (not using a single fread) since
    //     there are many types of node that contains different data...
    NodeInfo->Node = malloc(NodeInfo->Header.NumNodes * sizeof(BSDNode_t));
    if( !NodeInfo->Node ) {
        DPrintf("BSDReadNodeInfoBlock:Failed to allocate memory for node array\n");
        return false;
    }
    for( i = 0; i < NodeInfo->Header.NumNodes; i++ ) {
        NodeFilePosition = GetCurrentFilePosition(BSDFile);
        DPrintf(" -- NODE %i (Pos %i PosNoHeader %i)-- \n",i,NodeFilePosition,NodeFilePosition - BSD_HEADER_SIZE);
        assert(GetCurrentFilePosition(BSDFile) == (NodeInfo->Table[i].Offset + NodeTableEnd));
        fread(&NodeInfo->Node[i].Id,sizeof(NodeInfo->Node[i].Id),1,BSDFile);
        fread(&NodeInfo->Node[i].Size,sizeof(NodeInfo->Node[i].Size),1,BSDFile);
        fread(&NodeInfo->Node[i].u2,sizeof(NodeInfo->Node[i].u2),1,BSDFile);
        fread(&NodeInfo->Node[i].Type,sizeof(NodeInfo->Node[i].Type),1,BSDFile);
        fread(&NodeInfo->Node[i].Position,sizeof(NodeInfo->Node[i].Position),1,BSDFile);
        fread(&NodeInfo->Node[i].Rotation,sizeof(NodeInfo->Node[i].Rotation),1,BSDFile);
        fread(&NodeInfo->Node[i].Pad,sizeof(NodeInfo->Node[i].Pad),1,BSDFile);
        fread(&NodeInfo->Node[i].CollisionVolumeType,sizeof(NodeInfo->Node[i].CollisionVolumeType),1,BSDFile);
        fread(&NodeInfo->Node[i].CollisionInfo0,sizeof(NodeInfo->Node[i].CollisionInfo0),1,BSDFile);
        fread(&NodeInfo->Node[i].CollisionInfo1,sizeof(NodeInfo->Node[i].CollisionInfo1),1,BSDFile);
        fread(&NodeInfo->Node[i].CollisionInfo2,sizeof(NodeInfo->Node[i].CollisionInfo2),1,BSDFile);
        NodeInfo->Node[i].FilePosition = NodeFilePosition;
        NextNodeOffset = NodeFilePosition + NodeInfo->Node[i].Size;
        fseek(BSDFile,NextNodeOffset,SEEK_SET);
    }
    return true;
}

bool BSDReadRenderObjectTable(FILE *BSDFile,int GameEngine, BSDRenderObjectTable_t *RenderObjectTable)
{
    int FirstRenderObjectFilePosition;
    int PreviousFilePosition;
    int Result;
    int i;
    int TableOffset;
    
    if( !BSDFile ) {
        DPrintf("BSDReadRenderObjectTable: Invalid file\n");
        return false;
    }
    
    if( !RenderObjectTable ) {
        DPrintf("BSDReadRenderObjectTable: Invalid data\n");
        return false;
    }
    
    TableOffset = BSDGetRenderObjectTableOffset(GameEngine);
    
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(TableOffset)) {
        fseek(BSDFile, BSDGetRealOffset(TableOffset), SEEK_SET);
    }
    
    fread(&RenderObjectTable->NumRenderObject,sizeof(RenderObjectTable->NumRenderObject),1,BSDFile);
    FirstRenderObjectFilePosition = GetCurrentFilePosition(BSDFile);
    
    DPrintf("BSDReadRenderObjectTable:Reading %i RenderObject Elements...\n",RenderObjectTable->NumRenderObject);
    
    assert(sizeof(BSDRenderObjectElement_t) == MOH_RENDER_OBJECT_SIZE);
    
    RenderObjectTable->RenderObject = malloc(RenderObjectTable->NumRenderObject * sizeof(BSDRenderObjectElement_t));
    if( !RenderObjectTable->RenderObject ) {
        DPrintf("BSDReadRenderObjectTable:Failed to allocate memory for RenderObject Array\n");
        return 0;
    }
    for( i = 0; i < RenderObjectTable->NumRenderObject; i++ ) {
        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            assert(GetCurrentFilePosition(BSDFile) == FirstRenderObjectFilePosition + (i * MOH_UNDERGROUND_RENDER_OBJECT_SIZE));
        } else {
            assert(GetCurrentFilePosition(BSDFile) == FirstRenderObjectFilePosition + (i * MOH_RENDER_OBJECT_SIZE));
        }
        DPrintf("BSDReadRenderObjectTable:Reading RenderObject %i at %i...\n",i,GetCurrentFilePosition(BSDFile));
        fread(&RenderObjectTable->RenderObject[i],sizeof(RenderObjectTable->RenderObject[i]),1,BSDFile);
        DPrintf("RenderObject Id:%i\n",RenderObjectTable->RenderObject[i].Id);
        if( RenderObjectTable->RenderObject[i].Type == 1 ) {
            DPrintf("RenderObject Type:%i | %s\n",RenderObjectTable->RenderObject[i].Type,
                    BSDRenderObjectGetWeaponNameFromId(RenderObjectTable->RenderObject[i].Id));
        } else {
            DPrintf("RenderObject Type:%i | %s\n",RenderObjectTable->RenderObject[i].Type,
                    BSDRenderObjectGetStringFromType(RenderObjectTable->RenderObject[i].Type));
        }
        DPrintf("RenderObject Element Vertex Offset: %i (%i)\n",RenderObjectTable->RenderObject[i].VertexOffset,
                RenderObjectTable->RenderObject[i].VertexOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Element NumVertex: %i\n",RenderObjectTable->RenderObject[i].NumVertex);
        //Those offset are relative to the EntryTable.
        DPrintf("RenderObject FaceTableOffset: %i (%i)\n",RenderObjectTable->RenderObject[i].FaceTableOffset,
                RenderObjectTable->RenderObject[i].FaceTableOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject VertexTableIndexOffset: %i (%i)\n",RenderObjectTable->RenderObject[i].VertexTableIndexOffset,
                RenderObjectTable->RenderObject[i].VertexTableIndexOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Hierarchy Data Root Offset: %i (%i)\n",RenderObjectTable->RenderObject[i].HierarchyDataRootOffset,
                RenderObjectTable->RenderObject[i].HierarchyDataRootOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject FaceOffset: %i (%i)\n",RenderObjectTable->RenderObject[i].FaceOffset,
                RenderObjectTable->RenderObject[i].FaceOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Scale: %i;%i;%i (4096 is 1 meaning no scale)\n",
                RenderObjectTable->RenderObject[i].ScaleX / 4,
                RenderObjectTable->RenderObject[i].ScaleY / 4,
                RenderObjectTable->RenderObject[i].ScaleZ / 4);
        if( RenderObjectTable->RenderObject[i].ReferencedRenderObjectId != -1 ) {
            DPrintf("RenderObject References RenderObject Id:%i\n",RenderObjectTable->RenderObject[i].ReferencedRenderObjectId);
        } else {
            DPrintf("RenderObject No Reference set...\n");
        }
        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            if( RenderObjectTable->RenderObject[i].FaceOffset == 0 ) {
                fread(&RenderObjectTable->RenderObject[i].FaceOffset,sizeof(int),1,BSDFile);
                SkipFileSection(16,BSDFile);
            } else {
                SkipFileSection(20,BSDFile);
            }
        }
    }
    // Patch up the data using the referenced renderobjects ids...
    BSDPatchRenderObjects(BSDFile,RenderObjectTable);
    return 1;
}
bool BSDReadSkyBlock(FILE *BSDFile,BSDSky_t *Sky)
{    
    if( !BSDFile ) {
        DPrintf("BSDReadSkyBlock: Invalid file\n");
        return false;
    }
    
    if( !Sky ) {
        DPrintf("BSDReadSkyBlock: Invalid data\n");
        return false;
    }

    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_SKY_DATA_FILE_POSITION)) {
        fseek(BSDFile, BSDGetRealOffset(BSD_SKY_DATA_FILE_POSITION), SEEK_SET);
    }

    fread(&Sky->U0,sizeof(Sky->U0),1,BSDFile);
    fread(&Sky->U1,sizeof(Sky->U1),1,BSDFile);
    fread(&Sky->U2,sizeof(Sky->U2),1,BSDFile);
    fread(&Sky->StarRadius,sizeof(Sky->StarRadius),1,BSDFile);
    fread(&Sky->U3,sizeof(Sky->U3),1,BSDFile);
    fread(&Sky->MoonZ,sizeof(Sky->MoonZ),1,BSDFile);
    fread(&Sky->MoonY,sizeof(Sky->MoonY),1,BSDFile);
    fread(&Sky->U4,sizeof(Sky->U4),1,BSDFile);
    fread(&Sky->U5,sizeof(Sky->U5),1,BSDFile);
    fread(&Sky->U6,sizeof(Sky->U6),1,BSDFile);
    
    DPrintf("BSDReadSkyBlock:MoonY:%i MoonZ:%i\n",Sky->MoonY,Sky->MoonZ);
    DPrintf("BSDReadSkyBlock:Star Radius:%i\n",Sky->StarRadius);
    return true;
}

bool BSDReadEntryTableBlock(FILE *BSDFile,BSDEntryTable_t *EntryTable)
{    
    if( !BSDFile ) {
        DPrintf("BSDReadEntryTableBlock: Invalid file\n");
        return false;
    }
    if( !EntryTable ) {
        DPrintf("BSDReadEntryTableBlock: Invalid data\n");
        return false;
    }

    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_ENTRY_TABLE_FILE_POSITION)) {
        fseek(BSDFile, BSDGetRealOffset(BSD_ENTRY_TABLE_FILE_POSITION), SEEK_SET);
    }
    
    assert(sizeof(BSDEntryTable_t) == 80);
    DPrintf("BSDReadEntryTableBlock: Reading EntryTable at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - BSD_HEADER_SIZE);
    fread(EntryTable,sizeof(BSDEntryTable_t),1,BSDFile);
    DPrintf("Node table is at %i (%i)\n",EntryTable->NodeTableOffset,EntryTable->NodeTableOffset + BSD_HEADER_SIZE);
    DPrintf("Unknown data is at %i (%i)\n",EntryTable->UnknownDataOffset,EntryTable->UnknownDataOffset + BSD_HEADER_SIZE);
    DPrintf("AnimationTableOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationTableOffset,
            EntryTable->AnimationTableOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationTableEntries);
    DPrintf("AnimationDataOffset is at %i (%i) contains %i elements.\n",EntryTable->AnimationDataOffset,
            EntryTable->AnimationDataOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationData);
    DPrintf("AnimationQuaternionDataOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationQuaternionDataOffset,
            EntryTable->AnimationQuaternionDataOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationQuaternionData);
    DPrintf("AnimationHierarchyDataOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationHierarchyDataOffset,
            EntryTable->AnimationHierarchyDataOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationHierarchyData);
    DPrintf("AnimationFaceTableOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationFaceTableOffset,
            EntryTable->AnimationFaceTableOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationFaceTables);
    DPrintf("AnimationFaceDataOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationFaceDataOffset,
            EntryTable->AnimationFaceDataOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationFaces);
    DPrintf("AnimationVertexTableIndexOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationVertexTableIndexOffset,
            EntryTable->AnimationVertexTableIndexOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationVertexTableIndex);
    DPrintf("AnimationVertexTableOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationVertexTableOffset,
            EntryTable->AnimationVertexTableOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationVertexTableEntry);
    DPrintf("AnimationVertexDataOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationVertexDataOffset,
            EntryTable->AnimationVertexDataOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationVertex);
    return true;
}

bool BSDReadAnimatedLightTableBlock(FILE *BSDFile, BSDAnimatedLightTable_t *AnimatedLightTable)
{
    BSDAnimatedLight_t *AnimatedLight;
    int PreviousFilePosition;
    int i;
    int j;
    
    if( !BSDFile ) {
        DPrintf("BSDReadAnimatedLightTableBlock: Invalid file\n");
        return false;
    }
    if( !AnimatedLightTable ) {
        DPrintf("BSDReadAnimatedLightTableBlock: Invalid data\n");
        return false;
    }
    
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_ANIMATED_LIGHTS_FILE_POSITION)) {
        fseek(BSDFile, BSDGetRealOffset(BSD_ANIMATED_LIGHTS_FILE_POSITION), SEEK_SET);
    }
    DPrintf("BSDReadAnimatedLightTableBlock:AnimatedLightsTable is at %li\n",ftell(BSDFile));
    fread(&AnimatedLightTable->NumAnimatedLights,sizeof(AnimatedLightTable->NumAnimatedLights),1,BSDFile);
    DPrintf("BSDReadAnimatedLightTableBlock:AnimatedLightsTable:Reading %i colors at %li\n",AnimatedLightTable->NumAnimatedLights,ftell(BSDFile));

    for( i = 0; i < BSD_ANIMATED_LIGHTS_TABLE_SIZE; i++ ) {
        AnimatedLight = &AnimatedLightTable->AnimatedLightsList[i];
        fread(&AnimatedLight->NumColors,sizeof(AnimatedLight->NumColors),1,BSDFile);
        fread(&AnimatedLight->StartingColorOffset,sizeof(AnimatedLight->StartingColorOffset),1,BSDFile);
        fread(&AnimatedLight->ColorIndex,sizeof(AnimatedLight->ColorIndex),1,BSDFile);
        fread(&AnimatedLight->CurrentColor,sizeof(AnimatedLight->CurrentColor),1,BSDFile);
        fread(&AnimatedLight->Delay,sizeof(AnimatedLight->Delay),1,BSDFile);
        if( AnimatedLight->NumColors == 0 ) {
            continue;
        }
        AnimatedLight->LastUpdateTime = 0;
        AnimatedLight->ColorList = malloc(AnimatedLight->NumColors * sizeof(Color1i_t));
        if( !AnimatedLight->ColorList ) {
            DPrintf("BSDReadAnimatedLightTableBlock:Failed to allocate memory for Color List\n");
            return false;
        }
        DPrintf("BSDReadAnimatedLightTableBlock: Reading Animated Light %i...\n",i);
        DPrintf("StartingColorOffset:%i\n",AnimatedLight->StartingColorOffset);
        DPrintf("StartingColorOffset No Header:%i\n",BSDGetRealOffset(AnimatedLight->StartingColorOffset));
        DPrintf("CurrentColor:%i\n",AnimatedLight->CurrentColor);
        DPrintf("ColorIndex:%i\n",AnimatedLight->ColorIndex);
        DPrintf("Delay:%i\n",AnimatedLight->Delay);
        PreviousFilePosition = GetCurrentFilePosition(BSDFile);
        fseek(BSDFile,BSDGetRealOffset(AnimatedLight->StartingColorOffset),SEEK_SET);
        DPrintf("Reading color at %i\n",GetCurrentFilePosition(BSDFile));
        for( j = 0; j < AnimatedLight->NumColors; j++ ) {
            fread(&AnimatedLight->ColorList[j],sizeof(AnimatedLight->ColorList[j]),1,BSDFile);
            DPrintf("Color %i %i %i %i %i (As Int %u)\n",j,AnimatedLight->ColorList[j].rgba[0],
                    AnimatedLight->ColorList[j].rgba[1],AnimatedLight->ColorList[j].rgba[2],AnimatedLight->ColorList[j].rgba[3],
                    AnimatedLight->ColorList[j].c
            );
        }
        fseek(BSDFile,PreviousFilePosition,SEEK_SET);
    }
    return true;
}

bool BSDReadSceneInfoBlock(FILE *BSDFile, BSDSceneInfo_t *SceneInfo)
{
    if( !BSDFile ) {
        DPrintf("BSDReadSceneInfoBlock: Invalid file\n");
        return false;
    }
    if( !SceneInfo ) {
        DPrintf("BSDReadSceneInfoBlock: Invalid data\n");
        return false;
    }
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_SCENE_INFO_BLOCK_POSITION)) {
        fseek(BSDFile, BSDGetRealOffset(BSD_SCENE_INFO_BLOCK_POSITION), SEEK_SET);
    }
    fread(SceneInfo,sizeof(BSDSceneInfo_t),1,BSDFile);
    DPrintf("BSDReadSceneInfoBlock:Reading scene info...\n");
    DPrintf("Fog Near: %i\n",SceneInfo->FogNear);
    DPrintf("Clear Color: %i;%i;%i\n",SceneInfo->ClearColor.r,SceneInfo->ClearColor.g,SceneInfo->ClearColor.b);
    return true;
}

bool BSDReadTSPInfoBlock(FILE *BSDFile, BSDTSPInfo_t *TSPInfo)
{
    
    if( !BSDFile ) {
        DPrintf("BSDReadTSPInfoBlock: Invalid file\n");
        return false;
    }
    if( !TSPInfo ) {
        DPrintf("BSDReadTSPInfoBlock: Invalid data\n");
        return false;
    }
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_TSP_INFO_BLOCK_POSITION) ) {
        fseek(BSDFile, BSDGetRealOffset(BSD_TSP_INFO_BLOCK_POSITION), SEEK_SET);
    }
    fread(TSPInfo,sizeof(BSDTSPInfo_t),1,BSDFile);
    DPrintf("BSDReadTSPInfoBlock:Reading TSP info...\n");
    DPrintf("Compartment pattern: %s\n",TSPInfo->TSPPattern);
    DPrintf("Number of compartments: %i\n",TSPInfo->NumTSP);
    DPrintf("TargetInitialCompartment: %i\n",TSPInfo->TargetInitialCompartment);
    DPrintf("Starting Compartment: %i\n",TSPInfo->StartingComparment);
    DPrintf("u3: %i\n",TSPInfo->u3);
    DPrintf("TSP Block ends at %i\n",GetCurrentFilePosition(BSDFile));
    assert(TSPInfo->u3 == 0);
    return true;
}
