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
    if( BSDRenderObjectPack->Name ) {
        free(BSDRenderObjectPack->Name);
    }
    BSDFreeRenderObjectList(BSDRenderObjectPack->RenderObjectList);
    free(BSDRenderObjectPack);
}

void RenderObjectManagerFreeDialogData(GUIFileDialog_t *FileDialog)
{
    RenderObjectManagerDialogData_t *DialogData;
    if( !FileDialog ) {
        return;
    }
    DialogData = (RenderObjectManagerDialogData_t *) GUIFileDialogGetUserData(FileDialog);
    if( DialogData ) {
        free(DialogData);
    }
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
    
    //If the user didn't close the dialog free the user data that we passed to it.
    if( GUIFileDialogIsOpen(RenderObjectManager->BSDFileDialog) ) {
        RenderObjectManagerFreeDialogData(RenderObjectManager->BSDFileDialog);
    }
    
    free(RenderObjectManager);
}


BSDRenderObjectPack_t *RenderObjectManagerGetSelectedBSDPack(RenderObjectManager_t *RenderObjectManager)
{
    if( !RenderObjectManager ) {
        DPrintf("RenderObjectManagerGetSelectedBSDPack:Invalid RenderObjectManager\n");
        return NULL;
    }
    return RenderObjectManager->SelectedBSDPack;

}
BSDRenderObject_t *RenderObjectManagerGetSelectedRenderObject(RenderObjectManager_t *RenderObjectManager)
{
    BSDRenderObjectPack_t *SelectedBSDPack;
    if( !RenderObjectManager ) {
        DPrintf("RenderObjectManagerGetSelectedRenderObject:Invalid RenderObjectManager\n");
        return NULL;
    }
    SelectedBSDPack = RenderObjectManagerGetSelectedBSDPack(RenderObjectManager);
    if( !SelectedBSDPack ) {
        return NULL;
    }
    return SelectedBSDPack->SelectedRenderObject;
}
void RenderObjectManagerSetSelectedRenderObject(RenderObjectManager_t *RenderObjectManager,BSDRenderObjectPack_t *SelectedBSDPack,
                                                BSDRenderObject_t *SelectedRenderObject)
{
    if( !RenderObjectManager ) {
        DPrintf("RenderObjectManagerSetSelectedRenderObject:Invalid RenderObjectManager\n");
        return;
    }
    if( !SelectedBSDPack ) {
        return;
    }
    if( !SelectedRenderObject ) {
        return;
    }
    RenderObjectManager->SelectedBSDPack = SelectedBSDPack;
    RenderObjectManager->SelectedBSDPack->SelectedRenderObject = SelectedRenderObject;
}
void RenderObjectManagerAppendBSDPack(RenderObjectManager_t *RenderObjectManager,BSDRenderObjectPack_t *BSDPack)
{
    BSDRenderObjectPack_t *LastNode;
    if( !RenderObjectManager->BSDList ) {
        RenderObjectManager->BSDList = BSDPack;
    } else {
        LastNode = RenderObjectManager->BSDList;
        while( LastNode->Next ) {
            LastNode = LastNode->Next;
        }
        LastNode->Next = BSDPack;
    }
}
int RenderObjectManagerDeleteBSDPackFromList(RenderObjectManager_t *RenderObjectManager,const char *BSDPackName)
{
    BSDRenderObjectPack_t *Temp;
    BSDRenderObjectPack_t *Curr;
    BSDRenderObjectPack_t *Prev;
    Temp = RenderObjectManager->BSDList;
    if(  Temp != NULL && !strcmp(Temp->Name,BSDPackName) ) {
        RenderObjectManager->BSDList = Temp->Next;
        RenderObjectManagerFreeBSDRenderObjectPack(Temp);
        return 1;
    }
    Curr = RenderObjectManager->BSDList->Next;
    Prev = RenderObjectManager->BSDList;
    while( Curr && Prev ) {
        if( !strcmp(Curr->Name,BSDPackName) ) {
            Temp = Curr;
            Prev->Next = Curr->Next;
            RenderObjectManagerFreeBSDRenderObjectPack(Temp);
            return 1;
        }
        Prev = Curr;
        Curr = Curr->Next;
    }
    return 0;
}
int RenderObjectManagerDeleteBSDPack(RenderObjectManager_t *RenderObjectManager,const char *BSDPackName)
{
    if( RenderObjectManagerDeleteBSDPackFromList(RenderObjectManager,BSDPackName) ) {
        RenderObjectManager->SelectedBSDPack = RenderObjectManager->BSDList;
        if( RenderObjectManager->BSDList != NULL ) {
            RenderObjectManager->SelectedBSDPack->SelectedRenderObject = RenderObjectManager->BSDList->RenderObjectList;
        }
        return 1;
    }
    return 0;
}
int RenderObjectManagerLoadBSD(RenderObjectManager_t *RenderObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *File)
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

    GUIProgressBarReset(GUI);
    
    DPrintf("RenderObjectManagerLoadBSD:Attempting to load %s\n",File);
    BSDPack = malloc(sizeof(BSDRenderObjectPack_t));
    
    if( !BSDPack ) {
        DPrintf("RenderObjectManagerLoadBSD:Failed to allocate memory for BSD pack\n");
        goto Failure;
    }
    BSDPack->Name = GetBaseName(File);
    BSDPack->ImageList = NULL;
    BSDPack->VRAM = NULL;
    BSDPack->RenderObjectList = NULL;
    BSDPack->SelectedRenderObject = NULL;
    BSDPack->Next = NULL;
    TAFFile = NULL;
    
    GUIProgressBarIncrement(GUI,VideoSystem,0,"Loading all images");

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
    GUIProgressBarIncrement(GUI,VideoSystem,20,"Loading all animated RenderObjects from BSD file");
    BSDPack->RenderObjectList = BSDLoadAllAnimatedRenderObjects(File);
    if( !BSDPack->RenderObjectList ) {
        DPrintf("RenderObjectManagerLoadBSD:Failed to load render objects from file\n");
        goto Failure;
    }

    GUIProgressBarIncrement(GUI,VideoSystem,40,"Initializing VRAM");
    BSDPack->VRAM = VRAMInit(BSDPack->ImageList);
    if( !BSDPack->VRAM ) {
        DPrintf("RenderObjectManagerLoadBSD:Failed to initialize VRAM\n");
        goto Failure;
    }
    GUIProgressBarIncrement(GUI,VideoSystem,90,"Setting default pose");
    for( Iterator = BSDPack->RenderObjectList; Iterator; Iterator = Iterator->Next ) {
        BSDRenderObjectSetAnimationPose(Iterator,0);
    }
    GUIProgressBarIncrement(GUI,VideoSystem,100,"Done");
    RenderObjectManagerAppendBSDPack(RenderObjectManager,BSDPack);
    if( !RenderObjectManager->SelectedBSDPack ) {
        RenderObjectManager->SelectedBSDPack = BSDPack;
        RenderObjectManager->SelectedBSDPack->SelectedRenderObject = BSDPack->RenderObjectList;
    }
    free(TAFFile);
    return 1;
