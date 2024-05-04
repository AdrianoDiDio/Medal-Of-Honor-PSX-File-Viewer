// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    SSTViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SSTViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SSTViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#include "SSTManager.h"
#include "SSTViewer.h"

Config_t *SSTManagerBasePath;

int SSTManagerIsLevelLoaded(const SSTManager_t *SSTManager)
{
    if( !SSTManager ) {
        DPrintf("SSTManagerIsLevelLoaded:Called without a valid SSTManager\n");
        return 0;
    }
    return 1;
}

int SSTManagerGetGameEngine(SSTManager_t *SSTManager)
{
    return SSTManager->GameEngine;
}


void SSTManagerFreeDialogData(FileDialog_t *FileDialog)
{
    SSTManagerDialogData_t *DialogData;
    DialogData = (SSTManagerDialogData_t *) FileDialogGetUserData(FileDialog);
    if( DialogData ) {
        free(DialogData);
    }
}

void SSTManagerCloseDialog(GUI_t *GUI,FileDialog_t *FileDialog)
{
    SSTManagerFreeDialogData(FileDialog);
    FileDialogClose(FileDialog);
    GUIPopWindow(GUI);
}

void SSTManagerCleanUp(SSTManager_t *SSTManager)
{
    SoundSystemPause(SSTManager->SoundSystem);
    if( SSTManager->BasePath ) {
        free(SSTManager->BasePath);
    }
    if( SSTManager->GlobalRSCList ) {
        RSCFree(SSTManager->GlobalRSCList);
    }
    if( FileDialogIsOpen(SSTManager->FileDialog) ) {
        SSTManagerFreeDialogData(SSTManager->FileDialog);
    }
    if( SSTManager->SoundSystem ) {
        SoundSystemCleanUp(SSTManager->SoundSystem);
    }
    if( SSTManager->RenderObjectShader ) {
        free(SSTManager->RenderObjectShader);
    }
    free(SSTManager);
}

void SSTManagerOnAudioUpdate(void *UserData,Byte *Stream,int Length)
{
    SSTManager_t *SSTManager;
    VBMusic_t *CurrentMusic;
    VBMusic_t **CurrentMusicAddress;
    int ChunkLength;
    
    SSTManager = (SSTManager_t *) UserData;
//     CurrentMusic = SSTManager->CurrentLevel->CurrentMusic;
//     CurrentMusicAddress = &SSTManager->CurrentLevel->CurrentMusic;

//     if( CurrentMusic->DataPointer >= CurrentMusic->Size ) {
//         CurrentMusic->DataPointer = 0;
//         if( CurrentMusic->Next ) {
//             *CurrentMusicAddress = (*CurrentMusicAddress)->Next;
//         } else {
//             if( SSTManager->CurrentLevel->IsAmbient ) {
//                 *CurrentMusicAddress = SSTManager->CurrentLevel->AmbientMusicList;
//             } else {
//                 *CurrentMusicAddress = SSTManager->CurrentLevel->MusicList;
//             }
//         }
//     }
    for (int i = 0; i < Length; i++) {
        Stream[i] = 0;
    }
    ChunkLength = (CurrentMusic->Size - CurrentMusic->DataPointer);
    if( ChunkLength > Length ) {
        ChunkLength = Length;
    }
    if( SoundVolume->IValue < 0 || SoundVolume->IValue > 128 ) {
        ConfigSetNumber("SoundVolume",128);
    }
    SDL_MixAudioFormat(Stream, /*&CurrentMusic->Data[CurrentMusic->DataPointer]*/Stream, AUDIO_F32, ChunkLength, SoundVolume->IValue);
//     CurrentMusic->DataPointer += ChunkLength;
}

void SSTManagerDrawString(const SSTManager_t *SSTManager,const char *String,float x,float y,Color4f_t Color)
{
    if( !SSTManager ) {
        DPrintf("SSTManagerDrawString:Called without a valid level manager.\n");
        return;
    }
    if( !SSTManagerIsLevelLoaded(SSTManager) ) {
        DPrintf("SSTManagerDrawString:Called without a valid level\n");
        return;
    }
//     FontDrawString(SSTManager->CurrentLevel->Font,SSTManager->CurrentLevel->VRAM,String,x,y,Color);
}
void SSTManagerUpdateSoundSettings(SSTManager_t *SSTManager,int SoundValue)
{
    SoundSystemPause(SSTManager->SoundSystem);
//     LevelSetMusicTrackSettings(SSTManager->CurrentLevel,SSTManager->SoundSystem,SSTManager->GameEngine,SoundValue);
//     if( LevelEnableMusicTrack->IValue && SSTManager->CurrentLevel->MusicList ) {
//         SoundSystemPlay(SSTManager->SoundSystem);
//     }
}


/*
 * Initialize the game from the config string.
 * Returns 0 if the config value is not valid 1 otherwise.
 * Note that the config key is cleared if it was not valid.
 */
