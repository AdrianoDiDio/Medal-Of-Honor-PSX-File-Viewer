// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    SSTViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SSTViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SSTViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/
#include "GFX.h"
#include "SSTViewer.h"

void GFXFree(GFX_t *GFX)
{
    GFX_t *Temp;
    int i;
    int j;
    
    if( !GFX ) {
        return;
    }
    
    while( GFX ) {
        if( GFX->Vertex ) {
            free(GFX->Vertex);
        }
        if( GFX->Normal ) {
            free(GFX->Normal);
        }
        if( GFX->Face ) {
            free(GFX->Face);
        }
        if( GFX->AnimationIndex ) {
            free(GFX->AnimationIndex);
        }
        if( GFX->Animation ) {
            for( i = 0; i < GFX->Header.NumAnimationIndex; i++ ) {
                for( j = 0; j < GFX->Animation[i].NumFrames; j++ ) {
                    free(GFX->Animation[i].Frame[j].Vertex);
                }
                free(GFX->Animation[i].Frame);
            }
            free(GFX->Animation);
        }
        if( GFX->VAO ) {
            VAOFree(GFX->VAO);
        }
        Temp = GFX;
        GFX = GFX->Next;
        free(Temp);
    }
}
void GFXReadHeaderChunk(GFX_t *GFX,void **GFXFileBuffer)
{    
    if( !GFX || !GFXFileBuffer ) {
        bool InvalidFileBuffer = (GFXFileBuffer == NULL ? true : false);
        DPrintf("GFXReadHeaderChunk: Invalid %s\n",InvalidFileBuffer ? "file buffer" : "gfx struct");
        return;
    }
    memcpy(&GFX->Header,*GFXFileBuffer,sizeof(GFX->Header));
    *GFXFileBuffer += sizeof(GFX->Header);
    DPrintf(" -- GFX Header --\n");
    DPrintf("NumVertices:%i\n",GFX->Header.NumVertices);
    DPrintf("NumNormals:%i\n",GFX->Header.NumNormals);
    DPrintf("NumFaces:%i\n",GFX->Header.NumFaces);
    DPrintf("NumUnk3:%i\n",GFX->Header.NumUnk3);
    DPrintf("NumAnimationIndex:%i\n",GFX->Header.NumAnimationIndex);
    DPrintf("Pad:%i\n",GFX->Header.Pad);
    assert(GFX->Header.Pad == 0);
}

