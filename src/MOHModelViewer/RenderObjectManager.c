// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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
#include "RenderObjectManager.h"
#include "MOHModelViewer.h"

Config_t *EnableWireFrameMode;
Config_t *EnableAmbientLight;

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
    RenderObjectFreeList(BSDRenderObjectPack->RenderObjectList);
    free(BSDRenderObjectPack);
}

void RenderObjectManagerFreeDialogData(FileDialog_t *FileDialog)
{
    RenderObjectManagerDialogData_t *DialogData;
    if( !FileDialog ) {
        return;
    }
    DialogData = (RenderObjectManagerDialogData_t *) FileDialogGetUserData(FileDialog);
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
    
    //If the user didn't close the dialog free the user data that we passed to it.
    if( FileDialogIsOpen(RenderObjectManager->BSDFileDialog) ) {
        RenderObjectManagerFreeDialogData(RenderObjectManager->BSDFileDialog);
    }
    if( FileDialogIsOpen(RenderObjectManager->ExportFileDialog) ) {
        RenderObjectManagerFreeDialogData(RenderObjectManager->ExportFileDialog);
    }
    if( RenderObjectManager->RenderObjectShader ) {
        RenderObjectFreeShader(RenderObjectManager->RenderObjectShader);
    }
    free(RenderObjectManager);
}
void RenderObjectManagerAdvanceSelectedRenderObjectAnimationFrame(RenderObjectManager_t *RenderObjectManager)
{
    RenderObject_t *CurrentRenderObject;
    int NextFrame;
    
    CurrentRenderObject = RenderObjectManagerGetSelectedRenderObject(RenderObjectManager);
    if( CurrentRenderObject != NULL && !CurrentRenderObject->IsStatic ) {
        NextFrame = (CurrentRenderObject->CurrentFrameIndex + 1) % 
            CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].NumFrames;
        RenderObjectSetAnimationPose(CurrentRenderObject,CurrentRenderObject->CurrentAnimationIndex,NextFrame,0);
    }
}
void RenderObjectManagerAdvanceSelectedRenderObjectAnimationPose(RenderObjectManager_t *RenderObjectManager)
{
    RenderObject_t *CurrentRenderObject;
    int NextPose;
    CurrentRenderObject = RenderObjectManagerGetSelectedRenderObject(RenderObjectManager);
    if( CurrentRenderObject != NULL && !CurrentRenderObject->IsStatic && CurrentRenderObject->NumAnimations > 1) {
        NextPose = (CurrentRenderObject->CurrentAnimationIndex + 1) % 
        CurrentRenderObject->NumAnimations;
        //NOTE(Adriano): Scan through the available pose until we find one that it's valid and can be set
        while( !RenderObjectSetAnimationPose(CurrentRenderObject,NextPose,0,0) ) {
            NextPose = (NextPose + 1 ) % CurrentRenderObject->NumAnimations;
        }                
    }
}
int RenderObjectManagerIsAnimationPlaying(RenderObjectManager_t *RenderObjectManager)
{
    if( !RenderObjectManager ) {
        return 0;
    }
    return RenderObjectManager->PlayAnimation;
}
void RenderObjectManagerSetAnimationPlay(RenderObjectManager_t *RenderObjectManager,int Play)
{
    if( !RenderObjectManager ) {
        return;
    }
    RenderObjectManager->PlayAnimation = Play;
}
void RenderObjectManagerCloseDialog(FileDialog_t *FileDialog)
{
    RenderObjectManagerFreeDialogData(FileDialog);
    FileDialogClose(FileDialog);

}
void RenderObjectManagerExportSelectedModelToPly(RenderObjectManager_t *RenderObjectManager,ProgressBar_t *ProgressBar,VideoSystem_t *VideoSystem,
                                               const char *Directory,bool ExportCurrentAnimation)
{
    char *EngineName;
    char *PlyFile;
    char *FileName;
    char *TextureFile;
    char *BSDName;
    FILE *OutFile;
    BSDRenderObjectPack_t *CurrentBSDPack;
    RenderObject_t *CurrentRenderObject;
    
    if( !RenderObjectManager ) {
        DPrintf("RenderObjectManagerExportCurrentPoseToPly:Invalid RenderObjectManager\n");
        return;
    }
    CurrentBSDPack = RenderObjectManagerGetSelectedBSDPack(RenderObjectManager);
    if( !CurrentBSDPack ) {
        DPrintf("RenderObjectManagerExportCurrentPoseToPly:Invalid BSD Pack\n");
        return;
    }
    CurrentRenderObject = RenderObjectManagerGetSelectedRenderObject(RenderObjectManager);
    if( !CurrentRenderObject ) {
        DPrintf("RenderObjectManagerExportCurrentPoseToPly:Invalid RenderObject\n");
        return;
    }
    asprintf(&EngineName,"%s",(CurrentBSDPack->GameVersion == MOH_GAME_STANDARD) ? "MOH" : "MOHUndergound");
    asprintf(&FileName,"RenderObject-%u-%i-%s.ply",CurrentRenderObject->Id,CurrentRenderObject->CurrentAnimationIndex,EngineName);
    asprintf(&PlyFile,"%s%c%s",Directory,PATH_SEPARATOR,FileName);
    BSDName = SwitchExt(CurrentBSDPack->Name,"");
    asprintf(&TextureFile,"%s%cvram-%s.png",Directory,PATH_SEPARATOR,BSDName);
    ProgressBarSetDialogTitle(ProgressBar,"Exporting Current Pose to Ply...");
    ProgressBarIncrement(ProgressBar,VideoSystem,10,"Writing BSD data.");
    if( ExportCurrentAnimation ) {
        RenderObjectExportCurrentAnimationToPly(CurrentRenderObject,CurrentBSDPack->VRAM,Directory,EngineName);
    } else {
        DPrintf("RenderObjectManagerExportCurrentPoseToPly:Dumping it...%s\n",PlyFile);
        OutFile = fopen(PlyFile,"w");
        if( !OutFile ) {
            DPrintf("RenderObjectManagerExportCurrentPoseToPly:Failed to open %s for writing\n",PlyFile);
            return;
        }
        RenderObjectExportCurrentPoseToPly(CurrentRenderObject,CurrentBSDPack->VRAM,OutFile);
        fclose(OutFile);
    }
    ProgressBarIncrement(ProgressBar,VideoSystem,95,"Exporting VRAM.");
    VRAMSave(CurrentBSDPack->VRAM,TextureFile);
    ProgressBarIncrement(ProgressBar,VideoSystem,100,"Done.");
    free(EngineName);
    free(FileName);
    free(PlyFile);
    free(TextureFile);
    free(BSDName);
    return;
}

