// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2025 Adriano Di Dio.
    
    GFXModelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GFXModelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GFXModelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#include "GFXObjectManager.h"
#include "GFXModelViewer.h"

Config_t *EnableWireFrameMode;
Config_t *EnableAmbientLight;


void GFXObjectManagerFreeDialogData(FileDialog_t *FileDialog)
{
    GFXObjectManagerDialogData_t *DialogData;
    if( !FileDialog ) {
        return;
    }
    DialogData = (GFXObjectManagerDialogData_t *) FileDialogGetUserData(FileDialog);
    if( DialogData ) {
        free(DialogData);
    }
}
void GFXObjectManagerFreePack(GFXPack_t *Pack)
{
    if( !Pack ) {
        return;
    }
    
    GFXFree(Pack->GFX);
    if( Pack->ImageList ) {
        TIMImageListFree(Pack->ImageList);
    }
    if( Pack->VRAM ) {
        VRAMFree(Pack->VRAM);
    }
    if( Pack->Name ) {
        free(Pack->Name);
    }
    
    free(Pack);
}
void GFXObjectManagerCleanUp(GFXObjectManager_t *GFXObjectManager)
{
    if( !GFXObjectManager ) {
        return;
    }
    
    GFXObjectManagerFreePack(GFXObjectManager->GFXPack);
    //If the user didn't close the dialog free the user data that we passed to it.
    if( FileDialogIsOpen(GFXObjectManager->GFXFileDialog) ) {
        GFXObjectManagerFreeDialogData(GFXObjectManager->GFXFileDialog);
    }
    if( FileDialogIsOpen(GFXObjectManager->ExportFileDialog) ) {
        GFXObjectManagerFreeDialogData(GFXObjectManager->ExportFileDialog);
    }
    free(GFXObjectManager);
}
void GFXObjectManagerAdvanceCurrentGFXAnimationFrame(GFXObjectManager_t *GFXObjectManager)
{
    GFX_t *CurrentGFX;
    int NextFrame;
    
    CurrentGFX = GFXObjectManagerGetCurrentGFX(GFXObjectManager);
    if( CurrentGFX != NULL ) {
        NextFrame = (CurrentGFX->CurrentFrameIndex + 1) % 
            CurrentGFX->Animation[CurrentGFX->CurrentAnimationIndex].NumFrames;
        GFXSetAnimationPose(CurrentGFX,CurrentGFX->CurrentAnimationIndex,NextFrame);
    }
}
void GFXObjectManagerAdvanceCurrentGFXAnimationPose(GFXObjectManager_t *GFXObjectManager)
{
    GFX_t *CurrentGFX;
    int NextPose;
             
    CurrentGFX = GFXObjectManagerGetCurrentGFX(GFXObjectManager);
    if( CurrentGFX != NULL && CurrentGFX->Header.NumAnimationIndex > 1) {
        NextPose = (CurrentGFX->CurrentAnimationIndex + 1) % CurrentGFX->Header.NumAnimationIndex;
        //NOTE(Adriano): Scan through the available pose until we find one that it's valid and can be set
        while( !GFXSetAnimationPose(CurrentGFX,NextPose,0) ) {
            NextPose = (NextPose + 1 ) % CurrentGFX->Header.NumAnimationIndex;
        }
    }
}
int GFXObjectManagerIsAnimationPlaying(GFXObjectManager_t *GFXObjectManager)
{
    if( !GFXObjectManager ) {
        return 0;
    }
    return GFXObjectManager->PlayAnimation;
}
void GFXObjectManagerSetAnimationPlay(GFXObjectManager_t *GFXObjectManager,int Play)
{
    if( !GFXObjectManager ) {
        return;
    }
    GFXObjectManager->PlayAnimation = Play;
}
void GFXObjectManagerCloseDialog(FileDialog_t *FileDialog)
{
    GFXObjectManagerFreeDialogData(FileDialog);
    FileDialogClose(FileDialog);

}
void GFXObjectManagerExportSelectedModelToPly(GFXObjectManager_t *GFXObjectManager,ProgressBar_t *ProgressBar,VideoSystem_t *VideoSystem,
                                               const char *Directory,bool ExportCurrentAnimation)
{
    char *PlyFile;
    char *FileName;
    char *TextureFile;
    char *GFXName;
    FILE *OutFile;
    GFX_t *CurrentGFX;
    
    if( !GFXObjectManager ) {
        DPrintf("GFXObjectManagerExportSelectedModelToPly:Invalid GFXObjectManager\n");
        return;
    }
    if( !GFXObjectManager->GFXPack ) {
        DPrintf("GFXObjectManagerExportSelectedModelToPly:Invalid GFX Pack\n");
        return;
    }
    CurrentGFX = GFXObjectManagerGetCurrentGFX(GFXObjectManager);
    if( !CurrentGFX ) {
        DPrintf("GFXObjectManagerExportSelectedModelToPly:Invalid GFX model\n");
        return;
    }
    GFXName = SwitchExt(GFXObjectManager->GFXPack->Name,"");
    asprintf(&FileName,"GFX-%s-%i.ply",GFXName,CurrentGFX->CurrentAnimationIndex);
    asprintf(&PlyFile,"%s%c%s",Directory,PATH_SEPARATOR,FileName);
    asprintf(&TextureFile,"%s%cvram-%s.png",Directory,PATH_SEPARATOR,GFXName);
    ProgressBarSetDialogTitle(ProgressBar,"Exporting Current Pose to Ply...");
    ProgressBarIncrement(ProgressBar,VideoSystem,10,"Writing BSD data.");
    if( ExportCurrentAnimation ) {
        GFXExportCurrentAnimationToPly(CurrentGFX,GFXObjectManager->GFXPack->VRAM,Directory,GFXName);
    } else {
        DPrintf("GFXObjectManagerExportCurrentPoseToPly:Dumping it...%s\n",PlyFile);
        OutFile = fopen(PlyFile,"w");
        if( !OutFile ) {
            DPrintf("GFXObjectManagerExportCurrentPoseToPly:Failed to open %s for writing\n",PlyFile);
            return;
        }
        GFXExportCurrentPoseToPly(CurrentGFX,GFXObjectManager->GFXPack->VRAM,OutFile);
        fclose(OutFile);
    }
    ProgressBarIncrement(ProgressBar,VideoSystem,95,"Exporting VRAM.");
    VRAMSave(GFXObjectManager->GFXPack->VRAM,TextureFile);
    ProgressBarIncrement(ProgressBar,VideoSystem,100,"Done.");
    free(FileName);
    free(PlyFile);
    free(TextureFile);
    free(GFXName);
    return;
}