void GFXReadOffsetTableChunk(GFX_t *GFX,void **GFXFileBuffer)
{    
    if( !GFX || !GFXFileBuffer ) {
        bool InvalidFileBuffer = (GFXFileBuffer == NULL ? true : false);
        DPrintf("GFXReadOffsetTableChunk: Invalid %s\n",InvalidFileBuffer ? "file buffer" : "gfx struct");
        return;
    }
    memcpy(&GFX->OffsetTable,*GFXFileBuffer,sizeof(GFX->OffsetTable));
    *GFXFileBuffer += sizeof(GFX->OffsetTable);
    DPrintf(" -- GFX Offset Table --\n");
    DPrintf("Offset0:%i\n",GFX->OffsetTable.Offset0);
    DPrintf("Pad0:%i\n",GFX->OffsetTable.Pad0);
    DPrintf("Offset1:%i\n",GFX->OffsetTable.Offset1);
    DPrintf("Pad1:%i\n",GFX->OffsetTable.Pad1);
    DPrintf("Offset2:%i\n",GFX->OffsetTable.Offset2);
    DPrintf("Pad2:%i\n",GFX->OffsetTable.Pad2);
    DPrintf("Pad3:%i\n",GFX->OffsetTable.Pad3);
    assert(GFX->OffsetTable.Pad3 == 0);
}
void GFXReadAnimationIndexTable(GFX_t *GFX,void **GFXFileBuffer)
{
    int AnimationIndexSize;
    int i;
    
    if( !GFX || !GFXFileBuffer ) {
        bool InvalidFileBuffer = (GFXFileBuffer == NULL ? true : false);
        DPrintf("GFXReadAnimationIndexTable: Invalid %s\n",InvalidFileBuffer ? "file buffer" : "gfx struct");
        return;
    }
    
    if( GFX->Header.NumAnimationIndex == 0 ) {
        DPrintf("GFXReadAnimationIndexTable:0 animation index in header...skipping\n");
        return;
    }
    
    AnimationIndexSize = (GFX->Header.NumAnimationIndex) * sizeof(int);
    GFX->AnimationIndex = malloc(AnimationIndexSize);
    memcpy(GFX->AnimationIndex,*GFXFileBuffer,AnimationIndexSize);
    *GFXFileBuffer += AnimationIndexSize;
    for( i = 0; i < GFX->Header.NumAnimationIndex; i++ ) {
        DPrintf(" -- Index %i --\n",i);
        DPrintf("Animation Data: %i\n",GFX->AnimationIndex[i]);
    }
}
void GFXReadVertexChunk(GFX_t *GFX,void **GFXFileBuffer)
{
    int VertexSize;
    int i;
    
    if( !GFX || !GFXFileBuffer ) {
        bool InvalidFileBuffer = (GFXFileBuffer == NULL ? true : false);
        DPrintf("GFXReadVertexChunk: Invalid %s\n",InvalidFileBuffer ? "file buffer" : "gfx struct");
        return;
    }
    if( GFX->Header.NumVertices == 0 ) {
        DPrintf("GFXReadVertexChunk:0 vertex in header...skipping\n");
        return;
    }
    VertexSize = GFX->Header.NumVertices * sizeof(GFXVertex_t);
    GFX->Vertex = malloc(VertexSize);
    memcpy(GFX->Vertex,*GFXFileBuffer,VertexSize);
    *GFXFileBuffer += VertexSize;    
    for( i = 0; i < GFX->Header.NumVertices; i++ ) {
        DPrintf(" -- VERTEX %i --\n",i);
        DPrintf("Position:(%i;%i;%i)\n",GFX->Vertex[i].x,GFX->Vertex[i].y,GFX->Vertex[i].z);
        DPrintf("Pad:%i\n",GFX->Vertex[i].Pad);
//         assert(GFX->Vertex[i].Pad == 123);
    }
}

void GFXReadNormalChunk(GFX_t *GFX,void **GFXFileBuffer)
{
    int NormalSize;
    int i;
    
    if( !GFX || !GFXFileBuffer ) {
        bool InvalidFileBuffer = (GFXFileBuffer == NULL ? true : false);
        DPrintf("GFXReadNormalChunk: Invalid %s\n",InvalidFileBuffer ? "file buffer" : "gfx struct");
        return;
    }
    
    if( GFX->Header.NumNormals == 0 ) {
        DPrintf("GFXReadNormalChunk:0 normal in header...skipping\n");
        return;
    }
    
    NormalSize = (GFX->Header.NumNormals) * sizeof(GFXVertex_t);
    GFX->Normal = malloc(NormalSize);
    memcpy(GFX->Normal,*GFXFileBuffer,NormalSize);
    *GFXFileBuffer += NormalSize;
    for( i = 0; i < GFX->Header.NumNormals; i++ ) {
        DPrintf(" -- Normal %i --\n",i);
        DPrintf("Position:(%i;%i;%i)\n",GFX->Normal[i].x,GFX->Normal[i].y,GFX->Normal[i].z);
        DPrintf("Pad:%i\n",GFX->Normal[i].Pad);
//         assert(GFX->Normal[i].Pad == 123);
    }
}

