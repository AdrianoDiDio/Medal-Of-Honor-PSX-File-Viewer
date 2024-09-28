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
#include "../Common/ShaderManager.h"

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
        BSDRecusivelyFreeHierarchyBone(RenderObject->HierarchyDataRoot);
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


void BSDRenderObjectExportPoseToPly(BSDRenderObject_t *RenderObject,BSDVertexTable_t *VertexTable,VRAM_t *VRAM,FILE *OutFile)
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
void BSDRenderObjectExportCurrentPoseToPly(BSDRenderObject_t *RenderObject,VRAM_t *VRAM,FILE *OutFile)
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
void BSDRenderObjectExportCurrentAnimationToPly(BSDRenderObject_t *RenderObject,VRAM_t *VRAM,const char *Directory,const char *EngineName)
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
        BSDRecursivelyApplyHierachyData(RenderObject->HierarchyDataRoot,
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
void BSDFillFaceVertexBuffer(int *Buffer,int *BufferSize,BSDVertex_t Vertex,int U0,int V0,BSDColor_t Color,int CLUTX,int CLUTY,int ColorMode)
{
    if( !Buffer ) {
        DPrintf("BSDFillFaceVertexBuffer:Invalid Buffer\n");
        return;
    }
    if( !BufferSize ) {
        DPrintf("BSDFillFaceVertexBuffer:Invalid BufferSize\n");
        return;
    }
    Buffer[*BufferSize] =   Vertex.x;
    Buffer[*BufferSize+1] = Vertex.y;
    Buffer[*BufferSize+2] = Vertex.z;
    Buffer[*BufferSize+3] = U0;
    Buffer[*BufferSize+4] = V0;
    Buffer[*BufferSize+5] = Color.r;
    Buffer[*BufferSize+6] = Color.g;
    Buffer[*BufferSize+7] = Color.b;
    Buffer[*BufferSize+8] = CLUTX;
    Buffer[*BufferSize+9] = CLUTY;
    Buffer[*BufferSize+10] = ColorMode;
    *BufferSize += 11;
}

void BSDRenderObjectGenerateStaticVAO(BSDRenderObject_t *RenderObject)
{
    BSDFace_t *CurrentFace;
    int Vert0;
    int Vert1;
    int Vert2;
    int VertexOffset;
    int TextureOffset;
    int ColorOffset;
    int CLUTOffset;
    int ColorModeOffset;
    int Stride;
    int *VertexData;
    int VertexSize;
    int VertexPointer;
    int VRAMPage;
    int ColorMode;
    int CLUTPage;
    int CLUTPosX;
    int CLUTPosY;
    int CLUTDestX;
    int CLUTDestY;
    int U0;
    int V0;
    int U1;
    int V1;
    int U2;
    int V2;
    int i;
    
    if( !RenderObject ) {
        DPrintf("BSDRenderObjectGenerateStaticVAO:Invalid RenderObject\n");
        return;
    }
    //        XYZ UV RGB CLUT ColorMode
    Stride = (3 + 2 + 3 + 2 + 1) * sizeof(int);
                
    VertexOffset = 0;
    TextureOffset = 3;
    ColorOffset = 5;
    CLUTOffset = 8;
    ColorModeOffset = 10;
    
    VertexSize = Stride * 3 * RenderObject->NumFaces;
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    DPrintf("BSDRenderObjectGenerateStaticVAO:Generating for %i faces Id:%i\n",RenderObject->NumFaces,RenderObject->Id);
    for( i = 0; i < RenderObject->NumFaces; i++ ) {
        CurrentFace = &RenderObject->StaticFaceList[i];
        
        
        Vert0 = CurrentFace->Vert0;
        Vert1 = CurrentFace->Vert1;
        Vert2 = CurrentFace->Vert2;
        
        VRAMPage = CurrentFace->TexInfo & 0x1F;
        ColorMode = (CurrentFace->TexInfo >> 7) & 0x3;
        CLUTPosX = (CurrentFace->CBA << 4) & 0x3F0;
        CLUTPosY = (CurrentFace->CBA >> 6) & 0x1ff;
        CLUTPage = VRAMGetCLUTPage(CLUTPosX,CLUTPosY);
        CLUTDestX = VRAMGetCLUTPositionX(CLUTPosX,CLUTPosY,CLUTPage);
        CLUTDestY = CLUTPosY + VRAMGetCLUTOffsetY(ColorMode);
        CLUTDestX += VRAMGetTexturePageX(CLUTPage);
 
        U0 = CurrentFace->UV0.u + VRAMGetTexturePageX(VRAMPage);
        V0 = CurrentFace->UV0.v + VRAMGetTexturePageY(VRAMPage,ColorMode);
        U1 = CurrentFace->UV1.u + VRAMGetTexturePageX(VRAMPage);
        V1 = CurrentFace->UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode);
        U2 = CurrentFace->UV2.u + VRAMGetTexturePageX(VRAMPage);
        V2 = CurrentFace->UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode);

        
        BSDFillFaceVertexBuffer(VertexData,&VertexPointer,RenderObject->VertexList[Vert0],
                                U0,V0,RenderObject->ColorList[Vert0],CLUTDestX,CLUTDestY,ColorMode
                               );
        BSDFillFaceVertexBuffer(VertexData,&VertexPointer,RenderObject->VertexList[Vert1],
                                U1,V1,RenderObject->ColorList[Vert1],CLUTDestX,CLUTDestY,ColorMode
                               );
        BSDFillFaceVertexBuffer(VertexData,&VertexPointer,RenderObject->VertexList[Vert2],
                                U2,V2,RenderObject->ColorList[Vert2],CLUTDestX,CLUTDestY,ColorMode
                               );
    }
    RenderObject->VAO = VAOInitXYZUVRGBCLUTColorModeInteger(VertexData,VertexSize,Stride,VertexOffset,TextureOffset,
                                        ColorOffset,CLUTOffset,ColorModeOffset,RenderObject->NumFaces * 3);
    free(VertexData);
}

