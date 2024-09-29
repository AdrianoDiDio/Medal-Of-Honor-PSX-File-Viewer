// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
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


void BSDFreeRenderObjectList(RenderObject_t *RenderObjectList)
{
    RenderObject_t *Temp;
    while( RenderObjectList ) {
        Temp = RenderObjectList;
        RenderObjectList = RenderObjectList->Next;
        RenderObjectFree(Temp);
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


void BSDRenderObjectExportPoseToPly(RenderObject_t *RenderObject,BSDVertexTable_t *VertexTable,VRAM_t *VRAM,FILE *OutFile)
{
    char Buffer[256];
    float TextureWidth;
    float TextureHeight;
    int i;
    int VRAMPage;
    int ColorMode;
    float U0;
    float V0;
    float U1;
    float V1;
    float U2;
    float V2;
    vec3 VertPos;
    vec3 OutVector;
    vec3 RotationAxis;
    mat4 RotationMatrix;
    mat4 ScaleMatrix;
    mat4 ModelMatrix;
    BSDAnimatedModelFace_t *CurrentFace;
    
    if( !RenderObject || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        DPrintf("BSDRenderObjectExportPoseToObj: Invalid %s\n",InvalidFile ? "file" : "bsd struct");
        return;
    }
    
    if( !VRAM ) {
        DPrintf("BSDRenderObjectExportPoseToObj:Invalid VRAM data\n");
        return;
    }
    
    TextureWidth = VRAM->Page.Width;
    TextureHeight = VRAM->Page.Height;
    
    glm_mat4_identity(ModelMatrix);
    glm_mat4_identity(RotationMatrix);
    
    RotationAxis[0] = -1;
    RotationAxis[1] = 0;
    RotationAxis[2] = 0;
    glm_rotate(RotationMatrix,glm_rad(180.f), RotationAxis);    
    glm_scale_make(ScaleMatrix,RenderObject->Scale);
    
    glm_mat4_mul(RotationMatrix,ScaleMatrix,ModelMatrix);
    
    sprintf(Buffer,"ply\nformat ascii 1.0\n");
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    
    sprintf(Buffer,
        "element vertex %i\nproperty float x\nproperty float y\nproperty float z\nproperty float red\nproperty float green\nproperty float "
        "blue\nproperty float s\nproperty float t\n",RenderObject->NumFaces * 3);
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    sprintf(Buffer,"element face %i\nproperty list uchar int vertex_indices\nend_header\n",RenderObject->NumFaces);
    fwrite(Buffer,strlen(Buffer),1,OutFile);

    
    for( i = 0 ; i < RenderObject->NumFaces; i++ ) {
        CurrentFace = &RenderObject->FaceList[i];
        VRAMPage = CurrentFace->TexInfo;
        ColorMode = (CurrentFace->TexInfo & 0xC0) >> 7;
        U0 = (((float)CurrentFace->UV0.u + 
            VRAMGetTexturePageX(VRAMPage))/TextureWidth);
        V0 = /*255 -*/1.f - (((float)CurrentFace->UV0.v +
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        U1 = (((float)CurrentFace->UV1.u + 
            VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        V1 = /*255 -*/1.f - (((float)CurrentFace->UV1.v + 
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) /TextureHeight);
        U2 = (((float)CurrentFace->UV2.u + 
            VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        V2 = /*255 -*/1.f - (((float)CurrentFace->UV2.v + 
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        
        VertPos[0] = VertexTable[CurrentFace->VertexTableIndex0&0x1F].VertexList[CurrentFace->VertexTableDataIndex0].x;
        VertPos[1] = VertexTable[CurrentFace->VertexTableIndex0&0x1F].VertexList[CurrentFace->VertexTableDataIndex0].y;
        VertPos[2] = VertexTable[CurrentFace->VertexTableIndex0&0x1F].VertexList[CurrentFace->VertexTableDataIndex0].z;
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",OutVector[0] / 4096.f, 
                OutVector[1] / 4096.f, OutVector[2] / 4096.f,
                CurrentFace->RGB0.r / 255.f,CurrentFace->RGB0.g / 255.f,CurrentFace->RGB0.b / 255.f,U0,V0);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        
        VertPos[0] = VertexTable[CurrentFace->VertexTableIndex1&0x1F].VertexList[CurrentFace->VertexTableDataIndex1].x;
        VertPos[1] = VertexTable[CurrentFace->VertexTableIndex1&0x1F].VertexList[CurrentFace->VertexTableDataIndex1].y;
        VertPos[2] = VertexTable[CurrentFace->VertexTableIndex1&0x1F].VertexList[CurrentFace->VertexTableDataIndex1].z;
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",OutVector[0] / 4096.f, 
                OutVector[1] / 4096.f, OutVector[2] / 4096.f,
                CurrentFace->RGB1.r / 255.f,CurrentFace->RGB1.g / 255.f,CurrentFace->RGB1.b / 255.f,U1,V1);
        fwrite(Buffer,strlen(Buffer),1,OutFile);

        VertPos[0] = VertexTable[CurrentFace->VertexTableIndex2&0x1F].VertexList[CurrentFace->VertexTableDataIndex2].x;
        VertPos[1] = VertexTable[CurrentFace->VertexTableIndex2&0x1F].VertexList[CurrentFace->VertexTableDataIndex2].y;
        VertPos[2] = VertexTable[CurrentFace->VertexTableIndex2&0x1F].VertexList[CurrentFace->VertexTableDataIndex2].z;
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",OutVector[0] / 4096.f, 
                OutVector[1] / 4096.f, OutVector[2] / 4096.f,
                CurrentFace->RGB1.r / 255.f,CurrentFace->RGB1.g / 255.f,CurrentFace->RGB1.b / 255.f,U2,V2);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
    }
    for( i = 0; i < RenderObject->NumFaces; i++ ) {
        int Vert0 = (i * 3) + 0;
        int Vert1 = (i * 3) + 1;
        int Vert2 = (i * 3) + 2;
        sprintf(Buffer,"3 %i %i %i\n",Vert0,Vert1,Vert2);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
    }
}
void BSDRenderObjectExportCurrentPoseToPly(RenderObject_t *RenderObject,VRAM_t *VRAM,FILE *OutFile)
{    
    if( !RenderObject || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        DPrintf("BSDRenderObjectExportPoseToObj: Invalid %s\n",InvalidFile ? "file" : "bsd struct");
        return;
    }
    
    if( !VRAM ) {
        DPrintf("BSDRenderObjectExportPoseToObj:Invalid VRAM data\n");
        return;
    }
    BSDRenderObjectExportPoseToPly(RenderObject,RenderObject->CurrentVertexTable,VRAM,OutFile);
}
void BSDRenderObjectExportCurrentAnimationToPly(RenderObject_t *RenderObject,VRAM_t *VRAM,const char *Directory,const char *EngineName)
{
    BSDVertexTable_t *TempVertexTable;
    FILE *OutFile;
    mat4 TransformMatrix;
    vec3 Translation;
    int i;
    int j;
    char *PlyFile;
    int VertexTableSize;
    
    if(RenderObject->CurrentAnimationIndex == -1 ) {
        DPrintf("BSDRenderObjectExportCurrentAnimationToPly:Invalid animation index\n");
        return;
    }
    VertexTableSize = RenderObject->NumVertexTables * sizeof(BSDVertexTable_t);
    TempVertexTable = malloc(VertexTableSize);
    //Prepare the copy of the vertex table that will be used by the exporter
    for( i = 0; i < RenderObject->NumVertexTables; i++ ) {
        TempVertexTable[i].NumVertex = RenderObject->VertexTable[i].NumVertex;
        if( RenderObject->VertexTable[i].Offset == -1 ) {
            TempVertexTable[i].VertexList = NULL;
        } else {
            TempVertexTable[i].VertexList = malloc(RenderObject->VertexTable[i].NumVertex * sizeof(BSDVertex_t));
        }
    }
    for( i = 0; i < RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].NumFrames; i++ ) {
        asprintf(&PlyFile,"%s%cRenderObject-%u-%i-%i-%s.ply",Directory,PATH_SEPARATOR,RenderObject->Id,
                 RenderObject->CurrentAnimationIndex,i,EngineName);
        OutFile = fopen(PlyFile,"w");
        glm_mat4_identity(TransformMatrix);
        Translation[0] = RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].Frame[i].Vector.x / 4096.f;
        Translation[1] = RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].Frame[i].Vector.y / 4096.f;
        Translation[2] = RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].Frame[i].Vector.z / 4096.f;
        glm_translate_make(TransformMatrix,Translation);
        //Copy the vertices for the current frame
        for( j = 0; j < RenderObject->NumVertexTables; j++ ) {
            memcpy(TempVertexTable[j].VertexList,
                RenderObject->VertexTable[j].VertexList,sizeof(BSDVertex_t) * RenderObject->VertexTable[j].NumVertex);
        }
        //Apply the animation
        RenderObjectRecursivelyApplyHierachyData(RenderObject->HierarchyDataRoot,
                                        RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].Frame[i].QuaternionList,
                                        TempVertexTable,TransformMatrix);
        //Export it
        BSDRenderObjectExportPoseToPly(RenderObject,TempVertexTable,VRAM,OutFile);
        fclose(OutFile);
        free(PlyFile);
    }
    for( i = 0; i < RenderObject->NumVertexTables; i++ ) {
        if( TempVertexTable[i].VertexList ) {
            free(TempVertexTable[i].VertexList);
        }
    }
    free(TempVertexTable);
}


void BSDDrawRenderObject(RenderObject_t *RenderObject,const VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix)
{
    int EnableLightingId;
    int PaletteTextureId;
    int TextureIndexId;
    int MVPMatrixId;
    vec3 Temp;
    mat4 ModelMatrix;
    mat4 ModelViewMatrix;
    mat4 MVPMatrix;
    Shader_t *Shader;
    
    if( !RenderObject ) {
        return;
    }
    
    
    Shader = ShaderCache("BSDRenderObjectShader","Shaders/BSDRenderObjectVertexShader.glsl",
                         "Shaders/BSDRenderObjectFragmentShader.glsl");
    if( !Shader ) {
        return;
    }
    
    if( EnableWireFrameMode->IValue ) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    glm_mat4_identity(ModelMatrix);
    glm_mat4_identity(ModelViewMatrix);

    Temp[0] = -RenderObject->Center[0];
    Temp[1] = -RenderObject->Center[1];
    Temp[2] = -RenderObject->Center[2];
    glm_vec3_rotate(Temp, DEGTORAD(180.f), GLM_XUP);    
    glm_translate(ModelMatrix,Temp);
    Temp[0] = 0;
    Temp[1] = 1;
    Temp[2] = 0;
    glm_rotate(ModelMatrix,glm_rad(-90), Temp);
    glm_scale(ModelMatrix,RenderObject->Scale);
    
 
    glm_mat4_mul(Camera->ViewMatrix,ModelMatrix,ModelViewMatrix);
    glm_mat4_mul(ProjectionMatrix,ModelViewMatrix,MVPMatrix);
    //Emulate PSX Coordinate system...
    glm_rotate_x(MVPMatrix,glm_rad(180.f), MVPMatrix);

    glUseProgram(Shader->ProgramId);
    MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
    glUniformMatrix4fv(MVPMatrixId,1,false,&MVPMatrix[0][0]);
    EnableLightingId = glGetUniformLocation(Shader->ProgramId,"EnableLighting");
    PaletteTextureId = glGetUniformLocation(Shader->ProgramId,"ourPaletteTexture");
    TextureIndexId = glGetUniformLocation(Shader->ProgramId,"ourIndexTexture");
    glUniform1i(TextureIndexId, 0);
    glUniform1i(PaletteTextureId,  1);
    glUniform1i(EnableLightingId, EnableAmbientLight->IValue);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, VRAM->TextureIndexPage.TextureId);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, VRAM->PalettePage.TextureId);

    glDisable(GL_BLEND);
    glBindVertexArray(RenderObject->VAO->VAOId[0]);
    glDrawArrays(GL_TRIANGLES, 0, RenderObject->VAO->Count);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D,0);
    glDisable(GL_BLEND);
    glBlendColor(1.f, 1.f, 1.f, 1.f);
    glUseProgram(0);
    if( EnableWireFrameMode->IValue ) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void BSDDrawRenderObjectList(RenderObject_t *RenderObjectList,const VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix)
{
    RenderObject_t *Iterator;
    int i;
    if( !RenderObjectList ) {
        return;
    }

    i = 0;
    for( Iterator = RenderObjectList; Iterator; Iterator = Iterator->Next ) {
        BSDDrawRenderObject(Iterator,VRAM,Camera,ProjectionMatrix);
        i++;
    }
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
    RenderObject_t *RenderObject;
    int i;
    
    BSDFile = fopen(FName,"rb");
    if( BSDFile == NULL ) {
        DPrintf("Failed opening BSD File %s.\n",FName);
        return NULL;
    }
    BSD = BSDLoad(BSDFile,&LocalGameVersion);
    if( !BSD ) {
        fclose(BSDFile);
        return NULL;
    }
    RenderObjectList = NULL;
    
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        RenderObject = RenderObjectLoad(BSD->RenderObjectTable.RenderObject[i],
                                                   BSD->EntryTable,BSD->RenderObjectTable,BSDFile,LocalGameVersion);

        if( !RenderObject ) {
            DPrintf("BSDLoadRenderObjects:Failed to load RenderObject with Id:%i\n",BSD->RenderObjectTable.RenderObject[i].Id);
            continue;
        }
        BSDAppendRenderObjectToList(&RenderObjectList,RenderObject);
    }
    if( GameVersion ) {
        *GameVersion = LocalGameVersion;
    }
    
    BSDFree(BSD);
    fclose(BSDFile);
    return RenderObjectList;
}
