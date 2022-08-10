// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2022 Adriano Di Dio.
    
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
#include "BSD.h"
#include "MOHModelViewer.h" 

void BSDFreeRenderObject(BSDRenderObject_t *RenderObject)
{
    int i;
    if( !RenderObject ) {
        return;
    }
    if( RenderObject->VertexTable ) {
        for( i = 0; i < RenderObject->NumVertexTables; i++ ) {
            if( RenderObject->VertexTable[i].VertexList ) {
                free(RenderObject->VertexTable[i].VertexList);
            }
        }
        free(RenderObject->VertexTable);
    }
    if( RenderObject->FaceList ) {
        free(RenderObject->FaceList);
    }
        
    free(RenderObject);
}

void BSDFreeRenderObjectList(BSDRenderObject_t *RenderObjectList)
{
    BSDRenderObject_t *Temp;
    while( RenderObjectList ) {
        Temp = RenderObjectList;
        RenderObjectList = RenderObjectList->Next;
        BSDFreeRenderObject(Temp);
    }
}
void BSDFree(BSD_t *BSD)
{
    if( !BSD ) {
        return;
    }
    if( BSD->RenderObjectTable.RenderObject ) {
        free(BSD->RenderObjectTable.RenderObject);
    }
    free(BSD);
}

/*
  NOTE(Adriano):
  Since this tool is used to load BSD files without specifying any information about the game and since we
  are not loading the TSP files we don't have any idea of what kind of game this BSD belongs to.
  This causes issues due to the way RenderObject are stored since their size could be 256 if the game is MOH or
  276 if the game is MOH:Underground.
  In order to gain this information we need to find the start and end position of the RenderObject's data.
  RenderObjects usually starts at 1444 (3492 with the header) with an integer that tells how many RenderObject we have to load.
  Since MOH:Underground uses a slightly different offset we check if the value is not valid (either 0 or too big) and move the position
  to 1360 (3508 with the header).
  This could be enough to determine what version we are running but just to be on the safe side we also calculate the ending position by using
  the node table entry and color table to determine the end address of the RenderObject list.
  By subtracting the end and start position and dividing by the number of RenderObjects we find the actual size used.
 */
int BSDGetRenderObjectSize(BSD_t *BSD,FILE *BSDFile)
{
    int NumAnimatedLights;
    int NumColors;
    int NumRenderObjects;
    int StartRenderObjectDataOffset;
    int EndRenderObjectDataOffset;
    int AnimatedLightColorSectionSize;
    int i;
    
    fseek(BSDFile,BSD_ANIMATED_LIGHTS_FILE_POSITION + BSD_HEADER_SIZE, SEEK_SET);
    fread(&NumAnimatedLights,sizeof(NumAnimatedLights),1,BSDFile);
    AnimatedLightColorSectionSize = 0;
    if( NumAnimatedLights != 0 ) {
        for( i = 0; i < BSD_ANIMATED_LIGHTS_TABLE_SIZE; i++ ) {
            fread(&NumColors,sizeof(NumColors),1,BSDFile);
            fseek(BSDFile,16,SEEK_CUR);
            AnimatedLightColorSectionSize += NumColors * 4;
        }
    }
    fseek(BSDFile,BSD_RENDER_OBJECT_STARTING_OFFSET + BSD_HEADER_SIZE,SEEK_SET);
    fread(&NumRenderObjects,sizeof(NumRenderObjects),1,BSDFile);
    if( NumRenderObjects == 0 || NumRenderObjects > 1000 ) {
        //HACK HACK HACK:MOH:Underground stores it at a different offset...
        //NOTE(Adriano):By now, if we hit this branch we know that this file belongs to MOH:Underground...
        fseek(BSDFile,BSD_RENDER_OBJECT_STARTING_OFFSET + 16 + BSD_HEADER_SIZE,SEEK_SET);
        fread(&NumRenderObjects,sizeof(NumRenderObjects),1,BSDFile);
    }
    DPrintf("BSDGetRenderObjectSize:We have %i RenderObjects\n",NumRenderObjects);
    StartRenderObjectDataOffset = ftell(BSDFile);
    EndRenderObjectDataOffset = (BSD->EntryTable.NodeTableOffset  - AnimatedLightColorSectionSize) + BSD_HEADER_SIZE;
    assert(NumRenderObjects > 0);
    return ( EndRenderObjectDataOffset - StartRenderObjectDataOffset ) / NumRenderObjects;
}

