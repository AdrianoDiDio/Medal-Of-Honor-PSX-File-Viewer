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
    DPrintf("NumUnk4:%i\n",GFX->Header.NumUnk4);
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

void GFXReadVertexChunk(GFX_t *GFX,void **GFXFileBuffer)
{
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
    
    GFX->Vertex = malloc(GFX->Header.NumVertices * sizeof(GFXVertex_t));
//     DPrintf("Reading Vertex chunk at offset %i\n",*GFXFileBuffer - GFXFileBuffer);
    for( i = 0; i < GFX->Header.NumVertices; i++ ) {
        memcpy(&GFX->Vertex[i],*GFXFileBuffer,sizeof(GFX->Vertex[i]));
        *GFXFileBuffer += sizeof(GFX->Vertex[i]);
        DPrintf(" -- VERTEX %i --\n",i);
        DPrintf("Position:(%i;%i;%i)\n",GFX->Vertex[i].x,GFX->Vertex[i].y,GFX->Vertex[i].z);
        DPrintf("Pad:%i\n",GFX->Vertex[i].Pad);
//         assert(GFX->Vertex[i].Pad == 123);
    }
}

void GFXReadNormalChunk(GFX_t *GFX,void **GFXFileBuffer)
{
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
    
    GFX->Normal = malloc(GFX->Header.NumNormals * sizeof(GFXVertex_t));
    
    for( i = 0; i < GFX->Header.NumNormals; i++ ) {
        memcpy(&GFX->Normal[i],*GFXFileBuffer,sizeof(GFX->Normal[i]));
        *GFXFileBuffer += sizeof(GFX->Normal[i]);
        DPrintf(" -- Normal %i --\n",i);
        DPrintf("Position:(%i;%i;%i)\n",GFX->Normal[i].x,GFX->Normal[i].y,GFX->Normal[i].z);
        DPrintf("Pad:%i\n",GFX->Normal[i].Pad);
//         assert(GFX->Normal[i].Pad == 123);
    }
}

