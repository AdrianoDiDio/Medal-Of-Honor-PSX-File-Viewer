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

#include "RenderObject.h"

void RenderObjectRecusivelyFreeHierarchyBone(BSDHierarchyBone_t *Bone)
{
    if( !Bone ) {
        return;
    }
    RenderObjectRecusivelyFreeHierarchyBone(Bone->Child2);
    RenderObjectRecusivelyFreeHierarchyBone(Bone->Child1);
    free(Bone);
}

void RenderObjectFree(RenderObject_t *RenderObject)
{
    int i;
    int j;
    
    if( !RenderObject ) {
        return;
    }
    if( RenderObject->VertexTable ) {
        for( i = 0; i < RenderObject->NumVertexTables; i++ ) {
            if( RenderObject->VertexTable[i].VertexList ) {
                free(RenderObject->VertexTable[i].VertexList);
            }
            if( RenderObject->CurrentVertexTable[i].VertexList ) {
                free(RenderObject->CurrentVertexTable[i].VertexList);
            }
        }
        free(RenderObject->VertexTable);
        free(RenderObject->CurrentVertexTable);
    }
    if( RenderObject->FaceList ) {
        free(RenderObject->FaceList);
    }
    if( RenderObject->StaticFaceList ) {
        free(RenderObject->StaticFaceList);
    }
    if( RenderObject->VertexList ) {
        free(RenderObject->VertexList);
    }
    if( RenderObject->ColorList ) {
        free(RenderObject->ColorList);
    }
    if( RenderObject->HierarchyDataRoot ) {
        RenderObjectRecusivelyFreeHierarchyBone(RenderObject->HierarchyDataRoot);
    }
    if( RenderObject->AnimationList ) {
        for( i = 0; i < RenderObject->NumAnimations; i++ ) {
            for( j = 0; j < RenderObject->AnimationList[i].NumFrames; j++ ) {
                if( RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList ) {
                    free(RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList);
                }
                if( RenderObject->AnimationList[i].Frame[j].QuaternionList ) {
                    free(RenderObject->AnimationList[i].Frame[j].QuaternionList);
                }
                if( RenderObject->AnimationList[i].Frame[j].CurrentQuaternionList ) {
                    free(RenderObject->AnimationList[i].Frame[j].CurrentQuaternionList);
                }
            }
            free(RenderObject->AnimationList[i].Frame);
        }
        free(RenderObject->AnimationList);
    }
    VAOFree(RenderObject->VAO);
    free(RenderObject);
}

const char *RenderObjectGetStringFromType(RenderObjectType_t RenderObjectType)
{
    switch( RenderObjectType ) {
        case RENDER_OBJECT_CARRY_AUX_ELEMENTS:
            return "Carry Aux Elements";
        case RENDER_OBJECT_PICKUP_AND_EXPLOSIVE:
            return "Pickup And Explosive";
        case RENDER_OBJECT_ENEMY:
            return "Enemy Render Object";
        case RENDER_OBJECT_PLANE:
            return "Airplane";
        case RENDER_OBJECT_MG42:
            return "MG42";
        case RENDER_OBJECT_DOOR:
            return "Door";
        case RENDER_OBJECT_UNKNOWN1:
            return "Unknown1";
        case RENDER_OBJECT_DESTRUCTIBLE_WINDOW:
            return "Destructible Window";
        case RENDER_OBJECT_VALVE:
            return "Valve";
        case RENDER_OBJECT_RADIO:
            return "Radio";
        case RENDER_OBJECT_EXPLOSIVE_CHARGE:
            return "Explosive Charge";
        default:
            return "Unknown";
    }
}

const char *RenderObjectGetWeaponNameFromId(int RenderObjectId)
{
    switch( RenderObjectId ) {
        case RENDER_OBJECT_WEAPON_PISTOL_TYPE_1:
            return "Pistol Type 1";
        case RENDER_OBJECT_WEAPON_SMG_TYPE_1:
            return "SubMachineGun Type 1";
        case RENDER_OBJECT_WEAPON_BAZOOKA:
            return "Bazooka";
        case RENDER_OBJECT_WEAPON_AMERICAN_GRENADE:
            return "American Grenade";
        case RENDER_OBJECT_WEAPON_SHOTGUN:
            return "Shotgun";
        case RENDER_OBJECT_WEAPON_SNIPER_RIFLE:
            return "Sniper Rifle";
        case RENDER_OBJECT_WEAPON_SMG_TYPE_2:
            return "SubMachineGun Type 2";
        case RENDER_OBJECT_WEAPON_DOCUMENT_PAPERS:
            return "Document Papers";
        case RENDER_OBJECT_WEAPON_PISTOL_TYPE_2:
            return "Pistol Type 2";
        case RENDER_OBJECT_WEAPON_PISTOL_TYPE_3:
            return "Pistol Type 3";
        case RENDER_OBJECT_WEAPON_GERMAN_GRENADE:
            return "German Grenade";
        case RENDER_OBJECT_WEAPON_SMG_TYPE_3:
            return "SubMachineGun Type 3";
        case RENDER_OBJECT_WEAPON_M1_GARAND:
            return "M1 Garand";
        default:
            //Should never happen!
            return "Unknown";
    }
}