Failure:
    RenderObjectManagerFreeBSDRenderObjectPack(BSDPack);
    if( TAFFile ) {
        free(TAFFile);
    }
    return 0;
}

int RenderObjectManagerLoadPack(RenderObjectManager_t *RenderObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *File)
{
    char *BaseName;
    char *Title;
    int Result;
    
    BaseName = GetBaseName(File);
    asprintf(&Title,"Loading %s",BaseName);
    GUIProgressBarBegin(GUI,Title);
    GUIProgressBarBegin(GUI,Title);
    Result = RenderObjectManagerLoadBSD(RenderObjectManager,GUI,VideoSystem,File);
    GUIProgressBarEnd(GUI,VideoSystem);
    if( !Result ) {
                GUISetErrorMessage(GUI,"Selected file doesn't seems to contain any animated RenderObject...\n"
        "Please select a new file.");

    }
    free(Title);
    free(BaseName);
    return Result;
}
void RenderObjectManagerOnBSDFileDialogSelect(GUIFileDialog_t *FileDialog,GUI_t *GUI,const char *Directory,const char *File,void *UserData)
{
    RenderObjectManagerDialogData_t *Data;
    int Result;
    Data = (RenderObjectManagerDialogData_t *) UserData;

    Result = RenderObjectManagerLoadPack(Data->RenderObjectManager,GUI,Data->VideoSystem,File);
    if( !Result ) {
        GUISetErrorMessage(GUI,"Selected file doesn't seems to contain any animated RenderObject...\n"
        "Please select a new file.");
    }
    RenderObjectManagerFreeDialogData(FileDialog);
    GUIFileDialogClose(GUI,FileDialog);
}

