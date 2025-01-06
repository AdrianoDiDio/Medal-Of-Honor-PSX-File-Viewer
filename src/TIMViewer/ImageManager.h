/*
===========================================================================
    Copyright (C) 2018-2025 Adriano Di Dio.
    
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
#ifndef __IMAGE_MANAGER_H_
#define __IMAGE_MANAGER_H_

#include "GUI.h"
#include "../Common/VRAM.h"
#include "../Common/TIM.h"

typedef enum {
    IMAGE_MANAGER_EXPORT_FORMAT_PNG,
    IMAGE_MANAGER_EXPORT_FORMAT_UNKNOWN
} ImageManagerExportFormats_t;

typedef struct ImageManager_s {
    TIMImage_t              *ImageList;
    TIMImage_t              *SelectedImage;
    VRAM_t                  *VRAM;
    FileDialog_t            *ImageFileDialog;
    FileDialog_t            *ExportFileDialog;
} ImageManager_t;

typedef struct ImageManagerDialogData_s {
    ImageManager_t                  *ImageManager;
    VideoSystem_t                   *VideoSystem;
    GUI_t                           *GUI;
    TIMImage_t                      *FirstImage;
    TIMImage_t                      *LastImage;
    int                             OutputFormat;
} ImageManagerDialogData_t;


ImageManager_t          *ImageManagerInit(GUI_t *GUI);
void                    ImageManagerAudioUpdate(void *UserData,Byte *Stream,int Length);
int                     ImageManagerLoadTIMFile(ImageManager_t *ImageManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *File);
void                    ImageManagerExport(ImageManager_t *ImageManager,TIMImage_t *FirstImage,TIMImage_t *LastImage,
                                           GUI_t *GUI,VideoSystem_t  *VideoSystem);
void                    ImageManagerOpenFileDialog(ImageManager_t *ImageManager,GUI_t *GUI,VideoSystem_t *VideoSystem);
void                    ImageManagerCleanUp(ImageManager_t *ImageManager);

#endif//__IMAGE_MANAGER_H_
 
