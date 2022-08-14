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

void BSDRecusivelyFreeHierarchyBone(BSDHierarchyBone_t *Bone)
{
    if( !Bone ) {
        return;
    }
    BSDRecusivelyFreeHierarchyBone(Bone->Child2);
    BSDRecusivelyFreeHierarchyBone(Bone->Child1);
    free(Bone);
}
void BSDFreeRenderObject(BSDRenderObject_t *RenderObject)
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
        }
        free(RenderObject->VertexTable);
    }
    if( RenderObject->FaceList ) {
        free(RenderObject->FaceList);
    }
    if( RenderObject->HierarchyData ) {
        BSDRecusivelyFreeHierarchyBone(RenderObject->HierarchyData);
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
            }
            free(RenderObject->AnimationList[i].Frame);
        }
        free(RenderObject->AnimationList);
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
  Since MOH:Underground uses a slightly different offset we need to check if the value is not valid (either 0 or too big) and move the position
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
    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDGetRenderObjectSize: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return 0;
    }
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
    DPrintf("Reading table at %li\n",ftell(BSDFile));
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
BSDRenderObjectElement_t *BSDGetRenderObjectById(const BSD_t *BSD,int RenderObjectId)
{
    int i;
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObject[i].Id == RenderObjectId ) {
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
        ReferencedRenderObject = BSDGetRenderObjectById(BSD,BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId);
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
        CurrentRenderObject->VertexOffset = ReferencedRenderObject->VertexOffset;
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
    int MOHUndegroundFaceDataOffset;
    int FirstRenderObjectPosition;
    int i;
    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDReadRenderObjectChunk: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return 0;
    }
    
    fseek(BSDFile,BSD_RENDER_OBJECT_STARTING_OFFSET + BSD_HEADER_SIZE,SEEK_SET);
    if( GameEngine == MOH_GAME_UNDERGROUND) {
        fread(&MOHUndegroundFaceDataOffset,sizeof(MOHUndegroundFaceDataOffset),1,BSDFile);
        DPrintf("BSDReadRenderObjectChunk:Got MOH:Underground Face Offset:%i\n",MOHUndegroundFaceDataOffset);
        fseek(BSDFile,12,SEEK_CUR);
    }
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
        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            SkipFileSection(20,BSDFile);
        }
        DPrintf("RenderObject Id:%u\n",BSD->RenderObjectTable.RenderObject[i].Id);
        DPrintf("RenderObject Type:%i\n",BSD->RenderObjectTable.RenderObject[i].Type);
        
        if( BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId != -1 ) {
            DPrintf("RenderObject References RenderObject Id:%u\n",BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId);
        } else {
            DPrintf("RenderObject No Reference set...\n");
        }
        DPrintf("RenderObject Element Animation Offset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].AnimationDataOffset,
                BSD->RenderObjectTable.RenderObject[i].AnimationDataOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Element Vertex Offset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].VertexOffset,
                BSD->RenderObjectTable.RenderObject[i].VertexOffset + BSD_HEADER_SIZE);
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
void BSDCopyAnimatedModelFace(BSDAnimatedModelFace_t Src,BSDAnimatedModelFace_t *Dest)
{
    if( !Dest ) {
        DPrintf("BSDCopyAnimatedModelFace:Invalid Destination\n");
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
void BSDPrintAnimatedModelFace(BSDAnimatedModelFace_t Face)
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
    DPrintf("Table Index0 %i Data %i.\n",Face.VertexTableIndex0,Face.VertexTableDataIndex0);
    DPrintf("Table Index1 %i Data %i.\n",Face.VertexTableIndex1,Face.VertexTableDataIndex1);
    DPrintf("Table Index2 %i Data %i.\n",Face.VertexTableIndex2,Face.VertexTableDataIndex2);
}
/*
 * NOTE(Adriano):
 * MOH:Underground stores face data in a different way than the one used by Medal Of Honor.
 * In order to obtain the offset we need to first read the Global Face Offset which is stored at position
 * 0x5A4 and the total number of faces is stored at 0x5A8 (both offset are stored without counting the header size 2048).
 * Then we need to load the data from the RenderObject stored at position 0x106 (as a short) and 0x108 that are needed to load all the
 * required faces.
 * Data at 0x106 contains the number of faces that we need to load while the offset at 0x108 contains the local offset that must
 * be added to the global one in order to obtain the final face data position inside the BSD file.
 */
int BSDLoadMOHUndergroundAnimationFaceData(BSDRenderObject_t *RenderObject,int FaceTableOffset,int RenderObjectIndex,
                                           BSDEntryTable_t EntryTable,FILE *BSDFile)
{
    int GlobalFaceOffset;
    int LocalFaceOffset;
    short NumFaces;
    int CurrentFaceIndex;
    unsigned int Marker;
    BSDColor_t ColorData;
    BSDAnimatedModelFace_t TempFace;

    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDLoadMOHUndergroundAnimationFaceData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return 0;
    }
    fseek(BSDFile,BSD_RENDER_OBJECT_STARTING_OFFSET + BSD_HEADER_SIZE,SEEK_SET);
    fread(&GlobalFaceOffset,sizeof(GlobalFaceOffset),1,BSDFile);
    fseek(BSDFile,16 + (RenderObjectIndex * MOH_UNDERGROUND_RENDER_OBJECT_SIZE) + 262,SEEK_CUR);
    fread(&NumFaces,sizeof(NumFaces),1,BSDFile);
    fread(&LocalFaceOffset,sizeof(LocalFaceOffset),1,BSDFile);
    if( !NumFaces ) {
        DPrintf("BSDLoadMOHUndergroundAnimationFaceData:Invalid number of faces.\n");
        return 0;
    }
    //NOTE(Adriano):Now we can load the faces at the offset specified by the RenderObject.
    fseek(BSDFile,GlobalFaceOffset + LocalFaceOffset + BSD_HEADER_SIZE,SEEK_SET);
    DPrintf("BSDLoadMOHUndergroundAnimationFaceData:Reading %i faces at offset %li (%li)\n",NumFaces,ftell(BSDFile),ftell(BSDFile) - 2048);
    
    RenderObject->FaceList = malloc(NumFaces * sizeof(BSDAnimatedModelFace_t));
    if( !RenderObject->FaceList ) {
        DPrintf("BSDLoadAnimationFaceData:Failed to allocate memory for face list.\n");
        return 0;
    }
    CurrentFaceIndex = 0;
    while( CurrentFaceIndex < NumFaces ) {
        fread(&TempFace,sizeof(BSDAnimatedModelFace_t),1,BSDFile);
    
        BSDCopyAnimatedModelFace(TempFace,&RenderObject->FaceList[CurrentFaceIndex]);
        

        DPrintf(" -- FACE %i --\n",CurrentFaceIndex);
        BSDPrintAnimatedModelFace(RenderObject->FaceList[CurrentFaceIndex]);
        CurrentFaceIndex++;
        while( 1 ) {
            fread(&Marker,sizeof(Marker),1,BSDFile);
            if( ( Marker & 0x1FFF ) == 0x1FFF || Marker == 0x1fff1fff ) {
                DPrintf("BSDLoadAnimationFaceData:Aborting since a marker was found\n");
                break;
            }
            fread(&ColorData,sizeof(ColorData),1,BSDFile);
                    
            if( (Marker & 0x8000) != 0 ) {
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
            TempFace.VertexTableIndex2 = (Marker & 0x1FFF) >> 8;
            TempFace.VertexTableDataIndex2 = (Marker & 0x1FFF) & 0xFF;
            TempFace.UV2.u = (Marker >> 0x10) & 0xff;
            TempFace.UV2.v = (Marker >> 0x10) >> 8;
            TempFace.RGB2 = ColorData;
            
            BSDCopyAnimatedModelFace(TempFace,&RenderObject->FaceList[CurrentFaceIndex]);
            BSDPrintAnimatedModelFace(RenderObject->FaceList[CurrentFaceIndex]);
            CurrentFaceIndex++;
        }
        if( Marker == 0x1fff1fff ) {
            DPrintf("BSDLoadAnimationFaceData:Sentinel Face found Done reading faces for RenderObject\n");
            DPrintf("BSDLoadAnimationFaceData:Loaded %i faces (Expected %i)\n",CurrentFaceIndex,NumFaces);
            break;
        }
    }
    assert(CurrentFaceIndex == NumFaces);
    return 1;
}
int BSDLoadAnimationFaceData(BSDRenderObject_t *RenderObject,int FaceTableOffset,int RenderObjectIndex,
                             BSDEntryTable_t EntryTable,FILE *BSDFile,int GameVersion)
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
        if( GameVersion == MOH_GAME_UNDERGROUND ) {
            DPrintf("BSDLoadAnimationFaceData:Game is MOH:Underground...attempting to use a different face loader.\n");
            return BSDLoadMOHUndergroundAnimationFaceData(RenderObject,FaceTableOffset,RenderObjectIndex,EntryTable,BSDFile);
        } else {
            DPrintf("BSDLoadAnimationFaceData:Invalid Face Table Index Offset\n");
            return 0;
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
    if( !RenderObject->FaceList ) {
        DPrintf("BSDLoadAnimationFaceData:Failed to allocate memory for face list.\n");
        return 0;
    }
    for( i = 0; i < NumFaces; i++ ) {
        fread(&RenderObject->FaceList[i],sizeof(BSDAnimatedModelFace_t),1,BSDFile);
        DPrintf(" -- FACE %i --\n",i);
        BSDPrintAnimatedModelFace(RenderObject->FaceList[i]);
    }
    return 1;
}

BSDHierarchyBone_t *BSDRecursivelyLoadHierarchyData(int BoneDataStartingPosition,int BoneOffset,FILE *BSDFile)
{
    BSDHierarchyBone_t *Bone;
    int Child1Offset;
    int Child2Offset;
    
    if( !BSDFile ) {
        DPrintf("BSDRecursivelyLoadHierarchyData:Invalid Bone Table file\n");
        return NULL;
    }

    
    Bone = malloc(sizeof(BSDHierarchyBone_t));
    Bone->Child1 = NULL;
    Bone->Child2 = NULL;
    
    if( !Bone ) {
        DPrintf("BSDRecursivelyLoadHierarchyData:Failed to allocate bone data\n");
        return NULL;
    }

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
        Bone->Child2 = BSDRecursivelyLoadHierarchyData(BoneDataStartingPosition,Child2Offset,BSDFile);
    }
    if( Child1Offset != -1 ) {
        Bone->Child1 = BSDRecursivelyLoadHierarchyData(BoneDataStartingPosition,Child1Offset,BSDFile);
    }
    return Bone;
}

