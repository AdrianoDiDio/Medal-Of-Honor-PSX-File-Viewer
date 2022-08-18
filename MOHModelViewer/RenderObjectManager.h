// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
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
#ifndef __RENDER_OBJECT_MANAGER_H_
#define __RENDER_OBJECT_MANAGER_H_

#include "BSD.h"
#include "VRAM.h"
#include "TIM.h"
#include "Camera.h"

//NOTE(Adriano):A single BSD file that gets loaded together with his corresponding TAF goes
//              here...this allows for multiple BSD files to be loaded without overlapping VRAMs.
typedef struct BSDRenderObjectPack_s {
    VRAM_t                          *VRAM;
    TIMImage_t                      *ImageList;
    BSDRenderObject_t               *RenderObjectList;
    
    struct BSDRenderObjectPack_s    *Next;
} BSDRenderObjectPack_t;

typedef struct RenderObjectManager_s {
    BSDRenderObjectPack_t   *BSDList;
    
    unsigned int            FBO;
    unsigned int            FBOTexture;
    unsigned int            RBO;
} RenderObjectManager_t;

RenderObjectManager_t   *RenderObjectManagerInit();
void                    RenderObjectManagerDrawAll(RenderObjectManager_t *RenderObjectManager,Camera_t *Camera);
void                    RenderObjectManagerCleanUp(RenderObjectManager_t *RenderObjectManager);
int                     RenderObjectManagerLoadBSD(RenderObjectManager_t *RenderObjectManager,const char *File);
#endif//__RENDER_OBJECT_MANAGER_H_
