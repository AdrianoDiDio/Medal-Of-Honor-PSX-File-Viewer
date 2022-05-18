// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
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

#include "BSD.h"
#include "MOHLevelViewer.h"
#include "ShaderManager.h"

Color3b_t StarsColors[8] = {
    //R   G   B
    //128;128;128
    {128,128,128},
    //96;64;128
    {240,96,64},
    //32;240;64
    {128,128,32},
    //96;96;240
    {240,64,69},
    //255;255;255
    {96,96,240},
    //64;128;64
    {255,255,255},
    {255,128,64},
    {128,64,255}
};
// Color1i_t StarsColors[7] = {
//     //R   G   B
//     //128;128;128
//     {4034953344},
//     //96;64;128
//     {2155888736},
//     //32;240;64
//     {1077997600},
//     //96;96;240
//     {4293943392},
//     //255;255;255
//     {2164260863},
//     //64;128;64
//     {4282417216},
//     {0}
// };
/*
    1_1.BSD Compartment Trigger => 3246.604492;9.330523;-8456.515625
    673.832092;22.795897;-3504.162842
    
    FIXME:RenderObjectList,RenderObjectRealList,RenderObjectShowCaseList CLEANUP!
*/
void BSDDumpProperty(BSD_t *BSD,int PropertyIndex)
{
    int i;
    if( BSD == NULL ) {
        DPrintf("BSDDumpProperty:Invalid BSD File\n");
        return;
    }
    DPrintf("BSDDumpProperty:Property %i has %i nodes\n",PropertyIndex,BSD->PropertySetFile.Property[PropertyIndex].NumNodes);
    for( i = 0; i <  BSD->PropertySetFile.Property[PropertyIndex].NumNodes; i++ ) {
        DPrintf("BSDDumpProperty Property Node %i\n", BSD->PropertySetFile.Property[PropertyIndex].NodeList[i]);
//            assert(BSD->PropertySetFile.Property[i].Data[j] < 158);
    }
}

void BSDDumpFaceDataToObjFile(BSD_t *BSD,BSDRenderObjectDrawable_t *RenderObjectDrawable,mat4 ModelMatrix,VRAM_t *VRAM,FILE *OutFile)
{
    char Buffer[256];
    BSDRenderObjectElement_t *RenderObjectElement;
    vec3 VertPos;
    vec3 OutVector;
    Color1i_t Color;
    float TextureWidth;
    float TextureHeight;
    int i;
    
    RenderObjectElement = &BSD->RenderObjectTable.RenderObject[RenderObjectDrawable->RenderObjectIndex];
    TextureWidth = VRAM->Page.Width;
    TextureHeight = VRAM->Page.Height;
    
    for( i = RenderObjectElement->NumVertex - 1; i >= 0; i-- ) {
        VertPos[0] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[i].x;
        VertPos[1] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[i].y;
        VertPos[2] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[i].z;
        Color = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Color[i];
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"v %f %f %f %f %f %f\n",
                (OutVector[0] + RenderObjectDrawable->Position.x) / 4096.f,
                (OutVector[1] - RenderObjectDrawable->Position.y) / 4096.f,
                (OutVector[2] - RenderObjectDrawable->Position.z) / 4096.f,
                Color.rgba[0] / 255.f,Color.rgba[1] / 255.f,Color.rgba[2] / 255.f

        );
        fwrite(Buffer,strlen(Buffer),1,OutFile); 
    }
    for( i = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].NumFaces - 1; i >= 0 ; i-- ) {
        int VRAMPage = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].TexInfo & 0x1F;
        int ColorMode = (BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].TexInfo & 0xC0) >> 7;
        float U0 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV0.u + 
            VRAMGetTexturePageX(VRAMPage))/TextureWidth);
        float V0 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV0.v +
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        float U1 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV1.u + 
            VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        float V1 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV1.v + 
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) /TextureHeight);
        float U2 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV2.u + 
            VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        float V2 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV2.v + 
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        sprintf(Buffer,"vt %f %f\nvt %f %f\nvt %f %f\n",U0,V0,U1,V1,U2,V2);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
    }
    for( i = 0; i < BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].NumFaces; i++ ) {
        unsigned short Vert0;
        unsigned short Vert1;
        unsigned short Vert2;
        int BaseFaceUV;
        Vert0 = (BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].VData & 0xFF);
        Vert1 = (BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].VData & 0x3fc00) >> 10;
        Vert2 = (BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].VData & 0xFF00000 ) >> 20;

        sprintf(Buffer,"usemtl vram\n");
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        BaseFaceUV = i * 3;
        sprintf(Buffer,"f %i/%i %i/%i %i/%i\n",-(Vert0+1),-(BaseFaceUV+3),-(Vert1+1),-(BaseFaceUV+2),-(Vert2+1),-(BaseFaceUV+1));
        fwrite(Buffer,strlen(Buffer),1,OutFile);
    }
}

void BSDDumpFaceV2DataToObjFile(BSD_t *BSD,BSDRenderObjectDrawable_t *RenderObjectDrawable,mat4 ModelMatrix,VRAM_t *VRAM,FILE *OutFile)
{
    char Buffer[256];
    BSDRenderObjectElement_t *RenderObjectElement;
    vec3 VertPos;
    vec3 OutVector;
    Color1i_t Color;
    float TextureWidth;
    float TextureHeight;
    int i;
        
    RenderObjectElement = &BSD->RenderObjectTable.RenderObject[RenderObjectDrawable->RenderObjectIndex];
    TextureWidth = VRAM->Page.Width;
    TextureHeight = VRAM->Page.Height;
    
    for( i = RenderObjectElement->NumVertex - 1; i >= 0; i-- ) {
        VertPos[0] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[i].x;
        VertPos[1] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[i].y;
        VertPos[2] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[i].z;
        Color = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Color[i];
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"v %f %f %f %f %f %f\n",
                (OutVector[0] + RenderObjectDrawable->Position.x) / 4096.f,
                (OutVector[1] - RenderObjectDrawable->Position.y) / 4096.f,
                (OutVector[2] - RenderObjectDrawable->Position.z) / 4096.f,
                Color.rgba[0] / 255.f,Color.rgba[1] / 255.f,Color.rgba[2] / 255.f
        );
        fwrite(Buffer,strlen(Buffer),1,OutFile); 
    }
    for( i = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].NumFaces - 1; i >= 0 ; i-- ) {
        int VRAMPage = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].TexInfo;
        int ColorMode = (BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].TexInfo & 0xC0) >> 7;
        float U0 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV0.u + 
            VRAMGetTexturePageX(VRAMPage))/TextureWidth);
        float V0 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV0.v +
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        float U1 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV1.u + 
            VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        float V1 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV1.v + 
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) /TextureHeight);
        float U2 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV2.u + 
            VRAMGetTexturePageX(VRAMPage)) /TextureWidth);
        float V2 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV2.v + 
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        sprintf(Buffer,"vt %f %f\nvt %f %f\nvt %f %f\n",U0,V0,U1,V1,U2,V2);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
    }
    for( i = 0; i < BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].NumFaces; i++ ) {
        unsigned short Vert0;
        unsigned short Vert1;
        unsigned short Vert2;
        int BaseFaceUV;
        Vert0 = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].Vert0;
        Vert1 = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].Vert1;
        Vert2 = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].Vert2;

        sprintf(Buffer,"usemtl vram\n");
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        BaseFaceUV = i * 3;
        sprintf(Buffer,"f %i/%i %i/%i %i/%i\n",-(Vert0+1),-(BaseFaceUV+3),-(Vert1+1),-(BaseFaceUV+2),-(Vert2+1),-(BaseFaceUV+1));
        fwrite(Buffer,strlen(Buffer),1,OutFile);
    }
}

void BSDDumpDataToObjFile(BSD_t *BSD,VRAM_t *VRAM,int GameEngine,FILE *OutFile)
{
    char Buffer[256];
    BSDRenderObjectDrawable_t *RenderObjectIterator;
    BSDRenderObjectElement_t *RenderObjectElement;
    vec3 RotationAxis;
    mat4 RotationMatrix;
    mat4 ScaleMatrix;
    mat4 ModelMatrix;
    vec3 ScaleVector;

    
    if( !BSD || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        DPrintf("BSDDumpDataToFile: Invalid %s\n",InvalidFile ? "file" : "bsd struct");
        return;
    }
    
    if( !VRAM ) {
        DPrintf("BSDDumpDataToObjFile:Invalid VRAM data\n");
        return;
    }

    for( RenderObjectIterator = BSD->RenderObjectDrawableList; RenderObjectIterator; RenderObjectIterator = RenderObjectIterator->Next) {
        RenderObjectElement = &BSD->RenderObjectTable.RenderObject[RenderObjectIterator->RenderObjectIndex];
        sprintf(Buffer,"o BSD%i\n",RenderObjectElement->Id);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        glm_mat4_identity(RotationMatrix);
        
        RotationAxis[0] = 1;
        RotationAxis[1] = 0;
        RotationAxis[2] = 0;
        glm_rotate(RotationMatrix,glm_rad(180.f), RotationAxis);
        RotationAxis[0] = 0;
        RotationAxis[1] = 1;
        RotationAxis[2] = 0;
        glm_rotate(RotationMatrix,glm_rad(RenderObjectIterator->Rotation.y), RotationAxis);
        RotationAxis[0] = 1;
        RotationAxis[1] = 0;
        RotationAxis[2] = 0;
        glm_rotate(RotationMatrix,glm_rad(RenderObjectIterator->Rotation.x), RotationAxis);
        RotationAxis[0] = 0;
        RotationAxis[1] = 0;
        RotationAxis[2] = 1;
        glm_rotate(RotationMatrix,glm_rad(RenderObjectIterator->Rotation.z), RotationAxis);
        
        ScaleVector[0] = RenderObjectIterator->Scale.x;
        ScaleVector[1] = RenderObjectIterator->Scale.y;
        ScaleVector[2] = RenderObjectIterator->Scale.z;
        glm_scale_make(ScaleMatrix,ScaleVector);

        glm_mat4_mul(RotationMatrix,ScaleMatrix,ModelMatrix);
        
        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            BSDDumpFaceV2DataToObjFile(BSD,RenderObjectIterator,ModelMatrix,VRAM,OutFile);
        } else {
            BSDDumpFaceDataToObjFile(BSD,RenderObjectIterator,ModelMatrix,VRAM,OutFile);
        }
    }

}

void BSDDumpFaceDataToPlyFile(BSD_t *BSD,BSDRenderObjectDrawable_t *RenderObjectDrawable,mat4 ModelMatrix,VRAM_t *VRAM,FILE *OutFile)
{
    char Buffer[256];
    vec3 VertPos;
    vec3 OutVector;
    Color1i_t Color;
    float TextureWidth;
    float TextureHeight;
    int i;
        
    TextureWidth = VRAM->Page.Width;
    TextureHeight = VRAM->Page.Height;
    
    for( i = 0 ; i < BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].NumFaces; i++ ) {
        int VRAMPage = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].TexInfo;
        int ColorMode = (BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].TexInfo & 0xC0) >> 7;
        float U0 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV0.u + 
            VRAMGetTexturePageX(VRAMPage))/TextureWidth);
        float V0 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV0.v +
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        float U1 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV1.u + 
            VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        float V1 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV1.v + 
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) /TextureHeight);
        float U2 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV2.u + 
            VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        float V2 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].UV2.v + 
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        int Vert0 = (BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].VData & 0xFF);
        int Vert1 = (BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].VData & 0x3fc00) >> 10;
        int Vert2 = (BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Face[i].VData & 0xFF00000 ) >> 20;
        
        VertPos[0] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert0].x;
        VertPos[1] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert0].y;
        VertPos[2] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert0].z;
        Color = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Color[Vert0];
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",(OutVector[0] + RenderObjectDrawable->Position.x) / 4096.f, 
                (OutVector[1] - RenderObjectDrawable->Position.y) / 4096.f, (OutVector[2] - RenderObjectDrawable->Position.z) / 4096.f,
                Color.rgba[0] / 255.f,Color.rgba[1] / 255.f,Color.rgba[2] / 255.f,U0,V0);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        Color = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Color[Vert1];
        VertPos[0] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert1].x;
        VertPos[1] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert1].y;
        VertPos[2] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert1].z;
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",(OutVector[0] + RenderObjectDrawable->Position.x) / 4096.f, 
                (OutVector[1] - RenderObjectDrawable->Position.y) / 4096.f,(OutVector[2] - RenderObjectDrawable->Position.z) / 4096.f,
                Color.rgba[0] / 255.f,Color.rgba[1] / 255.f,Color.rgba[2] / 255.f,U1,V1);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        Color = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Color[Vert2];
        VertPos[0] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert2].x;
        VertPos[1] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert2].y;
        VertPos[2] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert2].z;
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",(OutVector[0] + RenderObjectDrawable->Position.x) / 4096.f, 
                (OutVector[1] - RenderObjectDrawable->Position.y) / 4096.f,(OutVector[2] - RenderObjectDrawable->Position.z) / 4096.f,
                Color.rgba[0] / 255.f,Color.rgba[1] / 255.f,Color.rgba[2] / 255.f,U2,V2);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
    }
}

