// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2026 Adriano Di Dio.
    
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

int SSTManagerAreScriptsLoaded(const SSTManager_t *SSTManager)
{
    if( !SSTManager ) {
        DPrintf("SSTManagerIsLevelLoaded:Called without a valid SSTManager\n");
        return 0;
    }
    return SSTManager->IsPathSet && SSTManager->ScriptList;
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

void SSTManagerCloseDialog(GUI_t *GUI,VideoSystem_t *VideoSystem,FileDialog_t *FileDialog)
{
    SSTManagerFreeDialogData(FileDialog);
    FileDialogClose(FileDialog);
    GUIPopWindow(GUI,VideoSystem);
}
void SSTManagerFreeScriptList(SST_t *ScriptList)
{
    SST_t *Temp;
    if( !ScriptList ) {
        return;
    }
    while( ScriptList ) {
        Temp = ScriptList;
        ScriptList = ScriptList->Next;
        SSTFree(Temp);
    }

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
    SSTManagerFreeScriptList(SSTManager->ScriptList);
    free(SSTManager);
}

void SSTManagerOnAudioUpdate(void *UserData,SDL_AudioStream *Stream,int AdditionalAmount, int TotalAmount)
{
    SSTManager_t *SSTManager;
    VBMusic_t *CurrentMusic;
    VBMusic_t **CurrentMusicAddress;
    int ChunkLength;
    float NormalizedVolume;
    Byte *Data;

    
    if( AdditionalAmount <= 0 ) {
        return;
    }
    
//     SSTManager = (SSTManager_t *) UserData;
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
//     ChunkLength = (CurrentMusic->Size - CurrentMusic->DataPointer);
//     if( ChunkLength > AdditionalAmount ) {
//         ChunkLength = AdditionalAmount;
//     }
//     if( SoundVolume->IValue < 0 || SoundVolume->IValue > 128 ) {
//         ConfigSetNumber("SoundVolume",128);
//     }
//     NormalizedVolume = SoundVolume->IValue / 128.f;
//     Data = SDL_calloc(1, AdditionalAmount);
// //     SDL_MixAudio(Data, &Data, SDL_AUDIO_F32LE,ChunkLength, NormalizedVolume);
//     SDL_PutAudioStreamData(Stream, Data, AdditionalAmount);
//     CurrentMusic->DataPointer += ChunkLength;
//     SDL_free(Data);
}

void SSTManagerDrawString(const SSTManager_t *SSTManager,const char *String,float x,float y,Color4f_t Color)
{
    if( !SSTManager ) {
        DPrintf("SSTManagerDrawString:Called without a valid sst manager.\n");
        return;
    }
    if( !SSTManagerAreScriptsLoaded(SSTManager) ) {
        DPrintf("SSTManagerDrawString:Called without a valid script\n");
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
        SSTManagerCloseDialog(SSTManagerDialogData->GUI,SSTManagerDialogData->VideoSystem,FileDialog);
    }
}
void SSTManagerOnDirSelectionCancelled(FileDialog_t *FileDialog)
{
    SSTManagerDialogData_t *SSTManagerDialogData;
    SSTManagerDialogData = (SSTManagerDialogData_t *) FileDialogGetUserData(FileDialog);
    if( SSTManagerDialogData->SSTManager->IsPathSet ) {
        SSTManagerCloseDialog(SSTManagerDialogData->GUI,SSTManagerDialogData->VideoSystem,FileDialog);
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

    if( !SSTManager->ActiveScript ) {
        return;
    }

    glm_ortho(0,(float) VidConfigWidth->IValue, (float) VidConfigHeight->IValue,0,-(1<<16),1<<16,ProjectionMatrix);
    SSTRender(SSTManager->ActiveScript,ProjectionMatrix);
}

void SSTManagerActivateScript(SSTManager_t *SSTManager,SST_t *Script)
{
    DPrintf("SSTManagerActivateScript:Activating script %s\n",Script->Name);
    if( SSTManager->ActiveScript ) {
        SSTUnload(SSTManager->ActiveScript);
    }
    SSTGenerateVAOs(Script);
    SSTManager->ActiveScript = Script;
}
int SSTManagerInitWithPath(SSTManager_t *SSTManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *Path)
{
    char *Buffer;
    char *RSCBuffer;
    RSC_t *RSC;
    RSCEntry_t *EntryList;
    char *ScriptName;
    SST_t *Script;
    int  NumScripts;
    int BasePercentage;
    int ScriptIncrement;
    int i;
    
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
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,10,"Loading Global1 RSC file");
    //First step, load the global and global2 files
    asprintf(&RSCBuffer,"%s%cDATA%cGLOBAL.RSC",Path,PATH_SEPARATOR,PATH_SEPARATOR);
    SSTManager->GlobalRSCList = RSCLoad(RSCBuffer);
    
    if( !SSTManager->GlobalRSCList ) {
        DPrintf("Failed to load global rsc file\n");
        goto Failure;
    }
    free(RSCBuffer);
    asprintf(&RSCBuffer,"%s%cDATA%cGLOBAL2.RSC",Path,PATH_SEPARATOR,PATH_SEPARATOR);
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,20,"Loading Global2.rsc");
    RSC = RSCLoad(RSCBuffer);
    
    if( !RSC ) {
        DPrintf("Failed to load global2 rsc file\n");
        goto Failure;
    }
    //NOTE(Adriano):MOH Underground has an empty global2 file
    DPrintf("SSTManagerInitWithPath:Global2 has %i files\n",RSC->Header.NumEntry);
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,30,"Detecting Game Engine.");
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

    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,40,"Retrieving script list");
    EntryList = RSCGetDirectoryEntries(SSTManager->GlobalRSCList,"global\\script\\",&NumScripts);
    
    if( !EntryList || !NumScripts ) {
        DPrintf("SSTManagerInitWithPath:No scripts found in global RSC file\n");
        goto Failure;
    }
    
    SSTManagerFreeScriptList(SSTManager->ScriptList);
    SSTManager->ActiveScript = NULL;
    SSTManager->ScriptList = NULL;
    
    DPrintf("SSTManagerInitWithPath:Found %i scripts\n",NumScripts);
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,50,"Loading all scripts");
    BasePercentage = 50;
    ScriptIncrement = (100 - BasePercentage) / NumScripts;
    for( i = 0; i < NumScripts; i++ ) {
        ScriptName = RSCGetBaseName(EntryList[i].Name);
        asprintf(&Buffer,"Loading Script %s...",ScriptName);
        ProgressBarIncrement(GUI->ProgressBar,VideoSystem,BasePercentage,Buffer);
        DPrintf("SSTManagerInitWithPath: Loading script %s\n",ScriptName);
        Script = SSTLoad(EntryList[i].Data,ScriptName,Path,SSTManager->GlobalRSCList,SSTManager->GameEngine);
        if( !Script ) {
            DPrintf("SSTManagerInitWithPath: Failed to load script %s\n",ScriptName);
        } else {
            if( !strcmp(Script->Name,"mtitle1.sst") ) {
                SSTManagerActivateScript(SSTManager,Script);
            }
            Script->Next = SSTManager->ScriptList;
            SSTManager->ScriptList = Script;
        }
        BasePercentage += ScriptIncrement;
        free(ScriptName);
        free(Buffer);
    }
    free(EntryList);
    
    if( SSTManager->BasePath ) {
        free(SSTManager->BasePath);
    }
    
    SSTManager->IsPathSet = 1;
    SSTManager->BasePath = StringCopy(Path);
    free(RSCBuffer);
    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
    return 1;
