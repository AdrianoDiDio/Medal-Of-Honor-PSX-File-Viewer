// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
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
#include "SoundManager.h"
#include "SoundExplorer.h"
#include "VAB.h"
#include "../Common/TIM.h"

void SoundManagerAudioUpdate(void *UserData,Byte *Stream,int Length)
{
    SoundManager_t *SoundManager;
    VBMusic_t *SelectedSound;
    int ChunkLength;
    
    SoundManager = (SoundManager_t *) UserData;
    SelectedSound = SoundManager->SelectedSound;
    
    for (int i = 0; i < Length; i++) {
        Stream[i] = 0;
    }
    if( !SelectedSound ) {
        return;
    }
    if( SelectedSound->DataPointer >= SelectedSound->Size ) {
        SelectedSound->DataPointer = 0;
        if( !SoundManager->Loop ) {
            SoundSystemPause(SoundManager->SoundSystem);
        }
    }
    ChunkLength = (SelectedSound->Size - SelectedSound->DataPointer);
    if( ChunkLength > Length ) {
        ChunkLength = Length;
    }
    if( SoundVolume->IValue < 0 || SoundVolume->IValue > 128 ) {
        ConfigSetNumber("SoundVolume",128);
    }
    SDL_MixAudioFormat(Stream, &SelectedSound->Data[SelectedSound->DataPointer], AUDIO_F32, ChunkLength, SoundVolume->IValue);
    SelectedSound->DataPointer += ChunkLength;
}
void SoundManagerFreeDialogData(FileDialog_t *FileDialog)
{
    SoundManagerDialogData_t *DialogData;
    if( !FileDialog ) {
        return;
    }
    DialogData = (SoundManagerDialogData_t *) FileDialogGetUserData(FileDialog);
    if( DialogData ) {
        free(DialogData);
    }
}

void SoundManagerCleanUp(SoundManager_t *SoundManager)
{
    if( !SoundManager ) {
        return;
    }
    
    if( SoundManager->SoundSystem ) {
        SoundSystemCleanUp(SoundManager->SoundSystem);
    }
    //If the user didn't close the dialog free the user data that we passed to it.
    if( FileDialogIsOpen(SoundManager->SoundFileDialog) ) {
        SoundManagerFreeDialogData(SoundManager->SoundFileDialog);
    }
    if( FileDialogIsOpen(SoundManager->ExportFileDialog) ) {
        SoundManagerFreeDialogData(SoundManager->ExportFileDialog);
    }
    
    SoundSystemClearMusicList(SoundManager->SoundList);
    free(SoundManager);
}


void SoundManagerCloseDialog(FileDialog_t *FileDialog)
{
    SoundManagerFreeDialogData(FileDialog);
    FileDialogClose(FileDialog);
}