void GFXReadFaceChunk(GFX_t *GFX,void **GFXFileBuffer)
{
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
    
//     GFXReadSpecialFaceChunk(GFX,InFile);
//     GFXSkipUnknownSection(8,InFile);
    
    assert(sizeof(GFXFace_t) == 36);
    GFX->Face = malloc((GFX->Header.NumFaces) * sizeof(GFXFace_t));
    
    for( i = 0; i < GFX->Header.NumFaces; i++ ) {
        DPrintf(" -- FACE %i --\n",i);
        memcpy(&GFX->Face[i],*GFXFileBuffer,sizeof(GFX->Face[i]));
        *GFXFileBuffer += sizeof(GFX->Face[i]);
        DPrintf("Texture Page:%i\n",GFX->Face[i].TextureInfo);
        DPrintf("U1V1:(%i;%i)\n",GFX->Face[i].U1, GFX->Face[i].V1);
        DPrintf("U2V2:(%i;%i)\n",GFX->Face[i].U2, GFX->Face[i].V2);
        DPrintf("U3V3:(%i;%i)\n",GFX->Face[i].U3, GFX->Face[i].V3);
        DPrintf("Vertex0:%i\n",GFX->Face[i].Vert0);
        DPrintf("Vertex0:%i\n",GFX->Face[i].Vert1);
        DPrintf("Vertex0:%i\n",GFX->Face[i].Vert2);
        DPrintf("Norm0:%i\n",GFX->Face[i].Norm0);
        DPrintf("Norm1:%i\n",GFX->Face[i].Norm1);
        DPrintf("Norm2:%i\n",GFX->Face[i].Norm2);
        DPrintf("Clut:%i\n",GFX->Face[i].Clut);
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

void GFXReadAnimationChunk(GFX_t *GFX,FILE *InFile)
{
    DPrintf("GFXReadAnimationChunk:Stub\n");
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
//     Clerkb seems to have 16121 vertices....
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
}

void GFXRender(GFX_t *GFX,VRAM_t *VRAM)
{
//     GL_Shader_t *Shader;
//     VAO_t *Iterator;
//     int MVPMatrixID;
//     int EnableLightingId;
//     int ColorMode;
//     int TexturePage;
//     
//     Shader = Shader_Cache("GFXShader","Shaders/GFXVertexShader.glsl","Shaders/GFXFragmentShader.glsl");
//     glUseProgram(Shader->ProgramID);
// 
//     MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
//     glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
//     EnableLightingId = glGetUniformLocation(Shader->ProgramID,"EnableLighting");
//     glUniform1i(EnableLightingId, LightEnable);
// 
// 
//     for( Iterator = GFX->VAOList; Iterator; Iterator = Iterator->Next ) {
// //         int VRamPage = Iterator->TSB & 0x1F;
//         ColorMode = (Iterator->TSB & 0xC0 ) >> 6;
//         TexturePage = Iterator->TSB  & 0x7F;
// //         if( ColorMode == 1) {
// //             glBindTexture(GL_TEXTURE_2D, VRam->Page8Bit[TexturePage].TextureID);
// //         } else {
// //             glBindTexture(GL_TEXTURE_2D, VRam->Page4Bit[TexturePage].TextureID);
// //         }
//         glBindVertexArray(Iterator->VaoID[0]);
//         glDrawArrays(GL_TRIANGLES, 0, 3);
//         glBindVertexArray(0);
//         glBindTexture(GL_TEXTURE_2D,0);
//     }
//     glUseProgram(0);

}

void GFXPrepareVAO(GFX_t *GFX)
{
    float Width = 256.f;
    float Height = 256.f;
    int i;
    float *VertexData;
    int VertexSize;
    int VertexPointer;
    int Stride;
    int VertexOffset;
    int TextureOffset;
    int NormalOffset;
    if( !GFX ) {
        DPrintf("GFXPrepareVAO: Invalid GFX File\n");
        return;
    }

    for( i = 0; i < GFX->Header.NumFaces; i++ ) {
        VAO_t *VAO;
        int Vert0 = GFX->Face[i].Vert0;
        int Vert1 = GFX->Face[i].Vert1;
        int Vert2 = GFX->Face[i].Vert2;
        int Norm0 = GFX->Face[i].Norm0;
        int Norm1 = GFX->Face[i].Norm1;
        int Norm2 = GFX->Face[i].Norm2;
        float U0 = (GFX->Face[i].U1)/Width;
        float V0 = (GFX->Face[i].V1) / Height;
        float U1 = (GFX->Face[i].U2) / Width;
        float V1 = (GFX->Face[i].V2) /Height;
        float U2 = (GFX->Face[i].U3) /Width;
        float V2 = (GFX->Face[i].V3) / Height;

            //    XYZ UV XYZ
        Stride = (3 + 2 + 3) * sizeof(float);
        
        VertexOffset = 0;
        TextureOffset = 3;
        NormalOffset = 5;
        
        VertexSize = Stride;
        VertexData = malloc(VertexSize * 3/** sizeof(float)*/);
        VertexPointer = 0;
                    
        VertexData[VertexPointer] =   GFX->Vertex[Vert0].x;
        VertexData[VertexPointer+1] = GFX->Vertex[Vert0].y;
        VertexData[VertexPointer+2] = GFX->Vertex[Vert0].z;
        VertexData[VertexPointer+3] = U0;
        VertexData[VertexPointer+4] = V0;
        VertexData[VertexPointer+5] = GFX->Face[i].RGB0.rgba[0] / 255.f;
        VertexData[VertexPointer+6] = GFX->Face[i].RGB0.rgba[1] / 255.f;
        VertexData[VertexPointer+7] = GFX->Face[i].RGB0.rgba[2] / 255.f;
        VertexPointer += 8;
            
        VertexData[VertexPointer] =   GFX->Vertex[Vert1].x;
        VertexData[VertexPointer+1] = GFX->Vertex[Vert1].y;
        VertexData[VertexPointer+2] = GFX->Vertex[Vert1].z;
        VertexData[VertexPointer+3] = U1;
        VertexData[VertexPointer+4] = V1;
        VertexData[VertexPointer+5] = GFX->Face[i].RGB1.rgba[0] / 255.f;
        VertexData[VertexPointer+6] = GFX->Face[i].RGB1.rgba[1] / 255.f;
        VertexData[VertexPointer+7] = GFX->Face[i].RGB1.rgba[2] / 255.f;
        VertexPointer += 8;
            
        VertexData[VertexPointer] =   GFX->Vertex[Vert2].x;
        VertexData[VertexPointer+1] = GFX->Vertex[Vert2].y;
        VertexData[VertexPointer+2] = GFX->Vertex[Vert2].z;
        VertexData[VertexPointer+3] = U2;
        VertexData[VertexPointer+4] = V2;
        VertexData[VertexPointer+5] = GFX->Face[i].RGB2.rgba[0] / 255.f;
        VertexData[VertexPointer+6] = GFX->Face[i].RGB2.rgba[1] / 255.f;
        VertexData[VertexPointer+7] = GFX->Face[i].RGB2.rgba[2] / 255.f;
        VertexPointer += 8;
            
//         VAO = VAOInitXYZUVXYZ(VertexData,VertexSize * 3,Stride,VertexOffset,TextureOffset,NormalOffset,GFX->Face[i].TextureInfo);
//         VAO->Next = GFX->VAOList;
//         GFX->VAOList = VAO;
        free(VertexData);
    }
}

GFX_t *GFXRead(void* GFXFileBuffer)
{
    GFX_t *GFXData;
    
    if( !GFXFileBuffer ) {
        DPrintf("GFXRead: Invalid buffer.\n");
        return NULL;
    }
    
    GFXData = malloc(sizeof(GFX_t));
    if( !GFXData ) {
        DPrintf("GFXRead: Failed to allocate memory for GFX model\n");
        return NULL;
    }
    GFXData->Vertex = NULL;
    GFXData->Normal = NULL;
    GFXData->Face = NULL;
    GFXData->Next = NULL;
    GFXData->VAOList = NULL;
    GFXReadHeaderChunk(GFXData,&GFXFileBuffer);
    GFXReadOffsetTableChunk(GFXData,&GFXFileBuffer);
    GFXFileBuffer += GFXData->Header.NumUnk4 * 4;
    GFXReadVertexChunk(GFXData,&GFXFileBuffer);
    GFXReadNormalChunk(GFXData,&GFXFileBuffer);
    GFXReadFaceChunk(GFXData,&GFXFileBuffer);
//     GFXFileBuffer += 316;
//     GFXReadAnimationChunk(GFXData,GFXFileBuffer);
    return GFXData;
}
