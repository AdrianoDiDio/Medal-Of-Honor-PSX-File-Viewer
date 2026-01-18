// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2026 Adriano Di Dio.
    
    TIMViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TIMViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TIMViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#include "ImageManager.h"
#include "TIMViewer.h"

void ImageManagerFreeDialogData(FileDialog_t *FileDialog)
{
    ImageManagerDialogData_t *DialogData;
    if( !FileDialog ) {
        return;
    }
    DialogData = (ImageManagerDialogData_t *) FileDialogGetUserData(FileDialog);
    if( DialogData ) {
        free(DialogData);
    }
}
void ImageManagerUnload(ImageManager_t *ImageManager)
{
    if( !ImageManager ) {
        return;
    }
    if( ImageManager->VRAM ) {
        VRAMFree(ImageManager->VRAM);
    }
    if( ImageManager->ImageList ) {
        TIMImageListFree(ImageManager->ImageList);
    }
    ImageManager->VRAM = NULL;
    ImageManager->ImageList = NULL;
    ImageManager->SelectedImage = NULL;
}
void ImageManagerCleanUp(ImageManager_t *ImageManager)
{
    if( !ImageManager ) {
        return;
    }
    
    //If the user didn't close the dialog free the user data that we passed to it.
    if( FileDialogIsOpen(ImageManager->ImageFileDialog) ) {
        ImageManagerFreeDialogData(ImageManager->ImageFileDialog);
    }
    if( FileDialogIsOpen(ImageManager->ExportFileDialog) ) {
        ImageManagerFreeDialogData(ImageManager->ExportFileDialog);
    }
    ImageManagerUnload(ImageManager);
    free(ImageManager);
}


void ImageManagerCloseDialog(FileDialog_t *FileDialog)
{
    ImageManagerFreeDialogData(FileDialog);
    FileDialogClose(FileDialog);
}

int ImageManagerLoadTIMFile(ImageManager_t *ImageManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *File)
{
    TIMImage_t *Result;
    VRAM_t     *VRAM;
    int NumImages;
    if( !ImageManager ) {
        DPrintf("ImageManagerLoadTIMFile:Invalid ImageManager\n");
        return 0;
    }
    if( !File ) {
        DPrintf("ImageManagerLoadTIMFile:Invalid File\n");
        return 0;
    }
    ProgressBarBegin(GUI->ProgressBar,"Loading TIM file");
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,70.f,"Loading all images");
    Result = TIMLoadAllImages(File,&NumImages);
    if( !Result || !NumImages ) {
        DPrintf("ImageManagerLoadTIMFile:File did not contain any TIM image.\n");
        goto Failure;
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,90.f,"Loading VRAM.");
    VRAM = VRAMInit(Result);
    if( !VRAM ) {
        DPrintf("ImageManagerLoadTIMFile:Failed to initialize VRAM\n");
        goto Failure;
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,100.f,"Done.");
    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
    ImageManagerUnload(ImageManager);
    ImageManager->VRAM = VRAM;
    ImageManager->ImageList = Result;
    ImageManager->SelectedImage = Result;
    return 1;
