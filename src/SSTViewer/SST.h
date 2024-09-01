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
#ifndef __SST_H_
#define __SST_H_

#include "../Common/RSC.h"
#include "../Common/VRAM.h"
#include "../Common/GFX.h"

typedef enum 
{
    SST_CLASS_TOKEN = 1,
    SST_CALLBACK_TOKEN = 2,
    SST_LABEL_TOKEN = 3,
    SST_BACKDROP_TOKEN = 5,
    SST_STR_FILE_TOKEN = 7,
    SST_UNKNOWN_1_TOKEN = 8,
    SST_UNKNOWN_2_TOKEN = 9,
    SST_GFX_TOKEN = 10,
    SST_UNKNOWN_3_TOKEN = 11,
} SSTToken_t;
typedef struct SSTRSCMap_s {
    const char *ClassName;
    int NumRSC;
    const char **RSCList;
} SSTRSCMap_t;

typedef struct SSTHeader_s {
    char Name[28];
} SSTHeader_t;

typedef struct SSTCallback_s {
    char SrcEvent[28];
    char DestEvent[28];
    int  Unknown;
    
    struct SSTCallback_s *Next;
} SSTCallback_t;

typedef struct SSTImageInfo_s {
    int TexturePage;
    int FrameBufferX;
    int FrameBufferY;
    int CLUTX;
    int CLUTY;
    int CLUTPage;
    int Width;
    int Height;
    int ColorMode;
} SSTImageInfo_t;

typedef struct SSTVideoInfo_s {
    char STRFile[28];
    int  Unknown;
    int  Unknown2;
} SSTVideoInfo_t;

typedef struct SSTLabel_s {
    char                TextureFile[28];
    int                 Unknown;
    unsigned short      x;
    short               Pad1;
    unsigned short      y;
    short               Pad2;
    unsigned short      Width;
    short               Pad3;
    unsigned short      Height;
    short               Pad4;
    Byte                FlipTexture;
    Byte                UseLabelSize;
    Byte                Unknown4;
    Byte                Unknown5;
    int                 Depth; // Z-Value
    Color1i_t           Color0;
    Color1i_t           Color1;
    Color1i_t           Color2;
    
    //Additional Data
    SSTImageInfo_t      ImageInfo;
    SSTCallback_t       *Callback;
    struct SSTLabel_s   *Next;
} SSTLabel_t;

typedef struct SSTCallbackData_s {
    SSTCallback_t Callback;
    SSTLabel_t *LabelList;
    
    struct SSTCallbackData_s *Next;
} SSTCallbackData_t;

typedef struct SSTClass_s {
    char                Name[28];
    SSTCallback_t       *CallbackList;
    SSTLabel_t          *LabelList;
    SSTVideoInfo_t      *VideoInfo;
    GFX_t               *GFXModelList;
    RSC_t               *RSCList;
    TIMImage_t          *ImageList;
    VRAM_t              *VRAM;
    VAO_t               *LabelsVAO;
    int                 NumLabels;
    struct SSTClass_s   *Next;
} SSTClass_t;
typedef struct SSTGFX_s {
    GFX_t *Model;
    TIMImage_t *Image;
} SSTGFX_t;

typedef struct SST_s {
    char            *Name;
    SSTClass_t      *ClassList;
    struct SST_s    *Next;
} SST_t;

typedef struct GFXVector_s {
    int x;
    int y;
    int z;
    int Pad;
} GFXVector_t;

SST_t   *SSTLoad(Byte *SSTBuffer,const char *ScriptName,const char *BasePath,const RSC_t *GlobalRSCList,int GameEngine);
void    SSTGenerateVAOs(SST_t *SST);
void    SSTUnload(SST_t *SST);
void    SSTRender(SST_t *SST,mat4 ProjectionMatrix);
void    SSTModelRender(VRAM_t *VRam);
void    SSTFree(SST_t *SST);

#endif//__SST_H_
 