int BSDReadEntryTableChunk(BSD_t *BSD,FILE *BSDFile)
{    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDReadEntryTableChunk: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return 0;
    }

    fseek(BSDFile,BSD_ENTRY_TABLE_FILE_POSITION + BSD_HEADER_SIZE,SEEK_SET);
    assert(sizeof(BSD->EntryTable) == 80);
    fread(&BSD->EntryTable,sizeof(BSD->EntryTable),1,BSDFile);
    DPrintf("Node table is at %i (%i)\n",BSD->EntryTable.NodeTableOffset,BSD->EntryTable.NodeTableOffset + BSD_HEADER_SIZE);
    DPrintf("Unknown data is at %i (%i)\n",BSD->EntryTable.UnknownDataOffset,BSD->EntryTable.UnknownDataOffset + BSD_HEADER_SIZE);
    DPrintf("AnimationTableOffset is at %i (%i) and contains %i elements.\n",BSD->EntryTable.AnimationTableOffset,
            BSD->EntryTable.AnimationTableOffset + BSD_HEADER_SIZE,BSD->EntryTable.NumAnimationTableEntries);
    DPrintf("AnimationDataOffset is at %i (%i) contains %i elements.\n",BSD->EntryTable.AnimationDataOffset,
            BSD->EntryTable.AnimationDataOffset + BSD_HEADER_SIZE,BSD->EntryTable.NumAnimationData);
    DPrintf("AnimationQuaternionDataOffset is at %i (%i) and contains %i elements.\n",BSD->EntryTable.AnimationQuaternionDataOffset,
            BSD->EntryTable.AnimationQuaternionDataOffset + BSD_HEADER_SIZE,BSD->EntryTable.NumAnimationQuaternionData);
    DPrintf("AnimationHierarchyDataOffset is at %i (%i) and contains %i elements.\n",BSD->EntryTable.AnimationHierarchyDataOffset,
            BSD->EntryTable.AnimationHierarchyDataOffset + BSD_HEADER_SIZE,BSD->EntryTable.NumAnimationHierarchyData);
    DPrintf("AnimationFaceTableOffset is at %i (%i) and contains %i elements.\n",BSD->EntryTable.AnimationFaceTableOffset,
            BSD->EntryTable.AnimationFaceTableOffset + BSD_HEADER_SIZE,BSD->EntryTable.NumAnimationFaceTables);
    DPrintf("AnimationFaceDataOffset is at %i (%i) and contains %i elements.\n",BSD->EntryTable.AnimationFaceDataOffset,
            BSD->EntryTable.AnimationFaceDataOffset + BSD_HEADER_SIZE,BSD->EntryTable.NumAnimationFaces);
    DPrintf("AnimationVertexTableIndexOffset is at %i (%i) and contains %i elements.\n",BSD->EntryTable.AnimationVertexTableIndexOffset,
            BSD->EntryTable.AnimationVertexTableIndexOffset + BSD_HEADER_SIZE,BSD->EntryTable.NumAnimationVertexTableIndex);
    DPrintf("AnimationVertexTableOffset is at %i (%i) and contains %i elements.\n",BSD->EntryTable.AnimationVertexTableOffset,
            BSD->EntryTable.AnimationVertexTableOffset + BSD_HEADER_SIZE,BSD->EntryTable.NumAnimationVertexTableEntry);
    DPrintf("AnimationVertexDataOffset is at %i (%i) and contains %i elements.\n",BSD->EntryTable.AnimationVertexDataOffset,
            BSD->EntryTable.AnimationVertexDataOffset + BSD_HEADER_SIZE,BSD->EntryTable.NumAnimationVertex);
    return 1;
}
BSDRenderObjectElement_t *BSDGetRenderObject(BSD_t *BSD,int RenderObjectID)
{
    int i;
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObject[i].Id == RenderObjectID ) {
            return &BSD->RenderObjectTable.RenderObject[i];
        }
    }
    return NULL;
}
/*
 Some RenderObjects uses the 'ReferencedRenderObjectId' field to reference a RenderObject that contains common
 informations shared by multiple RenderObjects.
 In order to correctly parse these entry we need to copy the field from the 'ReferencedRenderObjectId' to the RenderObject that
 requested it.
 NOTE(Adriano):Make sure to update the data when new fields are added.
 */