void BSDDumpFaceV2DataToPlyFile(BSD_t *BSD,BSDRenderObjectDrawable_t *RenderObjectDrawable,mat4 ModelMatrix,VRAM_t *VRAM,FILE *OutFile)
{
    char Buffer[256];
    vec3 VertPos;
    vec3 OutVector;
    Color1i_t Color;
    float TextureWidth;
    float TextureHeight;
    int i;
        
    TextureWidth = VRAM->Page.Width;
    TextureHeight = VRAM->Page.Height;
    
    for( i = 0 ; i < BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].NumFaces; i++ ) {
        int VRAMPage = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].TexInfo;
        int ColorMode = (BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].TexInfo & 0xC0) >> 7;
        float U0 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV0.u + 
            VRAMGetTexturePageX(VRAMPage))/TextureWidth);
        float V0 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV0.v +
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        float U1 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV1.u + 
            VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        float V1 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV1.v + 
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) /TextureHeight);
        float U2 = (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV2.u + 
            VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        float V2 = /*255 -*/1.f - (((float)BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].UV2.v + 
                    VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        int Vert0 = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].Vert0;
        int Vert1 = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].Vert1;
        int Vert2 = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].FaceV2[i].Vert2;
        Color = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Color[Vert0];
        VertPos[0] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert0].x;
        VertPos[1] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert0].y;
        VertPos[2] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert0].z;
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",(OutVector[0] + RenderObjectDrawable->Position.x) / 4096.f, 
                (OutVector[1] - RenderObjectDrawable->Position.y) / 4096.f,(OutVector[2] - RenderObjectDrawable->Position.z) / 4096.f,
                Color.rgba[0] / 255.f,Color.rgba[1] / 255.f,Color.rgba[2] / 255.f,U0,V0);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        Color = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Color[Vert1];
        VertPos[0] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert1].x;
        VertPos[1] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert1].y;
        VertPos[2] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert1].z;
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",(OutVector[0] + RenderObjectDrawable->Position.x) / 4096.f, 
                (OutVector[1] - RenderObjectDrawable->Position.y) / 4096.f,(OutVector[2] - RenderObjectDrawable->Position.z) / 4096.f,
                Color.rgba[0] / 255.f,Color.rgba[1] / 255.f,Color.rgba[2] / 255.f,U1,V1);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        Color = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Color[Vert2];
        VertPos[0] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert2].x;
        VertPos[1] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert2].y;
        VertPos[2] = BSD->RenderObjectList[RenderObjectDrawable->RenderObjectIndex].Vertex[Vert2].z;
        glm_mat4_mulv3(ModelMatrix,VertPos,1.f,OutVector);
        sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",(OutVector[0] + RenderObjectDrawable->Position.x) / 4096.f, 
                (OutVector[1] - RenderObjectDrawable->Position.y) / 4096.f,(OutVector[2] - RenderObjectDrawable->Position.z) / 4096.f,
                Color.rgba[0] / 255.f,Color.rgba[1] / 255.f,Color.rgba[2] / 255.f,U2,V2);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
    }
}
void BSDDumpDataToPlyFile(BSD_t *BSD,VRAM_t *VRAM,int GameEngine,FILE *OutFile)
{
    char Buffer[256];
    BSDRenderObjectDrawable_t *RenderObjectIterator;
    vec3 RotationAxis;
    mat4 RotationMatrix;
    mat4 ScaleMatrix;
    mat4 ModelMatrix;
    vec3 ScaleVector;
    int FaceCount;
    int VertexOffset;
    int i;
    if( !BSD || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        DPrintf("BSDDumpDataToPlyFile: Invalid %s\n",InvalidFile ? "file" : "bsd struct");
        return;
    }
    if( !VRAM ) {
        DPrintf("BSDDumpDataToPlyFile:Invalid VRAM data\n");
        return;
    }
    sprintf(Buffer,"ply\nformat ascii 1.0\n");
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    FaceCount = 0;
    for( RenderObjectIterator = BSD->RenderObjectDrawableList; RenderObjectIterator; RenderObjectIterator = RenderObjectIterator->Next) {
        FaceCount += BSD->RenderObjectList[RenderObjectIterator->RenderObjectIndex].NumFaces;
    }
    
    sprintf(Buffer,
        "element vertex %i\nproperty float x\nproperty float y\nproperty float z\nproperty float red\nproperty float green\nproperty float blue\nproperty float s\nproperty float t\n",FaceCount * 3);
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    sprintf(Buffer,"element face %i\nproperty list uchar int vertex_indices\nend_header\n",FaceCount);
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    for( RenderObjectIterator = BSD->RenderObjectDrawableList; RenderObjectIterator; RenderObjectIterator = RenderObjectIterator->Next) {
        glm_mat4_identity(RotationMatrix);
        
        RotationAxis[0] = 1;
        RotationAxis[1] = 0;
        RotationAxis[2] = 0;
        glm_rotate(RotationMatrix,glm_rad(180.f), RotationAxis);
        RotationAxis[0] = 0;
        RotationAxis[1] = -1;
        RotationAxis[2] = 0;
        glm_rotate(RotationMatrix,glm_rad(RenderObjectIterator->Rotation.y), RotationAxis);
        RotationAxis[0] = 1;
        RotationAxis[1] = 0;
        RotationAxis[2] = 0;
        glm_rotate(RotationMatrix,glm_rad(RenderObjectIterator->Rotation.x), RotationAxis);
        RotationAxis[0] = 0;
        RotationAxis[1] = 0;
        RotationAxis[2] = 1;
        glm_rotate(RotationMatrix,glm_rad(RenderObjectIterator->Rotation.z), RotationAxis);
        
        ScaleVector[0] = RenderObjectIterator->Scale.x;
        ScaleVector[1] = RenderObjectIterator->Scale.y;
        ScaleVector[2] = RenderObjectIterator->Scale.z;
        glm_scale_make(ScaleMatrix,ScaleVector);

        glm_mat4_mul(RotationMatrix,ScaleMatrix,ModelMatrix);
        
        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            BSDDumpFaceV2DataToPlyFile(BSD,RenderObjectIterator,ModelMatrix,VRAM,OutFile);
        } else {
            BSDDumpFaceDataToPlyFile(BSD,RenderObjectIterator,ModelMatrix,VRAM,OutFile);
        }
    }
    VertexOffset = 0;
    for( RenderObjectIterator = BSD->RenderObjectDrawableList; RenderObjectIterator; RenderObjectIterator = RenderObjectIterator->Next) {
        for( i = 0; i < BSD->RenderObjectList[RenderObjectIterator->RenderObjectIndex].NumFaces; i++ ) {
            int Vert0 = VertexOffset + (i * 3) + 0;
            int Vert1 = VertexOffset + (i * 3) + 1;
            int Vert2 = VertexOffset + (i * 3) + 2;
            sprintf(Buffer,"3 %i %i %i\n",Vert0,Vert1,Vert2);
            fwrite(Buffer,strlen(Buffer),1,OutFile);
        }
        VertexOffset += BSD->RenderObjectList[RenderObjectIterator->RenderObjectIndex].NumFaces * 3;
    }
}

void BSDFixRenderObjectPosition(Level_t *Level)
{
    BSDRenderObjectDrawable_t *RenderObjectIterator;
    TSPVec3_t Point;
    int Result;
    int OutY;
    int Delta;
    int PropertySetIndex;
    
    for( RenderObjectIterator = Level->BSD->RenderObjectDrawableList; RenderObjectIterator; 
        RenderObjectIterator = RenderObjectIterator->Next ) {
        Point.x = RenderObjectIterator->Position.x;
        Point.y = RenderObjectIterator->Position.y;
        Point.z = RenderObjectIterator->Position.z;
        Result = TSPGetPointYComponentFromKDTree(Point,Level->TSPList,&PropertySetIndex,&OutY);
        if( Result == -1 ) {
            DPrintf("BSDFixRenderObjectPosition:Not found in KD Tree!\n");
            continue;
        }
        Delta = abs(Point.y - OutY);
        DPrintf("Delta is %i (Limit 55) PropertySetIndex:%i\n",Delta,PropertySetIndex);
        BSDDumpProperty(Level->BSD,PropertySetIndex);
        if( Delta < 55 ) {
            RenderObjectIterator->Position.y = OutY;
        }
    }
}

void BSDRenderObjectListCleanUp(BSD_t *BSD)
{
    int i;
    
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectList[i].Face != NULL ) {
            free(BSD->RenderObjectList[i].Face);
        }
        if( BSD->RenderObjectList[i].FaceV2 != NULL ) {
            free(BSD->RenderObjectList[i].FaceV2);
        }
        if( BSD->RenderObjectList[i].Vertex != NULL ) {
            free(BSD->RenderObjectList[i].Vertex);
        }
        if( BSD->RenderObjectList[i].Color != NULL ) {
            free(BSD->RenderObjectList[i].Color);
        }
        if( BSD->RenderObjectList[i].VAO != NULL ) {
            VAOFree(BSD->RenderObjectList[i].VAO);
        }
    }
    free(BSD->RenderObjectList);
}

void BSDFree(BSD_t *BSD)
{
    BSDTSPStreamNode_t *Temp;
    BSDRenderObjectDrawable_t *Drawable;
    BSDAnimatedLight_t *AnimatedLight;
    int i;
    
    for( i = 0; i < BSD_ANIMATED_LIGHTS_TABLE_SIZE; i++ ) {
        AnimatedLight = &BSD->AnimatedLightsTable.AnimatedLightsList[i];
        if( AnimatedLight->NumColors == 0 ) {
            continue;
        }
        free(AnimatedLight->ColorList);
    }
    free(BSD->NodeData.Table);
    free(BSD->NodeData.Node);
    
    for( i = 0; i < BSD->PropertySetFile.NumProperties; i++ ) {
        free(BSD->PropertySetFile.Property[i].NodeList);
    }
    free(BSD->PropertySetFile.Property);
    free(BSD->RenderObjectTable.RenderObject);
    BSDRenderObjectListCleanUp(BSD);

    VAOFree(BSD->NodeVAO);
    VAOFree(BSD->RenderObjectPointVAO);
    
    if( BSDIsMoonEnabled(BSD) ) {
        VAOFree(BSD->SkyData.MoonVAO);
    }
    
    if( BSDAreStarsEnabled(BSD) ) {
        VAOFree(BSD->SkyData.StarsVAO);
    }
    
    while( BSD->RenderObjectDrawableList ) {
        Drawable = BSD->RenderObjectDrawableList;
        BSD->RenderObjectDrawableList = BSD->RenderObjectDrawableList->Next;
        free(Drawable);
    }
    
    while( BSD->TSPStreamNodeList ) {
        Temp = BSD->TSPStreamNodeList;
        BSD->TSPStreamNodeList = BSD->TSPStreamNodeList->Next;
        free(Temp);
    }
    
    free(BSD);
}

int BSDIsMoonEnabled(BSD_t *BSD)
{
    return BSD->SkyData.MoonZ != 0;
}

int BSDAreStarsEnabled(BSD_t *BSD)
{
    return BSD->SkyData.StarRadius != 0;
}

void BSDUpdateAnimatedLights(BSD_t *BSD)
{
    BSDAnimatedLight_t *AnimatedLight;
    int i;
    for( i = 0; i < BSD->AnimatedLightsTable.NumAnimatedLights; i++ ) {
        AnimatedLight = &BSD->AnimatedLightsTable.AnimatedLightsList[i];
        if( !AnimatedLight->NumColors ) {
            continue;
        }
        AnimatedLight->Delay--;
        if( AnimatedLight->Delay <= 0 ) {
            AnimatedLight->ColorIndex++;
            if( AnimatedLight->ColorIndex >= AnimatedLight->NumColors ) {
                AnimatedLight->ColorIndex = 0;
            }
            //PSX runs at 30FPS...increment the delay in order to simulate that speed...
            AnimatedLight->Delay = AnimatedLight->ColorList[AnimatedLight->ColorIndex].rgba[3] * 6;
            AnimatedLight->CurrentColor = AnimatedLight->ColorList[AnimatedLight->ColorIndex].c;
        }
    }
}

void BSDUpdateStarsColors(BSD_t *BSD)
{
    float Data[3];
    float DataSize;
    int BaseOffset;
    int Stride;
    int Random;
    int i;
    Color1i_t StarColor;
    
    DataSize = 3 * sizeof(float);
    Stride = (3 + 3) * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, BSD->SkyData.StarsVAO->VBOId[0]);
    
    for( i = 0; i < BSD_SKY_MAX_STARS_NUMBER; i++ ) {
        Random = rand();
        BaseOffset = (i * Stride);
        StarColor = BSD->SkyData.StarsColors[i];
        Data[0] = StarColor.rgba[0];
        Data[1] = StarColor.rgba[1];
        Data[2] = StarColor.rgba[2];
        if( (Random & 3 ) != 0 ) {
            if( StarColor.rgba[0] >= 33 ) {
                Data[0] -= 32;
            }
            if( StarColor.rgba[1] >= 33 ) {
                Data[1] -= 32;
            }
            if( StarColor.rgba[2] >= 33 ) {
                Data[2] -= 32;
            }
        }
        Data[0] /= 255.f;
        Data[1] /= 255.f;
        Data[2] /= 255.f;
        glBufferSubData(GL_ARRAY_BUFFER, BaseOffset + (3*sizeof(float)), DataSize, &Data);
    }
     glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int BSDGetCurrentAnimatedLightColorByIndex(BSD_t *BSD,int Index)
{
    if( Index < 0 || Index >= BSD_ANIMATED_LIGHTS_TABLE_SIZE ) {
        DPrintf("BSDGetCurrentAnimatedLightColorByIndex:Invalid index %i\n",Index);
        return 0;
    }
    return BSD->AnimatedLightsTable.AnimatedLightsList[Index].CurrentColor;
}