void RenderObjectManagerExportSelectedModel(RenderObjectManager_t *RenderObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem,int OutputFormat,
                                            bool ExportCurrentAnimation)
{
    RenderObjectManagerDialogData_t *Exporter;
    
    if( !RenderObjectManager ) {
        DPrintf("RenderObjectManagerExportCurrentPose:Invalid RenderObjectManager\n");
        return;
    }
    if( !GUI ) {
        DPrintf("RenderObjectManagerExportCurrentPose:Invalid GUI data\n");
        return;
    }
    if( !RenderObjectManager->SelectedBSDPack ) {
        DPrintf("RenderObjectManagerExportCurrentPose:No BSD pack selected\n");
        return;
    }
    Exporter = malloc(sizeof(RenderObjectManagerDialogData_t));
    if( !Exporter ) {
        DPrintf("RenderObjectManagerExportCurrentPose:Couldn't allocate data for the exporter\n");
        return;
    }
    Exporter->RenderObjectManager = RenderObjectManager;
    Exporter->VideoSystem = VideoSystem;
    Exporter->GUI = GUI;
    Exporter->OutputFormat = OutputFormat;
    Exporter->ExportCurrentAnimation = ExportCurrentAnimation;

    FileDialogSetTitle(RenderObjectManager->ExportFileDialog,"Export Current Pose");
    FileDialogOpen(RenderObjectManager->ExportFileDialog,Exporter);

}

