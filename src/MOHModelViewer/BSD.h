/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
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

#ifndef __BSD_H_
#define __BSD_H_

#include "../Common/Common.h"
#include "../Common/VAO.h"
#include "../Common/VRAM.h"
#include "../Common/BSDUtils.h"
#include "../Common/RenderObject.h"

typedef struct BSD_s {
    BSDEntryTable_t         EntryTable;
    BSDRenderObjectTable_t  RenderObjectTable;
} BSD_t;

typedef struct Camera_s Camera_t;

RenderObject_t       *BSDLoadAllAnimatedRenderObjects(const char *FName,int *GameVersion);
void                    BSDDrawRenderObjectList(RenderObject_t *RenderObjectList,const VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix);
void                    BSDDrawRenderObject(RenderObject_t *RenderObject,const VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix);
void                    BSDRecursivelyApplyHierachyData(const BSDHierarchyBone_t *Bone,const BSDQuaternion_t *QuaternionList,
                                                    BSDVertexTable_t *VertexTable,mat4 TransformMatrix);
int                     BSDRenderObjectSetAnimationPose(RenderObject_t *RenderObject,int AnimationIndex,int FrameIndex,int Override);
BSDAnimationFrame_t     *BSDRenderObjectGetCurrentFrame(RenderObject_t *RenderObject);
void                    BSDRenderObjectResetFrameQuaternionList(BSDAnimationFrame_t *Frame);

void                    BSDRenderObjectGenerateVAO(RenderObject_t *RenderObject);
void                    BSDRenderObjectGenerateStaticVAO(RenderObject_t *RenderObject);
void                    BSDRenderObjectExportCurrentPoseToPly(RenderObject_t *RenderObject,VRAM_t *VRAM,FILE *OutFile);
void                    BSDRenderObjectExportCurrentAnimationToPly(RenderObject_t *RenderObject,VRAM_t *VRAM,const char *Directory,
                                                                   const char *EngineName);
void                    BSDFree(BSD_t *BSD);
void                    BSDFreeRenderObjectList(RenderObject_t *RenderObjectList);


#endif //__BSD_H_
