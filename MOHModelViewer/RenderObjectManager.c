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
    glDeleteFramebuffers(1,&RenderObjectManager->FBO);
    glDeleteRenderbuffers(1,&RenderObjectManager->RBO);
    glDeleteTextures(1,&RenderObjectManager->FBOTexture);
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
    glBindFramebuffer(GL_FRAMEBUFFER, RenderObjectManager->FBO);
    glViewport(0,0,VidConfigWidth->IValue,VidConfigHeight->IValue);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm_perspective(glm_rad(110.f),(float) VidConfigWidth->IValue / (float) VidConfigHeight->IValue,1.f, 4096.f,ProjectionMatrix);     
    for( Iterator = RenderObjectManager->BSDList; Iterator; Iterator = Iterator->Next ) {
       RenderObjectManagerDrawPack(RenderObjectManager,Iterator,Camera,ProjectionMatrix); 
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    
    if( !BSDPack ) {
        DPrintf("RenderObjectManagerLoadBSD:Failed to allocate memory for BSD pack\n");
        goto Failure;
    }
    
    BSDPack->ImageList = NULL;
    BSDPack->VRAM = NULL;
    BSDPack->RenderObjectList = NULL;
    TAFFile = NULL;

    TAFFile = SwitchExt(File,".TAF");
    BSDPack->ImageList = TIMGetAllImages(TAFFile);
    if( !BSDPack->ImageList ) {
        free(TAFFile);
        TAFFile = SwitchExt(File,"0.TAF");
        BSDPack->ImageList = TIMGetAllImages(TAFFile);
        if( !BSDPack->ImageList ) {
            DPrintf("RenderObjectManagerLoadBSD:Failed to load images from TAF file %s\n",TAFFile);
            goto Failure;
        }
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
int RenderObjectManagerCreateFBO(RenderObjectManager_t *RenderObjectManager)
{
    glGenFramebuffers(1, &RenderObjectManager->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, RenderObjectManager->FBO);
    
    glGenTextures(1, &RenderObjectManager->FBOTexture);
    glBindTexture(GL_TEXTURE_2D, RenderObjectManager->FBOTexture);
  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VidConfigWidth->IValue, VidConfigHeight->IValue, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderObjectManager->FBOTexture, 0);
    
    glGenRenderbuffers(1, &RenderObjectManager->RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RenderObjectManager->RBO); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, VidConfigWidth->IValue, VidConfigHeight->IValue);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderObjectManager->RBO);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        DPrintf("RenderObjectManagerCreateFBO:Error when creating RenderObjectManager FBO\n");
        return 0;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return 1;
}
RenderObjectManager_t *RenderObjectManagerInit()
{
    RenderObjectManager_t *RenderObjectManager;
    
    RenderObjectManager = malloc(sizeof(RenderObjectManager_t));
    if( !RenderObjectManager ) {
        DPrintf("RenderObjectManagerInit:Couldn't allocate memory for RenderObjectManager\n");
        return NULL;
    }
    RenderObjectManager->BSDList = NULL;
    
    if( !RenderObjectManagerCreateFBO(RenderObjectManager) ) {
        DPrintf("RenderObjectManagerInit:Couldn't create FBO\n");
        free(RenderObjectManager);
        return NULL;
    }
    return RenderObjectManager;
}
