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
#include "SST.h"
#include "../Common/RSC.h"
#include "../Common/TIM.h"
#include "SSTViewer.h" 


/*
 Every SST File starts with a type 1 section containing the name of the script.
 Basic Idea to begin loading it is to read and link section 2 and 3.
 
 typedef struct SSTCallback_s {
    char  SrcEvent[28];
    char  DestEvent[28];
    int   Unknown;
 }
 
 typedef struct SSTLabelData_s {
    
    SSTLabelData_s *Next;
 }
 typedef struct SSTLabel {
    SSTCallback_t *Callback;
    SSTLabelData_t *Data;
 }
 
 */
SSTGFX_t Models[5];
VAO_t  *LabelsVao[512];

int NumModels;

void SSTFree(SST_t *SST)
{
    SSTClass_t *Temp;
    
    while( SST->ClassList ) {
        Temp = SST->ClassList;
        if( Temp->Callback ) {
            free(Temp->Callback);
        }
        if( Temp->VideoInfo ) {
            free(Temp->VideoInfo);
        }
        SST->ClassList = SST->ClassList->Next;
        free(Temp);
    }
    free(SST);
}
int SSTCompare( const void *a, const void *b)
{
    SSTLabel_t *LabelA = (SSTLabel_t*) a;
    SSTLabel_t *LabelB = (SSTLabel_t*) b;
    int int_a = LabelA->Depth;
    int int_b = LabelB->Depth;

    if ( int_a == int_b ) return 0;
    else if ( int_a < int_b ) return 1;
    else return -1;
}