const char *RenderObjectManagerErrorToString(int ErrorCode)
{
    switch( ErrorCode ) {
        case RENDER_OBJECT_MANAGER_BSD_ERROR_GENERIC:
            return "Generic Error";
        case RENDER_OBJECT_MANAGER_BSD_ERROR_INVALID_TAF_FILE:
            return "TAF file was not valid or not found";
        case RENDER_OBJECT_MANAGER_BSD_ERROR_NO_ANIMATED_RENDEROBJECTS:
            return "BSD file was not valid or no animated RenderObjects could be found";
        case RENDER_OBJECT_MANAGER_BSD_ERROR_ALREADY_LOADED:
            return "BSD File was already loaded";
        case RENDER_OBJECT_MANAGER_BSD_ERROR_VRAM_INITIALIZATION:
            return "Failed to initialize VRAM";
        case RENDER_OBJECT_MANAGER_BSD_NO_ERRORS:
        default:
            return "No errors reported";
    }
}

BSDRenderObjectPack_t *RenderObjectManagerGetSelectedBSDPack(RenderObjectManager_t *RenderObjectManager)
{
    if( !RenderObjectManager ) {
        DPrintf("RenderObjectManagerGetSelectedBSDPack:Invalid RenderObjectManager\n");
        return NULL;
    }
    return RenderObjectManager->SelectedBSDPack;

}
RenderObject_t *RenderObjectManagerGetSelectedRenderObject(RenderObjectManager_t *RenderObjectManager)
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
                                                RenderObject_t *SelectedRenderObject)
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
void RenderObjectManagerSetDefaultSelection(RenderObjectManager_t *RenderObjectManager)
{
    RenderObjectManager->SelectedBSDPack = RenderObjectManager->BSDList;
    if( RenderObjectManager->BSDList != NULL ) {
        RenderObjectManager->SelectedBSDPack->SelectedRenderObject = RenderObjectManager->BSDList->RenderObjectList;
    }
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
int RenderObjectManagerDeleteBSDPackFromList(RenderObjectManager_t *RenderObjectManager,const char *BSDPackName,int GameVersion)
{
    BSDRenderObjectPack_t *Temp;
    BSDRenderObjectPack_t *Current;
    BSDRenderObjectPack_t *Previous;
    
    Current = RenderObjectManager->BSDList;
    Previous = NULL;
    while( Current ) {
        if( !strcmp(Current->Name,BSDPackName) && Current->GameVersion == GameVersion) {
            Temp = Current;
            if( !Previous ) {
                RenderObjectManager->BSDList = Current->Next;
            } else {
                Previous->Next = Current->Next;
            }
            //NOTE(Adriano):If we are trying to free the selected one,make it invalid and reassign it later.
            if( Temp == RenderObjectManager->SelectedBSDPack ) {
                RenderObjectManager->SelectedBSDPack = NULL;
            }
            RenderObjectManagerFreeBSDRenderObjectPack(Temp);
            return 1;
        }
        Previous = Current;
        Current = Current->Next;
    }
    return 0;
}
int RenderObjectManagerDeleteBSDPack(RenderObjectManager_t *RenderObjectManager,const char *BSDPackName,int GameVersion)
{
    if( RenderObjectManagerDeleteBSDPackFromList(RenderObjectManager,BSDPackName,GameVersion) ) {
        //NOTE(Adriano):If the selected BSD pack belonged to the deleted one...then it now should be invalid....
        if( !RenderObjectManager->SelectedBSDPack ) {
            RenderObjectManagerSetDefaultSelection(RenderObjectManager);
        }
        return 1;
    }
    return 0;
}
BSDRenderObjectPack_t *RenderObjectManagerGetBSDPack(RenderObjectManager_t *RenderObjectManager,const char *Name,int GameVersion)
{
    BSDRenderObjectPack_t *Iterator;
    for( Iterator = RenderObjectManager->BSDList; Iterator; Iterator = Iterator->Next ) {
        if( !strcmp(Iterator->Name,Name) && Iterator->GameVersion == GameVersion) {
            return Iterator;
        }
    }
    return NULL;
}

int RenderObjectManagerLoadBSD(RenderObjectManager_t *RenderObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *File)
{
    BSDRenderObjectPack_t *BSDPack;
    RenderObject_t *Iterator;
    char *TAFFile;
    int ErrorCode;
    
    ErrorCode = RENDER_OBJECT_MANAGER_BSD_NO_ERRORS;
    
    if( !RenderObjectManager ) {
        DPrintf("RenderObjectManagerLoadBSD:Invalid RenderObjectManager\n");
        return RENDER_OBJECT_MANAGER_BSD_ERROR_GENERIC;
    }
    if( !File ) {
        DPrintf("RenderObjectManagerLoadBSD:Invalid file name\n");
        return RENDER_OBJECT_MANAGER_BSD_ERROR_GENERIC;
    }
    
    ProgressBarReset(GUI->ProgressBar);
    
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
    BSDPack->LastUpdateTime = 0;
    BSDPack->Next = NULL;
    TAFFile = NULL;
    
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,0,"Loading all images");
    TAFFile = SwitchExt(File,"0.TAF");
    BSDPack->ImageList = TIMLoadAllImages(TAFFile,NULL);
    if( !BSDPack->ImageList ) {
        free(TAFFile);
        TAFFile = SwitchExt(File,"1.TAF");
        BSDPack->ImageList = TIMLoadAllImages(TAFFile,NULL);
        if( !BSDPack->ImageList ) {
            DPrintf("RenderObjectManagerLoadBSD:Failed to load images from TAF file %s\n",TAFFile);
            ErrorCode = RENDER_OBJECT_MANAGER_BSD_ERROR_INVALID_TAF_FILE;
            goto Failure;
        }
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,20,"Loading all RenderObjects");
    BSDPack->RenderObjectList = BSDLoadRenderObjects(File,&BSDPack->GameVersion);
    if( !BSDPack->RenderObjectList ) {
        DPrintf("RenderObjectManagerLoadBSD:Failed to load render objects from file\n");
        ErrorCode = RENDER_OBJECT_MANAGER_BSD_ERROR_NO_ANIMATED_RENDEROBJECTS;
        goto Failure;
    }
    if( RenderObjectManagerGetBSDPack(RenderObjectManager,BSDPack->Name,BSDPack->GameVersion) != NULL ) {
        DPrintf("RenderObjectManagerLoadBSD:Duplicated found in list!\n");
        ErrorCode = RENDER_OBJECT_MANAGER_BSD_ERROR_ALREADY_LOADED;
        goto Failure;
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,40,"Initializing VRAM");
    BSDPack->VRAM = VRAMInit(BSDPack->ImageList);
    if( !BSDPack->VRAM ) {
        DPrintf("RenderObjectManagerLoadBSD:Failed to initialize VRAM\n");
        ErrorCode = RENDER_OBJECT_MANAGER_BSD_ERROR_VRAM_INITIALIZATION;
        goto Failure;
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,90,"Setting default pose");
    for( Iterator = BSDPack->RenderObjectList; Iterator; Iterator = Iterator->Next ) {
        RenderObjectGenerateVAO(Iterator);
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,100,"Done");
    RenderObjectManagerAppendBSDPack(RenderObjectManager,BSDPack);
    if( !RenderObjectManager->SelectedBSDPack ) {
        RenderObjectManagerSetSelectedRenderObject(RenderObjectManager,BSDPack,BSDPack->RenderObjectList);
    }
    free(TAFFile);
    return ErrorCode;
Failure:
    RenderObjectManagerFreeBSDRenderObjectPack(BSDPack);
    if( TAFFile ) {
        free(TAFFile);
    }
    return ErrorCode;
}

