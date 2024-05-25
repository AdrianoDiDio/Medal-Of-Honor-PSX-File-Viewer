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

#include "../Common/VRAM.h"
#include "GFX.h"

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
    char TextureFile[28];
    int  Unknown;
    unsigned short  x;
    short Pad1;
    unsigned short  y;
    short Pad2;
    unsigned short Width;
    short Pad3;
    unsigned short Height;
    short Pad4;
    Byte  Unknown2;
    Byte  Unknown3;
    Byte  Unknown4;
    Byte  Unknown5;
    int  Depth; // Z-Value
    char Unknown6[12];
    
    //Additional Data
    SSTImageInfo_t ImageInfo;
    struct SSTLabel_s *Next;
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
    struct SSTClass_s   *Next;
} SSTClass_t;
typedef struct SSTGFX_s {
    GFX_t *Model;
    TIMImage_t *Image;
} SSTGFX_t;

typedef struct SST_s {
    SSTHeader_t     Header;
    TIMImage_t      *ImageList;
    SSTClass_t      *ClassList;
    struct SST_s    *Next;
} SST_t;

SST_t *SSTLoad(Byte *SSTBuffer,int GameEngine);
void SSTLateInit(VRAM_t* VRam);
void SSTRender(VRAM_t *VRam);
void SSTModelRender(VRAM_t *VRam);
void SSTFree(SST_t *SST);

#endif//__SST_H_
 