void SSTPrepareLabelVAO(SSTLabel_t *Label,VRAM_t* VRAM,int Index)
{
    float x0,y0;
    float u0,v0;
    float x1,y1;
    float u1,v1;
    float x2,y2;
    float u2,v2;
    float x3,y3;
    float u3,v3;
    float TextureWidth = 256.f;
    float TextureHeight = 256.f;
    float BaseTextureX;
    float BaseTextureY;
    float ColorModeOffset;
    float *VertexData;
    int Stride;
    int DataSize;
    int VertexPointer;
    int TextureID;
    //        XYZ  UV
    Stride = (3 + 2) * sizeof(float);
    //We need 6 vertices to describe a quad...
    DataSize = Stride * 6;
    
    VertexData = malloc(Stride * 6/** sizeof(float)*/);
    VertexPointer = 0;
        
//     if( Label->ImageInfo.ColorMode == BPP_4 ) {
//         ColorModeOffset = 4;
// //         TextureID = VRam->Page4Bit[Label->ImageInfo.TexturePage].TextureID;
//     } else {
//         ColorModeOffset = 2;
// //         TextureID = VRam->Page8Bit[Label->ImageInfo.TexturePage].TextureID;
//     }
        
    if( Label->ImageInfo.FrameBufferY >= 256 ) {
        BaseTextureX = (Label->ImageInfo.FrameBufferX - ((Label->ImageInfo.TexturePage - 16) * 64)) * ColorModeOffset;
        BaseTextureY = Label->ImageInfo.FrameBufferY - 256;
    } else {
        BaseTextureX = (Label->ImageInfo.FrameBufferX - (Label->ImageInfo.TexturePage * 64)) * ColorModeOffset;
        BaseTextureY = Label->ImageInfo.FrameBufferY;
    }

    if( Label->Unknown2 == 0 ) {
        u0 = BaseTextureX / TextureWidth;
        v0 = BaseTextureY / TextureHeight;
        u1 = BaseTextureX / TextureWidth;
        v1 = (BaseTextureY + Label->ImageInfo.Height) / TextureHeight;
        u2 = (BaseTextureX + Label->ImageInfo.Width ) / TextureWidth;
        v2 = (BaseTextureY + Label->ImageInfo.Height) / TextureHeight;
        u3 = (BaseTextureX + Label->ImageInfo.Width) / TextureWidth;
        v3 =  BaseTextureY / TextureHeight;
        //NOTE(Adriano):Rotate the label if necessary...
        if( Label->Unknown3 ) {
            x0 = Label->x;
            y0 = Label->y;
            x1 = x0;
            y1 = Label->y + Label->Height;
            x2 = Label->x  + Label->Width;
            y2 = y1;
            x3 = x2;
            y3 = y0;
        } else {
            x0 = Label->x;
            y0 = Label->y;
            x1 = x0;
            y1 = Label->y + Label->ImageInfo.Height;
            x2 = Label->x  + Label->ImageInfo.Width;
            y2 = y1;
            x3 = x2;
            y3 = y0;
        }

    } else {
        u0 = BaseTextureX / TextureWidth;
        v0 = (BaseTextureY + Label->ImageInfo.Height) / TextureHeight;
        u1 = (BaseTextureX + Label->ImageInfo.Width) / TextureWidth;
        v1 = (BaseTextureY + Label->ImageInfo.Height) / TextureHeight;
        u2 = (BaseTextureX + Label->ImageInfo.Width) / TextureWidth;
        v2 = BaseTextureY / TextureHeight;
        u3 = BaseTextureX / TextureWidth;
        v3 = BaseTextureY / TextureHeight;
        //NOTE(Adriano):Rotate the label if necessary...
        if( Label->Unknown3 ) {
            x0 = Label->x;
            y0 = Label->y;
            x1 = x0;
            y1 = Label->y + Label->Height;
            x2 = Label->x  + Label->Width;
            y2 = y1;
            x3 = x2;
            y3 = y0;
        } else {
            x0 = Label->x;
            y0 = Label->y;
            x1 = x0;
            y1 = Label->y + Label->ImageInfo.Width;
            x2 = Label->x  + Label->ImageInfo.Height;
            y2 = y1;
            x3 = x2;
            y3 = y0;
        }
    }
    

    VertexData[VertexPointer] =  x1;
    VertexData[VertexPointer+1] = y1;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u1;
    VertexData[VertexPointer+4] = v1;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x0;
    VertexData[VertexPointer+1] = y0;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u0;
    VertexData[VertexPointer+4] = v0;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x2;
    VertexData[VertexPointer+1] = y2;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u2;
    VertexData[VertexPointer+4] = v2;
    VertexPointer += 5;
            

    VertexData[VertexPointer] =  x2;
    VertexData[VertexPointer+1] = y2;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u2;
    VertexData[VertexPointer+4] = v2;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x0;
    VertexData[VertexPointer+1] = y0;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u0;
    VertexData[VertexPointer+4] = v0;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x3;
    VertexData[VertexPointer+1] = y3;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u3;
    VertexData[VertexPointer+4] = v3;
    VertexPointer += 5;

//     LabelsVao[Index] = VaoInitXYZUV(VertexData,DataSize,Stride,0,3,-1,TextureID);
    free(VertexData);
}
void SSTModelRender(VRAM_t *VRAM)
{
    int i;
    for( i = 0; i < NumModels; i++ ) {
        GFXRender(Models[i].Model,VRAM);
    }
}
void SSTRender(VRAM_t *VRAM)
{
//     GL_Shader_t *Shader;
//     float PsxScreenWidth = 512.f;
//     float PsxScreenHeight = 256.f;
//     int OrthoMatrixID;
//     int i;
//     
//     Shader = Shader_Cache("SSTShader","Shaders/SSTVertexShader.glsl","Shaders/SSTFragmentShader.glsl");
//     
//     for( i = 0; i < NumLabels; i++ ) {
//         glUseProgram(Shader->ProgramID);
//         OrthoMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
// 
//         glm_mat4_identity(VidConf.ModelViewMatrix);
//         vec3 v;
//         v[0] = (VidConf.Width / PsxScreenWidth);
//         v[1] = (VidConf.Height / PsxScreenHeight);
//         v[3] = 0;
//         glm_scale(VidConf.ModelViewMatrix,v);
//         glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
//         glUniformMatrix4fv(OrthoMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
//         glBindTexture(GL_TEXTURE_2D, LabelsVao[i]->TextureID);
//         glBindVertexArray(LabelsVao[i]->VaoID[0]);
//         glDrawArrays(GL_TRIANGLES, 0, 6);
//         glBindVertexArray(0);
//     }
//     SSTModelRender(VRam);
}

void SSTLateInit(VRAM_t* VRAM)
{
    int i;
    /*for( i = 0; i < NumLabels; i++ ) {
        SSTPrepareLabelVAO(&Label[i],VRAM,i);
    }*/
    for( i = 0; i < NumModels; i++ ) {
        GFXPrepareVAO(Models[i].Model);
    }
}

