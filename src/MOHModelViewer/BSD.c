// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2025 Adriano Di Dio.
    
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
#include "../Common/RenderObject.h"
#include "../Common/ShaderManager.h"


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

void BSDAppendRenderObjectToList(RenderObject_t **List,RenderObject_t *Node)
{
    RenderObject_t *LastNode;
    if( !*List ) {
        *List = Node;
    } else {
        LastNode = *List;
        while( LastNode->Next ) {
            LastNode = LastNode->Next;
        }
        LastNode->Next = Node;
    }
}

BSD_t *BSDLoad(FILE *BSDFile,int *GameVersion)
{
    BSD_t *BSD;
    int LocalGameVersion;
    int RenderObjectSize;
    
    BSD = NULL;
    
    BSD = malloc(sizeof(BSD_t));
    
    if( !BSD ) {
        goto Failure;
    }
    
    if( !BSDReadEntryTableBlock(BSDFile, &BSD->EntryTable) ) {
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
    if( !BSDReadRenderObjectTable(BSDFile,LocalGameVersion,&BSD->RenderObjectTable) ) {
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

RenderObject_t *BSDLoadRenderObjects(const char *FName,int *GameVersion)
{
    FILE *BSDFile;
    BSD_t *BSD;
    int LocalGameVersion;
    RenderObject_t *RenderObjectList;
    
    BSDFile = fopen(FName,"rb");
    if( BSDFile == NULL ) {
        DPrintf("BSDLoadRenderObjects:Failed opening BSD File %s.\n",FName);
        return NULL;
    }
    BSD = BSDLoad(BSDFile,&LocalGameVersion);
    if( !BSD ) {
        fclose(BSDFile);
        return NULL;
    }
    RenderObjectList = RenderObjectLoadAllFromTable(BSD->EntryTable,BSD->RenderObjectTable,BSDFile,LocalGameVersion,true);
    
    if( !RenderObjectList ) {
        DPrintf("BSDLoadRenderObjects: Failed to load render objects...\n"); 
        BSDFree(BSD);
        fclose(BSDFile);
        return NULL;
    }
    
    if( GameVersion ) {
        *GameVersion = LocalGameVersion;
    }
    
    BSDFree(BSD);
    fclose(BSDFile);
    return RenderObjectList;
}
