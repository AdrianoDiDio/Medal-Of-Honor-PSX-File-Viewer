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
#include "RenderObjectManager.h"
#include "MOHModelViewer.h"

void RenderObjectManagerFreeBSDRenderObjectPack(BSDRenderObjectPack_t *BSDRenderObjectPack)
{
    if( !BSDRenderObjectPack ) {
        return;
    }
    if( BSDRenderObjectPack->ImageList ) {
        TIMImageListFree(BSDRenderObjectPack->ImageList);
    }
    if( BSDRenderObjectPack->VRAM ) {
        VRAMFree(BSDRenderObjectPack->VRAM);
    }
    BSDFreeRenderObjectList(BSDRenderObjectPack->RenderObjectList);
    free(BSDRenderObjectPack);
}
void RenderObjectManagerCleanUp(RenderObjectManager_t *RenderObjectManager)
{
    BSDRenderObjectPack_t *Temp;
    if( !RenderObjectManager ) {
        return;
    }
    while(RenderObjectManager->BSDList) {
        Temp = RenderObjectManager->BSDList;
        RenderObjectManager->BSDList = RenderObjectManager->BSDList->Next;
        RenderObjectManagerFreeBSDRenderObjectPack(Temp);
    }
    free(RenderObjectManager);
}
void RenderObjectManagerDrawPack(RenderObjectManager_t *RenderObjectManager,BSDRenderObjectPack_t *RenderObjectPack,Camera_t *Camera,
                                 mat4 ProjectionMatrix)
{
    if( !RenderObjectManager ) {
        return;
    }
    if( !RenderObjectPack ) {
        return;
    }
    BSDDrawRenderObjectList(RenderObjectPack->RenderObjectList,RenderObjectPack->VRAM,Camera,ProjectionMatrix);
}
void RenderObjectManagerDrawAll(RenderObjectManager_t *RenderObjectManager,Camera_t *Camera)
{
    BSDRenderObjectPack_t *Iterator;
    mat4 ProjectionMatrix;
    
    if( !RenderObjectManager ) {
        return;
    }
    
    glm_perspective(glm_rad(110.f),(float) VidConfigWidth->IValue / (float) VidConfigHeight->IValue,1.f, 4096.f,ProjectionMatrix);     
    for( Iterator = RenderObjectManager->BSDList; Iterator; Iterator = Iterator->Next ) {
       RenderObjectManagerDrawPack(RenderObjectManager,Iterator,Camera,ProjectionMatrix); 
    }
}
int RenderObjectManagerLoadBSD(RenderObjectManager_t *RenderObjectManager,const char *File)
{
    BSDRenderObjectPack_t *BSDPack;
    BSDRenderObject_t *Iterator;
    char *TAFFile;
    
    if( !RenderObjectManager ) {
        DPrintf("RenderObjectManagerLoadBSD:Invalid RenderObjectManager\n");
        return 0;
    }
    if( !File ) {
        DPrintf("RenderObjectManagerLoadBSD:Invalid file name\n");
        return 0;
    }
    
    DPrintf("RenderObjectManagerLoadBSD:Attempting to load %s\n",File);
    BSDPack = malloc(sizeof(BSDRenderObjectPack_t));
    BSDPack->ImageList = NULL;
    BSDPack->VRAM = NULL;
    BSDPack->RenderObjectList = NULL;
    TAFFile = NULL;
    
    if( !BSDPack ) {
        DPrintf("RenderObjectManagerLoadBSD:Failed to allocate memory for BSD pack\n");
        goto Failure;
    }
    TAFFile = SwitchExt(File,".TAF");
    BSDPack->ImageList = TIMGetAllImages(TAFFile);
    if( !BSDPack->ImageList ) {
        DPrintf("RenderObjectManagerLoadBSD:Failed to load images from TAF file %s\n",TAFFile);
        goto Failure;
    }
    BSDPack->RenderObjectList = BSDLoadAllAnimatedRenderObjects(File);
    if( !BSDPack->RenderObjectList ) {
        DPrintf("RenderObjectManagerLoadBSD:Failed to load render objects from file\n");
        goto Failure;
    }
    BSDPack->VRAM = VRAMInit(BSDPack->ImageList);
    if( !BSDPack->VRAM ) {
        DPrintf("RenderObjectManagerLoadBSD:Failed to initialize VRAM\n");
        goto Failure;
    }
    for( Iterator = BSDPack->RenderObjectList; Iterator; Iterator = Iterator->Next ) {
        BSDRenderObjectSetAnimationPose(Iterator,0);
        BSDRenderObjectGenerateVAO(Iterator);
    }
    BSDPack->Next = RenderObjectManager->BSDList;
    RenderObjectManager->BSDList = BSDPack;
    free(TAFFile);
    return 1;
Failure:
    RenderObjectManagerFreeBSDRenderObjectPack(BSDPack);
    if( TAFFile ) {
        free(TAFFile);
    }
    return 0;
}
RenderObjectManager_t *RenderObjectManagerInit()
{
    RenderObjectManager_t *RenderObjectManager;
    
    RenderObjectManager = malloc(sizeof(RenderObjectManager_t));
    RenderObjectManager->BSDList = NULL;
    return RenderObjectManager;
}