void RenderObjectManagerOnBSDFileDialogCancel(GUIFileDialog_t *FileDialog,GUI_t *GUI)
{
    RenderObjectManagerFreeDialogData(FileDialog);
    GUIFileDialogClose(GUI,FileDialog);
}

void RenderObjectManagerDrawPack(BSDRenderObjectPack_t *RenderObjectPack,Camera_t *Camera,
                                 mat4 ProjectionMatrix)
{
    if( !RenderObjectPack ) {
        return;
    }
    if( !RenderObjectPack->SelectedRenderObject ) {
        return;
    }
    BSDDrawRenderObject(RenderObjectPack->SelectedRenderObject,RenderObjectPack->VRAM,Camera,ProjectionMatrix);
}
void RenderObjectManagerOpenFileDialog(RenderObjectManager_t *RenderObjectManager,VideoSystem_t *VideoSystem)
{
    RenderObjectManagerDialogData_t *DialogData;
    if( !RenderObjectManager ) {
        return;
    }

    DialogData = malloc(sizeof(RenderObjectManagerDialogData_t));
    if( !DialogData ) {
        DPrintf("RenderObjectManagerOpenFileDialog:Couldn't allocate data for the extra data\n");
        return;
    }
    DialogData->RenderObjectManager = RenderObjectManager;
    DialogData->VideoSystem = VideoSystem;

    GUIFileDialogOpenWithUserData(RenderObjectManager->BSDFileDialog,DialogData);
}
void RenderObjectManagerDrawAll(RenderObjectManager_t *RenderObjectManager,Camera_t *Camera)
{
//     BSDRenderObjectPack_t *Iterator;
    mat4 ProjectionMatrix;
    
    if( !RenderObjectManager ) {
        return;
    }

    
    glBindFramebuffer(GL_FRAMEBUFFER, RenderObjectManager->FBO);
    glViewport(0,0,VidConfigWidth->IValue,VidConfigHeight->IValue);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if( RenderObjectManager->SelectedBSDPack ) {
        glm_perspective(glm_rad(110.f),(float) VidConfigWidth->IValue / (float) VidConfigHeight->IValue,1.f, 4096.f,ProjectionMatrix);
        RenderObjectManagerDrawPack(RenderObjectManager->SelectedBSDPack,Camera,ProjectionMatrix);
    }
//     for( Iterator = RenderObjectManager->BSDList; Iterator; Iterator = Iterator->Next ) {
//        RenderObjectManagerDrawPack(RenderObjectManager,Iterator,Camera,ProjectionMatrix); 
//     }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
RenderObjectManager_t *RenderObjectManagerInit(GUI_t *GUI)
{
    RenderObjectManager_t *RenderObjectManager;
    
    if( !GUI ) {
        DPrintf("RenderObjectManagerInit:Invalid GUI\n");
        return NULL;
    }
    RenderObjectManager = malloc(sizeof(RenderObjectManager_t));
    if( !RenderObjectManager ) {
        DPrintf("RenderObjectManagerInit:Couldn't allocate memory for RenderObjectManager\n");
        return NULL;
    }
    RenderObjectManager->BSDList = NULL;
    RenderObjectManager->SelectedBSDPack = NULL;
    
    if( !RenderObjectManagerCreateFBO(RenderObjectManager) ) {
        DPrintf("RenderObjectManagerInit:Couldn't create FBO\n");
        free(RenderObjectManager);
        return NULL;
    }
    RenderObjectManager->BSDFileDialog = GUIFileDialogRegister(GUI,"Open BSD File",
                                                               "BSD files(*.BSD){.BSD}",
                                                               RenderObjectManagerOnBSDFileDialogSelect,
                                                               RenderObjectManagerOnBSDFileDialogCancel);
    return RenderObjectManager;
}
