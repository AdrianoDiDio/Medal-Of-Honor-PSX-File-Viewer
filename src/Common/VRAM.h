/*
===========================================================================
    Copyright (C) 2018-2023 Adriano Di Dio.
    
    Medal-Of-Honor-PSX-File-Viewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Medal-Of-Honor-PSX-File-Viewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Medal-Of-Honor-PSX-File-Viewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#ifndef __VRAM_H_
#define __VRAM_H_

#include "Common.h"
#include "TIM.h"

typedef struct VRamPage_s {
    unsigned int TextureId;
    SDL_Surface *Surface;
    Byte *Data; //RGB Pixel value.
    float Width;
    float Height;
} VRAMPage_t;

typedef struct VRam_s {
    VRAMPage_t Page;
    VRAMPage_t PalettePage;
    VRAMPage_t TextureIndexPage;
} VRAM_t;

VRAM_t      *VRAMInit(TIMImage_t *ImageList);
void        VRAMFree(VRAM_t *VRAM);
int         VRAMGetTexturePageX(int VRAMPage);
int         VRAMGetTexturePageY(int VRAMPage,int ColorMode);
int         VRAMGetCLUTPage(int CLUTPosX,int CLUTPosY);
int         VRAMGetCLUTPositionX(int CLUTX,int CLUTY,int CLUTPage);
int         VRAMGetCLUTOffsetY(int ColorMode);
void        VRAMGetTIMImageCoordinates(TIMImage_t *Image,int *DestX,int *DestY);
void        VRAMDumpDataToFile(VRAM_t *VRam,const char *OutBaseDir);
void        VRAMSave(VRAM_t *VRAM,const char *File);
#endif //__VRAM_H_