void GFXReadFaceChunk(GFX_t *GFX,void **GFXFileBuffer)
{
    int FaceSize;
    int i;
    
    if( !GFX || !GFXFileBuffer ) {
        bool InvalidFileBuffer = (GFXFileBuffer == NULL ? true : false);
        DPrintf("GFXReadFaceChunk: Invalid %s\n",InvalidFileBuffer ? "file buffer" : "gfx struct");
        return;
    }
    
    if( GFX->Header.NumFaces == 0 ) {
        DPrintf("GFXReadFaceChunk:0 face in header...skipping\n");
        return;
    }
    
    assert(sizeof(GFXFace_t) == 36);
    FaceSize = (GFX->Header.NumFaces) * sizeof(GFXFace_t);
    GFX->Face = malloc(FaceSize);
    memcpy(GFX->Face,*GFXFileBuffer,FaceSize);
    *GFXFileBuffer += FaceSize;
    for( i = 0; i < GFX->Header.NumFaces; i++ ) {
        DPrintf(" -- FACE %i --\n",i);
 
        DPrintf("TSB:%i\n",GFX->Face[i].TSB);
        DPrintf("U1V1:(%i;%i)\n",GFX->Face[i].U1, GFX->Face[i].V1);
        DPrintf("U2V2:(%i;%i)\n",GFX->Face[i].U2, GFX->Face[i].V2);
        DPrintf("U3V3:(%i;%i)\n",GFX->Face[i].U3, GFX->Face[i].V3);
        DPrintf("Vertex0:%i\n",GFX->Face[i].Vert0);
        DPrintf("Vertex0:%i\n",GFX->Face[i].Vert1);
        DPrintf("Vertex0:%i\n",GFX->Face[i].Vert2);
        DPrintf("Norm0:%i\n",GFX->Face[i].Norm0);
        DPrintf("Norm1:%i\n",GFX->Face[i].Norm1);
        DPrintf("Norm2:%i\n",GFX->Face[i].Norm2);
        DPrintf("CBA:%i\n",GFX->Face[i].CBA);
        DPrintf("Unk0:%i\n",GFX->Face[i].Unk0);
        DPrintf("Color0:%i;%i;%i;%i\n",GFX->Face[i].RGB0.rgba[0],GFX->Face[i].RGB0.rgba[1],
            GFX->Face[i].RGB0.rgba[2],GFX->Face[i].RGB0.rgba[3]
        );
        DPrintf("Color1:%i;%i;%i;%i\n",GFX->Face[i].RGB1.rgba[0],GFX->Face[i].RGB1.rgba[1],
            GFX->Face[i].RGB1.rgba[2],GFX->Face[i].RGB1.rgba[3]
        );
        DPrintf("Color2:%i;%i;%i;%i\n",GFX->Face[i].RGB2.rgba[0],GFX->Face[i].RGB2.rgba[1],
            GFX->Face[i].RGB2.rgba[2],GFX->Face[i].RGB2.rgba[3]
        );
    }
}
void GFXReadAnimationChunk(GFX_t *GFX,void **GFXFileBuffer)
{
    int VertexSize;
    int i;
    int j;
    
    if( !GFX || !GFXFileBuffer ) {
        bool InvalidFileBuffer = (GFXFileBuffer == NULL ? true : false);
        DPrintf("GFXReadAnimationChunk: Invalid %s\n",InvalidFileBuffer ? "file buffer" : "gfx struct");
        return;
    }
    
    if( GFX->Header.NumAnimationIndex == 0 ) {
        DPrintf("GFXReadAnimationChunk:0 animation index in header...skipping\n");
        return;
    }
    
    GFX->Animation = malloc(GFX->Header.NumAnimationIndex * sizeof(GFXAnimation_t));
        
    if( !GFX->Animation ) {
        DPrintf("GFXReadAnimationChunk:failed to allocate memory for animation\n");
        return;
    }
    
    for( i = 0; i < GFX->Header.NumAnimationIndex; i++ ) {
        GFX->Animation[i].Frame = malloc(GFX->AnimationIndex[i] * sizeof(GFXAnimationFrame_t));
        GFX->Animation[i].NumFrames = GFX->AnimationIndex[i];

        if( !GFX->Animation[i].Frame ) {
            DPrintf("GFXReadAnimationChunk:failed to allocate memory for frame %i\n",i);
            continue;
        }
        for( j = 0; j < GFX->AnimationIndex[i]; j++ ) {
            VertexSize = GFX->Header.NumVertices * sizeof(GFXVertex_t);
            GFX->Animation[i].Frame[j].Vertex = malloc(VertexSize);
            memcpy(GFX->Animation[i].Frame[j].Vertex,*GFXFileBuffer,VertexSize);
            *GFXFileBuffer += VertexSize;
        }
    }
}
// void GFXReadAnimationChunk(GFX_t *GFX,void **GFXFileBuffer)
// {
//     int i;
//     int Ret;
//     int NumClerkbAnimationVertices = 16121;
//     
//     if( !GFX || !InFile ) {
//         bool InvalidFile = (InFile == NULL ? true : false);
//         DPrintf("GFXReadFaceChunk: Invalid %s\n",InvalidFile ? "file" : "gfx struct");
//         return;
//     }
//     GFX->NumAnimations = 0;
// //     Clerkb seems to have 16121 vertices....
//     GFX->AnimationData = malloc(NumClerkbAnimationVertices * sizeof(GFXVertex_t));
//     
//     for( i = 0; i < NumClerkbAnimationVertices; i++ ) {
//         DPrintf(" -- ANIMATION %i --\n",i);
//         DPrintf("Offset:%li\n",ftell(InFile));
//         Ret = fread(&GFX->AnimationData[i],sizeof(GFX->AnimationData[i]),1,InFile);
//         if( Ret != 1 ) {
//             DPrintf("GFXReadAnimationChunk:Early failure when reading animation data\n");
//             return;
//         }
//         DPrintf("Pad:%i\n",GFX->AnimationData[i].Pad);
//         assert(GFX->AnimationData[i].Pad == 0 || GFX->AnimationData[i].Pad == 101);
//         GFX->NumAnimations++;
//     }
//     DPrintf("Read %i animation vertex\n",GFX->NumAnimations);
// }
void GFXGetObjectMatrix(GFX_t *GFX,mat4 Result)
{
    vec3 Temp;
    vec3 Rotation;
    
    glm_mat4_identity(Result);
    
    Rotation[0] = ( GFX->RotationX / 4096.f) * 360.f;
    Rotation[1] = ( GFX->RotationY / 4096.f) * 360.f;
    Rotation[2] = ( GFX->RotationZ / 4096.f) * 360.f;
//     glm_vec3_copy(RenderObjectDrawable->Position,temp);
//     glm_vec3_rotate(temp, DEGTORAD(180.f), GLM_XUP);    
//     glm_translate(Result,temp);
    
    Temp[0] = 0;
    Temp[1] = 1;
    Temp[2] = 0;
    glm_rotate(Result,glm_rad(-Rotation[1]), Temp);
    Temp[0] = 1;
    Temp[1] = 0;
    Temp[2] = 0;
    glm_rotate(Result,glm_rad(Rotation[0]), Temp);
    Temp[0] = 0;
    Temp[1] = 0;
    Temp[2] = 1;
    glm_rotate(Result,glm_rad(Rotation[2]), Temp);
//     glm_scale(Result,RenderObjectDrawable->Scale);
}
void GFXRender(GFX_t *GFX,VRAM_t *VRAM,mat4 ProjectionMatrix)
{
    Shader_t *Shader;
    int PaletteTextureId;
    int TextureIndexId;
    int OrthoMatrixID;
    mat4 ModelMatrix;
    mat4 ModelViewMatrix;
    mat4 MVPMatrix;
    
    if( !GFX ) {
        return;
    }
    
    if( !VRAM ) {
        return;
    }
    
    Shader = ShaderCache("SSTShader","Shaders/SSTVertexShader.glsl","Shaders/SSTFragmentShader.glsl");
    
    if( Shader ) {
        glUseProgram(Shader->ProgramId);
        
        OrthoMatrixID = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
        glm_mat4_identity(ModelViewMatrix);
        GFXGetObjectMatrix(GFX, ModelViewMatrix);
        glm_mat4_mul(ProjectionMatrix,ModelViewMatrix,MVPMatrix);
        glm_rotate_x(MVPMatrix,glm_rad(180.f), MVPMatrix);
        glUniformMatrix4fv(OrthoMatrixID,1,false,&MVPMatrix[0][0]);
        PaletteTextureId = glGetUniformLocation(Shader->ProgramId,"ourPaletteTexture");
        TextureIndexId = glGetUniformLocation(Shader->ProgramId,"ourIndexTexture");
        glUniform1i(TextureIndexId, 0);
        glUniform1i(PaletteTextureId,  1);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, VRAM->TextureIndexPage.TextureId);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, VRAM->PalettePage.TextureId);
        glBindVertexArray(GFX->VAO->VAOId[0]);
        glDrawArrays(GL_TRIANGLES, 0, GFX->VAO->Count);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D,0);
        glUseProgram(0);

    }
}
void GFXFillVertexBuffer(int *Buffer,int *BufferSize,int x,int y,int z,Color1i_t Color,int U,int V,int CLUTX,int CLUTY,int ColorMode)
{
    if( !Buffer ) {
        DPrintf("GFXFillVertexBuffer:Invalid Buffer\n");
        return;
    }
    if( !BufferSize ) {
        DPrintf("GFXFillVertexBuffer:Invalid BufferSize\n");
        return;
    }
    Buffer[*BufferSize] =   x;
    Buffer[*BufferSize+1] = y;
    Buffer[*BufferSize+2] = z;
    Buffer[*BufferSize+3] = U;
    Buffer[*BufferSize+4] = V;
    Buffer[*BufferSize+5] = Color.rgba[0];
    Buffer[*BufferSize+6] = Color.rgba[1];
    Buffer[*BufferSize+7] = Color.rgba[2];
    Buffer[*BufferSize+8] = CLUTX;
    Buffer[*BufferSize+9] = CLUTY;
    Buffer[*BufferSize+10] = ColorMode;
    *BufferSize += 11;
}