int BSDLoadAnimationHierarchyData(BSDRenderObject_t *RenderObject,int RootBoneOffset,BSDEntryTable_t EntryTable,FILE *BSDFile)
{
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDLoadAnimationHierarchyData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return 0;
    }
    if( RootBoneOffset == -1 ) {
        DPrintf("BSDLoadAnimationHierarchyData:Invalid Face Table Index Offset\n");
        return 0;
    }
    
    RenderObject->HierarchyData = BSDRecursivelyLoadHierarchyData(EntryTable.AnimationHierarchyDataOffset,RootBoneOffset,BSDFile);
    
    if( !RenderObject->HierarchyData ) {
        DPrintf("BSDLoadAnimationHierarchyData:Couldn't load hierarchy data\n");
        return 0;
    }
    return 1;
}
int BSDLoadAnimationData(BSDRenderObject_t *RenderObject,int AnimationDataOffset,BSDEntryTable_t EntryTable,FILE *BSDFile)
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
    BSDQuaternion_t TempQuaternion;
    int Base;
    int QuatPart0;
    int QuatPart1;
    int QuatPart2;
    int NumEncodedQuaternions;
    int NumDecodedQuaternions;
    
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDLoadAnimationData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return 0;
    }
    if( AnimationDataOffset == -1 ) {
        DPrintf("BSDLoadAnimationData:Invalid Vertex Table Index Offset\n");
        return 0;
    }
    fseek(BSDFile,AnimationDataOffset + BSD_HEADER_SIZE,SEEK_SET);
    fread(&NumAnimationOffset,sizeof(NumAnimationOffset),1,BSDFile);
    fread(&Pad,sizeof(Pad),1,BSDFile);
    assert(Pad == 52685);
    
    AnimationOffsetTable = malloc(NumAnimationOffset * sizeof(int) );
    RenderObject->NumAnimations = 0;
    for( i = 0; i < NumAnimationOffset; i++ ) {
        fread(&AnimationOffsetTable[i],sizeof(AnimationOffsetTable[i]),1,BSDFile);
        if( AnimationOffsetTable[i] == -1 ) {
            continue;
        }
        RenderObject->NumAnimations++;
    }

    AnimationTableEntry = malloc(RenderObject->NumAnimations * sizeof(BSDAnimationTableEntry_t) );
    for( i = 0; i < RenderObject->NumAnimations; i++ ) {
        DPrintf("BSDLoadAnimationData:Animation Offset %i for entry %i\n",AnimationOffsetTable[i],i);
        if( AnimationOffsetTable[i] == -1 ) {
            continue;
        }
        DPrintf("BSDLoadAnimationData:Going to %i plus %i\n",EntryTable.AnimationTableOffset,AnimationOffsetTable[i]);
        fseek(BSDFile,EntryTable.AnimationTableOffset + AnimationOffsetTable[i] + BSD_HEADER_SIZE,SEEK_SET);
        fread(&AnimationTableEntry[i],sizeof(AnimationTableEntry[i]),1,BSDFile);
        DPrintf("BSDLoadAnimationData:AnimationEntry %i has pad %i\n",i,AnimationTableEntry[i].Pad);
        DPrintf("BSDLoadAnimationData:Loading %i vertices Size %i\n",AnimationTableEntry[i].NumAffectedVertex,
                AnimationTableEntry[i].NumAffectedVertex * 8);
        DPrintf("BSDLoadAnimationData: NumFrames %u NumAffectedVertex:%u || Offset %i\n",AnimationTableEntry[i].NumFrames,
                AnimationTableEntry[i].NumAffectedVertex,AnimationTableEntry[i].Offset);
        assert(AnimationTableEntry[i].Pad == 52480);
    }
    RenderObject->AnimationList = malloc(RenderObject->NumAnimations * sizeof(BSDAnimation_t));
    for( i = 0; i < RenderObject->NumAnimations; i++ ) {
        RenderObject->AnimationList[i].Frame = NULL;
        if( AnimationOffsetTable[i] == -1 ) {
            continue;
        }
        DPrintf(" -- ANIMATION ENTRY %i -- \n",i);
        DPrintf("Loading %i animations for entry %i\n",AnimationTableEntry[i].NumFrames,i);
        
        RenderObject->AnimationList[i].Frame = malloc(AnimationTableEntry[i].NumFrames * sizeof(BSDAnimationFrame_t));
        RenderObject->AnimationList[i].NumFrames = AnimationTableEntry[i].NumFrames;
        for( j = 0; j < AnimationTableEntry[i].NumFrames; j++ ) {
                        DPrintf(" -- ANIMATION %i/%i -- \n",j,AnimationTableEntry[i].NumFrames);
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
            fread(&RenderObject->AnimationList[i].Frame[j].Type,
                  sizeof(RenderObject->AnimationList[i].Frame[j].Type),1,BSDFile);
            fread(&RenderObject->AnimationList[i].Frame[j].NumQuaternions,
                  sizeof(RenderObject->AnimationList[i].Frame[j].NumQuaternions),1,BSDFile);
            fread(&QuaternionListOffset,sizeof(QuaternionListOffset),1,BSDFile);

            RenderObject->AnimationList[i].Frame[j].Vector.x = (RenderObject->AnimationList[i].Frame[j].EncodedVector << 0x16) >> 0x16;
            RenderObject->AnimationList[i].Frame[j].Vector.y = (RenderObject->AnimationList[i].Frame[j].EncodedVector << 0xb)  >> 0x15;
            RenderObject->AnimationList[i].Frame[j].Vector.z = (RenderObject->AnimationList[i].Frame[j].EncodedVector << 0x1)  >> 0x16;
            DPrintf("Entry %i => U0|U1|U2: %i;%i;%i QuaternionListOffset:%i\n",i,RenderObject->AnimationList[i].Frame[j].U0,
                    RenderObject->AnimationList[i].Frame[j].U1,
                    RenderObject->AnimationList[i].Frame[j].U2,QuaternionListOffset);
            DPrintf("U3: %i\n",RenderObject->AnimationList[i].Frame[j].U3);
            DPrintf("U4 is %i\n",RenderObject->AnimationList[i].Frame[j].U4);
            DPrintf("U5 is %i\n",RenderObject->AnimationList[i].Frame[j].U5);
            DPrintf("Animation Type is %i -- Number of quaternions is %i\n",RenderObject->AnimationList[i].Frame[j].Type,
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
            if( QuaternionListOffset != -1 ) {
                fseek(BSDFile,EntryTable.AnimationQuaternionDataOffset + QuaternionListOffset + BSD_HEADER_SIZE,SEEK_SET);
                DPrintf("Reading Vector definition at %li\n",ftell(BSDFile));
                NumEncodedQuaternions = (RenderObject->AnimationList[i].Frame[j].NumQuaternions / 2) * 3;
                RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList = malloc( NumEncodedQuaternions * sizeof(int));
                for( w = 0; w < NumEncodedQuaternions; w++ ) {
                    fread(&RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[w],
                          sizeof(RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[w]),1,BSDFile);
                }
                DPrintf("Done...loaded a list of %i encoded quaternions\n",RenderObject->AnimationList[i].Frame[j].NumQuaternions * 2);
                RenderObject->AnimationList[i].Frame[j].QuaternionList = malloc( 
                    RenderObject->AnimationList[i].Frame[j].NumQuaternions * sizeof(BSDQuaternion_t));

                NumDecodedQuaternions = 0;
                for( q = 0; q < RenderObject->AnimationList[i].Frame[j].NumQuaternions / 2; q++ ) {
                    Base = q * 3;
                    DPrintf("Generating with base %i V0:%i V1:%i V2:%i\n",q,Base,Base+1,Base+2);
                    QuatPart0 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[Base];
                    QuatPart1 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[Base+1];
                    QuatPart2 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[Base+2];
                    TempQuaternion.x = ( (QuatPart0 << 0x10) >> 20) * 2;
                    TempQuaternion.y = (QuatPart1 << 0x14) >> 0x13;
                    TempQuaternion.z = ( ( ( (QuatPart1 >> 0xC) << 0x1C ) >> 0x14) | ( (QuatPart0 >> 0xC) & 0xF0) | (QuatPart0 & 0xF) ) * 2;
                    TempQuaternion.w = (QuatPart0 >> 0x14) * 2;
                    RenderObject->AnimationList[i].Frame[j].QuaternionList[q*2].x = TempQuaternion.x;
                    RenderObject->AnimationList[i].Frame[j].QuaternionList[q*2].y = TempQuaternion.y;
                    RenderObject->AnimationList[i].Frame[j].QuaternionList[q*2].z = TempQuaternion.z;
                    RenderObject->AnimationList[i].Frame[j].QuaternionList[q*2].w = TempQuaternion.w;

                    DPrintf("{%i,%i,%i,%i},\n",TempQuaternion.x,TempQuaternion.y,TempQuaternion.z,TempQuaternion.w);
                    TempQuaternion.x = (QuatPart1  >> 0x14) * 2;
                    TempQuaternion.y = ( (QuatPart2 << 0x4) >> 0x14 ) * 2;
                    TempQuaternion.w = ( (QuatPart2 << 0x10) >> 0x14) * 2;
                    TempQuaternion.z = ( (QuatPart2 >> 0x1C) << 0x8 | (QuatPart2 & 0xF ) << 0x4 | ( (QuatPart1 >> 0x10) & 0xF ) ) * 2;
                    RenderObject->AnimationList[i].Frame[j].QuaternionList[(q*2) + 1].x = TempQuaternion.x;
                    RenderObject->AnimationList[i].Frame[j].QuaternionList[(q*2) + 1].y = TempQuaternion.y;
                    RenderObject->AnimationList[i].Frame[j].QuaternionList[(q*2) + 1].z = TempQuaternion.z;
                    RenderObject->AnimationList[i].Frame[j].QuaternionList[(q*2) + 1].w = TempQuaternion.w;
                    DPrintf("{%i,%i,%i,%i},\n",TempQuaternion.x,TempQuaternion.y,TempQuaternion.z,TempQuaternion.w);
                    NumDecodedQuaternions += 2;
                }
                if( NumDecodedQuaternions == (RenderObject->AnimationList[i].Frame[j].NumQuaternions - 1) ) {
                    QuatPart0 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[0];
                    QuatPart1 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[1];
                    TempQuaternion.x = ( (QuatPart0 << 0x10) >> 0x14) * 2;
                    TempQuaternion.y = (QuatPart1 << 0x14) >> 0x13;
                    TempQuaternion.z = ( ( ( (QuatPart1 >> 0xC) << 0x1C ) >> 0x14) | ( (QuatPart0 >> 0xC) & 0xF0) | (QuatPart0 & 0xF) ) * 2;
                    TempQuaternion.w = (QuatPart0 >> 0x14) * 2;
                    NumDecodedQuaternions++;
                }
                DPrintf("Decoded %i out of %i\n",NumDecodedQuaternions,RenderObject->AnimationList[i].Frame[j].NumQuaternions);
                assert(NumDecodedQuaternions == RenderObject->AnimationList[i].Frame[j].NumQuaternions);
            } else {
                DPrintf("QuaternionListOffset is not valid...\n");
            }
        }
    }
    free(AnimationOffsetTable);
    free(AnimationTableEntry);
    return 1;
}
BSDRenderObject_t *BSDLoadAnimatedRenderObject(BSDRenderObjectElement_t RenderObjectElement,BSDEntryTable_t BSDEntryTable,FILE *BSDFile,
                                               int RenderObjectIndex,int GameVersion
)
{
    BSDRenderObject_t *RenderObject;
    
    RenderObject = NULL;
    if( !BSDFile ) {
        DPrintf("BSDLoadAnimatedRenderObject:Invalid BSD file\n");
        goto Failure;
    }
    RenderObject = malloc(sizeof(BSDRenderObject_t));
    if( !RenderObject ) {
        DPrintf("BSDLoadAnimatedRenderObject:Failed to allocate memory for RenderObject\n");
        goto Failure;
    }
    RenderObject->VertexTable = NULL;
    RenderObject->FaceList = NULL;
    RenderObject->HierarchyData = NULL;
    RenderObject->AnimationList = NULL;
    if( !BSDLoadAnimationVertexData(RenderObject,RenderObjectElement.VertexTableIndexOffset,BSDEntryTable,BSDFile) ) {
        DPrintf("BSDLoadAnimatedRenderObject:Failed to load vertex data\n");
        goto Failure;
    }

    if( !BSDLoadAnimationFaceData(RenderObject,RenderObjectElement.FaceTableOffset,RenderObjectIndex,BSDEntryTable,BSDFile,GameVersion) ) {
        DPrintf("BSDLoadAnimatedRenderObject:Failed to load face data\n");
        goto Failure;
    }

    if( !BSDLoadAnimationHierarchyData(RenderObject,RenderObjectElement.RootBoneOffset,BSDEntryTable,BSDFile) ) {
        DPrintf("BSDLoadAnimatedRenderObject:Failed to load hierarchy data\n");
        goto Failure;
    }
    if( !BSDLoadAnimationData(RenderObject,RenderObjectElement.AnimationDataOffset,BSDEntryTable,BSDFile) ) {
        DPrintf("BSDLoadAnimatedRenderObject:Failed to load animation data\n");
        goto Failure;
    }
    return RenderObject;
Failure:
    BSDFreeRenderObject(RenderObject);
    return NULL;
}
BSD_t *BSDLoad(FILE *BSDFile,int *GameVersion)
{
    BSD_t *BSD;
    int LocalGameVersion;
    int RenderObjectSize;
    
    BSD = NULL;
    
    BSD = malloc(sizeof(BSD_t));
    
    if( !BSDReadEntryTableChunk(BSD,BSDFile) ) {
        goto Failure;
    }
    RenderObjectSize = BSDGetRenderObjectSize(BSD,BSDFile);
    assert( RenderObjectSize == MOH_RENDER_OBJECT_SIZE || RenderObjectSize == MOH_UNDERGROUND_RENDER_OBJECT_SIZE );
    if( RenderObjectSize == MOH_RENDER_OBJECT_SIZE ) {
        LocalGameVersion = MOH_GAME_STANDARD;
    } else {
        LocalGameVersion = MOH_GAME_UNDERGROUND;
    }
    DPrintf("Running BSD from %s since size is %i\n",LocalGameVersion == MOH_GAME_STANDARD ? 
        "Medal Of Honor" : "Medal Of Honor:Underground",RenderObjectSize);
    if( !BSDReadRenderObjectChunk(BSD,LocalGameVersion,BSDFile) ) {
        goto Failure;
    }
    if( GameVersion ) {
        *GameVersion = LocalGameVersion;
    }
    return BSD;
Failure:
    BSDFree(BSD);
    return NULL;
}