void BSDRenderObjectGenerateVAO(BSDRenderObject_t *RenderObject)
{
    BSDAnimatedModelFace_t *CurrentFace;
    int VertexOffset;
    int TextureOffset;
    int ColorOffset;
    int CLUTOffset;
    int ColorModeOffset;
    int Stride;
    int *VertexData;
    int VertexSize;
    int VertexPointer;
    int VRAMPage;
    int ColorMode;
    int CLUTPage;
    int CLUTPosX;
    int CLUTPosY;
    int CLUTDestX;
    int CLUTDestY;
    int U0;
    int V0;
    int U1;
    int V1;
    int U2;
    int V2;
    int i;
    
    if( !RenderObject ) {
        DPrintf("BSDRenderObjectGenerateVAO:Invalid RenderObject\n");
        return;
    }
    //        XYZ UV RGB CLUT ColorMode
    Stride = (3 + 2 + 3 + 2 + 1) * sizeof(int);
                
    VertexOffset = 0;
    TextureOffset = 3;
    ColorOffset = 5;
    CLUTOffset = 8;
    ColorModeOffset = 10;
    
    VertexSize = Stride * 3 * RenderObject->NumFaces;
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    DPrintf("BSDRenderObjectGenerateVAO:Generating for %i faces Id:%i\n",RenderObject->NumFaces,RenderObject->Id);
    for( i = 0; i < RenderObject->NumFaces; i++ ) {
        CurrentFace = &RenderObject->FaceList[i];
        VRAMPage = CurrentFace->TexInfo & 0x1F;
        ColorMode = (CurrentFace->TexInfo >> 7) & 0x3;
        CLUTPosX = (CurrentFace->CLUT << 4) & 0x3F0;
        CLUTPosY = (CurrentFace->CLUT >> 6) & 0x1ff;
        CLUTPage = VRAMGetCLUTPage(CLUTPosX,CLUTPosY);
        CLUTDestX = VRAMGetCLUTPositionX(CLUTPosX,CLUTPosY,CLUTPage);
        CLUTDestY = CLUTPosY + VRAMGetCLUTOffsetY(ColorMode);
        CLUTDestX += VRAMGetTexturePageX(CLUTPage);
 
        U0 = CurrentFace->UV0.u + VRAMGetTexturePageX(VRAMPage);
        V0 = CurrentFace->UV0.v + VRAMGetTexturePageY(VRAMPage,ColorMode);
        U1 = CurrentFace->UV1.u + VRAMGetTexturePageX(VRAMPage);
        V1 = CurrentFace->UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode);
        U2 = CurrentFace->UV2.u + VRAMGetTexturePageX(VRAMPage);
        V2 = CurrentFace->UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode);

        
        BSDFillFaceVertexBuffer(VertexData,&VertexPointer,
                                RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex0&0x1F].VertexList[CurrentFace->VertexTableDataIndex0],
                                U0,V0,CurrentFace->RGB0,CLUTDestX,CLUTDestY,ColorMode
                               );
        BSDFillFaceVertexBuffer(VertexData,&VertexPointer,
                                RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex1&0x1F].VertexList[CurrentFace->VertexTableDataIndex1],
                                U1,V1,CurrentFace->RGB1,CLUTDestX,CLUTDestY,ColorMode
                               );
        BSDFillFaceVertexBuffer(VertexData,&VertexPointer,
                                RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex2&0x1F].VertexList[CurrentFace->VertexTableDataIndex2],
                                U2,V2,CurrentFace->RGB2,CLUTDestX,CLUTDestY,ColorMode
                               );
    }
    RenderObject->VAO = VAOInitXYZUVRGBCLUTColorModeInteger(VertexData,VertexSize,Stride,VertexOffset,TextureOffset,
                                        ColorOffset,CLUTOffset,ColorModeOffset,RenderObject->NumFaces * 3);
    free(VertexData);
}
void BSDRenderObjectUpdateVAO(BSDRenderObject_t *RenderObject)
{
    BSDAnimatedModelFace_t *CurrentFace;
    int Stride;
    int BaseOffset;
    int VertexData[3];
    int i;
    
    if( !RenderObject ) {
        DPrintf("BSDRenderObjectUpdateVAO:Invalid RenderObject\n");
        return;
    }
    if( !RenderObject->VAO ) {
        DPrintf("BSDRenderObjectUpdateVAO:Invalid VAO\n");
        return;
    }
    
    Stride = (3 + 2 + 3 + 2 + 1) * sizeof(int);
    glBindBuffer(GL_ARRAY_BUFFER, RenderObject->VAO->VBOId[0]);
    
    for( i = 0; i < RenderObject->NumFaces; i++ ) {
        BaseOffset = (i * Stride * 3);
        CurrentFace = &RenderObject->FaceList[i];
        VertexData[0] = RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex0&0x1F].VertexList[CurrentFace->VertexTableDataIndex0].x;
        VertexData[1] = RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex0&0x1F].VertexList[CurrentFace->VertexTableDataIndex0].y;
        VertexData[2] = RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex0&0x1F].VertexList[CurrentFace->VertexTableDataIndex0].z;

        glBufferSubData(GL_ARRAY_BUFFER, BaseOffset + (Stride * 0), 3 * sizeof(int), &VertexData);
        
        VertexData[0] = RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex1&0x1F].VertexList[CurrentFace->VertexTableDataIndex1].x;
        VertexData[1] = RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex1&0x1F].VertexList[CurrentFace->VertexTableDataIndex1].y;
        VertexData[2] = RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex1&0x1F].VertexList[CurrentFace->VertexTableDataIndex1].z;

        glBufferSubData(GL_ARRAY_BUFFER, BaseOffset + (Stride * 1), 3 * sizeof(int), &VertexData);
        
        VertexData[0] = RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex2&0x1F].VertexList[CurrentFace->VertexTableDataIndex2].x;
        VertexData[1] = RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex2&0x1F].VertexList[CurrentFace->VertexTableDataIndex2].y;
        VertexData[2] = RenderObject->CurrentVertexTable[CurrentFace->VertexTableIndex2&0x1F].VertexList[CurrentFace->VertexTableDataIndex2].z;

        glBufferSubData(GL_ARRAY_BUFFER, BaseOffset + (Stride * 2), 3 * sizeof(int), &VertexData);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BSDRenderObjectResetVertexTable(BSDRenderObject_t *RenderObject)
{
    int i;

    if( !RenderObject ) {
        DPrintf("BSDRenderObjectResetVertexTable:Invalid RenderObject\n");
        return;
    }
    for( i = 0; i < RenderObject->NumVertexTables; i++ ) {
        RenderObject->CurrentVertexTable[i].Offset = RenderObject->VertexTable[i].Offset;
        RenderObject->CurrentVertexTable[i].NumVertex = RenderObject->VertexTable[i].NumVertex;
        memcpy(RenderObject->CurrentVertexTable[i].VertexList,
               RenderObject->VertexTable[i].VertexList,sizeof(BSDVertex_t) * RenderObject->VertexTable[i].NumVertex);
    }
}