void GFXPrepareVAO(GFX_t *GFX)
{
    int *VertexData;
    int VertexSize;
    int TotalVertexSize;
    int VertexPointer;
    int Stride;
    int VertexOffset;
    int TextureOffset;
    int ColorOffset;
    int CLUTOffset;
    int ColorModeOffset;
    int Vert0;
    int Vert1;
    int Vert2;
    int U0;
    int V0;
    int U1;
    int V1;
    int U2;
    int V2;
    int TSB;
    int CBA;
    int CLUTPosX;
    int CLUTPosY;
    int CLUTPage;
    int CLUTDestX;
    int CLUTDestY;
    int VRAMPage;
    int ColorMode;
    int i;

    if( !GFX ) {
        DPrintf("GFXPrepareVAO: Invalid GFX File\n");
        return;
    }
//            XYZ UV RGB CLUT ColorMode
    Stride = (3 + 2 + 3 + 2 + 1) * sizeof(int);
                
    VertexOffset = 0;
    TextureOffset = 3;
    ColorOffset = 5;
    CLUTOffset = 8;
    ColorModeOffset = 10;

    VertexSize = Stride * 3;
    TotalVertexSize = VertexSize * GFX->Header.NumFaces;
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    GFX->VAO = VAOInitXYZUVRGBCLUTColorModeInteger(NULL,TotalVertexSize,Stride,VertexOffset,TextureOffset,ColorOffset,CLUTOffset,ColorModeOffset,
                                              GFX->Header.NumFaces * 3);
    for( i = 0; i < GFX->Header.NumFaces; i++ ) {
        Vert0 = GFX->Face[i].Vert0;
        Vert1 = GFX->Face[i].Vert1;
        Vert2 = GFX->Face[i].Vert2;
        U0 = GFX->Face[i].U1;
        V0 = GFX->Face[i].V1;
        U1 = GFX->Face[i].U2;
        V1 = GFX->Face[i].V2;
        U2 = GFX->Face[i].U3;
        V2 = GFX->Face[i].V3;
        TSB = GFX->Face[i].TSB;
        CBA = GFX->Face[i].CBA;
        
        
        ColorMode = (TSB >> 7) & 0x3;
        VRAMPage = TSB & 0x1F;
        
        
        CLUTPosX = (CBA << 4) & 0x3F0;
        CLUTPosY = (CBA >> 6) & 0x1ff;
        CLUTPage = VRAMGetCLUTPage(CLUTPosX,CLUTPosY);
        CLUTDestX = VRAMGetCLUTPositionX(CLUTPosX,CLUTPosY,CLUTPage);
        CLUTDestY = CLUTPosY + VRAMGetCLUTOffsetY(ColorMode);
        CLUTDestX += VRAMGetTexturePageX(CLUTPage);

        U0 += VRAMGetTexturePageX(VRAMPage);
        V0 += VRAMGetTexturePageY(VRAMPage,ColorMode);
        U1 += VRAMGetTexturePageX(VRAMPage);
        V1 += VRAMGetTexturePageY(VRAMPage,ColorMode);
        U2 += VRAMGetTexturePageX(VRAMPage);
        V2 += VRAMGetTexturePageY(VRAMPage,ColorMode);
        
        GFXFillVertexBuffer(VertexData,&VertexPointer,GFX->Vertex[Vert0].x,GFX->Vertex[Vert0].y,GFX->Vertex[Vert0].z,
                                GFX->Face[i].RGB0,U0,V0,CLUTDestX,CLUTDestY,ColorMode);
        GFXFillVertexBuffer(VertexData,&VertexPointer,GFX->Vertex[Vert1].x,GFX->Vertex[Vert1].y,GFX->Vertex[Vert1].z,
                                GFX->Face[i].RGB1,U1,V1,CLUTDestX,CLUTDestY,ColorMode);
        GFXFillVertexBuffer(VertexData,&VertexPointer,GFX->Vertex[Vert2].x,GFX->Vertex[Vert2].y,GFX->Vertex[Vert2].z,
                                GFX->Face[i].RGB2,U2,V2,CLUTDestX,CLUTDestY,ColorMode);
        VAOUpdate(GFX->VAO,VertexData,VertexSize,3);
        VertexPointer = 0;
    }
    free(VertexData);
}