void SSTLoadLabel(SST_t *SST, SSTClass_t *Class,RSC_t *RSC,Byte **SSTBuffer)
{
    SSTLabel_t *Label;
    TIMImage_t *Image;
    RSCEntry_t Entry;
    int Ret;
    
    Label = malloc(sizeof(SSTLabel_t));
    if( !Label ) {
        DPrintf("SSTLoadLabel: Failed to load label for class %s\n",Class->Name);
        return;
    }
    Label->Next = NULL;
    memcpy(&Label->TextureFile,SSTBuffer,sizeof(Label->TextureFile));
    SSTBuffer += sizeof(Label->TextureFile);
    Label->Unknown = *(int *) SSTBuffer;
    SSTBuffer += 4;
    Label->x = *(short *) SSTBuffer;
    SSTBuffer += 2;
    Label->Pad1 = *(short *) SSTBuffer;
    SSTBuffer += 2;
    Label->y = *(short *) SSTBuffer;
    SSTBuffer += 2;
    Label->Pad2 = *(short *) SSTBuffer;
    SSTBuffer += 2;
    Label->Width = *(short *) SSTBuffer;
    SSTBuffer += 2;
    Label->Pad3 = *(short *) SSTBuffer;
    SSTBuffer += 2;
    Label->Height = *(short *) SSTBuffer;
    SSTBuffer += 2;
    Label->Pad4 = *(short *) SSTBuffer;
    SSTBuffer += 2;
    Label->Unknown2 = *(Byte *) SSTBuffer;
    SSTBuffer += 1;
    Label->Unknown3 = *(Byte *) SSTBuffer;
    SSTBuffer += 1;
    Label->Unknown4 = *(Byte *) SSTBuffer;
    SSTBuffer += 1;
    Label->Unknown5 = *(Byte *) SSTBuffer;
    SSTBuffer += 1;
    Label->Depth = *(int *) SSTBuffer;
    SSTBuffer += 4;
    memcpy(&Label->Unknown6,SSTBuffer,sizeof(Label->Unknown6));
    SSTBuffer += sizeof(Label->Unknown6);
    //Link it in!
    Label->Next = Class->LabelList;
    Class->LabelList = Label;
    if( strcmp(Label->TextureFile,"NULL") != 0 ) {
        if( Label->y > 512 ) {
            //Clamp to Height
            Label->y = 0;
        }
        DPrintf("SSTLoad:Label Texture:%s Unknown1:%i X:%i Y:%i Width:%i Height:%i Depth:%i %i %i %i %i\n",Label->TextureFile,
                Label->Unknown,Label->x,
                Label->y,Label->Width,Label->Height,Label->Depth,Label->Unknown2,
                Label->Unknown3,Label->Unknown4,Label->Unknown5
        );
        DPrintf("Unk6:");
        for( int i = 0; i < 12 ; i++ ) {
            DPrintf(" %i ",Label->Unknown6[i]);
        }
        DPrintf("\n");    
        Ret = RSCOpen(RSC,Label->TextureFile,&Entry);
        if( Ret > 0 ) {
            DPrintf("Texture is %s\n",Entry.Name);
            Image = TIMLoadAllImagesFromBuffer(Entry.Data);
            Label->ImageInfo.TexturePage = Image->TexturePage;
            Label->ImageInfo.FrameBufferX = Image->FrameBufferX;
            Label->ImageInfo.FrameBufferY = Image->FrameBufferY;
            Label->ImageInfo.Width = Image->Width;
            Label->ImageInfo.Height = Image->Height;
            Label->ImageInfo.ColorMode = Image->Header.BPP;
            Image->Next = SST->ImageList;
            SST->ImageList = Image;
        }
    }
    return;
}