void BSDPatchRenderObjects(BSD_t *BSD,FILE *BSDFile)
{
    BSDRenderObjectElement_t *CurrentRenderObject;
    BSDRenderObjectElement_t *ReferencedRenderObject;
    int i;
        
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId == -1 ) {
            continue;
        }
        ReferencedRenderObject = BSDGetRenderObject(BSD,BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId);
        CurrentRenderObject = &BSD->RenderObjectTable.RenderObject[i];
        if( !ReferencedRenderObject ) {
            DPrintf("BSDPatchRenderObjects:RenderObject Id %i not found\n",BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId);
            continue;
        }
        DPrintf("BSDPatchRenderObjects:Patching up RenderObject Id %i using Id %i\n",BSD->RenderObjectTable.RenderObject[i].Id,
            BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId
        );
        CurrentRenderObject->FaceOffset = ReferencedRenderObject->FaceOffset;
        CurrentRenderObject->FaceTableOffset = ReferencedRenderObject->FaceTableOffset;
        CurrentRenderObject->UnknownOffset4 = ReferencedRenderObject->UnknownOffset4;
        CurrentRenderObject->VertexTableIndexOffset = ReferencedRenderObject->VertexTableIndexOffset;
        CurrentRenderObject->VertOffset = ReferencedRenderObject->VertOffset;
        CurrentRenderObject->NumVertex = ReferencedRenderObject->NumVertex;
        CurrentRenderObject->RootBoneOffset = ReferencedRenderObject->RootBoneOffset;
        CurrentRenderObject->ColorOffset = ReferencedRenderObject->ColorOffset;
        if( CurrentRenderObject->Type == -1 ) {
            CurrentRenderObject->Type = ReferencedRenderObject->Type;
        }
    }
}
int BSDReadRenderObjectChunk(BSD_t *BSD,int GameEngine,FILE *BSDFile)
{
    int RenderObjectPositionOffset;
    int FirstRenderObjectPosition;
    int i;
    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDReadRenderObjectChunk: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return 0;
    }
    
    RenderObjectPositionOffset = 0;

    if( GameEngine == MOH_GAME_UNDERGROUND) {
        RenderObjectPositionOffset += 16;
    }
    fseek(BSDFile,BSD_RENDER_OBJECT_STARTING_OFFSET + RenderObjectPositionOffset + BSD_HEADER_SIZE,SEEK_SET);
    fread(&BSD->RenderObjectTable.NumRenderObject,sizeof(BSD->RenderObjectTable.NumRenderObject),1,BSDFile);
    FirstRenderObjectPosition = GetCurrentFilePosition(BSDFile);
    
    DPrintf("BSDReadRenderObjectChunk:Reading %i RenderObject Elements...\n",BSD->RenderObjectTable.NumRenderObject);
    
    assert(sizeof(BSDRenderObjectElement_t) == MOH_RENDER_OBJECT_SIZE);
    
    BSD->RenderObjectTable.RenderObject = malloc(BSD->RenderObjectTable.NumRenderObject * sizeof(BSDRenderObjectElement_t));
    if( !BSD->RenderObjectTable.RenderObject ) {
        DPrintf("BSDReadRenderObjectChunk:Failed to allocate memory for RenderObject Array\n");
        return 0;
    }
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            assert(GetCurrentFilePosition(BSDFile) == FirstRenderObjectPosition + (i * MOH_UNDERGROUND_RENDER_OBJECT_SIZE));
        } else {
            assert(GetCurrentFilePosition(BSDFile) == FirstRenderObjectPosition + (i * MOH_RENDER_OBJECT_SIZE));
        }
        DPrintf("Reading RenderObject %i at %i\n",i,GetCurrentFilePosition(BSDFile));
        fread(&BSD->RenderObjectTable.RenderObject[i],sizeof(BSD->RenderObjectTable.RenderObject[i]),1,BSDFile);
        DPrintf("RenderObject Id:%u\n",BSD->RenderObjectTable.RenderObject[i].Id);
        if( BSD->RenderObjectTable.RenderObject[i].Type == 1 ) {
            DPrintf("RenderObject Type:%i\n",BSD->RenderObjectTable.RenderObject[i].Type);
        } else {
            DPrintf("RenderObject Type:%i\n",BSD->RenderObjectTable.RenderObject[i].Type);
        }
        if( BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId != -1 ) {
            DPrintf("RenderObject References RenderObject Id:%u\n",BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId);
        } else {
            DPrintf("RenderObject No Reference set...\n");
        }
        DPrintf("RenderObject Element Vertex Offset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].VertOffset,
                BSD->RenderObjectTable.RenderObject[i].VertOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Element NumVertex: %i\n",BSD->RenderObjectTable.RenderObject[i].NumVertex);
        //These offsets are relative to the EntryTable.
        DPrintf("RenderObject FaceTableOffset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].FaceTableOffset,
                BSD->RenderObjectTable.RenderObject[i].FaceTableOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject VertexTableIndexOffset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].VertexTableIndexOffset,
                BSD->RenderObjectTable.RenderObject[i].VertexTableIndexOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Root Bone Offset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].RootBoneOffset,
                BSD->RenderObjectTable.RenderObject[i].RootBoneOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject FaceOffset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].FaceOffset,
                BSD->RenderObjectTable.RenderObject[i].FaceOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Scale: %i;%i;%i (4096 is 1 meaning no scale)\n",
                BSD->RenderObjectTable.RenderObject[i].ScaleX / 4,
                BSD->RenderObjectTable.RenderObject[i].ScaleY / 4,
                BSD->RenderObjectTable.RenderObject[i].ScaleZ / 4);
        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            SkipFileSection(20,BSDFile);
        }
    }
    BSDPatchRenderObjects(BSD,BSDFile);
    return 1;
}