Failure:
    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
    ErrorMessageDialogSet(GUI->ErrorMessageDialog,"No TIM files found.");
    return 0;
}
void ImageManagerOnImageFileDialogSelect(FileDialog_t *FileDialog,const char *Directory,const char *File,void *UserData)
{
    ImageManagerDialogData_t *Data;
    Data = (ImageManagerDialogData_t *) UserData;
    ImageManagerLoadTIMFile(Data->ImageManager,Data->GUI,Data->VideoSystem,File);
    ImageManagerCloseDialog(FileDialog);
}
char *ImageManagerBuildPNGExportPath(TIMImage_t *Image,const char *Directory)
{
    char *Result;
    char *FileName;
    char *CurrentExt;
    if( !Image ) {
        DPrintf("ImageManagerBuildPNGExportPath:Invalid image\n");
        return NULL;
    }
    if( !Directory ) {
        DPrintf("ImageManagerBuildPNGExportPath:Invalid Directory\n");
        return NULL;
    }

    CurrentExt = GetFileExtension(Image->Name);
    if( CurrentExt != NULL ) {
        FileName = SwitchExt(Image->Name, ".png");
    } else {
        FileName = StringAppend(Image->Name,".png");
    }
    asprintf(&Result,"%s%c%s",Directory,PATH_SEPARATOR,FileName);
    free(CurrentExt);
    free(FileName);
    return Result;
}
void ImageManagerExportToPNG(ImageManager_t *ImageManager,TIMImage_t *FirstImage,TIMImage_t *LastImage,
                             GUI_t *GUI,VideoSystem_t *VideoSystem,const char *Directory)
{
    TIMImage_t *Iterator;
    char *FileName;
    
    if( !ImageManager ) {
        DPrintf("ImageManagerExportToPNG:Invalid user data\n");
        return;
    }
    
    if( !FirstImage ) {
        DPrintf("ImageManagerExportToPNG:Invalid Start Image!\n");
        return;
    }
    
    ProgressBarSetDialogTitle(GUI->ProgressBar,"Exporting to PNG...");
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,70,"Counting output files.");
    if( FirstImage == LastImage ) {
        ProgressBarIncrement(GUI->ProgressBar,VideoSystem,80,"Exporting single file to PNG.");
        FileName = ImageManagerBuildPNGExportPath(FirstImage,Directory);
        TIMWritePNGImage(FirstImage,FileName);
        free(FileName);
    } else {
        ProgressBarIncrement(GUI->ProgressBar,VideoSystem,80,"Exporting multiple files to PNG.");
        for( Iterator = FirstImage; Iterator != LastImage; Iterator = Iterator->Next ) {
            FileName = ImageManagerBuildPNGExportPath(Iterator,Directory);
            TIMWritePNGImage(Iterator,FileName);
            free(FileName);
        }
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,100,"Done.");
}
void ImageManagerOnImageFileDialogCancel(FileDialog_t *FileDialog)
{
    ImageManagerCloseDialog(FileDialog);
}
void ImageManagerOnExportImageFileDialogSelect(FileDialog_t *FileDialog,const char *Directory,const char *File,void *UserData)
{
    ImageManagerDialogData_t *Data;
    Data = (ImageManagerDialogData_t *) UserData;
        
    ProgressBarBegin(Data->GUI->ProgressBar,"Exporting...");
    ProgressBarIncrement(Data->GUI->ProgressBar,Data->VideoSystem,30,"Determining output format.");

    switch( Data->OutputFormat ) {
        case IMAGE_MANAGER_EXPORT_FORMAT_PNG:
            ProgressBarIncrement(Data->GUI->ProgressBar,Data->VideoSystem,50,"Saving to PNG.");
            ImageManagerExportToPNG(Data->ImageManager,Data->FirstImage,Data->LastImage,Data->GUI,Data->VideoSystem,Directory);
            break;
        default:
            DPrintf("ImageManagerOnExportImageFileDialogSelect:Invalid output format\n");
            break;
    }
        
    ProgressBarEnd(Data->GUI->ProgressBar,Data->VideoSystem);
    ImageManagerCloseDialog(FileDialog);
}

void ImageManagerOnExportImageFileDialogCancel(FileDialog_t *FileDialog)
{
    ImageManagerCloseDialog(FileDialog);
}

void ImageManagerExport(ImageManager_t *ImageManager,TIMImage_t *FirstImage,TIMImage_t *LastImage,GUI_t *GUI,VideoSystem_t  *VideoSystem)
{
    ImageManagerDialogData_t *Exporter;
    
    if( !ImageManager ) {
        DPrintf("ImageManagerExport:Invalid ImageManager\n");
        return;
    }
    if( !GUI ) {
        DPrintf("ImageManagerExport:Invalid GUI data\n");
        return;
    }
    Exporter = malloc(sizeof(ImageManagerDialogData_t));
    if( !Exporter ) {
        DPrintf("ImageManagerExport:Couldn't allocate data for the exporter\n");
        return;
    }
    Exporter->ImageManager = ImageManager;
    Exporter->GUI = GUI;
    Exporter->VideoSystem = VideoSystem;
    Exporter->FirstImage = FirstImage;
    Exporter->LastImage = LastImage;
    Exporter->OutputFormat = IMAGE_MANAGER_EXPORT_FORMAT_PNG;

    FileDialogSetTitle(ImageManager->ExportFileDialog,"Export");
    FileDialogOpen(ImageManager->ExportFileDialog,Exporter);
}

void ImageManagerOpenFileDialog(ImageManager_t *ImageManager,GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    ImageManagerDialogData_t *DialogData;
    if( !ImageManager ) {
        return;
    }

    DialogData = malloc(sizeof(ImageManagerDialogData_t));
    if( !DialogData ) {
        DPrintf("ImageManagerOpenFileDialog:Couldn't allocate data for the extra data\n");
        return;
    }
    DialogData->ImageManager = ImageManager;
    DialogData->VideoSystem = VideoSystem;
    DialogData->GUI = GUI;

    FileDialogOpen(ImageManager->ImageFileDialog,DialogData);
}
ImageManager_t *ImageManagerInit(GUI_t *GUI)
{
    ImageManager_t *ImageManager;
    
    if( !GUI ) {
        DPrintf("ImageManagerInit:Invalid GUI\n");
        return NULL;
    }
    ImageManager = malloc(sizeof(ImageManager_t));
    if( !ImageManager ) {
        DPrintf("ImageManagerInit:Couldn't allocate memory for ImageManager\n");
        return NULL;
    }
    ImageManager->ImageList = NULL;
    ImageManager->SelectedImage = NULL;
    ImageManager->VRAM = NULL;

    ImageManager->ImageFileDialog = FileDialogRegister("Open TIM Files",
                                                               ".*",
                                                               ImageManagerOnImageFileDialogSelect,
                                                               ImageManagerOnImageFileDialogCancel);
    ImageManager->ExportFileDialog = FileDialogRegister("Export Audio File",
                                                               NULL,
                                                               ImageManagerOnExportImageFileDialogSelect,
                                                               ImageManagerOnExportImageFileDialogCancel);    
    return ImageManager;
}
