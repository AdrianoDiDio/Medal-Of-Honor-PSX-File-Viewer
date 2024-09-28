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

typedef struct BSDRenderObject_s {
    int                         Id;
    int                         ReferencedRenderObjectId;
    int                         Type;
    BSDVertexTable_t            *VertexTable;
    BSDVertexTable_t            *CurrentVertexTable;
    int                         NumVertexTables;
    BSDAnimatedModelFace_t      *FaceList;
    int                         NumFaces;
    BSDHierarchyBone_t          *HierarchyDataRoot;
    BSDAnimation_t              *AnimationList;
    int                         NumAnimations;
    int                         CurrentAnimationIndex;
    int                         CurrentFrameIndex;
    
    BSDVertex_t                 *VertexList;
    BSDColor_t                  *ColorList;
    BSDFace_t                   *StaticFaceList;

    vec3                        Scale;
    vec3                        Center;
    VAO_t                       *VAO;
    
    bool                        IsStatic;

    struct BSDRenderObject_s *Next;
} BSDRenderObject_t;

typedef struct BSD_s {
    BSDEntryTable_t         EntryTable;
    BSDRenderObjectBlock_t  RenderObjectTable;
} BSD_t;

typedef struct Camera_s Camera_t;

BSDRenderObject_t       *BSDLoadAllAnimatedRenderObjects(const char *FName,int *GameVersion);
void                    BSDDrawRenderObjectList(BSDRenderObject_t *RenderObjectList,const VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix);
void                    BSDDrawRenderObject(BSDRenderObject_t *RenderObject,const VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix);
void                    BSDRecursivelyApplyHierachyData(const BSDHierarchyBone_t *Bone,const BSDQuaternion_t *QuaternionList,
                                                    BSDVertexTable_t *VertexTable,mat4 TransformMatrix);
int                     BSDRenderObjectSetAnimationPose(BSDRenderObject_t *RenderObject,int AnimationIndex,int FrameIndex,int Override);
BSDAnimationFrame_t     *BSDRenderObjectGetCurrentFrame(BSDRenderObject_t *RenderObject);
void                    BSDRenderObjectResetFrameQuaternionList(BSDAnimationFrame_t *Frame);

void                    BSDRenderObjectGenerateVAO(BSDRenderObject_t *RenderObject);
void                    BSDRenderObjectGenerateStaticVAO(BSDRenderObject_t *RenderObject);
void                    BSDRenderObjectExportCurrentPoseToPly(BSDRenderObject_t *RenderObject,VRAM_t *VRAM,FILE *OutFile);
void                    BSDRenderObjectExportCurrentAnimationToPly(BSDRenderObject_t *RenderObject,VRAM_t *VRAM,const char *Directory,
                                                                   const char *EngineName);
void                    BSDFree(BSD_t *BSD);
void                    BSDFreeRenderObjectList(BSDRenderObject_t *RenderObjectList);


#endif //__BSD_H_