void GFXObjectManagerExportSelectedModel(GFXObjectManager_t *GFXObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem,int OutputFormat,
                                            bool ExportCurrentAnimation)
{
    GFXObjectManagerDialogData_t *Exporter;
    
    if( !GFXObjectManager ) {
        DPrintf("GFXObjectManagerExportCurrentPose:Invalid GFXObjectManager\n");
        return;
    }
    if( !GUI ) {
        DPrintf("GFXObjectManagerExportCurrentPose:Invalid GUI data\n");
        return;
    }
    Exporter = malloc(sizeof(GFXObjectManagerDialogData_t));
    if( !Exporter ) {
        DPrintf("GFXObjectManagerExportCurrentPose:Couldn't allocate data for the exporter\n");
        return;
    }
    Exporter->GFXObjectManager = GFXObjectManager;
    Exporter->VideoSystem = VideoSystem;
    Exporter->GUI = GUI;
    Exporter->OutputFormat = OutputFormat;
    Exporter->ExportCurrentAnimation = ExportCurrentAnimation;

    FileDialogSetTitle(GFXObjectManager->ExportFileDialog,"Export Current Pose");
    FileDialogOpen(GFXObjectManager->ExportFileDialog,Exporter);

}

GFX_t *GFXObjectManagerGetCurrentGFX(GFXObjectManager_t *GFXObjectManager)
{
    if( !GFXObjectManager ) {
        return NULL;
    }
    if( !GFXObjectManager->GFXPack ) {
        return NULL;
    }
    return GFXObjectManager->GFXPack->GFX;
}
const char *GFXObjectManagerErrorToString(int ErrorCode)
{
    switch( ErrorCode ) {
        case GFX_OBJECT_MANAGER_ERROR_GENERIC:
            return "Generic Error";
        case GFX_OBJECT_MANAGER_ERROR_INVALID_TIM_FILE:
            return "TIM file was not valid or not found";
        case GFX_OBJECT_MANAGER_ERROR_VRAM_INITIALIZATION:
            return "Failed to initialize VRAM";
        case GFX_OBJECT_MANAGER_NO_ERRORS:
        default:
            return "No errors reported";
    }
}