void BSDCreatePointListVAO(BSD_t *BSD)
{
    float *NodeData;
    int    NodeDataPointer;
    int    Stride;
    int    NodeDataSize;
    int    i;
    
    Stride = (3 + 3) * sizeof(float);
    NodeDataSize = Stride * BSD->NodeData.Header.NumNodes;
    NodeData = malloc(NodeDataSize);
    NodeDataPointer = 0;
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        NodeData[NodeDataPointer] =   BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer+1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer+2] = BSD->NodeData.Node[i].Position.z;

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
    BSD->NodeVAO = VAOInitXYZRGB(NodeData,NodeDataSize,Stride,0,3,0);            
    free(NodeData);
}
void BSDCreateRenderObjectPointListVAO(BSD_t *BSD)
{
    BSDRenderObjectDrawable_t *Iterator;
    BSDRenderObjectElement_t RenderObjectElement;
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
    for( Iterator = BSD->RenderObjectDrawableList; Iterator; Iterator = Iterator->Next ) {
        RenderObjectData[RenderObjectDataPointer] =   Iterator->Position.x;
        RenderObjectData[RenderObjectDataPointer+1] = Iterator->Position.y;
        RenderObjectData[RenderObjectDataPointer+2] = Iterator->Position.z;
        RenderObjectElement = BSD->RenderObjectTable.RenderObject[Iterator->RenderObjectIndex];
        if( RenderObjectElement.Type == BSD_RENDER_OBJECT_ENEMY ) {
            // BLUE
            RenderObjectData[RenderObjectDataPointer+3] = 0.f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        } else if (RenderObjectElement.Type == BSD_RENDER_OBJECT_DOOR ) {
            // GREEN
            RenderObjectData[RenderObjectDataPointer+3] = 0.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.f;
        } else if( RenderObjectElement.Type == BSD_RENDER_OBJECT_MG42 ) {
            // Yellow
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.f;
        } else if( RenderObjectElement.Type == BSD_RENDER_OBJECT_PLANE ) {
            // Fuchsia -- Plane.
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        } else if ( RenderObjectElement.Type == BSD_RENDER_OBJECT_UNKNOWN1 ) {
            // Maroon
            RenderObjectData[RenderObjectDataPointer+3] = 0.5f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.f;
        } else if( RenderObjectElement.Type == BSD_RENDER_OBJECT_DESTRUCTIBLE_WINDOW ) {
            // saddlebrown -- 
            RenderObjectData[RenderObjectDataPointer+3] = 0.54f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.27f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.07f;
        } else if( RenderObjectElement.Type == BSD_RENDER_OBJECT_VALVE ) {
            // White -- 
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        } else if( RenderObjectElement.Type == BSD_RENDER_OBJECT_EXPLOSIVE_CHARGE ) {
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
    BSD->RenderObjectPointVAO = VAOInitXYZRGB(RenderObjectData,RenderObjectDataSize - (Stride * NumSkip),Stride,0,3,0);            
    free(RenderObjectData);
}

void BSDAddNodeToRenderObjecDrawabletList(BSD_t *BSD,int IsMultiplayer,unsigned int NodeId,Vec3_t Position,Vec3_t Rotation)
{
    BSDRenderObjectDrawable_t *Object;
    unsigned int RenderObjectId;
    int RenderObjectIndex;

    if( IsMultiplayer ) {
        RenderObjectId = BSDMPNodeIdToRenderObjectId(NodeId);
    } else {
        RenderObjectId = BSDNodeIdToRenderObjectId(NodeId);
    }

    RenderObjectIndex = BSDGetRenderObjectIndexById(BSD,RenderObjectId);

    if( RenderObjectIndex == -1 ) {
        DPrintf("Failed adding new object...Id %u doesn't match any.\n",NodeId);
        return;
    }
    DPrintf("RenderObjectId %u for node %u Index %i\n",RenderObjectId,NodeId,RenderObjectIndex);

    Object = malloc(sizeof(BSDRenderObjectDrawable_t));
    Object->RenderObjectIndex = RenderObjectIndex;
    Object->Position = Position;
    //PSX GTE Uses 4096 as unit value only when dealing with fixed math operation.
    //When dealing with rotation then 4096 = 360 degrees.
    //We need to map it back to OpenGL standard format [0;360]. 
    Object->Rotation.x = (Rotation.x  / 4096) * 360.f;
    Object->Rotation.y = (Rotation.y  / 4096) * 360.f;
    Object->Rotation.z = (Rotation.z  / 4096) * 360.f;
    
    Object->Scale.x = (float) (BSD->RenderObjectTable.RenderObject[RenderObjectIndex].ScaleX  / 16 ) / 4096.f;
    Object->Scale.y = (float) (BSD->RenderObjectTable.RenderObject[RenderObjectIndex].ScaleY  / 16 ) / 4096.f;
    Object->Scale.z = (float) (BSD->RenderObjectTable.RenderObject[RenderObjectIndex].ScaleZ  / 16 ) / 4096.f;
    
    Object->Next = BSD->RenderObjectDrawableList;
    BSD->RenderObjectDrawableList = Object;
    BSD->NumRenderObjectPoint++;
}
void BSDCreateFaceVAO(BSDRenderObject_t *RenderObjectData,VRAM_t *VRAM)
{
    float TextureWidth;
    float TextureHeight;
    unsigned short Vert0;
    unsigned short Vert1;
    unsigned short Vert2;
    float *VertexData;
    int VertexSize;
    int VertexPointer;
    int Stride;
    int VertexOffset;
    int TextureOffset;
    int ColorOffset;
    VAO_t *VAO;
    int i;
    
    TextureWidth = VRAM->Page.Width;
    TextureHeight = VRAM->Page.Height;
//            XYZ UV RGB
    Stride = (3 + 2 + 3) * sizeof(float);
    VertexSize = Stride * 3 * RenderObjectData->NumFaces;
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    VertexOffset = 0;
    TextureOffset = 3;
    ColorOffset = 5;
    for( i = 0; i < RenderObjectData->NumFaces; i++ ) {

        Vert0 = (RenderObjectData->Face[i].VData & 0xFF);
        Vert1 = (RenderObjectData->Face[i].VData & 0x3fc00) >> 10;
        Vert2 = (RenderObjectData->Face[i].VData & 0xFF00000 ) >> 20;

        int VRAMPage = RenderObjectData->Face[i].TexInfo & 0x1F;
        int ColorMode = (RenderObjectData->Face[i].TexInfo & 0xC0) >> 7;
        float U0 = (((float)RenderObjectData->Face[i].UV0.u + VRAMGetTexturePageX(VRAMPage))/TextureWidth);
        float V0 = /*255 -*/(((float)RenderObjectData->Face[i].UV0.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        float U1 = (((float)RenderObjectData->Face[i].UV1.u + VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        float V1 = /*255 -*/(((float)RenderObjectData->Face[i].UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        float U2 = (((float)RenderObjectData->Face[i].UV2.u + VRAMGetTexturePageX(VRAMPage)) /TextureWidth);
        float V2 = /*255 -*/(((float)RenderObjectData->Face[i].UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);

                        
        VertexData[VertexPointer] =   RenderObjectData->Vertex[Vert0].x;
        VertexData[VertexPointer+1] = RenderObjectData->Vertex[Vert0].y;
        VertexData[VertexPointer+2] = RenderObjectData->Vertex[Vert0].z;
        VertexData[VertexPointer+3] = U0;
        VertexData[VertexPointer+4] = V0;
        VertexData[VertexPointer+5] = RenderObjectData->Color[Vert0].rgba[0] / 255.f;
        VertexData[VertexPointer+6] = RenderObjectData->Color[Vert0].rgba[1] / 255.f;
        VertexData[VertexPointer+7] = RenderObjectData->Color[Vert0].rgba[2] / 255.f;
        VertexPointer += 8;
                
        VertexData[VertexPointer] =   RenderObjectData->Vertex[Vert1].x;
        VertexData[VertexPointer+1] = RenderObjectData->Vertex[Vert1].y;
        VertexData[VertexPointer+2] = RenderObjectData->Vertex[Vert1].z;
        VertexData[VertexPointer+3] = U1;
        VertexData[VertexPointer+4] = V1;
        VertexData[VertexPointer+5] = RenderObjectData->Color[Vert1].rgba[0] / 255.f;
        VertexData[VertexPointer+6] = RenderObjectData->Color[Vert1].rgba[1] / 255.f;
        VertexData[VertexPointer+7] = RenderObjectData->Color[Vert1].rgba[2] / 255.f;
        VertexPointer += 8;
                
        VertexData[VertexPointer] =   RenderObjectData->Vertex[Vert2].x;
        VertexData[VertexPointer+1] = RenderObjectData->Vertex[Vert2].y;
        VertexData[VertexPointer+2] = RenderObjectData->Vertex[Vert2].z;
        VertexData[VertexPointer+3] = U2;
        VertexData[VertexPointer+4] = V2;
        VertexData[VertexPointer+5] = RenderObjectData->Color[Vert2].rgba[0] / 255.f;
        VertexData[VertexPointer+6] = RenderObjectData->Color[Vert2].rgba[1] / 255.f;
        VertexData[VertexPointer+7] = RenderObjectData->Color[Vert2].rgba[2] / 255.f;
        VertexPointer += 8;
    }
    VAO = VAOInitXYZUVRGB(VertexData,VertexSize,Stride,VertexOffset,TextureOffset,ColorOffset,-1,-1,
                        RenderObjectData->NumFaces * 3);
    VAO->Next = RenderObjectData->VAO;
    RenderObjectData->VAO = VAO;
    free(VertexData);
}

void BSDCreateFaceV2VAO(BSDRenderObject_t *RenderObjectData,VRAM_t *VRAM)
{
    float TextureWidth;
    float TextureHeight;
    unsigned short Vert0;
    unsigned short Vert1;
    unsigned short Vert2;
    float *VertexData;
    int VertexSize;
    int VertexPointer;
    int Stride;
    int VertexOffset;
    int TextureOffset;
    int ColorOffset;
    VAO_t *VAO;
    int i;
    
    TextureWidth = VRAM->Page.Width;
    TextureHeight = VRAM->Page.Height;
//            XYZ UV RGB
    Stride = (3 + 2 + 3) * sizeof(float);
    VertexSize = Stride * 3 * RenderObjectData->NumFaces;
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    VertexOffset = 0;
    TextureOffset = 3;
    ColorOffset = 5;
    for( i = 0; i < RenderObjectData->NumFaces; i++ ) {

        Vert0 = RenderObjectData->FaceV2[i].Vert0;
        Vert1 = RenderObjectData->FaceV2[i].Vert1;
        Vert2 = RenderObjectData->FaceV2[i].Vert2;

        int VRAMPage = RenderObjectData->FaceV2[i].TexInfo;
        int ColorMode = (RenderObjectData->FaceV2[i].TexInfo & 0xC0) >> 7;
        float U0 = (((float)RenderObjectData->FaceV2[i].UV0.u + VRAMGetTexturePageX(VRAMPage))/TextureWidth);
        float V0 = /*255 -*/(((float)RenderObjectData->FaceV2[i].UV0.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        float U1 = (((float)RenderObjectData->FaceV2[i].UV1.u + VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        float V1 = /*255 -*/(((float)RenderObjectData->FaceV2[i].UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        float U2 = (((float)RenderObjectData->FaceV2[i].UV2.u + VRAMGetTexturePageX(VRAMPage)) /TextureWidth);
        float V2 = /*255 -*/(((float)RenderObjectData->FaceV2[i].UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);

                        
        VertexData[VertexPointer] =   RenderObjectData->Vertex[Vert0].x;
        VertexData[VertexPointer+1] = RenderObjectData->Vertex[Vert0].y;
        VertexData[VertexPointer+2] = RenderObjectData->Vertex[Vert0].z;
        VertexData[VertexPointer+3] = U0;
        VertexData[VertexPointer+4] = V0;
        VertexData[VertexPointer+5] = RenderObjectData->Color[Vert0].rgba[0] / 255.f;
        VertexData[VertexPointer+6] = RenderObjectData->Color[Vert0].rgba[1] / 255.f;
        VertexData[VertexPointer+7] = RenderObjectData->Color[Vert0].rgba[2] / 255.f;
        VertexPointer += 8;
                
        VertexData[VertexPointer] =   RenderObjectData->Vertex[Vert1].x;
        VertexData[VertexPointer+1] = RenderObjectData->Vertex[Vert1].y;
        VertexData[VertexPointer+2] = RenderObjectData->Vertex[Vert1].z;
        VertexData[VertexPointer+3] = U1;
        VertexData[VertexPointer+4] = V1;
        VertexData[VertexPointer+5] = RenderObjectData->Color[Vert1].rgba[0] / 255.f;
        VertexData[VertexPointer+6] = RenderObjectData->Color[Vert1].rgba[1] / 255.f;
        VertexData[VertexPointer+7] = RenderObjectData->Color[Vert1].rgba[2] / 255.f;
        VertexPointer += 8;
                
        VertexData[VertexPointer] =   RenderObjectData->Vertex[Vert2].x;
        VertexData[VertexPointer+1] = RenderObjectData->Vertex[Vert2].y;
        VertexData[VertexPointer+2] = RenderObjectData->Vertex[Vert2].z;
        VertexData[VertexPointer+3] = U2;
        VertexData[VertexPointer+4] = V2;
        VertexData[VertexPointer+5] = RenderObjectData->Color[Vert2].rgba[0] / 255.f;
        VertexData[VertexPointer+6] = RenderObjectData->Color[Vert2].rgba[1] / 255.f;
        VertexData[VertexPointer+7] = RenderObjectData->Color[Vert2].rgba[2] / 255.f;
        VertexPointer += 8;
    }
    VAO = VAOInitXYZUVRGB(VertexData,VertexSize,Stride,VertexOffset,TextureOffset,ColorOffset,-1,-1,
                        RenderObjectData->NumFaces * 3);
    VAO->Next = RenderObjectData->VAO;
    RenderObjectData->VAO = VAO;
    free(VertexData);
}

void BSDCreateMoonVAO(BSD_t *BSD,VRAM_t *VRAM)
{
    float x;
    float y;
    float z;
    float w;
    float h;
    float u0;
    float v0;
    float TexWidth;
    float TexHeight;
    float ImageWidth;
    float ImageHeight;
    float *VertexData;
    int Stride;
    int DataSize;
    int VertexPointer;
    Vec3_t MoonPosition;
    
    
    if( !BSDIsMoonEnabled(BSD) ) {
        DPrintf("Moon is not enabled...\n");
        return;
    }
    
    //        XYZ  UV
    Stride = (3 + 2) * sizeof(float);
    DataSize = Stride * 6;
    
    ImageWidth = VRAM->Page.Width;
    ImageHeight = VRAM->Page.Height;

    u0 = ((float)BSD_MOON_TEXTURE_X + VRAMGetTexturePageX(BSD_MOON_VRAM_PAGE)) / ImageWidth;
    //Color Mode 0 => 4 BPP texture
    v0 = ((float)BSD_MOON_TEXTURE_Y + VRAMGetTexturePageY(BSD_MOON_VRAM_PAGE,0)) / ImageHeight;
    TexWidth = ((float)BSD_MOON_WIDTH) / ImageWidth;
    TexHeight = ((float)BSD_MOON_HEIGHT) / ImageHeight;
    
    VertexData = malloc(DataSize);
    VertexPointer = 0;
    MoonPosition = Vec3Build((BSD->SkyData.MoonZ * 32) / 200,BSD->SkyData.MoonY,BSD->SkyData.MoonZ);
    Vec3RotateXAxis(DEGTORAD(180.f),&MoonPosition);
    x = MoonPosition.x;
    y = MoonPosition.y;
    z = MoonPosition.z;
    
    w = BSD_MOON_WIDTH * (BSD->SkyData.StarRadius * 4);
    h = BSD_MOON_HEIGHT * (BSD->SkyData.StarRadius * 4);

    VertexData[VertexPointer] =  x;
    VertexData[VertexPointer+1] = y + h;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0;
    VertexData[VertexPointer+4] = v0 + TexHeight;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x;
    VertexData[VertexPointer+1] = y;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0;
    VertexData[VertexPointer+4] = v0;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x + w;
    VertexData[VertexPointer+1] = y + h;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0 + TexWidth;
    VertexData[VertexPointer+4] = v0 + TexHeight;
    VertexPointer += 5;
            

    VertexData[VertexPointer] =  x + w;
    VertexData[VertexPointer+1] = y + h;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0 + TexWidth;
    VertexData[VertexPointer+4] = v0 + TexHeight;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x;
    VertexData[VertexPointer+1] = y;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0;
    VertexData[VertexPointer+4] = v0;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x + w;
    VertexData[VertexPointer+1] = y;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0 + TexWidth;
    VertexData[VertexPointer+4] = v0;
    VertexPointer += 5;        
    BSD->SkyData.MoonVAO = VAOInitXYZUV(VertexData,DataSize,Stride,0,3,-1,-1,6);
    free(VertexData);
}

void BSDCreateStarsVAO(BSD_t *BSD)
{
    float *VertexData;
    float R;
    float Phi;
    float Theta;
    int VertexPointer;
    int Stride;
    int VertexSize;
    int i;
    Color3b_t RandColor;

    if( !BSDAreStarsEnabled(BSD) ) {
        DPrintf("Stars are not enabled...\n");
        return;
    }
    //        XYZ RGB
    Stride = (3 + 3) * sizeof(float);
    VertexSize = Stride * BSD_SKY_MAX_STARS_NUMBER;
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    
    for( i = 0; i < BSD_SKY_MAX_STARS_NUMBER; i++ ) {
        R = (BSD->SkyData.StarRadius*256) * sqrt(Rand01());
        Theta = Rand01() * 2 * M_PI;
        Phi = acos(2.0 * Rand01() - 1.0);/*BSDRand01() * M_PI;*/
        RandColor = StarsColors[RandRangeI(0,7)];
        BSD->SkyData.StarsColors[i].rgba[0] = RandColor.r;
        BSD->SkyData.StarsColors[i].rgba[1] = RandColor.g;
        BSD->SkyData.StarsColors[i].rgba[2] = RandColor.b;

        VertexData[VertexPointer] =  (R * sin(Phi) * cos(Theta) );
        VertexData[VertexPointer+1] = (R * sin(Theta) * sin(Phi) ) - (BSD->SkyData.StarRadius*264);
        VertexData[VertexPointer+2] = R * cos(Phi);
        VertexData[VertexPointer+3] = RandColor.r / 255.f;
        VertexData[VertexPointer+4] = RandColor.g / 255.f;
        VertexData[VertexPointer+5] = RandColor.b / 255.f;
        VertexPointer += 6;
    }
    BSD->SkyData.StarsVAO = VAOInitXYZRGB(VertexData,VertexSize,Stride,0,3,1);
    free(VertexData);
}

void BSDCreateSkyVAOs(BSD_t *BSD,VRAM_t *VRAM)
{
    BSDCreateMoonVAO(BSD,VRAM);
    BSDCreateStarsVAO(BSD);
}

void BSDCreateVAOs(BSD_t *BSD,int GameEngine,VRAM_t *VRAM)
{
    BSDRenderObject_t *RenderObjectData;
    int i;

  
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        RenderObjectData = &BSD->RenderObjectList[i];

        if( RenderObjectData->NumFaces == 0 ) {
            DPrintf("Failed setting vao...Invalid NumFace %i\n",RenderObjectData->NumFaces);
            continue;
        }

        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            BSDCreateFaceV2VAO(RenderObjectData,VRAM);
        } else {
            BSDCreateFaceVAO(RenderObjectData,VRAM);
        }
    }
    BSDCreatePointListVAO(BSD);
    BSDCreateRenderObjectPointListVAO(BSD);
    BSDCreateSkyVAOs(BSD,VRAM);
}



Vec3_t BSDGetPlayerSpawn(BSD_t *BSD,Vec3_t *Rotation)
{
    Vec3_t PlayerSpawn;
    int i;
    
    PlayerSpawn = Vec3Build(0,0,0);
    
    if( !BSD ) {
        DPrintf("BSDGetPlayerSpawn:Invalid BSD\n");
        return PlayerSpawn;
    }
    
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        if( BSD->NodeData.Node[i].Id != BSD_PLAYER_SPAWN ) {
            continue;
        }
        PlayerSpawn = Vec3Build(BSD->NodeData.Node[i].Position.x,BSD->NodeData.Node[i].Position.y,BSD->NodeData.Node[i].Position.z);
        if( Rotation ) {
            *Rotation = Vec3Build(BSD->NodeData.Node[i].Rotation.x,BSD->NodeData.Node[i].Rotation.y,BSD->NodeData.Node[i].Rotation.z);
        }
        break;
    }
    Vec3RotateXAxis(DEGTORAD(180.f),&PlayerSpawn);
    return PlayerSpawn;
}
int BSDGetRenderObjectIndexById(BSD_t *BSD,int Id)
{
    int i;
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObject[i].Id == Id ) {
            return i;
        }
    }
    return -1;
}

BSDRenderObject_t *BSDGetRenderObjectById(BSD_t *BSD,int Id)
{
    int i;
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectTable.RenderObject[i].Id == Id ) {
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

unsigned int BSDNodeIdToRenderObjectId(unsigned int NodeId)
{
    unsigned int RenderObjectId;
// TODO:Look at the assembly to find any other RenderBlockId/NodeRenderBlockId.
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
    switch ( NodeId ) {
        case 1821346472:
            RenderObjectId = 519275822;
            break;
        case 935865828:
            RenderObjectId = 2278882431;
            break;
        case 149114796:
            RenderObjectId = 1686008476;
            break;
        case 4262665218:
            RenderObjectId = 1771441436;
            break;
        case 2622993903:
            RenderObjectId = 1771441436;
            break;
        case 3832727321:
            RenderObjectId = 1335595487;
            break;
        case 3815705185:
        case 1444155995:
            RenderObjectId = 137063914;
            break;
        case 1730263601:
            RenderObjectId = 1771441436;
            break;
        case 3363099888:
            RenderObjectId = 1771441436;
            break;
        case 648959524:
            RenderObjectId = 1771441436;
            break;
        case 427407355:
            RenderObjectId = 1771441436;
            break;
        case 2966651809:
            RenderObjectId = 1771441436;
            break;
        case 1088149327:
            RenderObjectId = 1771441436;
            break;
        case 1696717421:
            RenderObjectId = 1771441436;
            break;
        case 633670612:
            RenderObjectId = 3487282118;
            break;
        case 2470021088:
            RenderObjectId = 1909080654;
            break;
        case 2522430033:
            RenderObjectId = 2018764808;
            break;
        case 1247503060:
        case 3545379694:
            RenderObjectId = 4294967295;
            break;
        case 0:
            RenderObjectId = 0;
            break;
        default:
            DPrintf("RenderObjectId was not in the mapping table...assuming it is the NodeId itself.\n");
            RenderObjectId = NodeId;
            break;
    }
    return RenderObjectId;
}

unsigned int BSDMPNodeIdToRenderObjectId(unsigned int NodeId)
{
    unsigned int RenderObjectId;
    switch( NodeId ) {
        //NEW
        case 3815705185:
            //Bazooka Ammo.
            RenderObjectId = 3139577012;
            break;
        case 3832727321:
            //SMG Ammo.
            RenderObjectId = 3158305228;
            break;
        case 2622993903:
            //Pistol Ammo.
            RenderObjectId = 523223373;
            break;
        case 1730263601:
            //Grenade Ammo.
            RenderObjectId = 1064080612;
            break;
        case 3363099888:
            //Shotgun Ammo.
            RenderObjectId = 1322820526;
            break;
        case 648959524:
            //Rifle Ammo.
            RenderObjectId = 414830070;
            break;
        case 1444155995:
            //Silencer Ammo.
            RenderObjectId = 1479327691;
            break;
        case 1088149327:
            //German Grenade Ammo.
            RenderObjectId = 1529087242;
            break;
        case 1696717421:
            //BAR Ammo.
            RenderObjectId = 1857331800;
            break;
        case 427407355:
            //Salute Ammo.
            RenderObjectId = 1008432070;
            break;
        //Undefined but found....
        case 1656540076:
            RenderObjectId = 1771441436;
            break;
        case 475277581:
            RenderObjectId = 1771441436;
            break;
        case 1587751891:
            RenderObjectId = 1771441436;
            break;
        case 2044083958:
            RenderObjectId = 1771441436;
            break;
        case 1271113291:
            RenderObjectId = 1771441436;
            break;
        case 2774162518:
            RenderObjectId = 1771441436;
            break;
        case 586892869:
            RenderObjectId = 1771441436;
            break;
        case 807017850:
            //HEALTH
            RenderObjectId = 2278882431;
            break;
        case 254902066:
            RenderObjectId = 1686008476;
            break;
        case 1801304630:
            RenderObjectId = 519275822;
            break;
        case 3482129947:
            RenderObjectId = 1771441436;
            break;
        case 3538476007:
            RenderObjectId = 1771441436;
            break;
        case 3364936547:
            RenderObjectId = 1771441436;
            break;
        default:
            RenderObjectId = NodeId;
            break;
    }
    return RenderObjectId;
}

bool BSDIsRenderObjectPresent(BSD_t *BSD,unsigned int RenderObjectId) {
    if( BSDGetRenderObjectIndexById(BSD,RenderObjectId) == -1 ) {
        DPrintf("Render Object Id %u not found..\n",RenderObjectId);
        return false;
    }
    return true;
}
char *BSDNodeGetEnumStringFromNodeId(unsigned int NodeId)
{
    switch( NodeId ) {
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

char *BSDRenderObjectGetWeaponNameFromId(int RenderObjectId)
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
            //Should never happens!
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

char *BSDGetCollisionVolumeStringFromType(int CollisionVolumeType)
{
    switch( CollisionVolumeType ) {
        case BSD_COLLISION_VOLUME_TYPE_SPHERE:
            return "Sphere";
        case BSD_COLLISION_VOLUME_TYPE_CYLINDER:
            return "Cylinder";
        case BSD_COLLISION_VOLUME_TYPE_BOX:
        default:
            return "Box";
    }
}

bool BSDPointInSphere(Vec3_t Point,BSDPosition_t Center,float Radius)
{
    Vec3_t Node;
    float DeltaX;
    float DeltaY;
    float DeltaZ;
  
    Node.x = Center.x;
    Node.y = Center.y;
    Node.z = Center.z;
    Vec3RotateXAxis(DEGTORAD(180.f),&Node);
    
    DeltaX = fabs(Point.x - Node.x);
    DeltaY = fabs(Point.y - Node.y);
    DeltaZ = fabs(Point.z - Node.z);
    
    return ( DeltaX*DeltaX + DeltaY*DeltaY + DeltaZ*DeltaZ <= Radius*Radius );
}

/*
    Cylinder is described using:
    Center Point
    Radius
    MinY/MaxY: Bottom and Top Y coordinate of the cylinder from the center position.
*/
bool BSDPointInCylinder(Vec3_t Point,BSDPosition_t Center,float Radius,float MinY,float MaxY)
{
//   int DeltaY;
//   int DeltaX;
    Vec3_t Node;
    float DeltaX;
    float DeltaY;
    float DeltaZ;
    float Temp;
  
    Node.x = Center.x;
    Node.y = Center.y;
    Node.z = Center.z;
    Vec3RotateXAxis(DEGTORAD(180.f),&Node);

    //Make sure Min/Max are not swapped out.
    if( MaxY < MinY ) {
        Temp = MaxY;
        MaxY = MinY;
        MinY = Temp;
    }
    
    DeltaX = Point.x - Node.x;
    DeltaY = Point.y - Node.y;
    DeltaZ = Point.z - Node.z;
    
    if( DeltaX * DeltaX + DeltaZ * DeltaZ <= Radius * Radius ) {
        if( DeltaY >= MinY && DeltaY <= MaxY ) {
            return true;
        } 
    }
    return false;
}

bool BSDPointInBox(Vec3_t Point,BSDPosition_t Center,BSDPosition_t NodeRotation,float Width,float Height,float Depth)
{
    Vec3_t Node;
    vec3 Delta;
    float HalfSizeX;
    float HalfSizeY;
    float HalfSizeZ;
    vec3  Rotation;
    mat4  RotationMatrix;
  
    Node.x = Center.x;
    Node.y = Center.y;
    Node.z = Center.z;
    Vec3RotateXAxis(DEGTORAD(180.f),&Node);
    
    HalfSizeX = fabs(Width) / 2.f;
    HalfSizeY = fabs(Height)/ 2.f;
    HalfSizeZ = fabs(Depth) / 2.f;

    Delta[0] = Point.x - Node.x;
    Delta[1] = Point.y - Node.y;
    Delta[2] = Point.z - Node.z;

    if( NodeRotation.x != 0 || NodeRotation.y != 0 || NodeRotation.z != 0 ) {
//         OOB Test...
        Rotation[0] = -glm_rad(((NodeRotation.x / 4096.f) * 360.f));
        Rotation[1] = -glm_rad(((NodeRotation.y / 4096.f) * 360.f));
        Rotation[2] = -glm_rad(((NodeRotation.z / 4096.f) * 360.f));
    
        glm_mat4_identity(RotationMatrix);
        glm_euler_yxz(Rotation,RotationMatrix);
        glm_mat4_mulv3(RotationMatrix, Delta, 1, Delta);
    }
    
    if( fabs(Delta[0]) <= HalfSizeX && fabs(Delta[1]) <= HalfSizeY && fabs(Delta[2]) <= HalfSizeZ ) {
        return true;
    }
    return false;
}

bool BSDPointInNode(Vec3_t Position,BSDNode_t *Node)
{
    switch( Node->CollisionVolumeType ) {
        case BSD_COLLISION_VOLUME_TYPE_SPHERE:
            return BSDPointInSphere(Position,Node->Position,Node->CollisionInfo0);
        case BSD_COLLISION_VOLUME_TYPE_CYLINDER:
            return BSDPointInCylinder(Position,Node->Position,Node->CollisionInfo0,Node->CollisionInfo1,Node->CollisionInfo2);
        case BSD_COLLISION_VOLUME_TYPE_BOX:
            return BSDPointInBox(Position,Node->Position,Node->Rotation,Node->CollisionInfo0,Node->CollisionInfo1,Node->CollisionInfo2);
        default:
            DPrintf("Unknown CollisionVolumeType %i for node %i\n",Node->CollisionVolumeType,Node->Id);
            return false;
    }
}

void BSDClearNodesFlag(BSD_t *BSD)
{
    int i;
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        BSD->NodeData.Node[i].Visited = 0;
    }
}

int BSDGetCurrentCameraNodeDynamicData(BSD_t *BSD)
{
    int i;
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        if( BSD->NodeData.Node[i].Visited ) {
            continue;
        }
        if( BSD->NodeData.Node[i].MessageData == -1 ) {
            continue;
        }
        
        if( BSDPointInNode(Camera.Position,&BSD->NodeData.Node[i]) ) {
            if( BSD->NodeData.Node[i].Type == 5 /*BSD->NodeData.Node[i].Type == 3 || BSD->NodeData.Node[i].Type == 5 ||
                BSD->NodeData.Node[i].Type == 6*/ ) {
                BSD->NodeData.Node[i].Visited = 1;
                return BSD->NodeData.Node[i].DynamicBlockIndex;
            }
        }
    }
    return -1;
}
//TODO:Spawn the RenderObject when loading node data!
//     Some nodes don't have a corresponding RenderObject like the PlayerSpawn.
//     BSDSpawnEntity(int UBlockId,Vec3_t NodePos) => Store into a list and transform to vao.
void BSDDraw(BSD_t *BSD,VRAM_t *VRAM)
{
    Shader_t *Shader;
    BSDRenderObjectDrawable_t *RenderObjectIterator;
    VAO_t *VAOIterator;
    int MVPMatrixId;
    int EnableLightingId;
    int i;
    
    if( 1 ) {
        for( int i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
            if( BSD->NodeData.Node[i].MessageData == -1 ) {
                continue;
            }
//             if( Level->BSD->NodeData.Node[i].CollisionVolumeType != 2 ) {
//                 continue;
//             }
            if( BSDPointInNode(Camera.Position,&BSD->NodeData.Node[i]) ) {
                DPrintf("Camera is inside node %i => %s\n",i,BSDNodeGetEnumStringFromNodeId(BSD->NodeData.Node[i].Id));
                DPrintf("Node CollisionVolumeType:%s\n",BSDGetCollisionVolumeStringFromType(BSD->NodeData.Node[i].CollisionVolumeType));
                if( BSD->NodeData.Node[i].Type == 5 ) {
                    //TODO:Update the TSP faces based on the node dynamic index that maps to the dynamic block inside TSP file.
                    DPrintf("Node has dynamic face index set to %i\n",BSD->NodeData.Node[i].DynamicBlockIndex);
                }
                break;
            }
        }
    }
    if( LevelDrawBSDNodesAsPoints->IValue ) {    
        Shader = ShaderCache("BSDShader","Shaders/BSDVertexShader.glsl","Shaders/BSDFragmentShader.glsl");
        glUseProgram(Shader->ProgramId);

        MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
        glUniformMatrix4fv(MVPMatrixId,1,false,&VidConf.MVPMatrix[0][0]);
        glBindVertexArray(BSD->NodeVAO->VAOId[0]);
        glPointSize(10.f);
        glDrawArrays(GL_POINTS, 0, BSD->NodeData.Header.NumNodes);
        glBindVertexArray(0);
        glUseProgram(0);
    }
    
    if( LevelDrawBSDRenderObjectsAsPoints->IValue ) {    
        Shader = ShaderCache("BSDShader","Shaders/BSDVertexShader.glsl","Shaders/BSDFragmentShader.glsl");
        glUseProgram(Shader->ProgramId);

        MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
        glUniformMatrix4fv(MVPMatrixId,1,false,&VidConf.MVPMatrix[0][0]);
        glBindVertexArray(BSD->RenderObjectPointVAO->VAOId[0]);
        glPointSize(10.f);
        glDrawArrays(GL_POINTS, 0, BSD->NumRenderObjectPoint);
        glBindVertexArray(0);
        glUseProgram(0);
    }
    
    
    if( LevelDrawBSDRenderObjects->IValue ) {
        Shader = ShaderCache("BSDObjectShader","Shaders/BSDObjectVertexShader.glsl","Shaders/BSDObjectFragmentShader.glsl");
        glUseProgram(Shader->ProgramId);
        MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
        EnableLightingId = glGetUniformLocation(Shader->ProgramId,"EnableLighting");
        glUniform1i(EnableLightingId, LevelEnableAmbientLight->IValue);
        glBindTexture(GL_TEXTURE_2D,VRAM->Page.TextureId);

        for( RenderObjectIterator = BSD->RenderObjectDrawableList; RenderObjectIterator; 
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
            
            temp[0] = RenderObjectIterator->Scale.x;
            temp[1] = RenderObjectIterator->Scale.y;
            temp[2] = RenderObjectIterator->Scale.z;
            glm_scale(VidConf.ModelViewMatrix,temp);

            glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
            
//             Emulate PSX Coordinate system...
            glm_rotate_x(VidConf.MVPMatrix,glm_rad(180.f), VidConf.MVPMatrix);
            glUniformMatrix4fv(MVPMatrixId,1,false,&VidConf.MVPMatrix[0][0]);

            for( VAOIterator = BSD->RenderObjectList[RenderObjectIterator->RenderObjectIndex].VAO; VAOIterator; 
                VAOIterator = VAOIterator->Next ) {
                glBindVertexArray(VAOIterator->VAOId[0]);
                glDrawArrays(GL_TRIANGLES, 0, VAOIterator->Count);
                glBindVertexArray(0);
            }
        }
        glBindTexture(GL_TEXTURE_2D,0);
        glUseProgram(0);
    }
    
    if( LevelDrawBSDShowcase->IValue ) {
        Shader = ShaderCache("BSDObjectShader","Shaders/BSDObjectVertexShader.glsl","Shaders/BSDObjectFragmentShader.glsl");
        glUseProgram(Shader->ProgramId);
        MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
        glBindTexture(GL_TEXTURE_2D,VRAM->Page.TextureId);
        Vec3_t PSpawn;
    
        PSpawn = BSDGetPlayerSpawn(BSD,NULL);
    
        for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
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
            
            temp[0] = -(Camera.Position.x - (PSpawn.x - (i * 200.f)));
            temp[1] = -(Camera.Position.y + PSpawn.y);
            temp[2] = -(Camera.Position.z - PSpawn.z);

            glm_translate(VidConf.ModelViewMatrix,temp);
                        temp[0] = 0;
            temp[1] = -1;
            temp[2] = 0;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(0), temp);
            temp[0] = 1;
            temp[1] = 0;
            temp[2] = 0;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(0), temp);
            temp[0] = 0;
            temp[1] = 0;
            temp[2] = 1;
            glm_rotate(VidConf.ModelViewMatrix,glm_rad(0), temp);
            glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
            
            //Emulate PSX Coordinate system...
            glm_rotate_x(VidConf.MVPMatrix,glm_rad(180.f), VidConf.MVPMatrix);
            glUniformMatrix4fv(MVPMatrixId,1,false,&VidConf.MVPMatrix[0][0]);

            for( VAOIterator = BSD->RenderObjectList[i].VAO; VAOIterator; VAOIterator = VAOIterator->Next ) {
                glBindVertexArray(VAOIterator->VAOId[0]);
                glDrawArrays(GL_TRIANGLES, 0, VAOIterator->Count);
                glBindVertexArray(0);
            }
        }
        glBindTexture(GL_TEXTURE_2D,0);
        glUseProgram(0);
    }
}

void BSDDrawSky(BSD_t *BSD,VRAM_t *VRAM)
{
    Shader_t *Shader;
    int MVPMatrixId;
    
    glDepthMask(0);
    if( BSDIsMoonEnabled(BSD) ) {
        Shader = ShaderCache("MoonShader","Shaders/MoonVertexShader.glsl","Shaders/MoonFragmentShader.glsl");
        glUseProgram(Shader->ProgramId);
        MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
        glBindTexture(GL_TEXTURE_2D,VRAM->Page.TextureId);

        glUniformMatrix4fv(MVPMatrixId,1,false,&VidConf.MVPMatrix[0][0]);

        glBindVertexArray(BSD->SkyData.MoonVAO->VAOId[0]);
        glDrawArrays(GL_TRIANGLES, 0, BSD->SkyData.MoonVAO->Count);
        glBindVertexArray(0);
                
        glBindTexture(GL_TEXTURE_2D,0);
        glUseProgram(0);
    }
    if( BSDAreStarsEnabled(BSD) ) {
        BSDUpdateStarsColors(BSD);
        
        Shader = ShaderCache("StarsShader","Shaders/StarsVertexShader.glsl","Shaders/StarsFragmentShader.glsl");
        glUseProgram(Shader->ProgramId);
        MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
        
        glUniformMatrix4fv(MVPMatrixId,1,false,&VidConf.MVPMatrix[0][0]);
        glPointSize(2.f);
        glBindVertexArray(BSD->SkyData.StarsVAO->VAOId[0]);
        glDrawArrays(GL_POINTS, 0, 255);
        glBindVertexArray(0);
        glUseProgram(0);
    }
    glDepthMask(1);
}
void ParseRenderObjectVertexData(BSD_t *BSD,FILE *BSDFile)
{
    int Size;
    int i;
    int j;
    
    BSD->RenderObjectList = malloc(BSD->RenderObjectTable.NumRenderObject * sizeof(BSDRenderObject_t));
    memset(BSD->RenderObjectList,0,BSD->RenderObjectTable.NumRenderObject * sizeof(BSDRenderObject_t));
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        BSD->RenderObjectList[i].Data = &BSD->RenderObjectTable.RenderObject[i];
        if( BSD->RenderObjectList[i].Data->VertOffset != 0 ) {
            Size = BSD->RenderObjectList[i].Data->NumVertex * sizeof(BSDPosition_t);
            BSD->RenderObjectList[i].Vertex = malloc(Size);
            memset(BSD->RenderObjectList[i].Vertex,0,Size);
            fseek(BSDFile,BSD->RenderObjectList[i].Data->VertOffset + 2048,SEEK_SET);
            DPrintf("Reading Vertex definition at %i (Current:%i)\n",
                    BSD->RenderObjectList[i].Data->VertOffset + 2048,GetCurrentFilePosition(BSDFile)); 
            for( j = 0; j < BSD->RenderObjectList[i].Data->NumVertex; j++ ) {
                DPrintf("Reading Vertex at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
                fread(&BSD->RenderObjectList[i].Vertex[j],sizeof(BSDPosition_t),1,BSDFile);
                DPrintf("Vertex %i;%i;%i %i\n",BSD->RenderObjectList[i].Vertex[j].x,
                        BSD->RenderObjectList[i].Vertex[j].y,BSD->RenderObjectList[i].Vertex[j].z,
                        BSD->RenderObjectList[i].Vertex[j].Pad
                );
                
            }
        }
        if( BSD->RenderObjectList[i].Data->ColorOffset != 0 ) {
            Size = BSD->RenderObjectList[i].Data->NumVertex * sizeof(Color1i_t);
            BSD->RenderObjectList[i].Color = malloc(Size);
            memset(BSD->RenderObjectList[i].Color,0,Size);
            fseek(BSDFile,BSD->RenderObjectList[i].Data->ColorOffset + 2048,SEEK_SET);
            DPrintf("Reading Color definition at %i (Current:%i)\n",
                    BSD->RenderObjectList[i].Data->ColorOffset + 2048,GetCurrentFilePosition(BSDFile)); 
            for( j = 0; j < BSD->RenderObjectList[i].Data->NumVertex; j++ ) {
                DPrintf("Reading Color at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
                fread(&BSD->RenderObjectList[i].Color[j],sizeof(Color1i_t),1,BSDFile);
                DPrintf("Color %i => %i;%i;%i;%i\n",BSD->RenderObjectList[i].Color[j].c,
                        BSD->RenderObjectList[i].Color[j].rgba[0],BSD->RenderObjectList[i].Color[j].rgba[1],BSD->RenderObjectList[i].Color[j].rgba[2],
                        BSD->RenderObjectList[i].Color[j].rgba[3]
                );
                
            }
        }

    }
}
void ParseRenderObjectFaceData(BSDRenderObject_t *RenderObject,FILE *BSDFile)
{
    unsigned short LV0;
    unsigned short LV1;
    unsigned short LV2;
    int FaceListSize;
    int i;
    
    if( !RenderObject ) {
        DPrintf("ParseRenderObjectFaceData:Invalid RenderObject!\n");
        return;
    }
    if( RenderObject->Data->FaceOffset == 0 ) {
        DPrintf("ParseRenderObjectFaceData:Invalid FaceOffset!\n");
        return;
    }
    
    fseek(BSDFile,RenderObject->Data->FaceOffset + 2048,SEEK_SET);
    fread(&RenderObject->NumFaces,sizeof(int),1,BSDFile);
    DPrintf("ParseRenderObjectFaceData:Reading %i faces\n",RenderObject->NumFaces);
    FaceListSize = RenderObject->NumFaces * sizeof(BSDFace_t);
    RenderObject->Face = malloc(FaceListSize);
    memset(RenderObject->Face,0,FaceListSize);
    RenderObject->FaceV2 = NULL;
    DPrintf("ParseRenderObjectFaceData:Reading Face definition at %i (Current:%i)\n",RenderObject->Data->FaceOffset + 2048,GetCurrentFilePosition(BSDFile)); 
    for( i = 0; i < RenderObject->NumFaces; i++ ) {
        DPrintf("ParseRenderObjectFaceData:Reading Face at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
        fread(&RenderObject->Face[i],sizeof(BSDFace_t),1,BSDFile);
        DPrintf(" -- FACE %i --\n",i);
        DPrintf("Tex info %i | Color mode %i | Texture Page %i\n",RenderObject->Face[i].TexInfo,
                (RenderObject->Face[i].TexInfo & 0xC0) >> 7,RenderObject->Face[i].TexInfo & 0x1f);
        DPrintf("TSB is %i %ix%i\n",RenderObject->Face[i].TSB,
                ((RenderObject->Face[i].TSB  & 0x3F ) << 4),((RenderObject->Face[i].TSB & 0x7FC0) >> 6));
        DPrintf("UV0:(%i;%i)\n",RenderObject->Face[i].UV0.u,RenderObject->Face[i].UV0.v);
        DPrintf("UV1:(%i;%i)\n",RenderObject->Face[i].UV1.u,RenderObject->Face[i].UV1.v);
        DPrintf("UV2:(%i;%i)\n",RenderObject->Face[i].UV2.u,RenderObject->Face[i].UV2.v);
        DPrintf("Pad is %i\n",RenderObject->Face[i].Pad);

        LV0 = (RenderObject->Face[i].VData & 0xFF);
        LV1 = (RenderObject->Face[i].VData & 0x3fc00) >> 10;
        LV2 = (RenderObject->Face[i].VData & 0xFF00000 ) >> 20;
        DPrintf("V0|V1|V2:%u;%u;%u\n",LV0,LV1,LV2);
        DPrintf("V0|V1|V2:(%i;%i;%i)|(%i;%i;%i)|(%i;%i;%i)\n",
                RenderObject->Vertex[LV0].x,RenderObject->Vertex[LV0].y,RenderObject->Vertex[LV0].z,
                RenderObject->Vertex[LV1].x,RenderObject->Vertex[LV1].y,RenderObject->Vertex[LV1].z,
                RenderObject->Vertex[LV2].x,RenderObject->Vertex[LV2].y,RenderObject->Vertex[LV2].z);
    }
}

void ParseRenderObjectFaceDataV2(BSDRenderObject_t *RenderObject,int FaceOffset,FILE *BSDFile)
{
    int FaceListSize;
    int CurrentFaceIndex;
    int Vert0;
    int Vert1;
    int Vert2;
    unsigned int Marker;
    BSDFaceV2_t TempFace;
    
    if( !RenderObject ) {
        DPrintf("ParseRenderObjectFaceDataV2:Invalid RenderObject!\n");
        return;
    }
    if( RenderObject->Data->FaceOffset == 0 ) {
        DPrintf("ParseRenderObjectFaceDataV2:Invalid FaceOffset!\n");
        return;
    }
    fseek(BSDFile,FaceOffset,SEEK_SET);
    fread(&RenderObject->NumFaces,sizeof(RenderObject->NumFaces),1,BSDFile);
    fseek(BSDFile,RenderObject->Data->FaceOffset + 2048,SEEK_SET);
    RenderObject->Face = NULL;
    FaceListSize = RenderObject->NumFaces * sizeof(BSDFaceV2_t);
    RenderObject->FaceV2 = malloc(FaceListSize);
    memset(RenderObject->FaceV2,0,FaceListSize);
    CurrentFaceIndex = 0;
    while( CurrentFaceIndex < RenderObject->NumFaces ) {
        DPrintf("ParseRenderObjectFaceDataV2:Reading Face at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);                                
        fread(&RenderObject->FaceV2[CurrentFaceIndex].V0V1,sizeof(RenderObject->FaceV2[CurrentFaceIndex].V0V1),1,BSDFile);
        fread(&RenderObject->FaceV2[CurrentFaceIndex].V2,sizeof(RenderObject->FaceV2[CurrentFaceIndex].V2),1,BSDFile);
        fread(&RenderObject->FaceV2[CurrentFaceIndex].UV0,sizeof(RenderObject->FaceV2[CurrentFaceIndex].UV0),1,BSDFile);
        fread(&RenderObject->FaceV2[CurrentFaceIndex].TSB,sizeof(RenderObject->FaceV2[CurrentFaceIndex].TSB),1,BSDFile);
        fread(&RenderObject->FaceV2[CurrentFaceIndex].UV1,sizeof(RenderObject->FaceV2[CurrentFaceIndex].UV1),1,BSDFile);
        fread(&RenderObject->FaceV2[CurrentFaceIndex].TexInfo,sizeof(RenderObject->FaceV2[CurrentFaceIndex].TexInfo),1,BSDFile);
        fread(&RenderObject->FaceV2[CurrentFaceIndex].UV2,sizeof(RenderObject->FaceV2[CurrentFaceIndex].UV2),1,BSDFile);
        DPrintf(" -- FACE %i --\n",CurrentFaceIndex);
        DPrintf("V0V1:%i V2:%i\n",RenderObject->FaceV2[CurrentFaceIndex].V0V1,RenderObject->FaceV2[CurrentFaceIndex].V2);
        DPrintf("Tex info %i | Color mode %i | Texture Page %i\n",RenderObject->FaceV2[CurrentFaceIndex].TexInfo,
                (RenderObject->FaceV2[CurrentFaceIndex].TexInfo & 0xC0) >> 7,RenderObject->FaceV2[CurrentFaceIndex].TexInfo & 0x1f);
        DPrintf("TSB is %i %ix%i\n",RenderObject->FaceV2[CurrentFaceIndex].TSB,
                ((RenderObject->FaceV2[CurrentFaceIndex].TSB  & 0x3F ) << 4),
                ((RenderObject->FaceV2[CurrentFaceIndex].TSB & 0x7FC0) >> 6));
        DPrintf("UV0:(%i;%i)\n",RenderObject->FaceV2[CurrentFaceIndex].UV0.u,
                RenderObject->FaceV2[CurrentFaceIndex].UV0.v);
        DPrintf("UV1:(%i;%i)\n",RenderObject->FaceV2[CurrentFaceIndex].UV1.u,
                RenderObject->FaceV2[CurrentFaceIndex].UV1.v);
        DPrintf("UV2:(%i;%i)\n",RenderObject->FaceV2[CurrentFaceIndex].UV2.u,
                RenderObject->FaceV2[CurrentFaceIndex].UV2.v);
        
        Vert0 = RenderObject->FaceV2[CurrentFaceIndex].V0V1 & 0x1FFF;
        Vert1 = ( RenderObject->FaceV2[CurrentFaceIndex].V0V1 >> 16 ) & 0X1FFF;
        Vert2 = RenderObject->FaceV2[CurrentFaceIndex].V2 & 0X1FFF;
        RenderObject->FaceV2[CurrentFaceIndex].Vert0 = TempFace.Vert0 = Vert0;
        RenderObject->FaceV2[CurrentFaceIndex].Vert1 = TempFace.Vert1 = Vert1;
        RenderObject->FaceV2[CurrentFaceIndex].Vert2 = TempFace.Vert2 = Vert2;
        TempFace.TexInfo = RenderObject->FaceV2[CurrentFaceIndex].TexInfo;
        TempFace.UV0 = RenderObject->FaceV2[CurrentFaceIndex].UV0;
        TempFace.UV1 = RenderObject->FaceV2[CurrentFaceIndex].UV1;
        TempFace.UV2 = RenderObject->FaceV2[CurrentFaceIndex].UV2;
        CurrentFaceIndex++;
        while( 1 ) {
            DPrintf("ParseRenderObjectFaceDataV2:Reading additional face %i \n",CurrentFaceIndex);
            fread(&Marker,sizeof(Marker),1,BSDFile);
            DPrintf("ParseRenderObjectFaceDataV2:Found Marker %u (Vertex %i) Texture:%u Mask %i\n",Marker,Marker & 0x1FFF,Marker >> 16,0x1FFF);
            if( ( Marker & 0x1FFF ) == 0x1FFF || Marker == 0x1fff1fff ) {
                DPrintf("ParseRenderObjectFaceDataV2:Aborting since a marker was found\n");
                break;
            }
            RenderObject->FaceV2[CurrentFaceIndex].TexInfo = TempFace.TexInfo;
                    
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

            RenderObject->FaceV2[CurrentFaceIndex].Vert0 = TempFace.Vert0;
            RenderObject->FaceV2[CurrentFaceIndex].Vert1 = TempFace.Vert1;
            RenderObject->FaceV2[CurrentFaceIndex].Vert2 = TempFace.Vert2;
            RenderObject->FaceV2[CurrentFaceIndex].UV0.u = TempFace.UV0.u;
            RenderObject->FaceV2[CurrentFaceIndex].UV0.v = TempFace.UV0.v;

            RenderObject->FaceV2[CurrentFaceIndex].UV1.u = TempFace.UV1.u;
            RenderObject->FaceV2[CurrentFaceIndex].UV1.v = TempFace.UV1.v;
            
            RenderObject->FaceV2[CurrentFaceIndex].UV2.u = TempFace.UV2.u;
            RenderObject->FaceV2[CurrentFaceIndex].UV2.v = TempFace.UV2.v;

            DPrintf("ParseRenderObjectFaceDataV2:Vert0:%i Vert1:%i Vert2:%i Additional Face %i\n",TempFace.Vert0,TempFace.Vert1,
                    TempFace.Vert2,RenderObject->FaceV2[CurrentFaceIndex].V2 >> 16);
            CurrentFaceIndex++;
        }
        if( Marker == 0x1fff1fff ) {
            DPrintf("ParseRenderObjectFaceDataV2:Sentinel Face found Done reading faces for renderobject %i\n",RenderObject->Data->Id);
            DPrintf("ParseRenderObjectFaceDataV2:Loaded %i faces (Expected %i)\n",CurrentFaceIndex,RenderObject->NumFaces);
            assert(CurrentFaceIndex == RenderObject->NumFaces);
            break;
        }
    }
}
void LoadRenderObjectsFaceData(BSD_t *BSD,int RenderObjectDataOffset,int GameEngine,FILE *BSDFile)
{
    int FaceOffset;
    int i;
    if( !BSD->RenderObjectList ) {
        DPrintf("LoadRenderObjectsFaceData:Object list is empty...\n");
        return;
    }
    assert(sizeof(BSDFace_t) == 16);

    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        if( BSD->RenderObjectList[i].Data->FaceOffset == 0 ) {
            continue;
        }
        DPrintf("LoadRenderObjectsFaceData:RenderObject Id %u\n",BSD->RenderObjectList[i].Data->Id);
        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            FaceOffset = ( RenderObjectDataOffset + (i * MOH_UNDERGROUND_RENDER_OBJECT_SIZE) ) + 260;
            ParseRenderObjectFaceDataV2(&BSD->RenderObjectList[i],FaceOffset,BSDFile);
        } else {
            ParseRenderObjectFaceData(&BSD->RenderObjectList[i],BSDFile);
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
    fseek(BSDFile,sizeof(BSD_Header_t) + BSD_PROPERTY_SET_FILE_POSITION,SEEK_SET);
    fread(&PropertySetFileOffset,sizeof(PropertySetFileOffset),1,BSDFile);
    if( PropertySetFileOffset == 0 ) {
        DPrintf("BSDReadPropertySetFile:BSD File has no property file set.\n");
        return;
    }
    fseek(BSDFile,sizeof(BSD_Header_t) + PropertySetFileOffset,SEEK_SET);
    fread(&BSD->PropertySetFile.NumProperties,sizeof(BSD->PropertySetFile.NumProperties),1,BSDFile);
    DPrintf("BSDReadPropertySetFile:Reading %i properties at %i (%i).\n",BSD->PropertySetFile.NumProperties,
            GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
    BSD->PropertySetFile.NumProperties++;
    BSD->PropertySetFile.Property = malloc(sizeof(BSDProperty_t) * BSD->PropertySetFile.NumProperties);
    for( i = 0; i < BSD->PropertySetFile.NumProperties; i++ ) {
        DPrintf("BSDReadPropertySetFile:Reading property %i at %i (%i)\n",i,GetCurrentFilePosition(BSDFile),
            GetCurrentFilePosition(BSDFile) - 2048
        );
        fread(&BSD->PropertySetFile.Property[i].NumNodes,sizeof(BSD->PropertySetFile.Property[i].NumNodes),1,BSDFile);
        BSD->PropertySetFile.Property[i].NumNodes = (255 - BSD->PropertySetFile.Property[i].NumNodes) /*<< 1*/;
        DPrintf("Property contains %i nodes\n",BSD->PropertySetFile.Property[i].NumNodes);
        
        SkipFileSection(1,BSDFile);
        BSD->PropertySetFile.Property[i].NodeList = malloc(BSD->PropertySetFile.Property[i].NumNodes * sizeof(unsigned short));
        DPrintf("BSDReadPropertySetFile:Reading %li bytes.\n",BSD->PropertySetFile.Property[i].NumNodes * sizeof(unsigned short));
        for( j = 0; j <  BSD->PropertySetFile.Property[i].NumNodes; j++ ) {
            fread(&BSD->PropertySetFile.Property[i].NodeList[j],sizeof(BSD->PropertySetFile.Property[i].NodeList[j]),1,BSDFile);
            DPrintf("Short %u\n", BSD->PropertySetFile.Property[i].NodeList[j]);
//             assert(BSD->PropertySetFile.Property[i].Data[j] < 158);
        }
    }
    DPrintf("BSDReadPropertySetFile:Property end at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
    fseek(BSDFile,PreviousFilePosition,SEEK_SET);
}

int BSDGetTSPDynamicIndexOffsetFromNodeType(int Type)
{
    switch( Type ) {
        case 3:
            return 90;
        case 5:
            return 88;
        case 6:
            return 108;
        default:
            return -1;
    }
}
void BSDReadAnimatedLightChunk(BSD_t *BSD,FILE *BSDFile)
{
    BSDAnimatedLight_t *AnimatedLight;
    int i;
    int j;
    int PreviousFilePosition;
    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDReadAnimatedLightChunk: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return;
    }
    
    assert((GetCurrentFilePosition(BSDFile) - 2048) == BSD_ANIMATED_LIGHTS_FILE_POSITION);
    DPrintf("AnimatedLightsTable is at %li\n",ftell(BSDFile));
    fread(&BSD->AnimatedLightsTable.NumAnimatedLights,sizeof(BSD->AnimatedLightsTable.NumAnimatedLights),1,BSDFile);
    DPrintf("AnimatedLightsTable:Reading %i colors at %li\n",BSD->AnimatedLightsTable.NumAnimatedLights,ftell(BSDFile));

    for( i = 0; i < BSD_ANIMATED_LIGHTS_TABLE_SIZE; i++ ) {
        AnimatedLight = &BSD->AnimatedLightsTable.AnimatedLightsList[i];
        fread(&AnimatedLight->NumColors,sizeof(AnimatedLight->NumColors),1,BSDFile);
        fread(&AnimatedLight->StartingColorOffset,sizeof(AnimatedLight->StartingColorOffset),1,BSDFile);
        fread(&AnimatedLight->ColorIndex,sizeof(AnimatedLight->ColorIndex),1,BSDFile);
        fread(&AnimatedLight->CurrentColor,sizeof(AnimatedLight->CurrentColor),1,BSDFile);
        fread(&AnimatedLight->Delay,sizeof(AnimatedLight->Delay),1,BSDFile);
        if( AnimatedLight->NumColors == 0 ) {
            continue;
        }
        AnimatedLight->ColorList = malloc(AnimatedLight->NumColors * sizeof(Color1i_t));
        DPrintf("Animated Light: %i\n",i);
        DPrintf("StartingColorOffset:%i\n",AnimatedLight->StartingColorOffset);
        DPrintf("StartingColorOffset No Header:%i\n",AnimatedLight->StartingColorOffset + 2048);
        DPrintf("CurrentColor:%i\n",AnimatedLight->CurrentColor);
        DPrintf("ColorIndex:%i\n",AnimatedLight->ColorIndex);
        DPrintf("Delay:%i\n",AnimatedLight->Delay);
        PreviousFilePosition = GetCurrentFilePosition(BSDFile);
        fseek(BSDFile,AnimatedLight->StartingColorOffset + 2048,SEEK_SET);
        DPrintf("Reading color at %i\n",GetCurrentFilePosition(BSDFile));
        for( j = 0; j < AnimatedLight->NumColors; j++ ) {
            fread(&AnimatedLight->ColorList[j],sizeof(AnimatedLight->ColorList[j]),1,BSDFile);
            DPrintf("Color %i %i %i %i %i (As Int %u)\n",j,AnimatedLight->ColorList[j].rgba[0],
                    AnimatedLight->ColorList[j].rgba[1],AnimatedLight->ColorList[j].rgba[2],AnimatedLight->ColorList[j].rgba[3],AnimatedLight->ColorList[j].c
            );
        }
        fseek(BSDFile,PreviousFilePosition,SEEK_SET);
    }
    DPrintf("AnimatedLightsTable ends at %i\n",GetCurrentFilePosition(BSDFile));
}

void BSDReadEntryTableChunk(BSD_t *BSD,FILE *BSDFile)
{    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDReadEntryTableChunk: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return;
    }
    DPrintf("EntryTable is at %i\n",GetCurrentFilePosition(BSDFile));
    assert((GetCurrentFilePosition(BSDFile) - 2048) == BSD_ENTRY_TABLE_FILE_POSITION);
    assert(sizeof(BSD->EntryTable) == 80);
    DPrintf("Reading EntryTable at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - 2048);
    fread(&BSD->EntryTable,sizeof(BSD->EntryTable),1,BSDFile);
    DPrintf("Node table is at %i (%i)\n",BSD->EntryTable.NodeTableOffset,BSD->EntryTable.NodeTableOffset + 2048);
    DPrintf("Unknown data is at %i (%i)\n",BSD->EntryTable.UnknownDataOffset,BSD->EntryTable.UnknownDataOffset + 2048);
    DPrintf("Data0 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off0,BSD->EntryTable.Off0 + 2048,BSD->EntryTable.Num0);
    DPrintf("Data1 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off1,BSD->EntryTable.Off1 + 2048,BSD->EntryTable.Num1);
    DPrintf("Data2 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off2,BSD->EntryTable.Off2 + 2048,BSD->EntryTable.Num2);
    DPrintf("Data3 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off3,BSD->EntryTable.Off3 + 2048,BSD->EntryTable.Num3);
    DPrintf("Data4 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off4,BSD->EntryTable.Off4 + 2048,BSD->EntryTable.Num4);
    DPrintf("Data5 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off5,BSD->EntryTable.Off5 + 2048,BSD->EntryTable.Num5);
    DPrintf("Data6 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off6,BSD->EntryTable.Off6 + 2048,BSD->EntryTable.Num6);
    DPrintf("Data7 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off7,BSD->EntryTable.Off7 + 2048,BSD->EntryTable.Num7);
    DPrintf("Data8 at %i (%i) contains %i elements.\n",BSD->EntryTable.Off8,BSD->EntryTable.Off8 + 2048,BSD->EntryTable.Num8);
}

void BSDReadSkyChunk(BSD_t *BSD,FILE *BSDFile)
{    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDReadSkyChunk: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return;
    }

    assert((GetCurrentFilePosition(BSDFile)  - 2048) == BSD_SKY_DATA_FILE_POSITION);
    fread(&BSD->SkyData.U0,sizeof(BSD->SkyData.U0),1,BSDFile);
    fread(&BSD->SkyData.U1,sizeof(BSD->SkyData.U1),1,BSDFile);
    fread(&BSD->SkyData.U2,sizeof(BSD->SkyData.U2),1,BSDFile);
    fread(&BSD->SkyData.StarRadius,sizeof(BSD->SkyData.StarRadius),1,BSDFile);
    fread(&BSD->SkyData.U3,sizeof(BSD->SkyData.U3),1,BSDFile);
    fread(&BSD->SkyData.MoonZ,sizeof(BSD->SkyData.MoonZ),1,BSDFile);
    fread(&BSD->SkyData.MoonY,sizeof(BSD->SkyData.MoonY),1,BSDFile);
    fread(&BSD->SkyData.U4,sizeof(BSD->SkyData.U4),1,BSDFile);
    fread(&BSD->SkyData.U5,sizeof(BSD->SkyData.U5),1,BSDFile);
    fread(&BSD->SkyData.U6,sizeof(BSD->SkyData.U6),1,BSDFile);
    
    DPrintf("MoonY:%i MoonZ:%i\n",BSD->SkyData.MoonY,BSD->SkyData.MoonZ);
    DPrintf("Star Radius:%i\n",BSD->SkyData.StarRadius);
}

void BSDReadRenderObjectChunk(BSD_t *BSD,int GameEngine,FILE *BSDFile)
{
    int FirstRenderObjectPosition;
    int PreviousFilePosition;
    int i;
    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDReadRenderObjectChunk: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return;
    }
    
    fread(&BSD->RenderObjectTable.NumRenderObject,sizeof(BSD->RenderObjectTable.NumRenderObject),1,BSDFile);
    FirstRenderObjectPosition = GetCurrentFilePosition(BSDFile);
    
    DPrintf("BSDReadRenderObjectChunk:Reading %i RenderObject Elements...\n",BSD->RenderObjectTable.NumRenderObject);
    
    assert(sizeof(BSDRenderObjectElement_t) == MOH_RENDER_OBJECT_SIZE);
    
    BSD->RenderObjectTable.RenderObject = malloc(BSD->RenderObjectTable.NumRenderObject * sizeof(BSDRenderObjectElement_t));
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
            DPrintf("RenderObject Type:%i | %s\n",BSD->RenderObjectTable.RenderObject[i].Type,
                    BSDRenderObjectGetWeaponNameFromId(BSD->RenderObjectTable.RenderObject[i].Id));
        } else {
            DPrintf("RenderObject Type:%i | %s\n",BSD->RenderObjectTable.RenderObject[i].Type,
                    BSDRenderObjectGetEnumStringFromType(BSD->RenderObjectTable.RenderObject[i].Type));
        }
        DPrintf("RenderObject Element Vertex Offset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].VertOffset,
                BSD->RenderObjectTable.RenderObject[i].VertOffset + 2048);
        DPrintf("RenderObject Element NumVertex: %i\n",BSD->RenderObjectTable.RenderObject[i].NumVertex);
        //Those offset are relative to the EntryTable.
        DPrintf("RenderObject UnknownOffset1: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].UnknownOffset1,
                BSD->RenderObjectTable.RenderObject[i].UnknownOffset1 + 2048);
        DPrintf("RenderObject UnknownOffset2: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].UnknownOffset2,
                BSD->RenderObjectTable.RenderObject[i].UnknownOffset2 + 2048);
        DPrintf("RenderObject Root Bone Offset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].RootBoneOffset,
                BSD->RenderObjectTable.RenderObject[i].RootBoneOffset + 2048);
        DPrintf("RenderObject FaceOffset: %i (%i)\n",BSD->RenderObjectTable.RenderObject[i].FaceOffset,
                BSD->RenderObjectTable.RenderObject[i].FaceOffset + 2048);
        DPrintf("RenderObject Scale: %i;%i;%i (4096 is 1 meaning no scale)\n",
                BSD->RenderObjectTable.RenderObject[i].ScaleX / 4,
                BSD->RenderObjectTable.RenderObject[i].ScaleY / 4,
                BSD->RenderObjectTable.RenderObject[i].ScaleZ / 4);
        if( BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObject != -1 ) {
            DPrintf("RenderObject References RenderObject Id:%u\n",BSD->RenderObjectTable.RenderObject[i].ReferencedRenderObject);
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
    }
    // Prepare vertices to be rendered!
    PreviousFilePosition = ftell(BSDFile);
    ParseRenderObjectVertexData(BSD,BSDFile);
    LoadRenderObjectsFaceData(BSD,FirstRenderObjectPosition,GameEngine,BSDFile);
    fseek(BSDFile,PreviousFilePosition,SEEK_SET);
}

void BSDParseNodeChunk(BSDNode_t *Node,BSD_t *BSD,int IsMultiplayer,int NodeFilePosition,FILE *BSDFile)
{
    BSDTSPStreamNode_t *StreamNode;
    int Offset;
    int NodeNumReferencedRenderObjectIdOffset;
    int NumReferencedRenderObjectId;
    unsigned int NodeRenderObjectId;
    Vec3_t NodePosition;
    Vec3_t NodeRotation;
    int PrevPos;
    int i;

    int DynamicIndexOffset;
    if( !Node || !BSDFile) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDParseNodeChunk: Invalid %s\n",InvalidFile ? "file" : "node struct");
        return;
    }
    DPrintf("BSDParseNodeChunk:Parsing node with Id:%u | Id:%s\n",Node->Id,BSDNodeGetEnumStringFromNodeId(Node->Id));
    DPrintf("Size:%i\n",Node->Size);
    DPrintf("U2:%i\n",Node->u2);
    DPrintf("Type:%i\n",Node->Type);
    DPrintf("Position:(%i;%i;%i) Pad %i\n",Node->Position.x,Node->Position.y,
            Node->Position.z,Node->Position.Pad);
    DPrintf("Rotation:(%i;%i;%i) Pad %i\n",Node->Rotation.x,Node->Rotation.y,
            Node->Rotation.z,Node->Rotation.Pad);
    if( Node->CollisionVolumeType == 0 ) {
        DPrintf("CollisionType:%s Radius:%i\n",BSDGetCollisionVolumeStringFromType(Node->CollisionVolumeType),
                Node->CollisionInfo0);
    } else if( Node->CollisionVolumeType == 1 ) {
        DPrintf("CollisionType:%s Radius:%i BottomY:%i TopY:%i \n",
                BSDGetCollisionVolumeStringFromType(Node->CollisionVolumeType),
                Node->CollisionInfo0,Node->CollisionInfo1,
                Node->CollisionInfo2);
    } else {
        DPrintf("CollisionType:%s Width:%i Height:%i Depth:%i \n",
                BSDGetCollisionVolumeStringFromType(Node->CollisionVolumeType),
                Node->CollisionInfo0,Node->CollisionInfo1,
                Node->CollisionInfo2);
    }

    assert(Node->Position.Pad == 0);
    assert(Node->Rotation.Pad == 0);
        
    if( (DynamicIndexOffset = BSDGetTSPDynamicIndexOffsetFromNodeType(Node->Type)) != -1 ) {
        PrevPos = GetCurrentFilePosition(BSDFile);
        fseek(BSDFile,NodeFilePosition + DynamicIndexOffset,SEEK_SET);
        fread(&Node->DynamicBlockIndex,sizeof(Node->DynamicBlockIndex),1,BSDFile);
        fseek(BSDFile,PrevPos,SEEK_SET);
        DPrintf("Node has Dynamic Index %i\n",Node->DynamicBlockIndex);
    } else {
        Node->DynamicBlockIndex = -1;
    }
        
                
    NodePosition.x = Node->Position.x;
    NodePosition.y = Node->Position.y;
    NodePosition.z = Node->Position.z;
    NodeRotation.x = Node->Rotation.x;
    NodeRotation.y = Node->Rotation.y;
    NodeRotation.z = Node->Rotation.z;
    
    if( Node->Id != BSD_TSP_LOAD_TRIGGER && Node->Size > 48 ) {
        PrevPos = GetCurrentFilePosition(BSDFile);
        fseek(BSDFile,NodeFilePosition + 84,SEEK_SET);
        fread(&Node->MessageData,sizeof(Node->MessageData),1,BSDFile);
        DPrintf("Node has message reg index %i\n",Node->MessageData);
        fseek(BSDFile,PrevPos,SEEK_SET);
    } else {
        Node->MessageData = 0;
    }
    
    if( Node->Type == 2 || Node->Type == 4 || Node->Type == 6 ) {
        //0x74
        Offset = 96;
    } else if ( Node->Type == 3 ) {
        //0x60
        Offset = 116;
    } else if( Node->Type == 0 && Node->Id == BSD_TSP_LOAD_TRIGGER ) {
        Offset = 48;
    } else {
        Offset = 0;
    }
    if( Offset != 0 ) {
        PrevPos = GetCurrentFilePosition(BSDFile);
        DPrintf("Node type %i has offset at %i Position:%i.\n",Node->Type,Offset,NodeFilePosition + Offset);
        fseek(BSDFile,NodeFilePosition + Offset,SEEK_SET);
        if( Node->Id == BSD_TSP_LOAD_TRIGGER ) {
            DPrintf("Node is a BSD_TSP_LOAD_TRIGGER.\n");
            StreamNode = malloc(sizeof(BSDTSPStreamNode_t));
            StreamNode->Position = NodePosition;
            for( i = 0; i < 4; i++ ) {
                StreamNode->TSPNumberRenderList[i] = -1;
                fread(&StreamNode->TSPNumberRenderList[i],sizeof(short),1,BSDFile);
                assert(StreamNode->TSPNumberRenderList[i] != -1);
                DPrintf("Node will ask TSP to draw Compartment %i\n",StreamNode->TSPNumberRenderList[i]);
            }
            StreamNode->Next = BSD->TSPStreamNodeList;
            BSD->TSPStreamNodeList = StreamNode;
        } else {
            fread(&NodeNumReferencedRenderObjectIdOffset,sizeof(NodeNumReferencedRenderObjectIdOffset),1,BSDFile);
            DPrintf("Node has RenderObject offset %i.\n",NodeNumReferencedRenderObjectIdOffset);
            if( NodeNumReferencedRenderObjectIdOffset != 0 ) {
                if( Node->Type == 4 ) {
                    DPrintf("Node has Type 4 so the RenderObject Id is %u.\n",NodeNumReferencedRenderObjectIdOffset);
                    BSDAddNodeToRenderObjecDrawabletList(BSD,IsMultiplayer,NodeNumReferencedRenderObjectIdOffset,NodePosition,NodeRotation);
                } else {
                    fseek(BSDFile,NodeFilePosition + NodeNumReferencedRenderObjectIdOffset,SEEK_SET);
                    fread(&NumReferencedRenderObjectId,sizeof(NumReferencedRenderObjectId),1,BSDFile);
                    DPrintf("Node is referencing %i RenderObjects.\n",NumReferencedRenderObjectId);
                    for( i = 0; i < NumReferencedRenderObjectId; i++ ) {
                        fread(&NodeRenderObjectId,sizeof(NodeRenderObjectId),1,BSDFile);
                        if( Node->Id == BSD_ENEMY_SPAWN && NodeRenderObjectId != 3817496448 && i == 0 ) {
                            DPrintf("We have a different RenderObject for this enemy spawn...\n");
                        }
                        BSDAddNodeToRenderObjecDrawabletList(BSD,IsMultiplayer,NodeRenderObjectId,NodePosition,NodeRotation);
                    }
                }
            }
        }
        fseek(BSDFile,PrevPos,SEEK_SET);
    } else {
        DPrintf("Zero Offset.\n");
    }
}
void BSDReadNodeChunk(BSD_t *BSD,int IsMultiplayer,FILE *BSDFile)
{
    int NodeFilePosition;
    int NodeTableEnd;
    int NextNodeOffset;
    int i;
    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDReadNodeTableChunk: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return;
    }
    assert( GetCurrentFilePosition(BSDFile) == (BSD->EntryTable.NodeTableOffset + 2048));
    DPrintf("BSDReadNodeTableChunk:Reading node table at %i...\n",GetCurrentFilePosition(BSDFile));
    fread(&BSD->NodeData.Header,sizeof(BSD->NodeData.Header),1,BSDFile);
    DPrintf("BSDReadNodeTableChunk:Reading %i entries.\n",BSD->NodeData.Header.NumNodes);
    DPrintf("TableSize: %i\n",BSD->NodeData.Header.TableSize);
    DPrintf("U2: %i\n",BSD->NodeData.Header.u2);
    DPrintf("U3: %i\n",BSD->NodeData.Header.u3);
    DPrintf("U4: %i\n",BSD->NodeData.Header.u4);
    DPrintf("U5: %i\n",BSD->NodeData.Header.u5);
    BSD->NodeData.Table = malloc(BSD->NodeData.Header.NumNodes * sizeof(BSDNodeTableEntry_t));
    DPrintf("BSDReadNodeTableChunk:Nodetable starts at %i\n",GetCurrentFilePosition(BSDFile));
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        fread(&BSD->NodeData.Table[i],sizeof(BSD->NodeData.Table[i]),1,BSDFile);
        DPrintf("-- NODE %i --\n",i);
        DPrintf("Pointer:%i\n",BSD->NodeData.Table[i].Pointer);
        DPrintf("Offset:%i\n",BSD->NodeData.Table[i].Offset);
    }
    NodeTableEnd = GetCurrentFilePosition(BSDFile);
    DPrintf("BSDReadNodeTableChunk:Nodetable ends at %i\n",NodeTableEnd);
    //All the node offset are calculated from the 0 node...
    //So all the offset inside a node are Offset+AddressOfFirstNode.
    //TODO:Load each node entry one by one (not using a single fread) since
    //     there are many types of node that contains different data...
    BSD->NodeData.Node = malloc(BSD->NodeData.Header.NumNodes * sizeof(BSDNode_t));
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        NodeFilePosition = GetCurrentFilePosition(BSDFile);
        DPrintf(" -- NODE %i (Pos %i PosNoHeader %i)-- \n",i,NodeFilePosition,NodeFilePosition - 2048);
        assert(GetCurrentFilePosition(BSDFile) == (BSD->NodeData.Table[i].Offset + NodeTableEnd));
        fread(&BSD->NodeData.Node[i].Id,sizeof(BSD->NodeData.Node[i].Id),1,BSDFile);
        fread(&BSD->NodeData.Node[i].Size,sizeof(BSD->NodeData.Node[i].Size),1,BSDFile);
        fread(&BSD->NodeData.Node[i].u2,sizeof(BSD->NodeData.Node[i].u2),1,BSDFile);
        fread(&BSD->NodeData.Node[i].Type,sizeof(BSD->NodeData.Node[i].Type),1,BSDFile);
        fread(&BSD->NodeData.Node[i].Position,sizeof(BSD->NodeData.Node[i].Position),1,BSDFile);
        fread(&BSD->NodeData.Node[i].Rotation,sizeof(BSD->NodeData.Node[i].Rotation),1,BSDFile);
        fread(&BSD->NodeData.Node[i].Pad,sizeof(BSD->NodeData.Node[i].Pad),1,BSDFile);
        fread(&BSD->NodeData.Node[i].CollisionVolumeType,sizeof(BSD->NodeData.Node[i].CollisionVolumeType),1,BSDFile);
        fread(&BSD->NodeData.Node[i].CollisionInfo0,sizeof(BSD->NodeData.Node[i].CollisionInfo0),1,BSDFile);
        fread(&BSD->NodeData.Node[i].CollisionInfo1,sizeof(BSD->NodeData.Node[i].CollisionInfo1),1,BSDFile);
        fread(&BSD->NodeData.Node[i].CollisionInfo2,sizeof(BSD->NodeData.Node[i].CollisionInfo2),1,BSDFile);
        BSDParseNodeChunk(&BSD->NodeData.Node[i],BSD,IsMultiplayer,NodeFilePosition,BSDFile);
        NextNodeOffset = NodeFilePosition + BSD->NodeData.Node[i].Size;
        fseek(BSDFile,NextNodeOffset,SEEK_SET);
    }
}

int BSDLoad(BSD_t *BSD,int GameEngine,int IsMultiplayer,FILE *BSDFile)
{   
    int MemBegin;
    int MemEnd;
    short Number;
    int Jump;
    BSDAnimatedLight_t *AnimatedLight;
    
    fread(&BSD->Unknown,sizeof(BSD->Unknown),1,BSDFile);
    BSDReadAnimatedLightChunk(BSD,BSDFile);
    //This section seems unused and should be constant in size (320 bytes).
    //TODO:Remove this useless code and just jump 320 bytes...
    MemBegin = GetCurrentFilePosition(BSDFile);
    fread(&Number,sizeof(Number),1,BSDFile);
    while( Number == -1 ) {
        SkipFileSection(14,BSDFile);
        fread(&Number,sizeof(Number),1,BSDFile);
    }
    fseek(BSDFile,-sizeof(Number),SEEK_CUR);
    MemEnd = GetCurrentFilePosition(BSDFile);
    assert( (MemEnd - MemBegin) == 320 );
    DPrintf("Mem end at %i\n",GetCurrentFilePosition(BSDFile));
    
    //This file section of 80 bytes contains the begin/end offset of some entries.
    //Like the NodeListTableStart/NodeListEnd after the header (+2048).
    //We can have a max of 20 offsets or 10 Begin/End Definitions.
    BSDReadEntryTableChunk(BSD,BSDFile);
    
    BSDReadSkyChunk(BSD,BSDFile);
    
    if( GameEngine == MOH_GAME_UNDERGROUND ) {
        SkipFileSection(16,BSDFile);
    }
    
    DPrintf("Current Position after entries is %i\n",GetCurrentFilePosition(BSDFile));
    BSDReadRenderObjectChunk(BSD,GameEngine,BSDFile);
    DPrintf("Current Position after RenderObject Table is: %i\n",GetCurrentFilePosition(BSDFile));
    //NOTE(Adriano):Altough we are able to load all the animated lights and grab the color data from there, BSD files are not meant to be read
    //              sequentially, this means that we need to skip a certain amount of bytes which corresponds to the area pointed 
    //              by each animated light,that contains a list of color values,in order to guarantee that we are reading it correctly.
    if( BSD->AnimatedLightsTable.NumAnimatedLights != 0 ) {
        DPrintf("Skipping block referenced by Animated lights Table...\n");
        AnimatedLight = &BSD->AnimatedLightsTable.AnimatedLightsList[BSD->AnimatedLightsTable.NumAnimatedLights - 1];
        Jump = ((AnimatedLight->StartingColorOffset + 2048) + (AnimatedLight->NumColors * 4)) - GetCurrentFilePosition(BSDFile);
        DPrintf("Skipping %i Bytes...\n",Jump);
        assert(Jump > 0);
        SkipFileSection(Jump,BSDFile);
    }
    DPrintf("Current Position after Color List Block is: %i\n",GetCurrentFilePosition(BSDFile));
    BSDReadNodeChunk(BSD,IsMultiplayer,BSDFile);
    BSDReadPropertySetFile(BSD,BSDFile);
    fclose(BSDFile);
    return 1;
}


FILE *BSDEarlyInit(BSD_t **BSD,char *MissionPath,int MissionNumber,int LevelNumber)
{
    BSD_t *LocalBSD;
    FILE *BSDFile;
    char Buffer[512];
    int i;
    
    snprintf(Buffer,sizeof(Buffer),"%s/%i_%i.BSD",MissionPath,MissionNumber,LevelNumber);

    DPrintf("BSDEarlyInit:Loading BSD file %s...\n",Buffer);
    
    BSDFile = fopen(Buffer,"rb");
    
    if( BSDFile == NULL ) {
        DPrintf("BSDEarlyInit:Failed opening BSD File.\n");
        return NULL;
    }
    LocalBSD = malloc(sizeof(BSD_t));
    
    LocalBSD->TSPStreamNodeList = NULL;
//     BSD->RenderObjectRealList = NULL;
    LocalBSD->RenderObjectList = NULL;
    LocalBSD->RenderObjectDrawableList = NULL;
//     BSD->RenderObjectShowCaseList = NULL;
    
    LocalBSD->NodeVAO = NULL;
    LocalBSD->RenderObjectPointVAO = NULL;
    LocalBSD->NumRenderObjectPoint = 0;
    LocalBSD->SkyData.MoonVAO = NULL;
    LocalBSD->SkyData.StarsVAO = NULL;

    
    assert(sizeof(LocalBSD->Header) == 2048);
    fread(&LocalBSD->Header,sizeof(LocalBSD->Header),1,BSDFile);
    DPrintf("BSDEarlyInit: BSD Header contains %i(%#02x) element.\n",LocalBSD->Header.NumHeadElements,LocalBSD->Header.NumHeadElements);
    for( i = 0; i < LocalBSD->Header.NumHeadElements; i++ ) {
        DPrintf("BSDEarlyInit:Got %i(%#02x)(%i)\n",LocalBSD->Header.Sector[i],LocalBSD->Header.Sector[i],LocalBSD->Header.Sector[i] >> 0xb);
    }
    //At position 152 after the header we have the SPRITE definitions...
    //Maybe hud/ammo...
    fread(&LocalBSD->TSPInfo,sizeof(LocalBSD->TSPInfo),1,BSDFile);
    DPrintf("BSDEarlyInit:Reading TSP info.\n");
    DPrintf("Compartment pattern: %s\n",LocalBSD->TSPInfo.TSPPattern);
    DPrintf("Number of compartments: %i\n",LocalBSD->TSPInfo.NumTSP);
    DPrintf("TargetInitialCompartment: %i\n",LocalBSD->TSPInfo.TargetInitialCompartment);
    DPrintf("Starting Compartment: %i\n",LocalBSD->TSPInfo.StartingComparment);
    DPrintf("u3: %i\n",LocalBSD->TSPInfo.u3);
    DPrintf("TSP Block ends at %i\n",GetCurrentFilePosition(BSDFile));
    assert(LocalBSD->TSPInfo.u3 == 0);
    *BSD = LocalBSD;
    return BSDFile;
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
