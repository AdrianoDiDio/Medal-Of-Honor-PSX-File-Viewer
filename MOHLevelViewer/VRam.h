/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
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
#ifndef __VRAM_H_
#define __VRAM_H_



typedef struct VRamPage_s {
    int ID;
    unsigned int TextureID;
    SDL_Surface *Surface;
    Byte *Data; //RGB Pixel value.
} VRamPage_t;

typedef struct VRam_s {
    VRamPage_t Page4Bit[32];
    VRamPage_t Page8Bit[32];
} VRam_t;

VRam_t *VRamInit(TIMImage_t *ImageList);

#endif //__VRAM_H_