SST_t *SSTLoad(Byte *SSTBuffer)
{
    SST_t *SST;
    SSTClass_t *CurrrentClass;
    SSTCallback_t SSTCallback;
    SSTVideoInfo_t *SSTVideoInfo;
    SSTLabel_t *SSTLabel;
    SSTLabel_t *TempLabel;
    RSC_t *RSCData;
    RSC_t *RSCData2;
    RSCEntry_t Entry;
    char Name[28];
    int Size;
    int Token;
    int Ret;
    int StoreLabel;
    
    if( !SSTBuffer ) {
        DPrintf("SSTLoad:Invalid data.\n");
        return NULL;
    }
    DPrintf("Loading it\n");
    
    SST = malloc(sizeof(SST_t));
    
    if( !SST ) {
        DPrintf("SSTLoad:Failed to allocate memory for struct\n");
        return NULL;
    }
    NumModels = 0;
    SST->Next = NULL;
    SST->ImageList = NULL;
    SST->ClassList = NULL;
    RSCData = RSCLoad("SSTScripts/mdev.rsc");
    RSCData2 = RSCLoad("SSTScripts/mdev2.rsc");

    StoreLabel = 0;

    while( 1 ) {
        if( !*SSTBuffer ) {
            DPrintf("SSTLoad:EOF reached or an error occurred...\n");
            break;
        }
        Token = *(int *) SSTBuffer;
        SSTBuffer += 4;
        DPrintf("SSTLoad:Got token %i\n",Token);
        switch( Token ) {
            case 1:
                CurrrentClass = malloc(sizeof(SSTClass_t));
                CurrrentClass->LabelList = NULL;
                CurrrentClass->Callback = NULL;
                CurrrentClass->Next = NULL;
                memcpy(&CurrrentClass->Name,SSTBuffer,sizeof(CurrrentClass->Name));
                SSTBuffer += sizeof(CurrrentClass->Name);
                //Link it in!
                CurrrentClass->Next = SST->ClassList;
                SST->ClassList = CurrrentClass;
                DPrintf("SSTLoad:Class Name is %s\n",CurrrentClass->Name);
                break;
            case 2:
                if( CurrrentClass->Callback ) {
                    DPrintf("SSTLoad: Main class should only have one callback...\n");
                    assert(1!=1);
                    break;
                }
                CurrrentClass->Callback = malloc(sizeof(SSTCallback_t));
                memcpy(CurrrentClass->Callback,SSTBuffer,sizeof(SSTCallback_t));
                SSTBuffer += sizeof(SSTCallback_t);
                DPrintf("SSTLoad:Callback SrcEvent:%s DestEvent:%s Unknown: %i\n",CurrrentClass->Callback->SrcEvent,
                        CurrrentClass->Callback->DestEvent,CurrrentClass->Callback->Unknown);
                //StoreLabel = 0;
                break;
            case 3:
                SSTLoadLabel(SST,CurrrentClass,RSCData,&SSTBuffer);
                break;
            case 5:
                DPrintf("BackDrop declaration started.\n");
                //StoreLabel = 1;
                break;
            case 7:
                DPrintf("STR file declaration\n");
                CurrrentClass->VideoInfo = malloc(sizeof(SSTVideoInfo_t));
                memcpy(CurrrentClass->VideoInfo,SSTBuffer,sizeof(SSTVideoInfo_t));
                SSTBuffer += sizeof(SSTVideoInfo_t);
                DPrintf("SSTLoad:Callback STR file:%s Unknown:%i Unknown2: %i\n",CurrrentClass->VideoInfo->STRFile,
                        CurrrentClass->VideoInfo->Unknown,CurrrentClass->VideoInfo->Unknown2);
                break;
            case 8:
                SSTBuffer += 276;
                break;
            case 9:
                memcpy(&Size,SSTBuffer,sizeof(Size));
                SSTBuffer += sizeof(Size);
                SSTBuffer += (4*Size) + 4;
                break;
            case 10:
                //GFX Model
                memcpy(&Name,SSTBuffer,sizeof(Name));
                SSTBuffer += sizeof(Name);
//                 if( !strcmp(Name,"global2\\model\\clerkb.gfx") ) {
//                     SkipFileSection(SSTFile,84);
//                     break;
//                 }
                DPrintf("Loading model %s\n",Name);
                Ret = RSCOpen(RSCData,Name,&Entry);
                if( Ret < 0 ) {
                    Ret = RSCOpen(RSCData2,Name,&Entry);
                    if( Ret < 0 ) {
                        DPrintf("File was not found inside RSC...%s\n",Name);
                        break;
                    }
                }
                Models[NumModels].Model = GFXRead(Entry.Data);
                memcpy(&Name,SSTBuffer,sizeof(Name));
                SSTBuffer += sizeof(Name);
                DPrintf("Loading texture %s\n",Name);
                Ret = RSCOpen(RSCData,Name,&Entry);
                if( Ret < 0 ) {
                    Ret = RSCOpen(RSCData2,Name,&Entry);
                    if( Ret < 0 ) {
                        DPrintf("File was not found inside RSC...%s\n",Name);
                        break;
                    }
                }
                int NumImages = 0;
                while( Entry.Data ) {
                    Models[NumModels].Image = TIMLoadImageFromBuffer(&Entry.Data,NumImages);
                    if( Models[NumModels].Image == NULL ) {
                        DPrintf("Image is NULL skippin\n");
                        break;
                    }
                    Models[NumModels].Image->Next = SST->ImageList;
                    SST->ImageList = Models[NumModels].Image;
                    NumImages++;
                }
                SSTBuffer += 56;
//                 exit(0);
                NumModels++;
                break;
            case 11:
                //After a GFX Model we have this token.
                SSTBuffer += 288;
                break;
            default:
                DPrintf("SSTLoad:Unknown token %i\n",Token);
                assert(1!=1);
                break;
        }
    }
    //qsort( &Label, NumLabels, sizeof(SSTLabel_t), SSTCompare );
    RSCFree(RSCData);
    return SST;
    // Test
}
