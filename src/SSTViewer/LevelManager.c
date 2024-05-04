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

#include "LevelManager.h"
#include "SSTViewer.h"

Config_t *LevelManagerBasePath;

int LevelManagerIsLevelLoaded(const LevelManager_t *LevelManager)
{
    if( !LevelManager ) {
        DPrintf("LevelManagerIsLevelLoaded:Called without a valid LevelManager\n");
        return 0;
    }
    return 1;
}

int LevelManagerGetGameEngine(LevelManager_t *LevelManager)
{
    return LevelManager->GameEngine;
}


void LevelManagerFreeDialogData(FileDialog_t *FileDialog)
{
    LevelManagerDialogData_t *DialogData;
    DialogData = (LevelManagerDialogData_t *) FileDialogGetUserData(FileDialog);
    if( DialogData ) {
        free(DialogData);
    }
}

void LevelManagerCloseDialog(GUI_t *GUI,FileDialog_t *FileDialog)
{
    LevelManagerFreeDialogData(FileDialog);
    FileDialogClose(FileDialog);
    GUIPopWindow(GUI);
}

void LevelManagerCleanUp(LevelManager_t *LevelManager)
{
    SoundSystemPause(LevelManager->SoundSystem);
    if( LevelManager->BasePath ) {
        free(LevelManager->BasePath);
    }
    if( FileDialogIsOpen(LevelManager->FileDialog) ) {
        LevelManagerFreeDialogData(LevelManager->FileDialog);
    }
    if( LevelManager->SoundSystem ) {
        SoundSystemCleanUp(LevelManager->SoundSystem);
    }
    if( LevelManager->RenderObjectShader ) {
        free(LevelManager->RenderObjectShader);
    }
    free(LevelManager);
}