int SSTManagerLoadFromConfig(SSTManager_t *SSTManager,GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    if( !SSTManagerBasePath->Value[0] ) {
        return 0;
    }
    if( !SSTManagerInitWithPath(SSTManager,GUI,VideoSystem,SSTManagerBasePath->Value) ) {
        ConfigSet("GameBasePath","");
        return 0;
    }
    return 1;
}
void SSTManagerOnDirSelected(FileDialog_t *FileDialog,const char *Path,const char *File,void *UserData)
{
    SSTManagerDialogData_t *SSTManagerDialogData;
    int LoadStatus;
    
    SSTManagerDialogData = (SSTManagerDialogData_t *) UserData;
    LoadStatus = SSTManagerInitWithPath(SSTManagerDialogData->SSTManager,SSTManagerDialogData->GUI,SSTManagerDialogData->VideoSystem,Path);

    if( !LoadStatus ) {
        ErrorMessageDialogSet(SSTManagerDialogData->GUI->ErrorMessageDialog,"Selected path doesn't seems to contain any game file...\n"
        "Please select a folder containing MOH or MOH:Undergound.");
    } else {
        //Close it if we managed to load it.
        ConfigSet("GameBasePath",Path);
        SSTManagerCloseDialog(SSTManagerDialogData->GUI,FileDialog);
    }
}
void SSTManagerOnDirSelectionCancelled(FileDialog_t *FileDialog)
{
    SSTManagerDialogData_t *SSTManagerDialogData;
    SSTManagerDialogData = (SSTManagerDialogData_t *) FileDialogGetUserData(FileDialog);
    if( SSTManagerDialogData->SSTManager->IsPathSet ) {
        SSTManagerCloseDialog(SSTManagerDialogData->GUI,FileDialog);
    }
}


void SSTManagerUpdate(SSTManager_t *SSTManager,Camera_t *Camera)
{    
    //SSTManager has not received a valid path yet.
    if( !SSTManager->IsPathSet ) {
        return;
    }
}
void SSTManagerDraw(SSTManager_t *SSTManager,Camera_t *Camera)
{
    mat4 ProjectionMatrix;
    //SSTManager has not received a valid path yet.
    if( !SSTManager->IsPathSet ) {
        return;
    }

    glm_perspective(glm_rad(110.f),(float) VidConfigWidth->IValue / (float) VidConfigHeight->IValue,1.f, 4096.f,ProjectionMatrix);     
    
}

int SSTManagerInitWithPath(SSTManager_t *SSTManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *Path)
{
    char *Buffer;
    char *RSCBuffer;
    RSC_t *RSC;
    
    if( !SSTManager ) {
        DPrintf("SSTManagerInitWithPath:Called without a valid struct\n");
        return 0;
    }
    if( !Path ) {
        DPrintf("SSTManagerInitWithPath:Called without a valid path\n");
        return 0;
    }
    if( SSTManager->GlobalRSCList ) {
        RSCFree(SSTManager->GlobalRSCList);
        SSTManager->GlobalRSCList = NULL;
    }
    ProgressBarBegin(GUI->ProgressBar,"Loading Global RSC files");
    
    //First step, load the global and global2 files
    asprintf(&RSCBuffer,"%s%cDATA%cGLOBAL.RSC",Path,PATH_SEPARATOR,PATH_SEPARATOR);
    SSTManager->GlobalRSCList = RSCLoad(RSCBuffer);
    
    if( !SSTManager->GlobalRSCList ) {
        DPrintf("Failed to load global rsc file\n");
        goto Failure;
    }
    free(RSCBuffer);
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,20,"Loading Global2 RSC file");
    asprintf(&RSCBuffer,"%s%cDATA%cGLOBAL2.RSC",Path,PATH_SEPARATOR,PATH_SEPARATOR);
    RSC = RSCLoad(RSCBuffer);
    
    if( !RSC ) {
        DPrintf("Failed to load global2 rsc file\n");
        goto Failure;
    }
    //NOTE(Adriano):MOH Underground has an empty global2 file
    DPrintf("SSTManagerInitWithPath:Global2 has %i files\n",RSC->Header.NumEntry);
    if( RSC->Header.NumEntry > 0 ) {
        SSTManager->GameEngine = MOH_GAME_STANDARD;
        RSCAppend(&SSTManager->GlobalRSCList,RSC);
    } else {
        DPrintf("SSTManagerInitWithPath:Underground!\n");
        SSTManager->GameEngine = MOH_GAME_UNDERGROUND;
        RSCFree(RSC);
    }
    sprintf(SSTManager->EngineName,"%s",SSTManager->GameEngine == MOH_GAME_STANDARD ? "Medal Of Honor" : "Medal of Honor:Underground");
    DPrintf("SSTManagerInitWithPath:Detected game engine %s\n",SSTManager->EngineName);
    DPrintf("Found %i scripts\n",RSCGetDirectoryFileCount(SSTManager->GlobalRSCList,"global\\script\\"));