BSDAnimationFrame_t *BSDRenderObjectGetCurrentFrame(BSDRenderObject_t *RenderObject)
{
    if( !RenderObject ) {
        return NULL;
    }
    if( RenderObject->CurrentAnimationIndex == -1 || RenderObject->CurrentFrameIndex == -1 ) {
        return NULL;
    }
    return &RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].Frame[RenderObject->CurrentFrameIndex];
}
/*
 Set the RenderObject to a specific pose, given AnimationIndex and FrameIndex.
 Returns 0 if the pose was not valid ( pose was already set,pose didn't exists), 1 otherwise.
 NOTE that calling this function will modify the RenderObject's VAO.
 If the VAO is NULL a new one is created otherwise it will be updated to reflect the pose that was applied to the model.
 If Override is true then the pose will be set again in case the AnimationIndex and FrameIndex did not change.
 */
int BSDRenderObjectSetAnimationPose(BSDRenderObject_t *RenderObject,int AnimationIndex,int FrameIndex,int Override)
{
    BSDQuaternion_t *QuaternionList;
    versor FromQuaternion;
    versor ToQuaternion;
    versor DestQuaternion;
    mat4 TransformMatrix;
    vec3 Translation;
    int NumVertices;
    int i;

    if( !RenderObject ) {
        DPrintf("BSDRenderObjectSetAnimationPose:Failed to set pose RenderObject is not valid\n");
        return 0;
    }
    
    if( RenderObject->IsStatic ) {
        DPrintf("BSDRenderObjectSetAnimationPose:Failed to set pose RenderObject is not animated\n");
        return 0;
    }
    if( AnimationIndex < 0 || AnimationIndex > RenderObject->NumAnimations ) {
        DPrintf("BSDRenderObjectSetAnimationPose:Failed to set pose using index %i...Index is out of bounds\n",AnimationIndex);
        return 0;
    }
    if( (AnimationIndex == RenderObject->CurrentAnimationIndex && FrameIndex == RenderObject->CurrentFrameIndex ) && !Override) {
        DPrintf("BSDRenderObjectSetAnimationPose:Pose is already set\n");
        return 0;
    }
    if( !RenderObject->AnimationList[AnimationIndex].NumFrames ) {
        DPrintf("BSDRenderObjectSetAnimationPose:Failed to set pose using index %i...animation has no frames\n",AnimationIndex);
        return 0;
    }
    if( FrameIndex < 0 || FrameIndex > RenderObject->AnimationList[AnimationIndex].NumFrames ) {
        DPrintf("BSDRenderObjectSetAnimationPose:Failed to set pose using frame %i...Frame Index is out of bounds\n",FrameIndex);
        return 0;
    }
    BSDRenderObjectResetVertexTable(RenderObject);
    glm_vec3_zero(RenderObject->Center);
    glm_mat4_identity(TransformMatrix);
    Translation[0] = RenderObject->AnimationList[AnimationIndex].Frame[FrameIndex].Vector.x / 4096.f;
    Translation[1] = RenderObject->AnimationList[AnimationIndex].Frame[FrameIndex].Vector.y / 4096.f;
    Translation[2] = RenderObject->AnimationList[AnimationIndex].Frame[FrameIndex].Vector.z / 4096.f;
    glm_translate_make(TransformMatrix,Translation);
    //NOTE(Adriano):Interpolate only between frames of the same animation and not in-between two different one.
    //              Also do not interpolate if the frame is the same as the previous one.
    if( RenderObject->CurrentAnimationIndex == AnimationIndex && RenderObject->CurrentFrameIndex != -1 && 
        RenderObject->CurrentFrameIndex != FrameIndex
    ) {
        assert(RenderObject->AnimationList[AnimationIndex].Frame[FrameIndex].NumQuaternions == 
            RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].Frame[RenderObject->CurrentFrameIndex].NumQuaternions
        );
        QuaternionList = malloc(RenderObject->AnimationList[AnimationIndex].Frame[FrameIndex].NumQuaternions * sizeof(BSDQuaternion_t));
        for( i = 0; i < RenderObject->AnimationList[AnimationIndex].Frame[FrameIndex].NumQuaternions; i++ ) {
            FromQuaternion[0] = RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].
                Frame[RenderObject->CurrentFrameIndex].QuaternionList[i].x / 4096.f;
            FromQuaternion[1] = RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].
                Frame[RenderObject->CurrentFrameIndex].QuaternionList[i].y / 4096.f;
            FromQuaternion[2] = RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].
                Frame[RenderObject->CurrentFrameIndex].QuaternionList[i].z / 4096.f;
            FromQuaternion[3] = RenderObject->AnimationList[RenderObject->CurrentAnimationIndex].
                Frame[RenderObject->CurrentFrameIndex].QuaternionList[i].w / 4096.f;
            ToQuaternion[0] = RenderObject->AnimationList[AnimationIndex].
                Frame[FrameIndex].QuaternionList[i].x / 4096.f;
            ToQuaternion[1] = RenderObject->AnimationList[AnimationIndex].
                Frame[FrameIndex].QuaternionList[i].y / 4096.f;
            ToQuaternion[2] = RenderObject->AnimationList[AnimationIndex].
                Frame[FrameIndex].QuaternionList[i].z / 4096.f;
            ToQuaternion[3] = RenderObject->AnimationList[AnimationIndex].
                Frame[FrameIndex].QuaternionList[i].w / 4096.f;
            glm_quat_nlerp(FromQuaternion,
                ToQuaternion,
                0.5f,
                DestQuaternion
            );
            QuaternionList[i].x = DestQuaternion[0] * 4096.f;
            QuaternionList[i].y = DestQuaternion[1] * 4096.f;
            QuaternionList[i].z = DestQuaternion[2] * 4096.f;
            QuaternionList[i].w = DestQuaternion[3] * 4096.f;
        }
        BSDRecursivelyApplyHierachyData(RenderObject->HierarchyDataRoot,QuaternionList,
                                    RenderObject->CurrentVertexTable,TransformMatrix);
        free(QuaternionList);
    } else {
        BSDRecursivelyApplyHierachyData(RenderObject->HierarchyDataRoot,
                                    RenderObject->AnimationList[AnimationIndex].Frame[FrameIndex].CurrentQuaternionList,
                                    RenderObject->CurrentVertexTable,TransformMatrix);
    }
    RenderObject->CurrentAnimationIndex = AnimationIndex;
    RenderObject->CurrentFrameIndex = FrameIndex;
    
    NumVertices = 0;
    for( int i = 0; i < RenderObject->NumVertexTables; i++ ) {
        for( int j = 0; j < RenderObject->CurrentVertexTable[i].NumVertex; j++ ) {
            RenderObject->Center[0] += RenderObject->CurrentVertexTable[i].VertexList[j].x;
            RenderObject->Center[1] += RenderObject->CurrentVertexTable[i].VertexList[j].y;
            RenderObject->Center[2] += RenderObject->CurrentVertexTable[i].VertexList[j].z;
            NumVertices++;
        }
    }
    glm_vec3_scale(RenderObject->Center,1.f/NumVertices,RenderObject->Center);
    if( !RenderObject->VAO ) {
        BSDRenderObjectGenerateVAO(RenderObject);
    } else {
        BSDRenderObjectUpdateVAO(RenderObject);
    }
    return 1;
}