int GFXObjectManagerLoadModel(GFXObjectManager_t *GFXObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *File)
{
    char *TIMFile;
    int ErrorCode;
    GFXPack_t *Pack;
    
    ErrorCode = GFX_OBJECT_MANAGER_NO_ERRORS;
    
    //TODO(Adriano): Instance a new GFX pack, if it can be loaded then swap the current one with the one that is being loaded
    if( !GFXObjectManager ) {
        DPrintf("GFXObjectManagerLoadModel:Invalid GFXObjectManager\n");
        return GFX_OBJECT_MANAGER_ERROR_GENERIC;
    }
    if( !File ) {
        DPrintf("GFXObjectManagerLoadModel:Invalid file name\n");
        return GFX_OBJECT_MANAGER_ERROR_GENERIC;
    }
    
    ProgressBarReset(GUI->ProgressBar);
    
    DPrintf("GFXObjectManagerLoadModel:Attempting to load %s\n",File);
    Pack = malloc(sizeof(GFXPack_t));
    
    if( !Pack ) {
        DPrintf("GFXObjectManagerLoadModel:Failed to allocate memory for GFX pack\n");
        goto Failure;
    }
    Pack->Name = GetBaseName(File);
    Pack->ImageList = NULL;
    Pack->VRAM = NULL;
    Pack->GFX = NULL;
    Pack->LastUpdateTime = 0;
    TIMFile = NULL;
    
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,0,"Loading all images");
    TIMFile = SwitchExt(File,".tim");
    Pack->ImageList = TIMLoadAllImages(TIMFile,NULL);
    if( !Pack->ImageList ) {
        free(TIMFile);
        TIMFile = SwitchExt(File,".TIM");
        Pack->ImageList = TIMLoadAllImages(TIMFile,NULL);
        if( !Pack->ImageList ) {
            DPrintf("GFXObjectManagerLoadModel:Failed to load images from TIM file %s\n",TIMFile);
            ErrorCode = GFX_OBJECT_MANAGER_ERROR_INVALID_TIM_FILE;
            goto Failure;
        }
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,20,"Loading GFX Model");
    Pack->GFX = GFXReadFromFile(File);
    if( !Pack->GFX ) {
        DPrintf("GFXObjectManagerLoadModel:Failed to load GFX model from file\n");
        ErrorCode = GFX_OBJECT_MANAGER_ERROR_INVALID_GFX_FILE;
        goto Failure;
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,40,"Initializing VRAM");
    Pack->VRAM = VRAMInit(Pack->ImageList);
    if( !Pack->VRAM ) {
        DPrintf("GFXObjectManagerLoadModel:Failed to initialize VRAM\n");
        ErrorCode = GFX_OBJECT_MANAGER_ERROR_VRAM_INITIALIZATION;
        goto Failure;
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,90,"Setting default pose");
    GFXSetAnimationPose(Pack->GFX,0,0);
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,100,"Done");
    GFXObjectManagerFreePack(GFXObjectManager->GFXPack);
    GFXObjectManager->GFXPack = Pack;
    free(TIMFile);
    return ErrorCode;
Failure:
    GFXObjectManagerFreePack(Pack);
    if( TIMFile ) {
        free(TIMFile);
    }
    return ErrorCode;
}

int GFXObjectManagerLoadPack(GFXObjectManager_t *GFXObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *File)
{
    char *BaseName;
    char *Title;
    int Result;
    
    BaseName = GetBaseName(File);
    asprintf(&Title,"Loading %s",BaseName);
    ProgressBarBegin(GUI->ProgressBar,Title);
    Result = GFXObjectManagerLoadModel(GFXObjectManager,GUI,VideoSystem,File);
    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
    if( Result <= 0) {
        ErrorMessageDialogSet(GUI->ErrorMessageDialog,GFXObjectManagerErrorToString(Result));
    }
    free(Title);
    free(BaseName);
    return Result;
}

void GFXObjectManagerOnExportDirSelect(FileDialog_t *FileDialog,const char *Directory,const char *File,void *UserData)
{
    GFXObjectManagerDialogData_t *Exporter;
    GFXObjectManager_t *GFXObjectManager;
    Exporter = (GFXObjectManagerDialogData_t *) UserData;
    GFXObjectManager = Exporter->GFXObjectManager;
        
    ProgressBarBegin(Exporter->GUI->ProgressBar,"Exporting...");

    switch( Exporter->OutputFormat ) {
        case GFX_OBJECT_MANAGER_EXPORT_FORMAT_PLY:
            GFXObjectManagerExportSelectedModelToPly(GFXObjectManager,Exporter->GUI->ProgressBar,Exporter->VideoSystem,Directory,
                                                        Exporter->ExportCurrentAnimation);
            break;
        default:
            DPrintf("GFXObjectManagerOnExportDirSelect:Invalid output format\n");
            break;
    }
        
    ProgressBarEnd(Exporter->GUI->ProgressBar,Exporter->VideoSystem);
    FileDialogClose(FileDialog);
    free(Exporter);
}