//     for( int i = 1; i <= 2; i++ ) {
//         asprintf(&Buffer,"Loading Mission %i Level 1",i);
//         ProgressBarSetDialogTitle(GUI->ProgressBar,Buffer);
//         Level = LevelInit(GUI,VideoSystem,SSTManager->SoundSystem,Path,i,1,&GameEngine);
//         free(Buffer);
//         if( Level ) {
//             if( SSTManager->BasePath ) {
//                 free(SSTManager->BasePath);
//             }
//             SSTManagerSwitchLevel(SSTManager,Level);
//             Loaded = 1;
//             SSTManager->IsPathSet = Loaded;
//             SSTManager->BasePath = StringCopy(Path);
//             SSTManager->GameEngine = GameEngine;
//             sprintf(SSTManager->EngineName,"%s",GameEngine == MOH_GAME_STANDARD ? "Medal Of Honor" : "Medal of Honor:Underground");
//             SSTManager->HasToSpawnCamera = 1;
//             break;
//         }
//     }
    free(RSCBuffer);
    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
    return 1;
Failure:
    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
    free(RSCBuffer);
    return 0;
}
int SSTManagerLoadLevel(SSTManager_t *SSTManager,GUI_t *GUI,VideoSystem_t *VideoSystem,int MissionNumber,int LevelNumber)
{
//     Level_t *Level;
    char *Buffer;

    if( !SSTManager->IsPathSet ) {
        DPrintf("SSTManagerLoadLevel:Called without a valid path set\n");
        return 0;
    }
    if( SSTManagerIsLevelLoaded(SSTManager) ) {
//         if( SSTManager->CurrentLevel->MissionNumber == MissionNumber && SSTManager->CurrentLevel->LevelNumber == LevelNumber ) {
//             DPrintf("SSTManagerLoadLevel:Attempted to load the same level...\n");
//             return 0;
//         }
    }
    asprintf(&Buffer,"Loading Mission %i Level %i...",MissionNumber,LevelNumber);
//     ProgressBarBegin(GUI->ProgressBar,Buffer);
//     Level = LevelInit(GUI,VideoSystem,SSTManager->SoundSystem,SSTManager->BasePath,MissionNumber,LevelNumber,NULL);
//     ProgressBarEnd(GUI->ProgressBar,VideoSystem);
//     if( !Level ) {
//         printf("SSTManagerLoadLevel:Couldn't load mission %i level %i...\n",MissionNumber,LevelNumber);
//         free(Buffer);
//         return 0;
//     }
//     SSTManagerSwitchLevel(SSTManager,Level);
//     SSTManager->HasToSpawnCamera = 1;
    free(Buffer);
    return 1;
}

void SSTManagerToggleFileDialog(SSTManager_t *SSTManager,GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    SSTManagerDialogData_t *DialogData;
    
    if( FileDialogIsOpen(SSTManager->FileDialog) ) {
        if( SSTManager->IsPathSet ) {
            SSTManagerCloseDialog(GUI,SSTManager->FileDialog);
        }
    } else {
        DialogData = malloc(sizeof(SSTManagerDialogData_t));
        if( !DialogData ) {
            DPrintf("SSTManagerToggleFileDialog:Couldn't allocate data for the exporter\n");
            return;
        }
        DialogData->SSTManager = SSTManager;
        DialogData->GUI = GUI;
        DialogData->VideoSystem = VideoSystem;
        FileDialogOpenWithUserData(SSTManager->FileDialog,SSTManager->BasePath,DialogData);
        GUIPushWindow(GUI);
    }
}

SSTManager_t *SSTManagerInit(GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    SSTManager_t *SSTManager;

    SSTManager = malloc(sizeof(SSTManager_t));
    
    if( !SSTManager ) {
        printf("SSTManagerInit:Failed to allocate memory for struct\n");
        return NULL;
    }
    SSTManager->SoundSystem = NULL;
    SSTManager->SoundSystem = SoundSystemInit(SSTManagerOnAudioUpdate,SSTManager);
    if( !SSTManager->SoundSystem ) {
        DPrintf("SSTManagerInit:Couldn't Initialize SoundSystem\n");
        free(SSTManager);
        return NULL;
    }
    SSTManager->RenderObjectShader = NULL;
//     if( !SSTManagerInitRenderObjectShader(SSTManager) ) {
//         DPrintf("SSTManagerInit:Couldn't load RenderObjectShader\n");
//         SoundSystemCleanUp(SSTManager->SoundSystem);
//         free(SSTManager);
//         return NULL;
//     }
    SSTManager->HasToSpawnCamera = 0;
    SSTManager->FileDialog = FileDialogRegister("Select Directory",NULL,
                                                     SSTManagerOnDirSelected,SSTManagerOnDirSelectionCancelled);
    SSTManager->GlobalRSCList = NULL;
    SSTManager->BasePath = NULL;
    //No path has been provided to it yet.
    SSTManager->IsPathSet = 0;
    
    SSTManagerBasePath = ConfigGet("GameBasePath");

    if( !SSTManagerLoadFromConfig(SSTManager,GUI,VideoSystem) ) {
        SSTManagerToggleFileDialog(SSTManager,GUI,VideoSystem);
    }
    return SSTManager;
}
