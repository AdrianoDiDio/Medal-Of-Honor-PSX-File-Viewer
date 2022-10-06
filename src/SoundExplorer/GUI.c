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

#include "GUI.h"
#include "../Common/VRAM.h"
#include "SoundExplorer.h"

void GUIFree(GUI_t *GUI)
{
    GUIReleaseContext(GUI->DefaultContext);
    ProgressBarDestroy(GUI->ProgressBar);
    FileDialogListFree();
    
    if( GUI->ErrorMessage ) {
        free(GUI->ErrorMessage);
    }
    free(GUI->ConfigFilePath);
    free(GUI);
}

void GUIProcessEvent(GUI_t *GUI,SDL_Event *Event)
{
    ImGui_ImplSDL2_ProcessEvent(Event);
}
void GUIDrawErrorMessage(GUI_t *GUI)
{
    ImVec2 ButtonSize;
    if( !GUI->ErrorMessage ) {
        return;
    }
    if( !GUI->ErrorDialogHandle ) {
        igOpenPopup_Str("Error",0);
        GUI->ErrorDialogHandle = 1;
    }
    ButtonSize.x = 120;
    ButtonSize.y = 0;
    GUIPrepareModalWindow();
    if( igBeginPopupModal("Error",NULL,ImGuiWindowFlags_AlwaysAutoResize) ) {
        igText(GUI->ErrorMessage);
        if (igButton("OK", ButtonSize) ) {
            igCloseCurrentPopup();
            GUI->ErrorDialogHandle = 0;
            free(GUI->ErrorMessage);
            GUI->ErrorMessage = NULL;
        }
        igEndPopup();
    }
}
void GUIDrawMenuBar(Application_t *Application)
{
    if( !igBeginMainMenuBar() ) {
        return;
    }
    if (igBeginMenu("File",true)) {
        if( igMenuItem_Bool("Open",NULL,false,true) ) {
            SoundManagerOpenFileDialog(Application->SoundManager,Application->GUI,Application->Engine->VideoSystem);
        }
        if( igMenuItem_Bool("Exit",NULL,false,true) ) {
            Quit(Application);
        }
        igEndMenu();
    }
    if (igBeginMenu("Settings",true)) {
        if( igMenuItem_Bool("Video",NULL,Application->GUI->VideoSettingsWindowHandle,true) ) {
            Application->GUI->VideoSettingsWindowHandle = !Application->GUI->VideoSettingsWindowHandle;
        }
        igEndMenu();
    }
    igEndMainMenuBar();
}