int RenderObjectLoadStaticVertexAndColorData(RenderObject_t *RenderObject,BSDRenderObjectElement_t RenderObjectElement,FILE *BSDFile)
{
    int i;
    int Size;
    
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("RenderObjectLoadStaticVertexAndColorData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return false;
    }
    if( RenderObjectElement.VertexOffset == -1 ) {
        DPrintf("RenderObjectLoadStaticVertexAndColorData:Invalid Vertex Offset\n");
        return false;
    }
    Size = RenderObjectElement.NumVertex * sizeof(BSDVertex_t);
    RenderObject->VertexList = malloc(Size);
    if( !RenderObject->VertexList ) {
        DPrintf("RenderObjectLoadStaticVertexAndColorData:Failed to allocate memory for VertexData\n");
        return false;
    } 
    memset(RenderObject->VertexList,0,Size);
    fseek(BSDFile,RenderObjectElement.VertexOffset + 2048,SEEK_SET);
    DPrintf("RenderObjectLoadStaticVertexAndColorData:Reading Vertex definition at %i (Current:%i)\n",
            RenderObjectElement.VertexOffset + 2048,GetCurrentFilePosition(BSDFile)); 
    for( i = 0; i < RenderObjectElement.NumVertex; i++ ) {
        DPrintf("RenderObjectLoadStaticVertexAndColorData:Reading Vertex at %i (%i)\n",
                GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
        fread(&RenderObject->VertexList[i],sizeof(BSDVertex_t),1,BSDFile);
        DPrintf("RenderObjectLoadStaticVertexAndColorData:Vertex %i;%i;%i %i\n",RenderObject->VertexList[i].x,
                RenderObject->VertexList[i].y,RenderObject->VertexList[i].z,
                RenderObject->VertexList[i].Pad
        );
    }
    
    if( RenderObjectElement.ColorOffset != 0 ) {
        Size = RenderObjectElement.NumVertex * sizeof(BSDColor_t);
        RenderObject->ColorList = malloc(Size);
        if( !RenderObject->ColorList ) {
            DPrintf("RenderObjectLoadStaticVertexAndColorData:Failed to allocate memory for ColorData\n");
            return false;
        } 
        memset(RenderObject->ColorList,0,Size);
        fseek(BSDFile,RenderObjectElement.ColorOffset + 2048,SEEK_SET);
        DPrintf("RenderObjectLoadStaticVertexAndColorData:Reading Color definition at %i (Current:%i)\n",
                RenderObjectElement.ColorOffset + 2048,GetCurrentFilePosition(BSDFile)); 
        for( i = 0; i < RenderObjectElement.NumVertex; i++ ) {
            DPrintf("RenderObjectLoadStaticVertexAndColorData:Reading Color at %i (%i)\n",GetCurrentFilePosition(BSDFile),
                    GetCurrentFilePosition(BSDFile) - 2048);
            fread(&RenderObject->ColorList[i],sizeof(Color1i_t),1,BSDFile);
            DPrintf("RenderObjectLoadStaticVertexAndColorData:Color %i;%i;%i;%i\n",RenderObject->ColorList[i].r,
                    RenderObject->ColorList[i].g,RenderObject->ColorList[i].b,RenderObject->ColorList[i].Pad
            );
        }
    }
    
    return true;
}

bool RenderObjectParseStaticFaceData(RenderObject_t *RenderObject,BSDRenderObjectElement_t RenderObjectElement,FILE *BSDFile)
{
    unsigned int   Vert0;
    unsigned int   Vert1;
    unsigned int   Vert2;
    unsigned int   PackedVertexData;
    int            FaceListSize;
    int            i;
    
    if( !RenderObject ) {
        DPrintf("RenderObjectParseStaticFaceData:Invalid RenderObject!\n");
        return false;
    }
    if( RenderObjectElement.FaceOffset == 0 ) {
        DPrintf("RenderObjectParseStaticFaceData:Invalid FaceOffset!\n");
        return false;
    }
    
    fseek(BSDFile,RenderObjectElement.FaceOffset + 2048,SEEK_SET);
    fread(&RenderObject->NumFaces,sizeof(int),1,BSDFile);
    DPrintf("RenderObjectParseStaticFaceData:Reading %i faces\n",RenderObject->NumFaces);
    FaceListSize = RenderObject->NumFaces * sizeof(BSDFace_t);
    RenderObject->StaticFaceList = malloc(FaceListSize);
    if( !RenderObject->StaticFaceList ) {
        DPrintf("RenderObjectParseStaticFaceData:Failed to allocate memory for face array\n");
        return false;
    }
    memset(RenderObject->StaticFaceList,0,FaceListSize);
    DPrintf("RenderObjectParseStaticFaceData:Reading Face definition at %i (Current:%i)\n",
            RenderObjectElement.FaceOffset + 2048,GetCurrentFilePosition(BSDFile)); 
    for( i = 0; i < RenderObject->NumFaces; i++ ) {
        DPrintf("RenderObjectParseStaticFaceData:Reading Face at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
        
        fread(&RenderObject->StaticFaceList[i].UV0,sizeof(RenderObject->StaticFaceList[i].UV0),1,BSDFile);
        fread(&RenderObject->StaticFaceList[i].CBA,sizeof(RenderObject->StaticFaceList[i].CBA),1,BSDFile);
        fread(&RenderObject->StaticFaceList[i].UV1,sizeof(RenderObject->StaticFaceList[i].UV1),1,BSDFile);
        fread(&RenderObject->StaticFaceList[i].TexInfo,sizeof(RenderObject->StaticFaceList[i].TexInfo),1,BSDFile);
        fread(&RenderObject->StaticFaceList[i].UV2,sizeof(RenderObject->StaticFaceList[i].UV2),1,BSDFile);
        fread(&RenderObject->StaticFaceList[i].Pad,sizeof(RenderObject->StaticFaceList[i].Pad),1,BSDFile);
        fread(&PackedVertexData,sizeof(PackedVertexData),1,BSDFile);
        
        DPrintf(" -- FACE %i --\n",i);
        DPrintf("Tex info %i | Color mode %i | Texture Page %i\n",RenderObject->StaticFaceList[i].TexInfo,
                (RenderObject->StaticFaceList[i].TexInfo & 0xC0) >> 7,RenderObject->StaticFaceList[i].TexInfo & 0x1f);
        DPrintf("CBA is %i %ix%i\n",RenderObject->StaticFaceList[i].CBA,
                ((RenderObject->StaticFaceList[i].CBA  & 0x3F ) << 4),((RenderObject->StaticFaceList[i].CBA & 0x7FC0) >> 6));
        DPrintf("UV0:(%i;%i)\n",RenderObject->StaticFaceList[i].UV0.u,RenderObject->StaticFaceList[i].UV0.v);
        DPrintf("UV1:(%i;%i)\n",RenderObject->StaticFaceList[i].UV1.u,RenderObject->StaticFaceList[i].UV1.v);
        DPrintf("UV2:(%i;%i)\n",RenderObject->StaticFaceList[i].UV2.u,RenderObject->StaticFaceList[i].UV2.v);
        DPrintf("Pad is %i\n",RenderObject->StaticFaceList[i].Pad);
        DPrintf("Packed Vertex Data is %i\n",PackedVertexData);

        Vert0 = (PackedVertexData & 0xFF);
        Vert1 = (PackedVertexData & 0x3fc00) >> 10;
        Vert2 = (PackedVertexData & 0xFF00000 ) >> 20;
        RenderObject->StaticFaceList[i].Vert0 = Vert0;
        RenderObject->StaticFaceList[i].Vert1 = Vert1;
        RenderObject->StaticFaceList[i].Vert2 = Vert2;
        DPrintf("V0|V1|V2:%u;%u;%u\n",Vert0,Vert1,Vert2);
        DPrintf("V0|V1|V2:(%i;%i;%i)|(%i;%i;%i)|(%i;%i;%i)\n",
                RenderObject->VertexList[Vert0].x,RenderObject->VertexList[Vert0].y,RenderObject->VertexList[Vert0].z,
                RenderObject->VertexList[Vert1].x,RenderObject->VertexList[Vert1].y,RenderObject->VertexList[Vert1].z,
                RenderObject->VertexList[Vert2].x,RenderObject->VertexList[Vert2].y,RenderObject->VertexList[Vert2].z);
    }
    return true;
}

bool RenderObjectParseStaticFaceDataV2(RenderObject_t *RenderObject,BSDRenderObjectElement_t RenderObjectElement,int RenderObjectIndex,
                                         int ReferencedRenderObjectIndex,FILE *BSDFile)
{
    int FaceListSize;
    int CurrentFaceIndex;
    unsigned int V0V1;
    unsigned short V2;
    int Vert0;
    int Vert1;
    int Vert2;
    unsigned int Marker;
    BSDFace_t TempFace;
    int FaceOffset;
    
    if( !RenderObject ) {
        DPrintf("RenderObjectParseStaticFaceDataV2:Invalid RenderObject!\n");
        return false;
    }
    if( RenderObjectElement.FaceOffset == 0 ) {
        DPrintf("RenderObjectParseStaticFaceDataV2:Invalid FaceOffset!\n");
        return false;
    }
    
    FaceOffset =   
        ( BSD_RENDER_OBJECT_STARTING_OFFSET + BSD_HEADER_SIZE + 20 + (RenderObjectIndex * MOH_UNDERGROUND_RENDER_OBJECT_SIZE) ) + 260;
    fseek(BSDFile,FaceOffset,SEEK_SET);
    fread(&RenderObject->NumFaces,sizeof(RenderObject->NumFaces),1,BSDFile);
    if( !RenderObject->NumFaces ) {
        FaceOffset =   
            ( BSD_RENDER_OBJECT_STARTING_OFFSET + BSD_HEADER_SIZE + 20 + (ReferencedRenderObjectIndex * MOH_UNDERGROUND_RENDER_OBJECT_SIZE) ) + 260;
        fseek(BSDFile,FaceOffset,SEEK_SET);
        fread(&RenderObject->NumFaces,sizeof(RenderObject->NumFaces),1,BSDFile);
    }
    fseek(BSDFile,RenderObjectElement.FaceOffset + 2048,SEEK_SET);

    FaceListSize = RenderObject->NumFaces * sizeof(BSDFace_t);
    RenderObject->StaticFaceList = malloc(FaceListSize);
    if( !RenderObject->StaticFaceList ) {
        DPrintf("RenderObjectParseStaticFaceDataV2:Failed to allocate memory for face array\n");
        return false;
    }
    memset(RenderObject->StaticFaceList,0,FaceListSize);
    CurrentFaceIndex = 0;
    while( CurrentFaceIndex < RenderObject->NumFaces ) {
        DPrintf("RenderObjectParseStaticFaceDataV2:Reading Face at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);                                
        fread(&V0V1,sizeof(V0V1),1,BSDFile);
        fread(&V2,sizeof(V2),1,BSDFile);
        fread(&RenderObject->StaticFaceList[CurrentFaceIndex].UV0,sizeof(RenderObject->StaticFaceList[CurrentFaceIndex].UV0),1,BSDFile);
        fread(&RenderObject->StaticFaceList[CurrentFaceIndex].CBA,sizeof(RenderObject->StaticFaceList[CurrentFaceIndex].CBA),1,BSDFile);
        fread(&RenderObject->StaticFaceList[CurrentFaceIndex].UV1,sizeof(RenderObject->StaticFaceList[CurrentFaceIndex].UV1),1,BSDFile);
        fread(&RenderObject->StaticFaceList[CurrentFaceIndex].TexInfo,sizeof(RenderObject->StaticFaceList[CurrentFaceIndex].TexInfo),1,BSDFile);
        fread(&RenderObject->StaticFaceList[CurrentFaceIndex].UV2,sizeof(RenderObject->StaticFaceList[CurrentFaceIndex].UV2),1,BSDFile);
        DPrintf(" -- FACE %i --\n",CurrentFaceIndex);
        DPrintf("V0V1:%i V2:%i\n",V0V1,V2);
        DPrintf("Tex info %i | Color mode %i | Texture Page %i\n",RenderObject->StaticFaceList[CurrentFaceIndex].TexInfo,
                (RenderObject->StaticFaceList[CurrentFaceIndex].TexInfo & 0xC0) >> 7,RenderObject->StaticFaceList[CurrentFaceIndex].TexInfo & 0x1f);
        DPrintf("CBA is %i %ix%i\n",RenderObject->StaticFaceList[CurrentFaceIndex].CBA,
                ((RenderObject->StaticFaceList[CurrentFaceIndex].CBA  & 0x3F ) << 4),
                ((RenderObject->StaticFaceList[CurrentFaceIndex].CBA & 0x7FC0) >> 6));
        DPrintf("UV0:(%i;%i)\n",RenderObject->StaticFaceList[CurrentFaceIndex].UV0.u,
                RenderObject->StaticFaceList[CurrentFaceIndex].UV0.v);
        DPrintf("UV1:(%i;%i)\n",RenderObject->StaticFaceList[CurrentFaceIndex].UV1.u,
                RenderObject->StaticFaceList[CurrentFaceIndex].UV1.v);
        DPrintf("UV2:(%i;%i)\n",RenderObject->StaticFaceList[CurrentFaceIndex].UV2.u,
                RenderObject->StaticFaceList[CurrentFaceIndex].UV2.v);
        
        Vert0 = V0V1 & 0x1FFF;
        Vert1 = ( V0V1 >> 16 ) & 0X1FFF;
        Vert2 = V2 & 0X1FFF;
        RenderObject->StaticFaceList[CurrentFaceIndex].Vert0 = TempFace.Vert0 = Vert0;
        RenderObject->StaticFaceList[CurrentFaceIndex].Vert1 = TempFace.Vert1 = Vert1;
        RenderObject->StaticFaceList[CurrentFaceIndex].Vert2 = TempFace.Vert2 = Vert2;
        TempFace.TexInfo = RenderObject->StaticFaceList[CurrentFaceIndex].TexInfo;
        TempFace.CBA = RenderObject->StaticFaceList[CurrentFaceIndex].CBA;
        TempFace.UV0 = RenderObject->StaticFaceList[CurrentFaceIndex].UV0;
        TempFace.UV1 = RenderObject->StaticFaceList[CurrentFaceIndex].UV1;
        TempFace.UV2 = RenderObject->StaticFaceList[CurrentFaceIndex].UV2;
        CurrentFaceIndex++;
        while( 1 ) {
            DPrintf("RenderObjectParseStaticFaceDataV2:Reading additional face %i \n",CurrentFaceIndex);
            fread(&Marker,sizeof(Marker),1,BSDFile);
            DPrintf("RenderObjectParseStaticFaceDataV2:Found Marker %u (Vertex %i) Texture:%u Mask %i\n",Marker,Marker & 0x1FFF,Marker >> 16,0x1FFF);
            if( ( Marker & 0x1FFF ) == 0x1FFF || Marker == 0x1fff1fff ) {
                DPrintf("RenderObjectParseStaticFaceDataV2:Aborting since a marker was found\n");
                break;
            }
            RenderObject->StaticFaceList[CurrentFaceIndex].TexInfo = TempFace.TexInfo;
            RenderObject->StaticFaceList[CurrentFaceIndex].CBA = TempFace.CBA;

            if( (Marker & 0x8000) != 0 ) {
                TempFace.Vert0 = TempFace.Vert2;
                TempFace.UV0 = TempFace.UV2;
            } else {
                TempFace.Vert0 = TempFace.Vert1;
                TempFace.UV0 = TempFace.UV1;
                TempFace.Vert1 = TempFace.Vert2;
                TempFace.UV1 = TempFace.UV2;
            }
            TempFace.Vert2 = Marker & 0x1FFF;
            TempFace.UV2.u = (Marker >> 0x10) & 0xff;
            TempFace.UV2.v = (Marker >> 0x10) >> 8;

            RenderObject->StaticFaceList[CurrentFaceIndex].Vert0 = TempFace.Vert0;
            RenderObject->StaticFaceList[CurrentFaceIndex].Vert1 = TempFace.Vert1;
            RenderObject->StaticFaceList[CurrentFaceIndex].Vert2 = TempFace.Vert2;
            RenderObject->StaticFaceList[CurrentFaceIndex].UV0.u = TempFace.UV0.u;
            RenderObject->StaticFaceList[CurrentFaceIndex].UV0.v = TempFace.UV0.v;

            RenderObject->StaticFaceList[CurrentFaceIndex].UV1.u = TempFace.UV1.u;
            RenderObject->StaticFaceList[CurrentFaceIndex].UV1.v = TempFace.UV1.v;
            
            RenderObject->StaticFaceList[CurrentFaceIndex].UV2.u = TempFace.UV2.u;
            RenderObject->StaticFaceList[CurrentFaceIndex].UV2.v = TempFace.UV2.v;

            DPrintf("RenderObjectParseStaticFaceDataV2:Vert0:%i Vert1:%i Vert2:%i\n",TempFace.Vert0,TempFace.Vert1,
                    TempFace.Vert2);
            CurrentFaceIndex++;
        }
        if( Marker == 0x1fff1fff ) {
            DPrintf("RenderObjectParseStaticFaceDataV2:Sentinel Face found Done reading faces for renderobject %i\n",RenderObjectElement.Id);
            DPrintf("RenderObjectParseStaticFaceDataV2:Loaded %i faces (Expected %i)\n",CurrentFaceIndex,RenderObject->NumFaces);
            assert(CurrentFaceIndex == RenderObject->NumFaces);
            break;
        }
    }
    return true;
}

bool RenderObjectLoadStaticFaceData(RenderObject_t *RenderObject,BSDRenderObjectElement_t RenderObjectElement,int RenderObjectIndex,
                          int ReferencedRenderObjectIndex,int GameVersion,FILE *BSDFile)
{
    assert(sizeof(BSDFace_t) == 24);

    if( RenderObjectElement.FaceOffset == 0 ) {
        return false;
    }
    DPrintf("BSDLoadStaticFaceData:RenderObject Id %i\n",RenderObjectElement.Id);
    
    if( GameVersion == MOH_GAME_UNDERGROUND ) {
        if( !RenderObjectParseStaticFaceDataV2(RenderObject,RenderObjectElement,RenderObjectIndex,ReferencedRenderObjectIndex,BSDFile) ) {
            return false;
        }
    } else {
        if( !RenderObjectParseStaticFaceData(RenderObject,RenderObjectElement,BSDFile) ) {
            return false;
            
        }
    }
    return true;
}

bool RenderObjectLoadAnimationVertexData(RenderObject_t *RenderObject,int VertexTableIndexOffset,BSDEntryTable_t EntryTable,FILE *BSDFile)
{
    int VertexTableOffset;
    int i;
    int j;
    
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("RenderObjectLoadAnimationVertexData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return false;
    }
    if( VertexTableIndexOffset == -1 ) {
        DPrintf("RenderObjectLoadAnimationVertexData:Invalid Vertex Table Index Offset\n");
        return false;
    }
  
    VertexTableIndexOffset += EntryTable.AnimationVertexTableIndexOffset + BSD_HEADER_SIZE;
    fseek(BSDFile,VertexTableIndexOffset,SEEK_SET);
    fread(&VertexTableOffset,sizeof(VertexTableOffset),1,BSDFile);
    fread(&RenderObject->NumVertexTables,sizeof(RenderObject->NumVertexTables),1,BSDFile);
    VertexTableOffset += EntryTable.AnimationVertexTableOffset + BSD_HEADER_SIZE;
    fseek(BSDFile,VertexTableOffset,SEEK_SET);
    
    RenderObject->VertexTable = malloc(RenderObject->NumVertexTables * sizeof(BSDVertexTable_t));

    if( !RenderObject->VertexTable ) {
        DPrintf("RenderObjectLoadAnimationVertexData:Failed to allocate memory for VertexTable.\n");
        return false;
    }
    RenderObject->CurrentVertexTable = malloc(RenderObject->NumVertexTables * sizeof(BSDVertexTable_t));
    if( !RenderObject->CurrentVertexTable ) {
        DPrintf("RenderObjectLoadAnimationVertexData:Failed to allocate memory for VertexTable.\n");
        return false;
    }
    for( i = 0; i < RenderObject->NumVertexTables; i++ ) {
        fread(&RenderObject->VertexTable[i].Offset,sizeof(RenderObject->VertexTable[i].Offset),1,BSDFile);
        fread(&RenderObject->VertexTable[i].NumVertex,sizeof(RenderObject->VertexTable[i].NumVertex),1,BSDFile);
        DPrintf("Table Index %i has %i vertices\n",i,RenderObject->VertexTable[i].NumVertex);
        RenderObject->VertexTable[i].VertexList = NULL;
        
        RenderObject->CurrentVertexTable[i].Offset = RenderObject->VertexTable[i].Offset;
        RenderObject->CurrentVertexTable[i].NumVertex = RenderObject->VertexTable[i].NumVertex;
        RenderObject->CurrentVertexTable[i].VertexList = NULL;
    }
    
    fseek(BSDFile,EntryTable.AnimationVertexDataOffset + BSD_HEADER_SIZE,SEEK_SET);
    for( i = 0; i < RenderObject->NumVertexTables; i++ ) {
        if( RenderObject->VertexTable[i].Offset == -1 ) {
            continue;
        }
        fseek(BSDFile,EntryTable.AnimationVertexDataOffset + RenderObject->VertexTable[i].Offset + BSD_HEADER_SIZE,SEEK_SET);
        RenderObject->VertexTable[i].VertexList = malloc(RenderObject->VertexTable[i].NumVertex * sizeof(BSDVertex_t));
        RenderObject->CurrentVertexTable[i].VertexList = malloc(RenderObject->VertexTable[i].NumVertex * sizeof(BSDVertex_t));
        for( j = 0; j < RenderObject->VertexTable[i].NumVertex; j++ ) {
            fread(&RenderObject->VertexTable[i].VertexList[j],sizeof(BSDVertex_t),1,BSDFile);
            RenderObject->CurrentVertexTable[i].VertexList[j] = RenderObject->VertexTable[i].VertexList[j];
        }
    }
    return true;
}
void RenderObjectCopyAnimatedModelFace(BSDAnimatedModelFace_t Src,BSDAnimatedModelFace_t *Dest)
{
    if( !Dest ) {
        DPrintf("RenderObjectCopyAnimatedModelFace:Invalid Destination\n");
        return;
    }
    Dest->RGB0 = Src.RGB0;
    Dest->RGB1 = Src.RGB1;
    Dest->RGB2 = Src.RGB2;
    Dest->UV0 = Src.UV0;
    Dest->UV1 = Src.UV1;
    Dest->UV2 = Src.UV2;
    Dest->CLUT = Src.CLUT;
    Dest->TexInfo = Src.TexInfo;
    Dest->VertexTableIndex0 = Src.VertexTableIndex0;
    Dest->VertexTableDataIndex0 = Src.VertexTableDataIndex0;
    Dest->VertexTableIndex1 = Src.VertexTableIndex1;
    Dest->VertexTableDataIndex1 = Src.VertexTableDataIndex1;
    Dest->VertexTableIndex2 = Src.VertexTableIndex2;
    Dest->VertexTableDataIndex2 = Src.VertexTableDataIndex2;
}
void RenderObjectPrintAnimatedModelFace(BSDAnimatedModelFace_t Face)
{
    int ColorMode;
    int VRAMPage;
    int ABRRate;
    int CLUTPosX;
    int CLUTPosY;
    
    ColorMode = (Face.TexInfo >> 7) & 0x3;
    VRAMPage = Face.TexInfo & 0x1F;
    ABRRate = (Face.TexInfo & 0x60) >> 5;
    CLUTPosX = (Face.CLUT << 4) & 0x3F0;
    CLUTPosY = (Face.CLUT >> 6) & 0x1ff;
    DPrintf("Tex info %i | Color mode %i | Texture Page %i | ABR Rate %i\n",Face.TexInfo,
            ColorMode,VRAMPage,ABRRate);
    DPrintf("CLUT:%i X:%i Y:%i\n",Face.CLUT,CLUTPosX,CLUTPosY);
    DPrintf("UV0:(%i;%i)\n",Face.UV0.u,Face.UV0.v);
    DPrintf("UV1:(%i;%i)\n",Face.UV1.u,Face.UV1.v);
    DPrintf("UV2:(%i;%i)\n",Face.UV2.u,Face.UV2.v);
    DPrintf("RGB0:(%i;%i;%i)\n",Face.RGB0.r,Face.RGB0.g,Face.RGB0.b);
    DPrintf("RGB1:(%i;%i;%i)\n",Face.RGB1.r,Face.RGB1.g,Face.RGB1.b);
    DPrintf("RGB2:(%i;%i;%i)\n",Face.RGB2.r,Face.RGB2.g,Face.RGB2.b);
    DPrintf("Table Index0 %i Data %i.\n",Face.VertexTableIndex0&0x1F,Face.VertexTableDataIndex0);
    DPrintf("Table Index1 %i Data %i.\n",Face.VertexTableIndex1&0x1F,Face.VertexTableDataIndex1);
    DPrintf("Table Index2 %i Data %i.\n",Face.VertexTableIndex2&0x1F,Face.VertexTableDataIndex2);
}
/*
 * NOTE(Adriano):
 * MOH:Underground stores face data in a different way than the one used by Medal Of Honor.
 * In order to obtain the offset we need to first read the Global Face Offset which is stored at position
 * 0x5A4 and the total number of faces stored at 0x5A8 (both offset are stored without counting the header size 2048).
 * Then we need to load the data from the RenderObject stored at position 0x106 (as a short) and 0x108 that are needed to load all the
 * required faces.
 * Data at 0x106 contains the number of faces that we need to load while the offset at 0x108 contains the local offset that must
 * be added to the global one in order to obtain the final face data position inside the BSD file.
 */
bool RenderObjectLoadMOHUndergroundAnimationFaceData(RenderObject_t *RenderObject,int FaceTableOffset,int RenderObjectIndex,
                                           int ReferencedRenderObjectIndex,BSDEntryTable_t EntryTable,FILE *BSDFile)
{
    int GlobalFaceOffset;
    int LocalFaceOffset;
    short NumFaces;
    int ReferencedLocalFaceOffset;
    short ReferencedNumFaces;
    int CurrentFaceIndex;
    short Marker1;
    short Marker2;
    BSDColor_t ColorData;
    BSDAnimatedModelFace_t TempFace;

    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("RenderObjectLoadMOHUndergroundAnimationFaceData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return 0;
    }
    fseek(BSDFile,BSD_RENDER_OBJECT_STARTING_OFFSET + BSD_HEADER_SIZE,SEEK_SET);
    fread(&GlobalFaceOffset,sizeof(GlobalFaceOffset),1,BSDFile);
    fseek(BSDFile,16 + (RenderObjectIndex * MOH_UNDERGROUND_RENDER_OBJECT_SIZE) + 262,SEEK_CUR);
    fread(&NumFaces,sizeof(NumFaces),1,BSDFile);
    fread(&LocalFaceOffset,sizeof(LocalFaceOffset),1,BSDFile);
    if( ReferencedRenderObjectIndex != -1 ) {
        fseek(BSDFile,16 + (ReferencedRenderObjectIndex * MOH_UNDERGROUND_RENDER_OBJECT_SIZE) + 262,SEEK_CUR);
        fread(&ReferencedNumFaces,sizeof(NumFaces),1,BSDFile);
        fread(&ReferencedLocalFaceOffset,sizeof(LocalFaceOffset),1,BSDFile);
        if( !NumFaces ) {
            NumFaces = ReferencedNumFaces;
        }
        if( LocalFaceOffset == -1 ) {
            LocalFaceOffset = ReferencedLocalFaceOffset;
        }
    }

    if( !NumFaces ) {
        DPrintf("RenderObjectLoadMOHUndergroundAnimationFaceData:Invalid number of faces.\n");
        return false;
    }
    //NOTE(Adriano):Now we can load the faces at the offset specified by the RenderObject.
    fseek(BSDFile,GlobalFaceOffset + LocalFaceOffset + BSD_HEADER_SIZE,SEEK_SET);
    DPrintf("RenderObjectLoadMOHUndergroundAnimationFaceData:Reading %i faces at offset %li (%li)\n",NumFaces,ftell(BSDFile),ftell(BSDFile) - 2048);
    
    RenderObject->FaceList = malloc(NumFaces * sizeof(BSDAnimatedModelFace_t));
    RenderObject->NumFaces = NumFaces;
    if( !RenderObject->FaceList ) {
        DPrintf("RenderObjectLoadMOHUndergroundAnimationFaceData:Failed to allocate memory for face list.\n");
        return false;
    }
    CurrentFaceIndex = 0;
    while( CurrentFaceIndex < NumFaces ) {
        DPrintf("Reading it at %li\n",ftell(BSDFile) - 2048);
        fread(&TempFace,sizeof(BSDAnimatedModelFace_t),1,BSDFile);
    
        RenderObjectCopyAnimatedModelFace(TempFace,&RenderObject->FaceList[CurrentFaceIndex]);
        
        DPrintf(" -- FACE %i --\n",CurrentFaceIndex);
        RenderObjectPrintAnimatedModelFace(RenderObject->FaceList[CurrentFaceIndex]);
        CurrentFaceIndex++;
        while( 1 ) {
            //NOTE(Adriano):
            //Data is split into two shorts.
            //First one (Marker1) contains data that references the VertexTable while the
            //other one (Marker2) contains the UV coordinates for the new vertex.
            fread(&Marker1,sizeof(Marker1),1,BSDFile);
            fread(&Marker2,sizeof(Marker2),1,BSDFile);

            if( Marker1 == 0x1FFF ) {
                DPrintf("RenderObjectLoadMOHUndergroundAnimationFaceData:Aborting since a marker was found\n");
                break;
            }
            fread(&ColorData,sizeof(ColorData),1,BSDFile);
                    
            if( (Marker1 & 0x80) != 0 ) {
                TempFace.VertexTableIndex0 = TempFace.VertexTableIndex2;
                TempFace.VertexTableDataIndex0 = TempFace.VertexTableDataIndex2;                
                TempFace.UV0 = TempFace.UV2;
                TempFace.RGB0 = TempFace.RGB2;
            } else {
                TempFace.VertexTableIndex0 = TempFace.VertexTableIndex1;
                TempFace.VertexTableDataIndex0 = TempFace.VertexTableDataIndex1;
                TempFace.VertexTableIndex1 = TempFace.VertexTableIndex2;
                TempFace.VertexTableDataIndex1 = TempFace.VertexTableDataIndex2;
                TempFace.UV0 = TempFace.UV1;
                TempFace.UV1 = TempFace.UV2;
                TempFace.RGB0 = TempFace.RGB1;
                TempFace.RGB1 = TempFace.RGB2;
            }
            TempFace.VertexTableDataIndex2 = Marker1 >> 8;
            TempFace.VertexTableIndex2 = Marker1 & 0x1F;
            TempFace.UV2.u = Marker2 & 0xff;
            TempFace.UV2.v = Marker2 >> 8;
            TempFace.RGB2 = ColorData;
            RenderObjectCopyAnimatedModelFace(TempFace,&RenderObject->FaceList[CurrentFaceIndex]);
            DPrintf(" -- FACE %i --\n",CurrentFaceIndex);
            RenderObjectPrintAnimatedModelFace(RenderObject->FaceList[CurrentFaceIndex]);
            CurrentFaceIndex++;
        }
        //NOTE(Adriano):Last Data is identified by the two marker being set to 0x1FFF
        if( (Marker1 == 0x1fff && Marker2 == 0x1fff ) ) {
            DPrintf("RenderObjectLoadMOHUndergroundAnimationFaceData:Sentinel Face found Done reading faces for RenderObject\n");
            DPrintf("RenderObjectLoadMOHUndergroundAnimationFaceData:Loaded %i faces (Expected %i)\n",CurrentFaceIndex,NumFaces);
            break;
        }
    }
    assert(CurrentFaceIndex == NumFaces);
    return true;
}
bool RenderObjectLoadAnimationFaceData(RenderObject_t *RenderObject,int FaceTableOffset,int RenderObjectIndex,int ReferencedRenderObjectIndex,
                             BSDEntryTable_t EntryTable,FILE *BSDFile,int GameVersion)
{
    int GlobalFaceTableOffset;
    int GlobalFaceDataOffset;
    int FaceDataOffset;
    int NumFaces;
    int i;
    
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("RenderObjectLoadAnimationFaceData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return false;
    }
    if( FaceTableOffset == -1 ) {
        if( GameVersion == MOH_GAME_UNDERGROUND ) {
            DPrintf("RenderObjectLoadAnimationFaceData:Game is MOH:Underground...attempting to use a different face loader.\n");
            return RenderObjectLoadMOHUndergroundAnimationFaceData(RenderObject,FaceTableOffset,RenderObjectIndex,ReferencedRenderObjectIndex,
                                                          EntryTable,BSDFile);
        } else {
            DPrintf("RenderObjectLoadAnimationFaceData:Invalid Face Table Index Offset\n");
            return false;
        }
    }
    GlobalFaceTableOffset = EntryTable.AnimationFaceTableOffset + FaceTableOffset + BSD_HEADER_SIZE;
    fseek(BSDFile,GlobalFaceTableOffset,SEEK_SET);
    fread(&FaceDataOffset,sizeof(FaceDataOffset),1,BSDFile);
    fread(&NumFaces,sizeof(NumFaces),1,BSDFile);
    GlobalFaceDataOffset = EntryTable.AnimationFaceDataOffset + FaceDataOffset + BSD_HEADER_SIZE;
    fseek(BSDFile,GlobalFaceDataOffset,SEEK_SET);
    assert(sizeof(BSDAnimatedModelFace_t) == 28);
    RenderObject->FaceList = malloc(NumFaces * sizeof(BSDAnimatedModelFace_t));
    RenderObject->NumFaces = NumFaces;
    if( !RenderObject->FaceList ) {
        DPrintf("RenderObjectLoadAnimationFaceData:Failed to allocate memory for face list.\n");
        return false;
    }
    DPrintf("RenderObjectLoadAnimationFaceData:Loading %i faces\n",NumFaces);
    for( i = 0; i < NumFaces; i++ ) {
        fread(&RenderObject->FaceList[i],sizeof(BSDAnimatedModelFace_t),1,BSDFile);
        DPrintf(" -- FACE %i --\n",i);
        RenderObjectPrintAnimatedModelFace(RenderObject->FaceList[i]);
    }
    return true;
}

BSDHierarchyBone_t *RenderObjectRecursivelyLoadHierarchyData(int BoneDataStartingPosition,int BoneOffset,FILE *BSDFile)
{
    BSDHierarchyBone_t *Bone;
    int Child1Offset;
    int Child2Offset;
    
    if( !BSDFile ) {
        DPrintf("RenderObjectRecursivelyLoadHierarchyData:Invalid Bone Table file\n");
        return NULL;
    }

    
    Bone = malloc(sizeof(BSDHierarchyBone_t));
    
    if( !Bone ) {
        DPrintf("RenderObjectRecursivelyLoadHierarchyData:Failed to allocate bone data\n");
        return NULL;
    }

    Bone->Child1 = NULL;
    Bone->Child2 = NULL;

    fseek(BSDFile,BoneDataStartingPosition + BoneOffset + BSD_HEADER_SIZE,SEEK_SET);
    fread(&Bone->VertexTableIndex,sizeof(Bone->VertexTableIndex),1,BSDFile);
    fread(&Bone->Position,sizeof(Bone->Position),1,BSDFile);
    fread(&Bone->Pad,sizeof(Bone->Pad),1,BSDFile);
    fread(&Child1Offset,sizeof(Child1Offset),1,BSDFile);
    fread(&Child2Offset,sizeof(Child2Offset),1,BSDFile);
    
    DPrintf("Bone:VertexTableIndex:%i\n",Bone->VertexTableIndex);
    DPrintf("Bone:Position:%i;%i;%i\n",Bone->Position.x,Bone->Position.y,Bone->Position.z);

    assert(  Bone->Pad == -12851 );

    if( Child2Offset != -1 ) {
        Bone->Child2 = RenderObjectRecursivelyLoadHierarchyData(BoneDataStartingPosition,Child2Offset,BSDFile);
    }
    if( Child1Offset != -1 ) {
        Bone->Child1 = RenderObjectRecursivelyLoadHierarchyData(BoneDataStartingPosition,Child1Offset,BSDFile);
    }
    return Bone;
}

int RenderObjectLoadAnimationHierarchyData(RenderObject_t *RenderObject,int HierarchyDataRootOffset,BSDEntryTable_t EntryTable,FILE *BSDFile)
{
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("RenderObject: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return false;
    }
    if( HierarchyDataRootOffset == -1 ) {
        DPrintf("RenderObject:Invalid Face Table Index Offset\n");
        return false;
    }
    
    RenderObject->HierarchyDataRoot = RenderObjectRecursivelyLoadHierarchyData(EntryTable.AnimationHierarchyDataOffset,HierarchyDataRootOffset,BSDFile);
    
    if( !RenderObject->HierarchyDataRoot ) {
        DPrintf("RenderObject:Couldn't load hierarchy data\n");
        return false;
    }
    return true;
}
void RenderObjectDecodeQuaternions(int QuatPart0,int QuatPart1,int QuatPart2,BSDQuaternion_t *OutQuaternion1,BSDQuaternion_t *OutQuaternion2)
{
    if( OutQuaternion1 ) {
        OutQuaternion1->x = ( (QuatPart0 << 0x10) >> 0x14) * 2;
        OutQuaternion1->y = (QuatPart1 << 0x14) >> 0x13;
        OutQuaternion1->z = ( ( ( (QuatPart1 >> 0xC) << 0x1C ) >> 0x14) | ( (QuatPart0 >> 0xC) & 0xF0) | (QuatPart0 & 0xF) ) * 2;
        OutQuaternion1->w = (QuatPart0 >> 0x14) * 2;
    }
    if( OutQuaternion2 ) {
        OutQuaternion2->x = (QuatPart1  >> 0x14) * 2;
        OutQuaternion2->y = ( (QuatPart2 << 0x4) >> 0x14 ) * 2;
        OutQuaternion2->w = ( (QuatPart2 << 0x10) >> 0x14) * 2;
        OutQuaternion2->z = ( (QuatPart2 >> 0x1C) << 0x8 | (QuatPart2 & 0xF ) << 0x4 | ( (QuatPart1 >> 0x10) & 0xF ) ) * 2;
    }
}

bool RenderObjectLoadAnimationData(RenderObject_t *RenderObject,int AnimationDataOffset,BSDEntryTable_t EntryTable,FILE *BSDFile)
{
    short NumAnimationOffset;
    unsigned short Pad;
    BSDAnimationTableEntry_t *AnimationTableEntry;
    int *AnimationOffsetTable;
    int QuaternionListOffset;
    int i;
    int j;
    int w;
    int q;
    int Base;
    int QuatPart0;
    int QuatPart1;
    int QuatPart2;
    int NumEncodedQuaternions;
    int NumDecodedQuaternions;
    int NextFrame;
    int PrevFrame;
    int Jump;
    versor FromQuaternion;
    versor ToQuaternion;
    versor DestQuaternion;
    
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("RenderObjectLoadAnimationData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return false;
    }
    if( AnimationDataOffset == -1 ) {
        DPrintf("RenderObjectLoadAnimationData:Invalid Vertex Table Index Offset\n");
        return false;
    }
    fseek(BSDFile,AnimationDataOffset + BSD_HEADER_SIZE,SEEK_SET);
    fread(&NumAnimationOffset,sizeof(NumAnimationOffset),1,BSDFile);
    fread(&Pad,sizeof(Pad),1,BSDFile);
    assert(Pad == 52685);
    
    AnimationOffsetTable = malloc(NumAnimationOffset * sizeof(int) );
    RenderObject->NumAnimations = NumAnimationOffset;
    for( i = 0; i < NumAnimationOffset; i++ ) {
        fread(&AnimationOffsetTable[i],sizeof(AnimationOffsetTable[i]),1,BSDFile);
        if( AnimationOffsetTable[i] == -1 ) {
            continue;
        }
    }

    AnimationTableEntry = malloc(RenderObject->NumAnimations * sizeof(BSDAnimationTableEntry_t) );
    for( i = 0; i < NumAnimationOffset; i++ ) {
        DPrintf("RenderObjectLoadAnimationData:Animation Offset %i for entry %i\n",AnimationOffsetTable[i],i);
        if( AnimationOffsetTable[i] == -1 ) {
            continue;
        }
        DPrintf("RenderObjectLoadAnimationData:Going to %i plus %i\n",EntryTable.AnimationTableOffset,AnimationOffsetTable[i]);
        fseek(BSDFile,EntryTable.AnimationTableOffset + AnimationOffsetTable[i] + BSD_HEADER_SIZE,SEEK_SET);
        fread(&AnimationTableEntry[i],sizeof(AnimationTableEntry[i]),1,BSDFile);
        DPrintf("RenderObjectLoadAnimationData:AnimationEntry %i has pad %i\n",i,AnimationTableEntry[i].Pad);
        DPrintf("RenderObjectLoadAnimationData:Loading %i vertices Size %i\n",AnimationTableEntry[i].NumAffectedVertex,
                AnimationTableEntry[i].NumAffectedVertex * 8);
        DPrintf("RenderObjectLoadAnimationData: NumFrames %u NumAffectedVertex:%u || Offset %i\n",AnimationTableEntry[i].NumFrames,
                AnimationTableEntry[i].NumAffectedVertex,AnimationTableEntry[i].Offset);
        assert(AnimationTableEntry[i].Pad == 52480);
    }
    RenderObject->AnimationList = malloc(RenderObject->NumAnimations * sizeof(BSDAnimation_t));
    for( i = 0; i < NumAnimationOffset; i++ ) {
        RenderObject->AnimationList[i].Frame = NULL;
        RenderObject->AnimationList[i].NumFrames = 0;
        if( AnimationOffsetTable[i] == -1 ) {
            continue;
        }
        DPrintf(" -- ANIMATION ENTRY %i -- \n",i);
        DPrintf("Loading %i animations for entry %i\n",AnimationTableEntry[i].NumFrames,i);
        
        RenderObject->AnimationList[i].Frame = malloc(AnimationTableEntry[i].NumFrames * sizeof(BSDAnimationFrame_t));
        RenderObject->AnimationList[i].NumFrames = AnimationTableEntry[i].NumFrames;
        for( j = 0; j < AnimationTableEntry[i].NumFrames; j++ ) {
            DPrintf(" -- FRAME %i/%i -- \n",j,AnimationTableEntry[i].NumFrames);
            // 20 is the sizeof an animation
            fseek(BSDFile,EntryTable.AnimationDataOffset + AnimationTableEntry[i].Offset + BSD_HEADER_SIZE 
                + j * BSD_ANIMATION_FRAME_DATA_SIZE,SEEK_SET);
            DPrintf("Reading animation definition at %li each entry is %li bytes\n",ftell(BSDFile),sizeof(BSDAnimationFrame_t));

            fread(&RenderObject->AnimationList[i].Frame[j].U0,sizeof(RenderObject->AnimationList[i].Frame[j].U0),1,BSDFile);
            fread(&RenderObject->AnimationList[i].Frame[j].U4,sizeof(RenderObject->AnimationList[i].Frame[j].U4),1,BSDFile);
            fread(&RenderObject->AnimationList[i].Frame[j].EncodedVector,
                  sizeof(RenderObject->AnimationList[i].Frame[j].EncodedVector),1,BSDFile);
            fread(&RenderObject->AnimationList[i].Frame[j].U1,sizeof(RenderObject->AnimationList[i].Frame[j].U1),1,BSDFile);
            fread(&RenderObject->AnimationList[i].Frame[j].U2,sizeof(RenderObject->AnimationList[i].Frame[j].U2),1,BSDFile);
            fread(&RenderObject->AnimationList[i].Frame[j].U3,sizeof(RenderObject->AnimationList[i].Frame[j].U3),1,BSDFile);
            fread(&RenderObject->AnimationList[i].Frame[j].U5,sizeof(RenderObject->AnimationList[i].Frame[j].U5),1,BSDFile);
            fread(&RenderObject->AnimationList[i].Frame[j].FrameInterpolationIndex,
                  sizeof(RenderObject->AnimationList[i].Frame[j].FrameInterpolationIndex),1,BSDFile);
            fread(&RenderObject->AnimationList[i].Frame[j].NumQuaternions,
                  sizeof(RenderObject->AnimationList[i].Frame[j].NumQuaternions),1,BSDFile);
            fread(&QuaternionListOffset,sizeof(QuaternionListOffset),1,BSDFile);

            RenderObject->AnimationList[i].Frame[j].Vector.x = (RenderObject->AnimationList[i].Frame[j].EncodedVector << 0x16) >> 0x16;
            RenderObject->AnimationList[i].Frame[j].Vector.y = (RenderObject->AnimationList[i].Frame[j].EncodedVector << 0xb)  >> 0x15;
            RenderObject->AnimationList[i].Frame[j].Vector.z = (RenderObject->AnimationList[i].Frame[j].EncodedVector << 0x1)  >> 0x16;
            
            RenderObject->AnimationList[i].Frame[j].Vector.x = (RenderObject->AnimationList[i].Frame[j].EncodedVector << 6) >> 6;
            RenderObject->AnimationList[i].Frame[j].Vector.y = (RenderObject->AnimationList[i].Frame[j].EncodedVector << 5)  >> 5;
            RenderObject->AnimationList[i].Frame[j].Vector.z = (RenderObject->AnimationList[i].Frame[j].EncodedVector >> 15) >> 6;
            DPrintf("Entry %i => U0|U1|U2: %i;%i;%i QuaternionListOffset:%i\n",i,RenderObject->AnimationList[i].Frame[j].U0,
                    RenderObject->AnimationList[i].Frame[j].U1,
                    RenderObject->AnimationList[i].Frame[j].U2,QuaternionListOffset);
            DPrintf("U3: %i\n",RenderObject->AnimationList[i].Frame[j].U3);
            DPrintf("U4 is %i\n",RenderObject->AnimationList[i].Frame[j].U4);
            DPrintf("U5 is %i\n",RenderObject->AnimationList[i].Frame[j].U5);
            DPrintf("Frame Interpolation Index is %i -- Number of quaternions is %i\n",
                    RenderObject->AnimationList[i].Frame[j].FrameInterpolationIndex,
                RenderObject->AnimationList[i].Frame[j].NumQuaternions
            );
            DPrintf("Encoded Vector is %i\n",RenderObject->AnimationList[i].Frame[j].EncodedVector);
            DPrintf("We are at %li  AnimOffset:%i LocalOffset:%i Index %i times 20 (%i)\n",ftell(BSDFile),
                EntryTable.AnimationDataOffset,AnimationTableEntry[i].Offset,j,j*20
            );
            assert(ftell(BSDFile) - (EntryTable.AnimationDataOffset + AnimationTableEntry[i].Offset + BSD_HEADER_SIZE 
                + j * BSD_ANIMATION_FRAME_DATA_SIZE) == BSD_ANIMATION_FRAME_DATA_SIZE );
            RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList = NULL;
            RenderObject->AnimationList[i].Frame[j].QuaternionList = NULL;
            RenderObject->AnimationList[i].Frame[j].CurrentQuaternionList = NULL;
            if( QuaternionListOffset != -1 ) {
                fseek(BSDFile,EntryTable.AnimationQuaternionDataOffset + QuaternionListOffset + BSD_HEADER_SIZE,SEEK_SET);
                DPrintf("Reading Vector definition at %li\n",ftell(BSDFile));
                NumEncodedQuaternions = (RenderObject->AnimationList[i].Frame[j].NumQuaternions / 2) * 3;
                if( (RenderObject->AnimationList[i].Frame[j].NumQuaternions & 1 ) != 0 ) {
                    NumEncodedQuaternions += 2;
                }
                RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList = malloc( NumEncodedQuaternions * sizeof(int));
                for( w = 0; w < NumEncodedQuaternions; w++ ) {
                    DPrintf("Reading Encoded quaternion at %li\n",ftell(BSDFile));
                    fread(&RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[w],
                          sizeof(RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[w]),1,BSDFile);
                }
                DPrintf("Done...loaded a list of %i encoded quaternions\n",RenderObject->AnimationList[i].Frame[j].NumQuaternions * 2);
                RenderObject->AnimationList[i].Frame[j].QuaternionList = malloc( 
                    RenderObject->AnimationList[i].Frame[j].NumQuaternions * sizeof(BSDQuaternion_t));
                RenderObject->AnimationList[i].Frame[j].CurrentQuaternionList = malloc( 
                    RenderObject->AnimationList[i].Frame[j].NumQuaternions * sizeof(BSDQuaternion_t));
                NumDecodedQuaternions = 0;
                for( q = 0; q < RenderObject->AnimationList[i].Frame[j].NumQuaternions / 2; q++ ) {
                    Base = q * 3;
//                     DPrintf("Generating with base %i V0:%i V1:%i V2:%i\n",q,Base,Base+1,Base+2);
                    QuatPart0 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[Base];
                    QuatPart1 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[Base+1];
                    QuatPart2 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[Base+2];
                    
                    RenderObjectDecodeQuaternions(QuatPart0,QuatPart1,QuatPart2,
                                         &RenderObject->AnimationList[i].Frame[j].QuaternionList[NumDecodedQuaternions],
                                         &RenderObject->AnimationList[i].Frame[j].QuaternionList[NumDecodedQuaternions+1]);
                    NumDecodedQuaternions += 2;
                }
//                 DPrintf("Decoded %i quaternions out of %i\n",NumDecodedQuaternions,RenderObject->AnimationList[i].Frame[j].NumQuaternions);
                if( NumDecodedQuaternions == (RenderObject->AnimationList[i].Frame[j].NumQuaternions - 1) ) {
                    QuatPart0 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[NumEncodedQuaternions-2];
                    QuatPart1 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[NumEncodedQuaternions-1];
//                     DPrintf("QuatPart0:%i QuatPart1:%i\n",QuatPart0,QuatPart1);
                    RenderObjectDecodeQuaternions(QuatPart0,QuatPart1,-1,
                                         &RenderObject->AnimationList[i].Frame[j].QuaternionList[NumDecodedQuaternions],
                                         NULL);
//                     DPrintf("New quat is %i;%i;%i;%i\n",TempQuaternion.x,TempQuaternion.y,TempQuaternion.z,TempQuaternion.w);
                    NumDecodedQuaternions++;
                }
                DPrintf("Decoded %i out of %i\n",NumDecodedQuaternions,RenderObject->AnimationList[i].Frame[j].NumQuaternions);
                assert(NumDecodedQuaternions == RenderObject->AnimationList[i].Frame[j].NumQuaternions);
                BSDRenderObjectResetFrameQuaternionList(&RenderObject->AnimationList[i].Frame[j]);
            } else {
                DPrintf("QuaternionListOffset is not valid...\n");
            }
        }
    }
    for( i = 0; i < RenderObject->NumAnimations; i++ ) {
        if( AnimationOffsetTable[i] == -1 ) {
            continue;
        }
        for( j = 0; j < AnimationTableEntry[i].NumFrames; j++ ) {
            if( RenderObject->AnimationList[i].Frame[j].QuaternionList != NULL ) {
                continue;
            }
            RenderObject->AnimationList[i].Frame[j].QuaternionList = malloc(sizeof(BSDQuaternion_t) * 
                RenderObject->AnimationList[i].Frame[j].NumQuaternions);
            RenderObject->AnimationList[i].Frame[j].CurrentQuaternionList = malloc(sizeof(BSDQuaternion_t) * 
                RenderObject->AnimationList[i].Frame[j].NumQuaternions);
            NextFrame = j + (HighNibble(RenderObject->AnimationList[i].Frame[j].FrameInterpolationIndex));
            PrevFrame = j - (LowNibble(RenderObject->AnimationList[i].Frame[j].FrameInterpolationIndex));
            Jump = NextFrame-PrevFrame;
            DPrintf("Current FrameIndex:%i\n",j);
            DPrintf("Next FrameIndex:%i\n",NextFrame);
            DPrintf("Previous FrameIndex:%i\n",PrevFrame);
            DPrintf("Jump:%i\n",Jump);
            DPrintf("NumQuaternions:%i\n",RenderObject->AnimationList[i].Frame[j].NumQuaternions);
            for( q = 0; q < RenderObject->AnimationList[i].Frame[j].NumQuaternions; q++ ) {
                FromQuaternion[0] = RenderObject->AnimationList[i].Frame[PrevFrame].QuaternionList[q].x / 4096.f;
                FromQuaternion[1] = RenderObject->AnimationList[i].Frame[PrevFrame].QuaternionList[q].y / 4096.f;
                FromQuaternion[2] = RenderObject->AnimationList[i].Frame[PrevFrame].QuaternionList[q].z / 4096.f;
                FromQuaternion[3] = RenderObject->AnimationList[i].Frame[PrevFrame].QuaternionList[q].w / 4096.f;
                ToQuaternion[0] = RenderObject->AnimationList[i].Frame[NextFrame].QuaternionList[q].x / 4096.f;
                ToQuaternion[1] = RenderObject->AnimationList[i].Frame[NextFrame].QuaternionList[q].y / 4096.f;
                ToQuaternion[2] = RenderObject->AnimationList[i].Frame[NextFrame].QuaternionList[q].z / 4096.f;
                ToQuaternion[3] = RenderObject->AnimationList[i].Frame[NextFrame].QuaternionList[q].w / 4096.f;
                glm_quat_nlerp(FromQuaternion,
                    ToQuaternion,
                    1.f/Jump,
                    DestQuaternion
                );
                RenderObject->AnimationList[i].Frame[j].QuaternionList[q].x = DestQuaternion[0] * 4096.f;
                RenderObject->AnimationList[i].Frame[j].QuaternionList[q].y = DestQuaternion[1] * 4096.f;
                RenderObject->AnimationList[i].Frame[j].QuaternionList[q].z = DestQuaternion[2] * 4096.f;
                RenderObject->AnimationList[i].Frame[j].QuaternionList[q].w = DestQuaternion[3] * 4096.f;
            }
            BSDRenderObjectResetFrameQuaternionList(&RenderObject->AnimationList[i].Frame[j]);
        }
    }
    free(AnimationOffsetTable);
    free(AnimationTableEntry);
    return true;
}

RenderObject_t *RenderObjectLoad(BSDRenderObjectElement_t RenderObjectElement,BSDEntryTable_t EntryTable,
                                 BSDRenderObjectTable_t RenderObjectTable,FILE *BSDFile,int GameVersion)
{
    RenderObject_t *RenderObject;
    int RenderObjectIndex;
    int ReferencedRenderObjectIndex;
    
    RenderObject = NULL;
    if( !BSDFile ) {
        DPrintf("RenderObjectLoad:Invalid BSD file\n");
        goto Failure;
    }
    RenderObject = malloc(sizeof(RenderObject_t));
    if( !RenderObject ) {
        DPrintf("RenderObjectLoad:Failed to allocate memory for RenderObject\n");
        goto Failure;
    }
    RenderObject->Id = RenderObjectElement.Id;
    RenderObject->ReferencedRenderObjectId = RenderObjectElement.ReferencedRenderObjectId;
    RenderObject->Type = RenderObjectElement.Type;
    RenderObject->VertexTable = NULL;
    RenderObject->VertexList = NULL;
    RenderObject->ColorList = NULL;
    RenderObject->StaticFaceList = NULL;
    RenderObject->CurrentVertexTable = NULL;
    RenderObject->FaceList = NULL;
    RenderObject->HierarchyDataRoot = NULL;
    RenderObject->AnimationList = NULL;
    RenderObject->VAO = NULL;
    RenderObject->CurrentAnimationIndex = -1;
    RenderObject->CurrentFrameIndex = -1;
    RenderObject->Next = NULL;
    RenderObject->IsStatic = RenderObjectElement.AnimationDataOffset == -1;

    RenderObject->Scale[0] = (float) (RenderObjectElement.ScaleX  / 16 ) / 4096.f;
    RenderObject->Scale[1] = (float) (RenderObjectElement.ScaleY  / 16 ) / 4096.f;
    RenderObject->Scale[2] = (float) (RenderObjectElement.ScaleZ  / 16 ) / 4096.f;

    glm_vec3_zero(RenderObject->Center);
    
    ReferencedRenderObjectIndex = -1;
    RenderObjectIndex = BSDGetRenderObjectIndexById(&RenderObjectTable,RenderObjectElement.Id);
    if( RenderObjectElement.ReferencedRenderObjectId != -1 ) {
        //NOTE(Adriano):Some RenderObjects are bundled in other files and referenced by Id.
        //This could happens for example when loading the SET1 BSD that is looking for another RenderObject containted inside
        //the level file.
        //For the moment this is bypassed and could cause the RenderObject to not be loaded due to missing offsets...
        ReferencedRenderObjectIndex = BSDGetRenderObjectIndexById(&RenderObjectTable,
                                                                  RenderObjectElement.ReferencedRenderObjectId);
    }
    
    if( RenderObject->IsStatic ) {
        if( !RenderObjectLoadStaticVertexAndColorData(RenderObject,RenderObjectElement,BSDFile) ) {
            DPrintf("RenderObjectLoad:Failed to load vertex/color data\n");
            goto Failure;
        }
        if( !RenderObjectLoadStaticFaceData(RenderObject,RenderObjectElement,RenderObjectIndex,ReferencedRenderObjectIndex,GameVersion,BSDFile) ) {
            DPrintf("RenderObjectLoad:Failed to load face data\n");
            goto Failure;
        }
    } else {
        if( !RenderObjectLoadAnimationVertexData(RenderObject,RenderObjectElement.VertexTableIndexOffset,EntryTable,BSDFile) ) {
            DPrintf("RenderObjectLoad:Failed to load vertex data\n");
            goto Failure;
        }

        if( !RenderObjectLoadAnimationFaceData(RenderObject,RenderObjectElement.FaceTableOffset,RenderObjectIndex,ReferencedRenderObjectIndex,
            EntryTable,BSDFile,GameVersion) ) {
            DPrintf("RenderObjectLoad:Failed to load face data\n");
            goto Failure;
        }

        if( !RenderObjectLoadAnimationHierarchyData(RenderObject,RenderObjectElement.HierarchyDataRootOffset,EntryTable,BSDFile) ) {
            DPrintf("RenderObjectLoad:Failed to load hierarchy data\n");
            goto Failure;
        }
        
        if( !RenderObjectLoadAnimationData(RenderObject,RenderObjectElement.AnimationDataOffset,EntryTable,BSDFile) ) {
            DPrintf("RenderObjectLoad:Failed to load animation data\n");
            goto Failure;
        }
    }


    return RenderObject;
Failure:
    RenderObjectFree(RenderObject);
    return NULL;
}