GFX_t *GFXRead(void* GFXFileBuffer,int GFXLength)
{
    GFX_t *GFXData;
    void *GFXEnd;
    int TotalAnimSize;
    int i;
    
    if( !GFXFileBuffer ) {
        DPrintf("GFXRead: Invalid buffer.\n");
        return NULL;
    }
    
    GFXData = malloc(sizeof(GFX_t));
    if( !GFXData ) {
        DPrintf("GFXRead: Failed to allocate memory for GFX model\n");
        return NULL;
    }
    
    GFXEnd = GFXFileBuffer + GFXLength;
    GFXData->Vertex = NULL;
    GFXData->Normal = NULL;
    GFXData->Face = NULL;
    GFXData->Next = NULL;
    GFXData->VAO = NULL;
    GFXReadHeaderChunk(GFXData,&GFXFileBuffer);
    GFXReadOffsetTableChunk(GFXData,&GFXFileBuffer);
    GFXReadAnimationIndexTable(GFXData,&GFXFileBuffer);
    GFXReadVertexChunk(GFXData,&GFXFileBuffer);
    GFXReadNormalChunk(GFXData,&GFXFileBuffer);
    GFXReadFaceChunk(GFXData,&GFXFileBuffer);
    GFXReadAnimationChunk(GFXData,&GFXFileBuffer);
    assert(GFXFileBuffer == GFXEnd);
    return GFXData;
}