void GUIDrawMainWindow(GUI_t *GUI,VideoSystem_t *VideoSystem,SoundManager_t *SoundManager)
{
    ImGuiViewport *Viewport;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    ImVec2 MaxSize;
    ImVec2 ZeroSize;
    ImVec2 TablePadding;
    ImGuiTreeNodeFlags TreeNodeFlags;
    ImGuiTableFlags TableFlags;
    int MaxLengthMinutes;
    int MaxLengthSeconds;
    int CurrentLengthMinutes;
    int CurrentLengthSeconds;
    VBMusic_t *CurrentSound;
    VBMusic_t *VBMusicIterator;

    Viewport = igGetMainViewport();
    WindowPosition.x = Viewport->WorkPos.x;
    WindowPosition.y = Viewport->WorkPos.y;
    WindowPivot.x = 0.f;
    WindowPivot.y = 0.f;
    MaxSize.x = Viewport->WorkSize.x;
    MaxSize.y = Viewport->WorkSize.y;
    ZeroSize.x = 0.f;
    ZeroSize.y = 0.f;
    
    igSetNextWindowSize(MaxSize,0);
    igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);

    if( !igBegin("Main Window", NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | 
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus
    ) ) {
        return;
    }
    
    TableFlags = ImGuiTableFlags_BordersInnerV;
    TablePadding.x = 10.f;
    TablePadding.y = 0.f;
    igPushStyleVar_Vec2(ImGuiStyleVar_CellPadding, TablePadding);

    if( igBeginTable("##WindowLayout",2,TableFlags,ZeroSize,0) ) {
        igTableNextRow(0,MaxSize.y - 10.f);

        igTableSetColumnIndex(0);
        igBeginChild_Str("Settings",ZeroSize,false,0);
        GUIDrawTitleBar("Info",igGetColumnWidth(0));
        igCheckbox("Loop Sound",&SoundManager->Loop);
        igSameLine(0,-1);
        if( SoundManager->SoundList ) {
            if( igButton("Export All Sounds to WAV",ZeroSize) ) {
                SoundManagerExportAll(SoundManager,GUI,VideoSystem);
            }
        }
        TreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        if( igCollapsingHeader_TreeNodeFlags("Current Sound Info",TreeNodeFlags) ) {
            if( SoundManager->SelectedSound ) {
                CurrentSound = SoundManager->SelectedSound;
                igText("Name:%s",CurrentSound->Name);
                igTextWrapped("Sampling Frequency:");
                igTextWrapped(
                        "Sound effects and music are usually encoded with a frequency of 22050 Hz "
                        "while voice effects are sampled using a frequency of 11025 Hz.\n"
                        "When loading a TAF file only sounds contained inside the third and fourth VAB are sampled using a frequency "
                        "of 11025 since they mostly contains only voices.\n"
                        "Before exporting make sure to set the right frequency otherwise the sound will not play correctly.");
                if( igRadioButton_Bool("11025",CurrentSound->Frequency == 11025.f) ) {
                    ProgressBarBegin(GUI->ProgressBar,"Resampling");
                    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,20.f,"Downsampling");
                    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,50.f,"Downsampling");
                    SoundSystemResampleMusic(CurrentSound,11025.f);
                    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,100.f,"Done");
                    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
                }
                igSameLine(0,-1);
                if( igRadioButton_Bool("22050",CurrentSound->Frequency == 22050.f) ) {
                    ProgressBarBegin(GUI->ProgressBar,"Resampling");
                    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,20.f,"Upsampling");
                    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,50.f,"Upsampling");
                    SoundSystemResampleMusic(CurrentSound,22050.f);
                    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,100.f,"Done");
                    ProgressBarEnd(GUI->ProgressBar,VideoSystem);
                }
                igSeparator();
                SoundSystemGetSoundDuration(CurrentSound,&MaxLengthMinutes,&MaxLengthSeconds);
                SoundSystemGetCurrentSoundTime(CurrentSound,&CurrentLengthMinutes,&CurrentLengthSeconds);
                igText("Length:");
                igText("%02i:%02i/%02i:%02i",CurrentLengthMinutes,CurrentLengthSeconds,MaxLengthMinutes,MaxLengthSeconds);

                if( SoundSystemIsPaused(SoundManager->SoundSystem) ) {
                    if( igButton("Play",ZeroSize) ) {
                        SoundSystemPlay(SoundManager->SoundSystem);
                    }
                } else {
                    if( igButton("Pause",ZeroSize) ) {
                        SoundSystemPause(SoundManager->SoundSystem);
                    }
                }
      
                igSameLine(0,-1);
                if( igButton("Stop",ZeroSize) ) {
                    if( !SoundSystemIsPaused(SoundManager->SoundSystem) ) {
                        SoundSystemPause(SoundManager->SoundSystem);
                    }
                    SoundManager->SelectedSound->DataPointer = 0;
                }
                if( igSliderInt("Sound Volume",&SoundVolume->IValue,0,128,"%i",0) ) {
                    ConfigSetNumber("SoundVolume",SoundVolume->IValue);
                }
                if( igButton("Export to WAV",ZeroSize) ) {
                    SoundManagerExport(SoundManager,CurrentSound,CurrentSound,GUI,VideoSystem);
                }
            } else {
                    igText("Please select a sound in order to see the details.");
            }
        }
        igEnd();
        igTableSetColumnIndex(1);
        igBeginChild_Str("SoundList",ZeroSize,false,0);
        TreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;
        GUIDrawTitleBar("Sound List",igGetColumnWidth(1));
        for(VBMusicIterator = SoundManager->SoundList; VBMusicIterator; VBMusicIterator = VBMusicIterator->Next) {
            TreeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
            if( VBMusicIterator == SoundManager->SelectedSound ) {
                TreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
            }
            if( igTreeNodeEx_Str(VBMusicIterator->Name,TreeNodeFlags) ) {
                if (igIsMouseDoubleClicked(0) && igIsItemHovered(ImGuiHoveredFlags_None) ) {
                    SoundManager->SelectedSound = VBMusicIterator;
                    SoundSystemPlay(SoundManager->SoundSystem);
                }
            }
        }
        igEnd();
        igEndTable();
    }
    igPopStyleVar(1);
    igEnd();
}

void GUIDraw(Application_t *Application)
{
    
    GUIBeginFrame();
    GUIDrawMenuBar(Application);
    GUIDrawMainWindow(Application->GUI,Application->Engine->VideoSystem,Application->SoundManager);
    GUIDrawVideoSettingsWindow(&Application->GUI->VideoSettingsWindowHandle,Application->Engine->VideoSystem);
    FileDialogRenderList();
//     igShowDemoWindow(NULL);
    GUIEndFrame();
}

GUI_t *GUIInit(VideoSystem_t *VideoSystem)
{
    GUI_t *GUI;
    char *ConfigPath;

    GUI = malloc(sizeof(GUI_t));
    
    if( !GUI ) {
        DPrintf("GUIInit:Failed to allocate memory for struct\n");
        return NULL;
    }
    
    memset(GUI,0,sizeof(GUI_t));
    GUI->ErrorMessage = NULL;
    GUI->ErrorDialogHandle = 0;
    
    ConfigPath = AppGetConfigPath();
    asprintf(&GUI->ConfigFilePath,"%simgui.ini",ConfigPath);
    free(ConfigPath);

    GUILoadCommonSettings();
    
    GUI->DefaultContext = igCreateContext(NULL);
    GUI->ProgressBar = ProgressBarInitialize(VideoSystem);
    
    if( !GUI->ProgressBar ) {
        DPrintf("GUIInit:Failed to initialize ProgressBar\n");
        free(GUI);
        return NULL;
    }
    GUIContextInit(GUI->DefaultContext,VideoSystem,GUI->ConfigFilePath);

    return GUI;
} 