void GFXObjectManagerOnExportDirCancel(FileDialog_t *FileDialog)
{
    GFXObjectManagerCloseDialog(FileDialog);
}
void GFXObjectManagerOnGFXFileDialogSelect(FileDialog_t *FileDialog,const char *Directory,const char *File,void *UserData)
{
    GFXObjectManagerDialogData_t *Data;
    Data = (GFXObjectManagerDialogData_t *) UserData;
    GFXObjectManagerLoadPack(Data->GFXObjectManager,Data->GUI,Data->VideoSystem,File);
    GFXObjectManagerFreeDialogData(FileDialog);
    FileDialogClose(FileDialog);
}

void GFXObjectManagerOnGFXFileDialogCancel(FileDialog_t *FileDialog)
{
    GFXObjectManagerCloseDialog(FileDialog);
}

void GFXObjectManagerDrawPack(GFXPack_t *GFXPack,Camera_t *Camera,
                                 mat4 ProjectionMatrix)
{
    if( !GFXPack ) {
        return;
    }

    GFXRender(GFXPack->GFX,GFXPack->VRAM,Camera->ViewMatrix,ProjectionMatrix,EnableWireFrameMode->IValue,EnableAmbientLight->IValue);
}
void GFXObjectManagerOpenFileDialog(GFXObjectManager_t *GFXObjectManager,GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    GFXObjectManagerDialogData_t *DialogData;
    if( !GFXObjectManager ) {
        return;
    }

    DialogData = malloc(sizeof(GFXObjectManagerDialogData_t));
    if( !DialogData ) {
        DPrintf("GFXObjectManagerOpenFileDialog:Couldn't allocate data for the extra data\n");
        return;
    }
    DialogData->GFXObjectManager = GFXObjectManager;
    DialogData->VideoSystem = VideoSystem;
    DialogData->GUI = GUI;

    FileDialogOpen(GFXObjectManager->GFXFileDialog,DialogData);
}
void GFXObjectManagerUpdate(GFXObjectManager_t *GFXObjectManager)
{
    GFX_t *GFX;
    int NextFrame;
    int Now;
    if( !GFXObjectManager ) {
        return;
    }
    if( !GFXObjectManager->PlayAnimation ) {
        return;
    }
    if( !GFXObjectManager->GFXPack ) {
        return;
    }

    Now = SysMilliseconds();
    //NOTE(Adriano):Avoid running too fast...
    if( (Now - GFXObjectManager->GFXPack->LastUpdateTime ) < 30 ) {
        return;
    }
    //TODO(Adriano):Animation code
    GFX = GFXObjectManager->GFXPack->GFX;
    NextFrame = (GFX->CurrentFrameIndex + 1) % GFX->Animation[GFX->CurrentAnimationIndex].NumFrames;
    GFXSetAnimationPose(GFX,GFX->CurrentAnimationIndex,NextFrame);
    GFXObjectManager->GFXPack->LastUpdateTime = Now;
}
void GFXObjectManagerDraw(GFXObjectManager_t *GFXObjectManager,Camera_t *Camera)
{
    mat4 ProjectionMatrix;
    
    if( !GFXObjectManager ) {
        return;
    }
    glViewport(0,0,VidConfigWidth->IValue,VidConfigHeight->IValue);
    if( GFXObjectManager->GFXPack ) {
        glm_perspective(glm_rad(90.f),(float) VidConfigWidth->IValue / (float) VidConfigHeight->IValue,1.f, 4096.f,ProjectionMatrix);
        GFXObjectManagerDrawPack(GFXObjectManager->GFXPack,Camera,ProjectionMatrix);
    }
}

GFXObjectManager_t *GFXObjectManagerInit(GUI_t *GUI)
{
    GFXObjectManager_t *GFXObjectManager;
    
    if( !GUI ) {
        DPrintf("GFXObjectManagerInit:Invalid GUI\n");
        return NULL;
    }
    GFXObjectManager = malloc(sizeof(GFXObjectManager_t));
    if( !GFXObjectManager ) {
        DPrintf("GFXObjectManagerInit:Couldn't allocate memory for GFXObjectManager\n");
        return NULL;
    }
    GFXObjectManager->GFXPack = NULL;

    GFXObjectManager->GFXFileDialog = FileDialogRegister("Open GFX File",
                                                               "GFX files(*.GFX){.GFX}",
                                                               GFXObjectManagerOnGFXFileDialogSelect,
                                                               GFXObjectManagerOnGFXFileDialogCancel);
    GFXObjectManager->ExportFileDialog = FileDialogRegister("Export Current Pose",NULL,
                                                           GFXObjectManagerOnExportDirSelect,
                                                           GFXObjectManagerOnExportDirCancel);
    EnableWireFrameMode = ConfigGet("EnableWireFrameMode");
    EnableAmbientLight = ConfigGet("EnableAmbientLight");
    
    GFXObjectManager->PlayAnimation = 0;

    return GFXObjectManager;
}