void BSDDrawRenderObject(BSDRenderObject_t *RenderObject,const VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix)
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

void BSDDrawRenderObjectList(BSDRenderObject_t *RenderObjectList,const VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix)
{
    BSDRenderObject_t *Iterator;
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
BSDRenderObjectElement_t *BSDGetRenderObjectById(const BSD_t *BSD,unsigned int RenderObjectId)
{
    int i;
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObject[i].Id == RenderObjectId ) {
            return &BSD->RenderObjectTable.RenderObject[i];
        }
    }
    return NULL;
}
int BSDGetRenderObjectIndexById(const BSD_t *BSD,unsigned int RenderObjectId)
{
    int i;
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObject[i].Id == RenderObjectId ) {
            return i;
        }
    }
    return -1;
}
void BSDAppendRenderObjectToList(BSDRenderObject_t **List,BSDRenderObject_t *Node)
{
    BSDRenderObject_t *LastNode;
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
/*
 * NOTE(Adriano):
 * Some RenderObjects uses the 'ReferencedRenderObjectId' field to reference a RenderObject that contains common
 * informations shared by multiple RenderObjects.
 * In order to correctly parse these entry we need to copy the field from the 'ReferencedRenderObjectId' to the RenderObject that
 * requested it.
 * NOTE(Adriano):Make sure to update the data when new fields are added.
 */
void BSDPatchRenderObjects(BSD_t *BSD,FILE *BSDFile,int GameEngine)
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
int BSDReadRenderObjectChunk(BSD_t *BSD,int GameEngine,FILE *BSDFile)
{
    int FirstRenderObjectPosition;
    int i;
    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDReadRenderObjectChunk: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return 0;
    }
    
    fseek(BSDFile,BSD_RENDER_OBJECT_STARTING_OFFSET + BSD_HEADER_SIZE,SEEK_SET);
    if( GameEngine == MOH_GAME_UNDERGROUND) {
        //NOTE(Adriano):Skips the face offset...
        fseek(BSDFile,16,SEEK_CUR);
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
//         if( GameEngine == MOH_GAME_UNDERGROUND ) {
//             SkipFileSection(20,BSDFile);
//         }
        DPrintf("RenderObject Id:%i\n",BSD->RenderObjectTable.RenderObject[i].Id);
        DPrintf("RenderObject Type:%i\n",BSD->RenderObjectTable.RenderObject[i].Type);
        if( BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId != -1 ) {
            DPrintf("RenderObject References RenderObject Id:%i\n",BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId);
        } else {
            DPrintf("RenderObject No Reference set...\n");
        }
        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            if( BSD->RenderObjectTable.RenderObject[i].FaceOffset == 0 ) {
                fread(&BSD->RenderObjectTable.RenderObject[i].FaceOffset,sizeof(int),1,BSDFile);
                SkipFileSection(16,BSDFile);
            } else {
                SkipFileSection(20,BSDFile);
            }
        }
        DPrintf("RenderObject Element Unknown Offset0: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].UnknownOffset0,
                BSD->RenderObjectTable.RenderObject[i].UnknownOffset0 + BSD_HEADER_SIZE);
        DPrintf("RenderObject Element Animation Offset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].AnimationDataOffset,
                BSD->RenderObjectTable.RenderObject[i].AnimationDataOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Element Unknown Offset1: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].UnknownOffset1,
                BSD->RenderObjectTable.RenderObject[i].UnknownOffset1 + BSD_HEADER_SIZE);
        DPrintf("RenderObject Element Vertex Offset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].VertexOffset,
                BSD->RenderObjectTable.RenderObject[i].VertexOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Element NumVertex: %i\n",BSD->RenderObjectTable.RenderObject[i].NumVertex);
        //These offsets are relative to the EntryTable.
        DPrintf("RenderObject FaceTableOffset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].FaceTableOffset,
                BSD->RenderObjectTable.RenderObject[i].FaceTableOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject VertexTableIndexOffset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].VertexTableIndexOffset,
                BSD->RenderObjectTable.RenderObject[i].VertexTableIndexOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Hierarchy Data Root Offset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].HierarchyDataRootOffset,
                BSD->RenderObjectTable.RenderObject[i].HierarchyDataRootOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject FaceOffset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].FaceOffset,
                BSD->RenderObjectTable.RenderObject[i].FaceOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Scale: %i;%i;%i (4096 is 1 meaning no scale)\n",
                BSD->RenderObjectTable.RenderObject[i].ScaleX / 4,
                BSD->RenderObjectTable.RenderObject[i].ScaleY / 4,
                BSD->RenderObjectTable.RenderObject[i].ScaleZ / 4);
 
    }
    BSDPatchRenderObjects(BSD,BSDFile,GameEngine);
    return 1;
}