int RenderObjectManagerLoadPack(RenderObjectManager_t *RenderObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *File)
{
    char *BaseName;
    char *Title;
    int Result;
    
    BaseName = GetBaseName(File);
    asprintf(&Title,"Loading %s",BaseName);
    ProgressBarBegin(GUI->ProgressBar,Title);
    Result = RenderObjectManagerLoadBSD(RenderObjectManager,GUI,VideoSystem,File);
    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
    if( Result <= 0) {
        ErrorMessageDialogSet(GUI->ErrorMessageDialog,RenderObjectManagerErrorToString(Result));
    }
    free(Title);
    free(BaseName);
    return Result;
}

void RenderObjectManagerOnExportDirSelect(FileDialog_t *FileDialog,const char *Directory,const char *File,void *UserData)
{
    RenderObjectManagerDialogData_t *Exporter;
    RenderObjectManager_t *RenderObjectManager;
    Exporter = (RenderObjectManagerDialogData_t *) UserData;
    RenderObjectManager = Exporter->RenderObjectManager;
        
    ProgressBarBegin(Exporter->GUI->ProgressBar,"Exporting...");

    switch( Exporter->OutputFormat ) {
        case RENDER_OBJECT_MANAGER_EXPORT_FORMAT_PLY:
            RenderObjectManagerExportSelectedModelToPly(RenderObjectManager,Exporter->GUI->ProgressBar,Exporter->VideoSystem,Directory,
                                                        Exporter->ExportCurrentAnimation);
            break;
        default:
            DPrintf("RenderObjectManagerOnExportDirSelect:Invalid output format\n");
            break;
    }
        
    ProgressBarEnd(Exporter->GUI->ProgressBar,Exporter->VideoSystem);
    FileDialogClose(FileDialog);
    free(Exporter);
}

