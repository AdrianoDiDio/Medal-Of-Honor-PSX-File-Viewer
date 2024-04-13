/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    SoundExplorer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SoundExplorer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SoundExplorer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#ifndef __SOUND_MANAGER_H_
#define __SOUND_MANAGER_H_

#include "GUI.h"
#include "../Common/Sound.h"

typedef enum {
    SOUND_MANAGER_EXPORT_FORMAT_WAV,
    SOUND_MANAGER_EXPORT_FORMAT_UNKNOWN
} SoundManagerExportFormats_t;

typedef struct SoundManager_s {    
    SoundSystem_t           *SoundSystem;
    VBMusic_t               *SoundList;
    VBMusic_t               *SelectedSound;
    FileDialog_t            *SoundFileDialog;
    FileDialog_t            *ExportFileDialog;

    bool                    Loop;
} SoundManager_t;

typedef struct SoundManagerDialogData_s {
    SoundManager_t                  *SoundManager;
    VideoSystem_t                   *VideoSystem;
    GUI_t                           *GUI;
    VBMusic_t                       *FirstSound;
    VBMusic_t                       *LastSound;
    int                             OutputFormat;
} SoundManagerDialogData_t;


SoundManager_t          *SoundManagerInit(GUI_t *GUI);
void                    SoundManagerAudioUpdate(void *UserData,Byte *Stream,int Length);
void                    SoundManagerLoadAudioFile(SoundManager_t *SoundManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *File);
void                    SoundManagerResample(SoundManager_t *SoundManager,GUI_t *GUI,VideoSystem_t *VideoSystem,VBMusic_t *Sound,int UpSample);
void                    SoundManagerExportAll(SoundManager_t *SoundManager,GUI_t *GUI,VideoSystem_t  *VideoSystem);
void                    SoundManagerExport(SoundManager_t *SoundManager,VBMusic_t *FirstSound,VBMusic_t *LastSound,
                                           GUI_t *GUI,VideoSystem_t  *VideoSystem);
void                    SoundManagerOpenFileDialog(SoundManager_t *SoundManager,GUI_t *GUI,VideoSystem_t *VideoSystem);
void                    SoundManagerCleanUp(SoundManager_t *SoundManager);
#endif//__SOUND_MANAGER_H_
 