int BSDLoadStaticVertexAndColorData(BSDRenderObject_t *RenderObject,BSDRenderObjectElement_t RenderObjectElement,FILE *BSDFile)
{
    int i;
    int Size;
    
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDLoadStaticVertexAndColorData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return 0;
    }
    if( RenderObjectElement.VertexOffset == -1 ) {
        DPrintf("BSDLoadStaticVertexAndColorData:Invalid Vertex Offset\n");
        return 0;
    }
    Size = RenderObjectElement.NumVertex * sizeof(BSDVertex_t);
    RenderObject->VertexList = malloc(Size);
    if( !RenderObject->VertexList ) {
        DPrintf("BSDParseRenderObjectVertexData:Failed to allocate memory for VertexData\n");
        return 0;
    } 
    memset(RenderObject->VertexList,0,Size);
    fseek(BSDFile,RenderObjectElement.VertexOffset + 2048,SEEK_SET);
    DPrintf("BSDParseRenderObjectVertexData:Reading Vertex definition at %i (Current:%i)\n",
            RenderObjectElement.VertexOffset + 2048,GetCurrentFilePosition(BSDFile)); 
    for( i = 0; i < RenderObjectElement.NumVertex; i++ ) {
        DPrintf("BSDParseRenderObjectVertexData:Reading Vertex at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
        fread(&RenderObject->VertexList[i],sizeof(BSDVertex_t),1,BSDFile);
        DPrintf("BSDParseRenderObjectVertexData:Vertex %i;%i;%i %i\n",RenderObject->VertexList[i].x,
                RenderObject->VertexList[i].y,RenderObject->VertexList[i].z,
                RenderObject->VertexList[i].Pad
        );
    }
    
    if( RenderObjectElement.ColorOffset != 0 ) {
        Size = RenderObjectElement.NumVertex * sizeof(BSDColor_t);
        RenderObject->ColorList = malloc(Size);
        if( !RenderObject->ColorList ) {
            DPrintf("BSDParseRenderObjectVertexData:Failed to allocate memory for ColorData\n");
            return 0;
        } 
        memset(RenderObject->ColorList,0,Size);
        fseek(BSDFile,RenderObjectElement.ColorOffset + 2048,SEEK_SET);
        DPrintf("BSDParseRenderObjectVertexData:Reading Color definition at %i (Current:%i)\n",
                RenderObjectElement.ColorOffset + 2048,GetCurrentFilePosition(BSDFile)); 
        for( i = 0; i < RenderObjectElement.NumVertex; i++ ) {
            DPrintf("BSDParseRenderObjectVertexData:Reading Color at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
            fread(&RenderObject->ColorList[i],sizeof(Color1i_t),1,BSDFile);
            DPrintf("BSDParseRenderObjectVertexData:Color %i;%i;%i;%i\n",RenderObject->ColorList[i].r,
                    RenderObject->ColorList[i].g,RenderObject->ColorList[i].b,RenderObject->ColorList[i].Pad
            );
        }
    }
    
    return 1;
}

int BSDParseRenderObjectStaticFaceData(BSDRenderObject_t *RenderObject,BSDRenderObjectElement_t RenderObjectElement,FILE *BSDFile)
{
    unsigned int   Vert0;
    unsigned int   Vert1;
    unsigned int   Vert2;
    unsigned int   PackedVertexData;
    int            FaceListSize;
    int            i;
    
    if( !RenderObject ) {
        DPrintf("BSDParseRenderObjectStaticFaceData:Invalid RenderObject!\n");
        return 0;
    }
    if( RenderObjectElement.FaceOffset == 0 ) {
        DPrintf("BSDParseRenderObjectStaticFaceData:Invalid FaceOffset!\n");
        return 0;
    }
    
    fseek(BSDFile,RenderObjectElement.FaceOffset + 2048,SEEK_SET);
    fread(&RenderObject->NumFaces,sizeof(int),1,BSDFile);
    DPrintf("BSDParseRenderObjectFaceData:Reading %i faces\n",RenderObject->NumFaces);
    FaceListSize = RenderObject->NumFaces * sizeof(BSDFace_t);
    RenderObject->StaticFaceList = malloc(FaceListSize);
    if( !RenderObject->StaticFaceList ) {
        DPrintf("BSDParseRenderObjectFaceData:Failed to allocate memory for face array\n");
        return 0;
    }
    memset(RenderObject->StaticFaceList,0,FaceListSize);
    DPrintf("BSDParseRenderObjectFaceData:Reading Face definition at %i (Current:%i)\n",
            RenderObjectElement.FaceOffset + 2048,GetCurrentFilePosition(BSDFile)); 
    for( i = 0; i < RenderObject->NumFaces; i++ ) {
        DPrintf("BSDParseRenderObjectFaceData:Reading Face at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
        
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
    return 1;
}

int BSDParseRenderObjectStaticFaceDataV2(BSDRenderObject_t *RenderObject,BSDRenderObjectElement_t RenderObjectElement,int RenderObjectIndex,
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
        DPrintf("BSDParseRenderObjectFaceDataV2:Invalid RenderObject!\n");
        return 0;
    }
    if( RenderObjectElement.FaceOffset == 0 ) {
        DPrintf("BSDParseRenderObjectFaceDataV2:Invalid FaceOffset!\n");
        return 0;
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
        DPrintf("BSDParseRenderObjectFaceDataV2:Failed to allocate memory for face array\n");
        return 0;
    }
    memset(RenderObject->StaticFaceList,0,FaceListSize);
    CurrentFaceIndex = 0;
    while( CurrentFaceIndex < RenderObject->NumFaces ) {
        DPrintf("BSDParseRenderObjectFaceDataV2:Reading Face at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);                                
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
            DPrintf("BSDParseRenderObjectFaceDataV2:Reading additional face %i \n",CurrentFaceIndex);
            fread(&Marker,sizeof(Marker),1,BSDFile);
            DPrintf("BSDParseRenderObjectFaceDataV2:Found Marker %u (Vertex %i) Texture:%u Mask %i\n",Marker,Marker & 0x1FFF,Marker >> 16,0x1FFF);
            if( ( Marker & 0x1FFF ) == 0x1FFF || Marker == 0x1fff1fff ) {
                DPrintf("BSDParseRenderObjectFaceDataV2:Aborting since a marker was found\n");
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

            DPrintf("BSDParseRenderObjectFaceDataV2:Vert0:%i Vert1:%i Vert2:%i\n",TempFace.Vert0,TempFace.Vert1,
                    TempFace.Vert2);
            CurrentFaceIndex++;
        }
        if( Marker == 0x1fff1fff ) {
            DPrintf("BSDParseRenderObjectFaceDataV2:Sentinel Face found Done reading faces for renderobject %i\n",RenderObjectElement.Id);
            DPrintf("BSDParseRenderObjectFaceDataV2:Loaded %i faces (Expected %i)\n",CurrentFaceIndex,RenderObject->NumFaces);
            assert(CurrentFaceIndex == RenderObject->NumFaces);
            break;
        }
    }
    return 1;
}


int BSDLoadStaticFaceData(BSDRenderObject_t *RenderObject,BSDRenderObjectElement_t RenderObjectElement,int RenderObjectIndex,
                          int ReferencedRenderObjectIndex,int GameVersion,FILE *BSDFile)
{
    assert(sizeof(BSDFace_t) == 24);

    if( RenderObjectElement.FaceOffset == 0 ) {
        return 0;
    }
    DPrintf("BSDLoadStaticFaceData:RenderObject Id %i\n",RenderObjectElement.Id);
    
    if( GameVersion == MOH_GAME_UNDERGROUND ) {
        if( !BSDParseRenderObjectStaticFaceDataV2(RenderObject,RenderObjectElement,RenderObjectIndex,ReferencedRenderObjectIndex,BSDFile) ) {
            return 0;
        }
    } else {
        if( !BSDParseRenderObjectStaticFaceData(RenderObject,RenderObjectElement,BSDFile) ) {
            return 0;
            
        }
    }
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
    RenderObject->CurrentVertexTable = malloc(RenderObject->NumVertexTables * sizeof(BSDVertexTable_t));
    if( !RenderObject->CurrentVertexTable ) {
        DPrintf("BSDLoadAnimationVertexData:Failed to allocate memory for VertexTable.\n");
        return 0;
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
int BSDLoadMOHUndergroundAnimationFaceData(BSDRenderObject_t *RenderObject,int FaceTableOffset,int RenderObjectIndex,
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
        printf("BSDLoadMOHUndergroundAnimationFaceData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
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
        DPrintf("BSDLoadMOHUndergroundAnimationFaceData:Invalid number of faces.\n");
        return 0;
    }
    //NOTE(Adriano):Now we can load the faces at the offset specified by the RenderObject.
    fseek(BSDFile,GlobalFaceOffset + LocalFaceOffset + BSD_HEADER_SIZE,SEEK_SET);
    DPrintf("BSDLoadMOHUndergroundAnimationFaceData:Reading %i faces at offset %li (%li)\n",NumFaces,ftell(BSDFile),ftell(BSDFile) - 2048);
    
    RenderObject->FaceList = malloc(NumFaces * sizeof(BSDAnimatedModelFace_t));
    RenderObject->NumFaces = NumFaces;
    if( !RenderObject->FaceList ) {
        DPrintf("BSDLoadMOHUndergroundAnimationFaceData:Failed to allocate memory for face list.\n");
        return 0;
    }
    CurrentFaceIndex = 0;
    while( CurrentFaceIndex < NumFaces ) {
        DPrintf("Reading it at %li\n",ftell(BSDFile) - 2048);
        fread(&TempFace,sizeof(BSDAnimatedModelFace_t),1,BSDFile);
    
        BSDCopyAnimatedModelFace(TempFace,&RenderObject->FaceList[CurrentFaceIndex]);
        
        DPrintf(" -- FACE %i --\n",CurrentFaceIndex);
        BSDPrintAnimatedModelFace(RenderObject->FaceList[CurrentFaceIndex]);
        CurrentFaceIndex++;
        while( 1 ) {
            //NOTE(Adriano):
            //Data is split into two shorts.
            //First one (Marker1) contains data that references the VertexTable while the
            //other one (Marker2) contains the UV coordinates for the new vertex.
            fread(&Marker1,sizeof(Marker1),1,BSDFile);
            fread(&Marker2,sizeof(Marker2),1,BSDFile);

            if( Marker1 == 0x1FFF ) {
                DPrintf("BSDLoadMOHUndergroundAnimationFaceData:Aborting since a marker was found\n");
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
            BSDCopyAnimatedModelFace(TempFace,&RenderObject->FaceList[CurrentFaceIndex]);
            DPrintf(" -- FACE %i --\n",CurrentFaceIndex);
            BSDPrintAnimatedModelFace(RenderObject->FaceList[CurrentFaceIndex]);
            CurrentFaceIndex++;
        }
        //NOTE(Adriano):Last Data is identified by the two marker being set to 0x1FFF
        if( (Marker1 == 0x1fff && Marker2 == 0x1fff ) ) {
            DPrintf("BSDLoadMOHUndergroundAnimationFaceData:Sentinel Face found Done reading faces for RenderObject\n");
            DPrintf("BSDLoadMOHUndergroundAnimationFaceData:Loaded %i faces (Expected %i)\n",CurrentFaceIndex,NumFaces);
            break;
        }
    }
    assert(CurrentFaceIndex == NumFaces);
    return 1;
}
int BSDLoadAnimationFaceData(BSDRenderObject_t *RenderObject,int FaceTableOffset,int RenderObjectIndex,int ReferencedRenderObjectIndex,
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
            return BSDLoadMOHUndergroundAnimationFaceData(RenderObject,FaceTableOffset,RenderObjectIndex,ReferencedRenderObjectIndex,
                                                          EntryTable,BSDFile);
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
    RenderObject->NumFaces = NumFaces;
    if( !RenderObject->FaceList ) {
        DPrintf("BSDLoadAnimationFaceData:Failed to allocate memory for face list.\n");
        return 0;
    }
    DPrintf("BSDLoadAnimationFaceData:Loading %i faces\n",NumFaces);
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
    
    if( !Bone ) {
        DPrintf("BSDRecursivelyLoadHierarchyData:Failed to allocate bone data\n");
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
        Bone->Child2 = BSDRecursivelyLoadHierarchyData(BoneDataStartingPosition,Child2Offset,BSDFile);
    }
    if( Child1Offset != -1 ) {
        Bone->Child1 = BSDRecursivelyLoadHierarchyData(BoneDataStartingPosition,Child1Offset,BSDFile);
    }
    return Bone;
}

int BSDLoadAnimationHierarchyData(BSDRenderObject_t *RenderObject,int HierarchyDataRootOffset,BSDEntryTable_t EntryTable,FILE *BSDFile)
{
    if( !RenderObject || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDLoadAnimationHierarchyData: Invalid %s\n",InvalidFile ? "file" : "RenderObject struct");
        return 0;
    }
    if( HierarchyDataRootOffset == -1 ) {
        DPrintf("BSDLoadAnimationHierarchyData:Invalid Face Table Index Offset\n");
        return 0;
    }
    
    RenderObject->HierarchyDataRoot = BSDRecursivelyLoadHierarchyData(EntryTable.AnimationHierarchyDataOffset,HierarchyDataRootOffset,BSDFile);
    
    if( !RenderObject->HierarchyDataRoot ) {
        DPrintf("BSDLoadAnimationHierarchyData:Couldn't load hierarchy data\n");
        return 0;
    }
    return 1;
}
void BSDDecodeQuaternions(int QuatPart0,int QuatPart1,int QuatPart2,BSDQuaternion_t *OutQuaternion1,BSDQuaternion_t *OutQuaternion2)
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
    RenderObject->NumAnimations = NumAnimationOffset;
    for( i = 0; i < NumAnimationOffset; i++ ) {
        fread(&AnimationOffsetTable[i],sizeof(AnimationOffsetTable[i]),1,BSDFile);
        if( AnimationOffsetTable[i] == -1 ) {
            continue;
        }
    }

    AnimationTableEntry = malloc(RenderObject->NumAnimations * sizeof(BSDAnimationTableEntry_t) );
    for( i = 0; i < NumAnimationOffset; i++ ) {
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
                    
                    BSDDecodeQuaternions(QuatPart0,QuatPart1,QuatPart2,
                                         &RenderObject->AnimationList[i].Frame[j].QuaternionList[NumDecodedQuaternions],
                                         &RenderObject->AnimationList[i].Frame[j].QuaternionList[NumDecodedQuaternions+1]);
                    NumDecodedQuaternions += 2;
                }
//                 DPrintf("Decoded %i quaternions out of %i\n",NumDecodedQuaternions,RenderObject->AnimationList[i].Frame[j].NumQuaternions);
                if( NumDecodedQuaternions == (RenderObject->AnimationList[i].Frame[j].NumQuaternions - 1) ) {
                    QuatPart0 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[NumEncodedQuaternions-2];
                    QuatPart1 = RenderObject->AnimationList[i].Frame[j].EncodedQuaternionList[NumEncodedQuaternions-1];
//                     DPrintf("QuatPart0:%i QuatPart1:%i\n",QuatPart0,QuatPart1);
                    BSDDecodeQuaternions(QuatPart0,QuatPart1,-1,
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
    return 1;
}
BSDRenderObject_t *BSDLoadAnimatedRenderObject(BSDRenderObjectElement_t RenderObjectElement,BSDEntryTable_t BSDEntryTable,FILE *BSDFile,
                                               int RenderObjectIndex,int ReferencedRenderObjectIndex,int GameVersion
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
    RenderObject->Id = RenderObjectElement.Id;
    RenderObject->ReferencedRenderObjectId = RenderObjectElement.ReferencedRenderObjectId;
    RenderObject->Type = RenderObjectElement.Type;
    RenderObject->VertexTable = NULL;
    RenderObject->CurrentVertexTable = NULL;
    RenderObject->StaticFaceList = NULL;
    RenderObject->VertexList = NULL;
    RenderObject->ColorList = NULL;
    RenderObject->FaceList = NULL;
    RenderObject->HierarchyDataRoot = NULL;
    RenderObject->AnimationList = NULL;
    RenderObject->VAO = NULL;
    RenderObject->CurrentAnimationIndex = -1;
    RenderObject->CurrentFrameIndex = -1;
    RenderObject->Next = NULL;
    RenderObject->IsStatic = false;
    
    RenderObject->Scale[0] = (float) (RenderObjectElement.ScaleX  / 16 ) / 4096.f;
    RenderObject->Scale[1] = (float) (RenderObjectElement.ScaleY  / 16 ) / 4096.f;
    RenderObject->Scale[2] = (float) (RenderObjectElement.ScaleZ  / 16 ) / 4096.f;

    glm_vec3_zero(RenderObject->Center);

    if( !BSDLoadAnimationVertexData(RenderObject,RenderObjectElement.VertexTableIndexOffset,BSDEntryTable,BSDFile) ) {
        DPrintf("BSDLoadAnimatedRenderObject:Failed to load vertex data\n");
        goto Failure;
    }

    if( !BSDLoadAnimationFaceData(RenderObject,RenderObjectElement.FaceTableOffset,RenderObjectIndex,ReferencedRenderObjectIndex,
        BSDEntryTable,BSDFile,GameVersion) ) {
        DPrintf("BSDLoadAnimatedRenderObject:Failed to load face data\n");
        goto Failure;
    }

    if( !BSDLoadAnimationHierarchyData(RenderObject,RenderObjectElement.HierarchyDataRootOffset,BSDEntryTable,BSDFile) ) {
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

BSDRenderObject_t *BSDLoadStaticRenderObject(BSDRenderObjectElement_t RenderObjectElement,BSDEntryTable_t BSDEntryTable,FILE *BSDFile,
                                               int RenderObjectIndex,int ReferencedRenderObjectIndex,int GameVersion
)
{
    BSDRenderObject_t *RenderObject;
    
    RenderObject = NULL;
    if( !BSDFile ) {
        DPrintf("BSDLoadStaticRenderObject:Invalid BSD file\n");
        goto Failure;
    }
    RenderObject = malloc(sizeof(BSDRenderObject_t));
    if( !RenderObject ) {
        DPrintf("BSDLoadStaticRenderObject:Failed to allocate memory for RenderObject\n");
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
    RenderObject->IsStatic = true;

    RenderObject->Scale[0] = (float) (RenderObjectElement.ScaleX  / 16 ) / 4096.f;
    RenderObject->Scale[1] = (float) (RenderObjectElement.ScaleY  / 16 ) / 4096.f;
    RenderObject->Scale[2] = (float) (RenderObjectElement.ScaleZ  / 16 ) / 4096.f;

    glm_vec3_zero(RenderObject->Center);

    if( !BSDLoadStaticVertexAndColorData(RenderObject,RenderObjectElement,BSDFile) ) {
        DPrintf("BSDLoadStaticRenderObject:Failed to load vertex/color data\n");
        goto Failure;
    }
    if( !BSDLoadStaticFaceData(RenderObject,RenderObjectElement,RenderObjectIndex,ReferencedRenderObjectIndex,GameVersion,BSDFile) ) {
        DPrintf("BSDLoadStaticRenderObject:Failed to load face data\n");
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

BSDRenderObject_t *BSDLoadAllAnimatedRenderObjects(const char *FName,int *GameVersion)
{
    FILE *BSDFile;
    BSD_t *BSD;
    int LocalGameVersion;
    BSDRenderObject_t *RenderObjectList;
    BSDRenderObject_t *RenderObject;
    int i;
    int ReferencedRenderObjectIndex;
    
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
        /*
         * NOTE(Adriano):
         * RenderObjectIndex is only used by BSDLoadAnimatedRenderObject only when the game is
         * MOH:Underground and the FaceTableOffset is equals to -1.
         * We need to use the referenced RenderObject index if it is set otherwise the offset may
         * not be valid.
         */
        ReferencedRenderObjectIndex = -1;
        if( BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId != -1 ) {
            //NOTE(Adriano):Some RenderObjects are bundled in other files and referenced by Id.
            //This could happens for example when loading the SET1 BSD that is looking for another RenderObject containted inside
            //the level file.
            //For the moment this is bypassed and could cause the RenderObject to not be loaded due to missing offsets...
            ReferencedRenderObjectIndex = BSDGetRenderObjectIndexById(BSD,BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObjectId);
        }
        DPrintf("BSDLoadAllAnimatedRenderObjects:Loading Animated RenderObject %i\n",BSD->RenderObjectTable.RenderObject[i].Id);
        if( BSD->RenderObjectTable.RenderObject[i].AnimationDataOffset == -1 ) {
            RenderObject = BSDLoadStaticRenderObject(BSD->RenderObjectTable.RenderObject[i],BSD->EntryTable,
                                                   BSDFile,i,ReferencedRenderObjectIndex,LocalGameVersion);
        } else {
            RenderObject = BSDLoadAnimatedRenderObject(BSD->RenderObjectTable.RenderObject[i],BSD->EntryTable,
                                                   BSDFile,i,ReferencedRenderObjectIndex,LocalGameVersion);
        }
        if( !RenderObject ) {
            DPrintf("BSDLoadAllAnimatedRenderObjects:Failed to load animated RenderObject.\n");
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