Failure:
    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
    free(RSCBuffer);
    return 0;
}

void SSTManagerToggleFileDialog(SSTManager_t *SSTManager,GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    SSTManagerDialogData_t *DialogData;
    
    if( FileDialogIsOpen(SSTManager->FileDialog) ) {
        if( SSTManager->IsPathSet ) {
            SSTManagerCloseDialog(GUI,VideoSystem,SSTManager->FileDialog);
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
        GUIPushWindow(GUI,VideoSystem);
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
    
    SSTLoadDefaultSettings();

    SSTManager->SoundSystem = NULL;
    SSTManager->SoundSystem = SoundSystemInit(SSTManagerOnAudioUpdate,SSTManager);
    if( !SSTManager->SoundSystem ) {
        DPrintf("SSTManagerInit:Couldn't Initialize SoundSystem\n");
        free(SSTManager);
        return NULL;
    }
    SSTManager->RenderObjectShader = NULL;
    SSTManager->ScriptList = NULL;
    SSTManager->FileDialog = FileDialogRegister("Select Directory",NULL,
                                                     SSTManagerOnDirSelected,SSTManagerOnDirSelectionCancelled);
    SSTManager->GlobalRSCList = NULL;
    SSTManager->BasePath = NULL;
    SSTManager->ActiveScript = NULL;
    //No path has been provided to it yet.
    SSTManager->IsPathSet = 0;
    
    SSTManagerBasePath = ConfigGet("GameBasePath");

    if( !SSTManagerLoadFromConfig(SSTManager,GUI,VideoSystem) ) {
        SSTManagerToggleFileDialog(SSTManager,GUI,VideoSystem);
    }
    return SSTManager;
}