void RenderObjectManagerOnExportDirCancel(FileDialog_t *FileDialog)
{
    RenderObjectManagerCloseDialog(FileDialog);
}
void RenderObjectManagerOnBSDFileDialogSelect(FileDialog_t *FileDialog,const char *Directory,const char *File,void *UserData)
{
    RenderObjectManagerDialogData_t *Data;
    Data = (RenderObjectManagerDialogData_t *) UserData;
    RenderObjectManagerLoadPack(Data->RenderObjectManager,Data->GUI,Data->VideoSystem,File);
    RenderObjectManagerFreeDialogData(FileDialog);
    FileDialogClose(FileDialog);
}

void RenderObjectManagerOnBSDFileDialogCancel(FileDialog_t *FileDialog)
{
    RenderObjectManagerCloseDialog(FileDialog);
}

void RenderObjectManagerDrawPack(BSDRenderObjectPack_t *RenderObjectPack,RenderObjectShader_t *RenderObjectShader,Camera_t *Camera,
                                 mat4 ProjectionMatrix)
{
    RenderObject_t *RenderObject;
    mat4 ModelMatrix;
    vec3 Temp;
    
    if( !RenderObjectPack ) {
        return;
    }
    if( !RenderObjectPack->SelectedRenderObject ) {
        return;
    }
    
    RenderObject = RenderObjectPack->SelectedRenderObject;
    
    glm_mat4_identity(ModelMatrix);
    Temp[0] = -RenderObject->Center[0];
    Temp[1] = -RenderObject->Center[1];
    Temp[2] = -RenderObject->Center[2];
    glm_vec3_rotate(Temp, DEGTORAD(180.f), GLM_XUP);    
    glm_translate(ModelMatrix,Temp);
    Temp[0] = 0;
    Temp[1] = 1;
    Temp[2] = 0;
    glm_rotate(ModelMatrix,glm_rad(-90), Temp);
    glm_scale(ModelMatrix,RenderObject->Scale);
    RenderObjectBeginDraw(RenderObjectPack->VRAM,RenderObjectShader,EnableAmbientLight->IValue,EnableWireFrameMode->IValue,false);
    RenderObjectDraw(RenderObjectPack->SelectedRenderObject,RenderObjectShader,ModelMatrix,Camera->ViewMatrix,ProjectionMatrix);
    RenderObjectEndDraw(EnableWireFrameMode->IValue);
}
void RenderObjectManagerOpenFileDialog(RenderObjectManager_t *RenderObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem)
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
    DialogData->GUI = GUI;

    FileDialogOpen(RenderObjectManager->BSDFileDialog,DialogData);
}
void RenderObjectManagerUpdate(RenderObjectManager_t *RenderObjectManager)
{
    RenderObject_t *CurrentRenderObject;
    int NextFrame;
    int Now;
    if( !RenderObjectManager ) {
        return;
    }
    if( !RenderObjectManager->PlayAnimation ) {
        return;
    }
    if( !RenderObjectManager->SelectedBSDPack ) {
        return;
    }
    if( !RenderObjectManager->SelectedBSDPack->SelectedRenderObject ) {
        return;
    }
    
    CurrentRenderObject = RenderObjectManager->SelectedBSDPack->SelectedRenderObject;
    
    if( CurrentRenderObject->IsStatic ) {
        return;
    }
    
    Now = SysMilliseconds();
    //NOTE(Adriano):Avoid running too fast...
    if( (Now - RenderObjectManager->SelectedBSDPack->LastUpdateTime ) < 30 ) {
        return;
    }
    NextFrame = (CurrentRenderObject->CurrentFrameIndex + 1) % 
                    CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].NumFrames;
    RenderObjectSetAnimationPose(CurrentRenderObject,CurrentRenderObject->CurrentAnimationIndex,NextFrame,0);
    RenderObjectManager->SelectedBSDPack->LastUpdateTime = Now;
}
void RenderObjectManagerDraw(RenderObjectManager_t *RenderObjectManager,Camera_t *Camera)
{
    mat4 ProjectionMatrix;
    
    if( !RenderObjectManager ) {
        return;
    }
    glViewport(0,0,VidConfigWidth->IValue,VidConfigHeight->IValue);
    if( RenderObjectManager->SelectedBSDPack ) {
        glm_perspective(glm_rad(90.f),(float) VidConfigWidth->IValue / (float) VidConfigHeight->IValue,1.f, 4096.f,ProjectionMatrix);
        RenderObjectManagerDrawPack(RenderObjectManager->SelectedBSDPack,RenderObjectManager->RenderObjectShader,Camera,ProjectionMatrix);
    }
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
    RenderObjectManager->RenderObjectShader = NULL;
    
    RenderObjectManager->RenderObjectShader = RenderObjectInitShader();
    if( !RenderObjectManager->RenderObjectShader ) {
        DPrintf("RenderObjectManagerInit:Couldn't load RenderObjectShader\n");
        free(RenderObjectManager);
        return NULL;
    }

    RenderObjectManager->BSDFileDialog = FileDialogRegister("Open BSD File",
                                                               "BSD files(*.BSD){.BSD}",
                                                               RenderObjectManagerOnBSDFileDialogSelect,
                                                               RenderObjectManagerOnBSDFileDialogCancel);
    RenderObjectManager->ExportFileDialog = FileDialogRegister("Export Current Pose",NULL,
                                                           RenderObjectManagerOnExportDirSelect,
                                                           RenderObjectManagerOnExportDirCancel);
    EnableWireFrameMode = ConfigGet("EnableWireFrameMode");
    EnableAmbientLight = ConfigGet("EnableAmbientLight");
    
    RenderObjectManager->PlayAnimation = 0;

    return RenderObjectManager;
}