VBMusic_t *SoundManagerLoadVBFile(const char *File,GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    VBMusic_t *VBFile;

    if( !File ) {
        DPrintf("SoundManagerLoadVBFile:Invalid File\n");
        return NULL;
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,50.f,"Loading Music file");
    VBFile = SoundSystemLoadVBMusic(File,-1);
    if( !VBFile ) {
        DPrintf("SoundManagerLoadVBFile:Couldn't load VB file %s\n",File);
        return NULL;
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,100.f,"Done");
    return VBFile;

}
int SoundManagerLoadVABFile(VBMusic_t **SoundList,FILE *VABFile,int VABOffset,int VABNumber)
{
    VBMusic_t *VAGList;

    if( !SoundList || !VABFile ) {
        bool InvalidFile = (VABFile == NULL ? true : false);
        printf("SoundManagerLoadVABFile: Invalid %s\n",InvalidFile ? "file" : "SoundList");
        return 0;
    }
    
    VAGList = VABLoadAllVAGs(VABFile,VABNumber,VABOffset);
    if( !VAGList ) {
        return 0;
    }
    SoundSystemAddMusicToList(SoundList,VAGList);
    return 1;
}
VBMusic_t *SoundManagerLoadTAFFile(const char *File,GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    FILE *TAFFile;
    TIMImage_t *Image;
    VBMusic_t *MusicList;
    int NumImages;
    int NumVAB;
    int *VABOffsetList;
    int TAFHeaderPosition;
    int i;
    
    //First load all the TIM images and free them in order to obtain the correct offset....
    TAFFile = fopen(File,"rb");
    MusicList = NULL;
    
    if( !TAFFile ) {
        DPrintf("SoundManagerLoadTAFFile:Error opening file %s!\n",File);
        return NULL;
    }
    NumImages = 0;
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,10.f,"Loading all TIM Images");
    while( 1 ) {
        Image = TIMLoadImage(TAFFile,NULL,NumImages);
        if( Image == NULL ) {
            break;
        }
        TIMImageListFree(Image);
        NumImages++;
    }
    if( !NumImages ) {
        DPrintf("SoundManagerLoadTAFFile:Invalid TAF files (No images found...)\n");
        return NULL;
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,20.f,"Reading TAF Header");
    DPrintf("SoundManagerLoadTAFFile:Loaded %i images\n",NumImages);
    // Then read the header to see how many VABs we have to load...
    TAFHeaderPosition = ftell(TAFFile);
    fread(&NumVAB,sizeof(NumVAB),1,TAFFile);
    if( !NumVAB ) {
        DPrintf("SoundManagerLoadTAFFile:No VABs found inside the file\n");
        return NULL;
    }
    VABOffsetList = malloc(NumVAB * sizeof(int));
    for( i = 0; i < NumVAB; i++ ) {
        fread(&VABOffsetList[i],sizeof(VABOffsetList[i]),1,TAFFile);
        VABOffsetList[i] += TAFHeaderPosition;
        DPrintf("SoundManagerLoadTAFFile:VAB %i at %i\n",i,VABOffsetList[i]);
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,70.f,"Loading sound files");
    for( i = 0; i < NumVAB; i++ ) {
        SoundManagerLoadVABFile(&MusicList,TAFFile,VABOffsetList[i],i + 1);
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,100.f,"Done");
    free(VABOffsetList);
    fclose(TAFFile);
    return MusicList;
}
void SoundManagerLoadAudioFile(SoundManager_t *SoundManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *File)
{
    VBMusic_t *VBMusicList;
    const char *Extension;

    if( !SoundManager ) {
        DPrintf("SoundManagerLoadAudioFile:Invalid SoundManager\n");
        return;
    }
    if( !File ) {
        DPrintf("SoundManagerLoadAudioFile:Invalid file\n");
        return;
    }
    Extension = GetFileExtension(File);
    VBMusicList = NULL;
    ProgressBarBegin(GUI->ProgressBar,"Loading Audio File");
    if( Extension ) {
        if( !strcasecmp(Extension,"TAF") ) {
            DPrintf("SoundManagerLoadAudioFile:Loading TAF file\n");
            ProgressBarIncrement(GUI->ProgressBar,VideoSystem,10.f,"Loading TAF file");
            VBMusicList = SoundManagerLoadTAFFile(File,GUI,VideoSystem);
        } else if( !strcasecmp(Extension,"VB") ) {
            DPrintf("SoundManagerLoadAudioFile:Loading VB file\n");
            ProgressBarIncrement(GUI->ProgressBar,VideoSystem,10.f,"Loading VB file");
            VBMusicList = SoundManagerLoadVBFile(File,GUI,VideoSystem);
        }
    }
    ProgressBarEnd(GUI->ProgressBar,VideoSystem);

    if( VBMusicList ) {
        if( SoundManager->SoundList ) {
            SoundSystemClearMusicList(SoundManager->SoundList);
        }
        SoundManager->SoundList = VBMusicList;
        SoundManager->SelectedSound = VBMusicList;
    } else {
        ErrorMessageDialogSet(GUI->ErrorMessageDialog,"Couldn't open the file");
    }
}
void SoundManagerOnAudioFileDialogSelect(FileDialog_t *FileDialog,const char *Directory,const char *File,void *UserData)
{
    SoundManagerDialogData_t *Data;
    Data = (SoundManagerDialogData_t *) UserData;
    SoundManagerLoadAudioFile(Data->SoundManager,Data->GUI,Data->VideoSystem,File);
    SoundManagerCloseDialog(FileDialog);
}
void SoundManagerExportToWav(SoundManager_t *SoundManager,VBMusic_t *FirstSound,VBMusic_t *LastSound,
                             GUI_t *GUI,VideoSystem_t *VideoSystem,const char *Directory)
{
    VBMusic_t *Iterator;
    if( !SoundManager ) {
        DPrintf("SoundManagerExportToWav:Invalid user data\n");
        return;
    }
    
    if( !FirstSound ) {
        DPrintf("SoundManagerExportToWav:Invalid Start Sound!\n");
        return;
    }    
    ProgressBarSetDialogTitle(GUI->ProgressBar,"Exporting to Wav...");
    if( FirstSound == LastSound ) {
        ProgressBarIncrement(GUI->ProgressBar,VideoSystem,80,"Exporting single file to WAV.");
        SoundSystemDumpMusicToWav(FirstSound,NULL,Directory);
    } else {
        ProgressBarIncrement(GUI->ProgressBar,VideoSystem,80,"Exporting multiple files to WAV.");
        for( Iterator = FirstSound; Iterator != LastSound; Iterator = Iterator->Next ) {
            SoundSystemDumpMusicToWav(Iterator,NULL,Directory);
        }
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,100,"Done.");
}
void SoundManagerOnAudioFileDialogCancel(FileDialog_t *FileDialog)
{
    SoundManagerCloseDialog(FileDialog);
}
void SoundManagerOnExportAudioFileDialogSelect(FileDialog_t *FileDialog,const char *Directory,const char *File,void *UserData)
{
    SoundManagerDialogData_t *Data;
    Data = (SoundManagerDialogData_t *) UserData;
        
    ProgressBarBegin(Data->GUI->ProgressBar,"Exporting...");
    ProgressBarIncrement(Data->GUI->ProgressBar,Data->VideoSystem,30,"Determining output format.");

    switch( Data->OutputFormat ) {
        case SOUND_MANAGER_EXPORT_FORMAT_WAV:
            ProgressBarIncrement(Data->GUI->ProgressBar,Data->VideoSystem,50,"Saving to WAV.");
            SoundManagerExportToWav(Data->SoundManager,Data->FirstSound,Data->LastSound,Data->GUI,Data->VideoSystem,Directory);
            break;
        default:
            DPrintf("SoundManagerOnExportAudioFileDialogSelect:Invalid output format\n");
            break;
    }
        
    ProgressBarEnd(Data->GUI->ProgressBar,Data->VideoSystem);
    SoundManagerCloseDialog(FileDialog);
}

