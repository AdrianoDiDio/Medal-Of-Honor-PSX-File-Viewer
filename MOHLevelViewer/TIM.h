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
#ifndef __TIM_H_
#define __TIM_H_

#include "Common.h"

typedef enum {
    BPP_4 = 8,
    BPP_4_NO_CLUT = 0,
    BPP_8 = 9,
    BPP_8_NO_CLUT = 1,
    BPP_16 = 2,
    BPP_24 = 3
} TIMBPP;

typedef struct TIMHeader_s {
    unsigned int Magic; // Always 10 00 00 00 or 0x10
    unsigned int BPP;
    unsigned int CLUTSize;
    unsigned short CLUTOrgX; // Hard-Wired clut memory location.
    unsigned short CLUTOrgY; // Hard-Wired clut memory location.
    unsigned short NumClutColors;
    unsigned short NumCluts;
} TIMHeader_t;

typedef struct TIMImage_s {
    char        Name[256];
    TIMHeader_t Header;
    unsigned int NumPixels;
    unsigned short RowCount; // in Direct color mode (16/24 BPP) is equal to the Width.
    unsigned short Width;
    unsigned short Height;
    unsigned short FrameBufferX; // Hard-Wired texture memory location.
    unsigned short FrameBufferY; // Hard-Wired texture memory location.
    unsigned int   TexturePage;
    unsigned int   CLUTTexturePage;
    unsigned short *CLUT;
    unsigned short /*Byte*/ *Data;
    struct TIMImage_s *Next;
} TIMImage_t;

TIMImage_t *TIMLoadImage(FILE *TIMImage,int NumImages);
TIMImage_t *TIMGetAllImages(char *File);
Byte *TIMExpandCLUTImageData(TIMImage_t *Image);
Byte *TIMToOpenGL24(TIMImage_t *Image);
Byte *TIMToOpenGL32(TIMImage_t *Image);
void TIMImageListFree(TIMImage_t *ImageList);
#endif //__TIM_H_