int BSDLoadAnimationVertexData(BSDRenderObject_t *RenderObject,int VertexTableIndexOffset,BSDEntryTable_t EntryTable,FILE *BSDFile)
{
    int VertexTableOffset;
    int i;
    int j;
    
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDLoadAnimationVertexData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return 0;
    }
    if( VertexTableIndexOffset == -1 ) {
        DPrintf("BSDLoadAnimationVertexData:Invalid Vertex Table Index Offset\n");
        return 0;
    }
  
    VertexTableIndexOffset += EntryTable.AnimationVertexTableIndexOffset + BSD_HEADER_SIZE;
    fseek(BSDFile,VertexTableIndexOffset,SEEK_SET);
    fread(&VertexTableOffset,sizeof(VertexTableOffset),1,BSDFile);
    fread(&RenderObject->NumVertexTables,sizeof(RenderObject->NumVertexTables),1,BSDFile);
    VertexTableOffset += EntryTable.AnimationVertexTableOffset + BSD_HEADER_SIZE;
    fseek(BSDFile,VertexTableOffset,SEEK_SET);
    
    RenderObject->VertexTable = malloc(RenderObject->NumVertexTables * sizeof(BSDVertexTable_t));
    if( !RenderObject->VertexTable ) {
        DPrintf("BSDLoadAnimationVertexData:Failed to allocate memory for VertexTable.\n");
        return 0;
    }
    for( i = 0; i < RenderObject->NumVertexTables; i++ ) {
        fread(&RenderObject->VertexTable[i].Offset,sizeof(RenderObject->VertexTable[i].Offset),1,BSDFile);
        fread(&RenderObject->VertexTable[i].NumVertex,sizeof(RenderObject->VertexTable[i].NumVertex),1,BSDFile);
        RenderObject->VertexTable[i].VertexList = NULL;
    }
    
    fseek(BSDFile,EntryTable.AnimationVertexDataOffset + BSD_HEADER_SIZE,SEEK_SET);
    for( i = 0; i < RenderObject->NumVertexTables; i++ ) {
        if( RenderObject->VertexTable[i].Offset == -1 ) {
            continue;
        }
        fseek(BSDFile,EntryTable.AnimationVertexDataOffset + RenderObject->VertexTable[i].Offset + BSD_HEADER_SIZE,SEEK_SET);
        RenderObject->VertexTable[i].VertexList = malloc(RenderObject->VertexTable[i].NumVertex * sizeof(BSDVertex_t));
        for( j = 0; j < RenderObject->VertexTable[i].NumVertex; j++ ) {
            fread(&RenderObject->VertexTable[i].VertexList[j],sizeof(BSDVertex_t),1,BSDFile);
        }
    }
    return 1;
}
int BSDLoadAnimationFaceData(BSDRenderObject_t *RenderObject,int FaceTableOffset,BSDEntryTable_t EntryTable,FILE *BSDFile)
{
    int GlobalFaceTableOffset;
    int GlobalFaceDataOffset;
    int FaceDataOffset;
    int NumFaces;
    int i;
    
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDLoadAnimationFaceData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return 0;
    }
    if( FaceTableOffset == -1 ) {
        DPrintf("BSDLoadAnimationFaceData:Invalid Face Table Index Offset\n");
        return 0;
    }
    GlobalFaceTableOffset = EntryTable.AnimationFaceTableOffset + FaceTableOffset + BSD_HEADER_SIZE;
    fseek(BSDFile,GlobalFaceTableOffset,SEEK_SET);
    fread(&FaceDataOffset,sizeof(FaceDataOffset),1,BSDFile);
    fread(&NumFaces,sizeof(NumFaces),1,BSDFile);
    GlobalFaceDataOffset = EntryTable.AnimationFaceDataOffset + FaceDataOffset + BSD_HEADER_SIZE;
    fseek(BSDFile,GlobalFaceDataOffset,SEEK_SET);
    assert(sizeof(BSDAnimatedModelFace_t) == 28);
    RenderObject->FaceList = malloc(NumFaces * sizeof(BSDAnimatedModelFace_t));
    if( !RenderObject->FaceList ) {
        DPrintf("BSDLoadAnimationFaceData:Failed to allocate memory for face list.\n");
        return 0;
    }
    for( i = 0; i < NumFaces; i++ ) {
        fread(&RenderObject->FaceList[i],sizeof(BSDAnimatedModelFace_t),1,BSDFile);
        int ColorMode = (RenderObject->FaceList[i].TexInfo >> 7) & 0x3;
        int VRAMPage = RenderObject->FaceList[i].TexInfo & 0x1F;
        int ABRRate = (RenderObject->FaceList[i].TexInfo & 0x60) >> 5;
        int CLUTPosX = (RenderObject->FaceList[i].CLUT << 4) & 0x3F0;
        int CLUTPosY = (RenderObject->FaceList[i].CLUT >> 6) & 0x1ff;
        DPrintf(" -- FACE %i --\n",i);
        DPrintf("Tex info %i | Color mode %i | Texture Page %i | ABR Rate %i\n",RenderObject->FaceList[i].TexInfo,
                ColorMode,VRAMPage,ABRRate);
        DPrintf("CLUT:%i X:%i Y:%i\n",RenderObject->FaceList[i].CLUT,CLUTPosX,CLUTPosY);
        DPrintf("UV0:(%i;%i)\n",RenderObject->FaceList[i].UV0.u,RenderObject->FaceList[i].UV0.v);
        DPrintf("UV1:(%i;%i)\n",RenderObject->FaceList[i].UV1.u,RenderObject->FaceList[i].UV1.v);
        DPrintf("UV2:(%i;%i)\n",RenderObject->FaceList[i].UV2.u,RenderObject->FaceList[i].UV2.v);
        DPrintf("RGB0:(%i;%i;%i)\n",RenderObject->FaceList[i].RGB0.r,RenderObject->FaceList[i].RGB0.g,RenderObject->FaceList[i].RGB0.b);
        DPrintf("RGB1:(%i;%i;%i)\n",RenderObject->FaceList[i].RGB1.r,RenderObject->FaceList[i].RGB1.g,RenderObject->FaceList[i].RGB1.b);
        DPrintf("RGB2:(%i;%i;%i)\n",RenderObject->FaceList[i].RGB2.r,RenderObject->FaceList[i].RGB2.g,RenderObject->FaceList[i].RGB2.b);
        DPrintf("Table Index 0 %i Data %i.\n",RenderObject->FaceList[i].VertexTableIndex0,RenderObject->FaceList[i].VertexTableDataIndex0);
        DPrintf("Table Index 1 %i Data %i.\n",RenderObject->FaceList[i].VertexTableIndex1,RenderObject->FaceList[i].VertexTableDataIndex1);
        DPrintf("Table Index 2 %i Data %i.\n",RenderObject->FaceList[i].VertexTableIndex2,RenderObject->FaceList[i].VertexTableDataIndex2);
    }
    return 1;
}
BSDRenderObject_t *BSDLoadAnimatedRenderObject(BSDRenderObjectElement_t RenderObjectElement,BSDEntryTable_t BSDEntryTable,FILE *BSDFile)
{
    BSDRenderObject_t *RenderObject;
    
    RenderObject = NULL;
    if( !BSDFile ) {
        DPrintf("BSDLoadAnimatedRenderObject:Invalid BSD file\n");
        goto Failure;
    }
    RenderObject = malloc(sizeof(BSDRenderObject_t));
    RenderObject->VertexTable = NULL;
    RenderObject->FaceList = NULL;
    if( !RenderObject ) {
        DPrintf("BSDLoadAnimatedRenderObject:Failed to allocate memory for RenderObject\n");
        goto Failure;
    }
    if( !BSDLoadAnimationVertexData(RenderObject,RenderObjectElement.VertexTableIndexOffset,BSDEntryTable,BSDFile) ) {
        DPrintf("BSDLoadAnimatedRenderObject:Failed to load vertex data\n");
        goto Failure;
    }
    if( !BSDLoadAnimationFaceData(RenderObject,RenderObjectElement.FaceTableOffset,BSDEntryTable,BSDFile) ) {
        DPrintf("BSDLoadAnimatedRenderObject:Failed to load face data\n");
        goto Failure;  
    }
    return RenderObject;
Failure:
    BSDFreeRenderObject(RenderObject);
    return NULL;
}
BSD_t *BSDLoad(FILE *BSDFile)
{
    BSD_t *BSD;
    int BSDVersion;
    int RenderObjectSize;
    
    BSD = NULL;
    
    BSD = malloc(sizeof(BSD_t));
    
    if( !BSDReadEntryTableChunk(BSD,BSDFile) ) {
        goto Failure;
    }
    RenderObjectSize = BSDGetRenderObjectSize(BSD,BSDFile);
    assert( RenderObjectSize == MOH_RENDER_OBJECT_SIZE || RenderObjectSize == MOH_UNDERGROUND_RENDER_OBJECT_SIZE );
    if( RenderObjectSize == MOH_RENDER_OBJECT_SIZE ) {
        BSDVersion = MOH_GAME_STANDARD;
    } else {
        BSDVersion = MOH_GAME_UNDERGROUND;
    }
    DPrintf("Running BSD from %s since size is %i\n",BSDVersion == MOH_GAME_STANDARD ? 
        "Medal Of Honor" : "Medal Of Honor:Underground",RenderObjectSize);
    if( !BSDReadRenderObjectChunk(BSD,BSDVersion,BSDFile) ) {
        goto Failure;
    }
    return BSD;
Failure:
    BSDFree(BSD);
    if( BSDFile ) {
        fclose(BSDFile);
    }
    return NULL;
}