BSDRenderObject_t *BSDLoadAllAnimatedRenderObjects(const char *FName)
{
    FILE *BSDFile;
    BSD_t *BSD;
    int GameVersion;
    BSDRenderObject_t *RenderObjectList;
    BSDRenderObject_t *RenderObject;
    int i;
    
    BSDFile = fopen(FName,"rb");
    if( BSDFile == NULL ) {
        DPrintf("Failed opening BSD File %s.\n",FName);
        return NULL;
    }
    BSD = BSDLoad(BSDFile,&GameVersion);
    if( !BSD ) {
        fclose(BSDFile);
        return NULL;
    }
    RenderObjectList = NULL;
    
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObject[i].AnimationDataOffset == -1 ) {
            continue;
        }
        DPrintf("BSDLoadAllAnimatedRenderObjects:Loading Animated RenderObject %u\n",BSD->RenderObjectTable.RenderObject[i].Id);
        RenderObject = BSDLoadAnimatedRenderObject(BSD->RenderObjectTable.RenderObject[i],BSD->EntryTable,
                                                   BSDFile,i,GameVersion);
        if( !RenderObject ) {
            DPrintf("BSDLoadAllAnimatedRenderObjects:Failed to load animated RenderObject.\n");
            continue;
        }
        RenderObject->Next = RenderObjectList;
        RenderObjectList = RenderObject;
    }
    
    BSDFree(BSD);
    fclose(BSDFile);
    return RenderObjectList;
}