void SoundManagerOnExportAudioFileDialogCancel(FileDialog_t *FileDialog)
{
    SoundManagerCloseDialog(FileDialog);
}

void SoundManagerExport(SoundManager_t *SoundManager,VBMusic_t *FirstSound,VBMusic_t *LastSound,GUI_t *GUI,VideoSystem_t  *VideoSystem)
{
    SoundManagerDialogData_t *Exporter;
    
    if( !SoundManager ) {
        DPrintf("SoundManagerExport:Invalid SoundManager\n");
        return;
    }
    if( !GUI ) {
        DPrintf("SoundManagerExport:Invalid GUI data\n");
        return;
    }
    Exporter = malloc(sizeof(SoundManagerDialogData_t));
    if( !Exporter ) {
        DPrintf("SoundManagerExport:Couldn't allocate data for the exporter\n");
        return;
    }
    Exporter->SoundManager = SoundManager;
    Exporter->GUI = GUI;
    Exporter->VideoSystem = VideoSystem;
    Exporter->FirstSound = FirstSound;
    Exporter->LastSound = LastSound;
    Exporter->OutputFormat = SOUND_MANAGER_EXPORT_FORMAT_WAV;

    FileDialogSetTitle(SoundManager->ExportFileDialog,"Export");
    FileDialogOpenWithUserData(SoundManager->ExportFileDialog,Exporter);
}
void SoundManagerExportAll(SoundManager_t *SoundManager,GUI_t *GUI,VideoSystem_t  *VideoSystem)
{
    SoundManagerExport(SoundManager,SoundManager->SoundList,NULL,GUI,VideoSystem);
}
void SoundManagerOpenFileDialog(SoundManager_t *SoundManager,GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    SoundManagerDialogData_t *DialogData;
    if( !SoundManager ) {
        return;
    }

    DialogData = malloc(sizeof(SoundManagerDialogData_t));
    if( !DialogData ) {
        DPrintf("SoundManagerOpenFileDialog:Couldn't allocate data for the extra data\n");
        return;
    }
    DialogData->SoundManager = SoundManager;
    DialogData->VideoSystem = VideoSystem;
    DialogData->GUI = GUI;

    FileDialogOpenWithUserData(SoundManager->SoundFileDialog,DialogData);
}
SoundManager_t *SoundManagerInit(GUI_t *GUI)
{
    SoundManager_t *SoundManager;
    
    if( !GUI ) {
        DPrintf("SoundManagerInit:Invalid GUI\n");
        return NULL;
    }
    SoundManager = malloc(sizeof(SoundManager_t));
    if( !SoundManager ) {
        DPrintf("SoundManagerInit:Couldn't allocate memory for SoundManager\n");
        return NULL;
    }
    SoundManager->SoundSystem = NULL;
    SoundManager->SoundList = NULL;
    SoundManager->SelectedSound = NULL;
    SoundManager->Loop = 0;
    
    SoundManager->SoundSystem = SoundSystemInit(SoundManagerAudioUpdate,SoundManager);
    if( !SoundManager->SoundSystem ) {
        printf("ApplicationInit:Failed to initialize the Sound System\n");
        free(SoundManager);
        return NULL;
    }
    SoundManager->SoundFileDialog = FileDialogRegister("Open Audio File",
                                                               "Audio files (*.VB *.TAF){.VB,.TAF}",
                                                               SoundManagerOnAudioFileDialogSelect,
                                                               SoundManagerOnAudioFileDialogCancel);
    SoundManager->ExportFileDialog = FileDialogRegister("Export Audio File",
                                                               NULL,
                                                               SoundManagerOnExportAudioFileDialogSelect,
                                                               SoundManagerOnExportAudioFileDialogCancel);
    return SoundManager;
}