BSDRenderObject_t *BSDLoadAllAnimatedRenderObjects(const char *FName)
{
    FILE *BSDFile;
    BSD_t *BSD;
    BSDRenderObject_t *RenderObjectList;
    BSDRenderObject_t *RenderObject;
    int i;
    
    BSDFile = fopen(FName,"rb");
    if( BSDFile == NULL ) {
        DPrintf("Failed opening BSD File %s.\n",FName);
        goto Failure;
    }
    BSD = BSDLoad(BSDFile);
    if( !BSD ) {
        return NULL;
    }
    RenderObjectList = NULL;
    
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObject[i].AnimationDataOffset == -1 ) {
            continue;
        }
        DPrintf("BSDLoadAllAnimatedRenderObjects:Loading Animated RenderObject %u\n",BSD->RenderObjectTable.RenderObject[i].Id);
        RenderObject = BSDLoadAnimatedRenderObject(BSD->RenderObjectTable.RenderObject[i],BSD->EntryTable,
                                                   BSDFile);
        if( !RenderObject ) {
            DPrintf("BSDLoadAllAnimatedRenderObjects:Failed to load animated RenderObject.\n");
            continue;
        }
        RenderObject->Next = RenderObjectList;
        RenderObjectList = RenderObject;
    }
    
    BSDFree(BSD);
    return RenderObjectList;
Failure:
    if( BSDFile ) {
        fclose(BSDFile);
    }
    return NULL;
}