void LevelManagerOnAudioUpdate(void *UserData,Byte *Stream,int Length)
{
    LevelManager_t *LevelManager;
    VBMusic_t *CurrentMusic;
    VBMusic_t **CurrentMusicAddress;
    int ChunkLength;
    
    LevelManager = (LevelManager_t *) UserData;
//     CurrentMusic = LevelManager->CurrentLevel->CurrentMusic;
//     CurrentMusicAddress = &LevelManager->CurrentLevel->CurrentMusic;

//     if( CurrentMusic->DataPointer >= CurrentMusic->Size ) {
//         CurrentMusic->DataPointer = 0;
//         if( CurrentMusic->Next ) {
//             *CurrentMusicAddress = (*CurrentMusicAddress)->Next;
//         } else {
//             if( LevelManager->CurrentLevel->IsAmbient ) {
//                 *CurrentMusicAddress = LevelManager->CurrentLevel->AmbientMusicList;
//             } else {
//                 *CurrentMusicAddress = LevelManager->CurrentLevel->MusicList;
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

void LevelManagerDrawString(const LevelManager_t *LevelManager,const char *String,float x,float y,Color4f_t Color)
{
    if( !LevelManager ) {
        DPrintf("LevelManagerDrawString:Called without a valid level manager.\n");
        return;
    }
    if( !LevelManagerIsLevelLoaded(LevelManager) ) {
        DPrintf("LevelManagerDrawString:Called without a valid level\n");
        return;
    }
//     FontDrawString(LevelManager->CurrentLevel->Font,LevelManager->CurrentLevel->VRAM,String,x,y,Color);
}
void LevelManagerUpdateSoundSettings(LevelManager_t *LevelManager,int SoundValue)
{
    SoundSystemPause(LevelManager->SoundSystem);
//     LevelSetMusicTrackSettings(LevelManager->CurrentLevel,LevelManager->SoundSystem,LevelManager->GameEngine,SoundValue);
//     if( LevelEnableMusicTrack->IValue && LevelManager->CurrentLevel->MusicList ) {
//         SoundSystemPlay(LevelManager->SoundSystem);
//     }
}


/*
 * Initialize the game from the config string.
 * Returns 0 if the config value is not valid 1 otherwise.
 * Note that the config key is cleared if it was not valid.
 */
int LevelManagerLoadFromConfig(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    if( !LevelManagerBasePath->Value[0] ) {
        return 0;
    }
    if( !LevelManagerInitWithPath(LevelManager,GUI,VideoSystem,LevelManagerBasePath->Value) ) {
        ConfigSet("GameBasePath","");
        return 0;
    }
    return 1;
}
void LevelManagerOnDirSelected(FileDialog_t *FileDialog,const char *Path,const char *File,void *UserData)
{
    LevelManagerDialogData_t *LevelManagerDialogData;
    int LoadStatus;
    
    LevelManagerDialogData = (LevelManagerDialogData_t *) UserData;
    LoadStatus = LevelManagerInitWithPath(LevelManagerDialogData->LevelManager,LevelManagerDialogData->GUI,LevelManagerDialogData->VideoSystem,Path);

    if( !LoadStatus ) {
        ErrorMessageDialogSet(LevelManagerDialogData->GUI->ErrorMessageDialog,"Selected path doesn't seems to contain any game file...\n"
        "Please select a folder containing MOH or MOH:Undergound.");
    } else {
        //Close it if we managed to load it.
        ConfigSet("GameBasePath",Path);
        LevelManagerCloseDialog(LevelManagerDialogData->GUI,FileDialog);
    }
}
void LevelManagerOnDirSelectionCancelled(FileDialog_t *FileDialog)
{
    LevelManagerDialogData_t *LevelManagerDialogData;
    LevelManagerDialogData = (LevelManagerDialogData_t *) FileDialogGetUserData(FileDialog);
    if( LevelManagerDialogData->LevelManager->IsPathSet ) {
        LevelManagerCloseDialog(LevelManagerDialogData->GUI,FileDialog);
    }
}


void LevelManagerUpdate(LevelManager_t *LevelManager,Camera_t *Camera)
{    
    //LevelManager has not received a valid path yet.
    if( !LevelManager->IsPathSet ) {
        return;
    }
}
void LevelManagerDraw(LevelManager_t *LevelManager,Camera_t *Camera)
{
    mat4 ProjectionMatrix;
    //LevelManager has not received a valid path yet.
    if( !LevelManager->IsPathSet ) {
        return;
    }

    glm_perspective(glm_rad(110.f),(float) VidConfigWidth->IValue / (float) VidConfigHeight->IValue,1.f, 4096.f,ProjectionMatrix);     
    
}

int LevelManagerInitWithPath(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *Path)
{
//     Level_t *Level;
    int Loaded;
    int GameEngine;
    char *Buffer;
    
    if( !LevelManager ) {
        DPrintf("LevelManagerInitWithPath:Called without a valid struct\n");
        return 0;
    }
    if( !Path ) {
        DPrintf("LevelManagerInitWithPath:Called without a valid path\n");
        return 0;
    }
    ProgressBarBegin(GUI->ProgressBar,"Loading Mission 1 Level 1");
    Loaded = 0;

//     for( int i = 1; i <= 2; i++ ) {
//         asprintf(&Buffer,"Loading Mission %i Level 1",i);
//         ProgressBarSetDialogTitle(GUI->ProgressBar,Buffer);
//         Level = LevelInit(GUI,VideoSystem,LevelManager->SoundSystem,Path,i,1,&GameEngine);
//         free(Buffer);
//         if( Level ) {
//             if( LevelManager->BasePath ) {
//                 free(LevelManager->BasePath);
//             }
//             LevelManagerSwitchLevel(LevelManager,Level);
//             Loaded = 1;
//             LevelManager->IsPathSet = Loaded;
//             LevelManager->BasePath = StringCopy(Path);
//             LevelManager->GameEngine = GameEngine;
//             sprintf(LevelManager->EngineName,"%s",GameEngine == MOH_GAME_STANDARD ? "Medal Of Honor" : "Medal of Honor:Underground");
//             LevelManager->HasToSpawnCamera = 1;
//             break;
//         }
//     }
    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
    return Loaded;
}
int LevelManagerLoadLevel(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem,int MissionNumber,int LevelNumber)
{
//     Level_t *Level;
    char *Buffer;

    if( !LevelManager->IsPathSet ) {
        DPrintf("LevelManagerLoadLevel:Called without a valid path set\n");
        return 0;
    }
    if( LevelManagerIsLevelLoaded(LevelManager) ) {
//         if( LevelManager->CurrentLevel->MissionNumber == MissionNumber && LevelManager->CurrentLevel->LevelNumber == LevelNumber ) {
//             DPrintf("LevelManagerLoadLevel:Attempted to load the same level...\n");
//             return 0;
//         }
    }
    asprintf(&Buffer,"Loading Mission %i Level %i...",MissionNumber,LevelNumber);
//     ProgressBarBegin(GUI->ProgressBar,Buffer);
//     Level = LevelInit(GUI,VideoSystem,LevelManager->SoundSystem,LevelManager->BasePath,MissionNumber,LevelNumber,NULL);
//     ProgressBarEnd(GUI->ProgressBar,VideoSystem);
//     if( !Level ) {
//         printf("LevelManagerLoadLevel:Couldn't load mission %i level %i...\n",MissionNumber,LevelNumber);
//         free(Buffer);
//         return 0;
//     }
//     LevelManagerSwitchLevel(LevelManager,Level);
//     LevelManager->HasToSpawnCamera = 1;
    free(Buffer);
    return 1;
}

void LevelManagerToggleFileDialog(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    LevelManagerDialogData_t *DialogData;
    
    if( FileDialogIsOpen(LevelManager->FileDialog) ) {
        if( LevelManager->IsPathSet ) {
            LevelManagerCloseDialog(GUI,LevelManager->FileDialog);
        }
    } else {
        DialogData = malloc(sizeof(LevelManagerDialogData_t));
        if( !DialogData ) {
            DPrintf("LevelManagerToggleFileDialog:Couldn't allocate data for the exporter\n");
            return;
        }
        DialogData->LevelManager = LevelManager;
        DialogData->GUI = GUI;
        DialogData->VideoSystem = VideoSystem;
        FileDialogOpenWithUserData(LevelManager->FileDialog,LevelManager->BasePath,DialogData);
        GUIPushWindow(GUI);
    }
}

LevelManager_t *LevelManagerInit(GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    LevelManager_t *LevelManager;

    LevelManager = malloc(sizeof(LevelManager_t));
    
    if( !LevelManager ) {
        printf("LevelManagerInit:Failed to allocate memory for struct\n");
        return NULL;
    }
    LevelManager->SoundSystem = NULL;
    LevelManager->SoundSystem = SoundSystemInit(LevelManagerOnAudioUpdate,LevelManager);
    if( !LevelManager->SoundSystem ) {
        DPrintf("LevelManagerInit:Couldn't Initialize SoundSystem\n");
        free(LevelManager);
        return NULL;
    }
    LevelManager->RenderObjectShader = NULL;
//     if( !LevelManagerInitRenderObjectShader(LevelManager) ) {
//         DPrintf("LevelManagerInit:Couldn't load RenderObjectShader\n");
//         SoundSystemCleanUp(LevelManager->SoundSystem);
//         free(LevelManager);
//         return NULL;
//     }
    LevelManager->HasToSpawnCamera = 0;
    LevelManager->FileDialog = FileDialogRegister("Select Directory",NULL,
                                                     LevelManagerOnDirSelected,LevelManagerOnDirSelectionCancelled);
    LevelManager->BasePath = NULL;
    //No path has been provided to it yet.
    LevelManager->IsPathSet = 0;
    
    LevelManagerBasePath = ConfigGet("GameBasePath");

    if( !LevelManagerLoadFromConfig(LevelManager,GUI,VideoSystem) ) {
        LevelManagerToggleFileDialog(LevelManager,GUI,VideoSystem);
    }
    return LevelManager;
}
