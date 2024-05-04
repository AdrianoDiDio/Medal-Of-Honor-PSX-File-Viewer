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

#include "GUI.h"
#include "../Common/VRAM.h"
#include "SSTViewer.h"


const char* LevelMusicOptions[] = { 
    "Disable",
    "Music and Ambient Sounds",
    "Ambient Sounds Only" 
};

int NumLevelMusicOptions = sizeof(LevelMusicOptions) / sizeof(LevelMusicOptions[0]);

void GUIFree(GUI_t *GUI)
{
    GUIReleaseContext(GUI->DefaultContext);
    ProgressBarDestroy(GUI->ProgressBar);
    FileDialogListFree();
    ErrorMessageDialogFree(GUI->ErrorMessageDialog);
    free(GUI->ConfigFilePath);
    free(GUI);
}

/*
 * Process a new event from the SDL system only when
 * it is active.
 * Returns 0 if GUI has ignored the event 1 otherwise.
 */
int GUIProcessEvent(GUI_t *GUI,SDL_Event *Event)
{
    if( !GUIIsActive(GUI) ) {
        return 0;
    }
    ImGui_ImplSDL2_ProcessEvent(Event);
    return 1;
}

int GUIIsActive(GUI_t *GUI)
{
    return GUI->NumActiveWindows > 0;
}

/*
 Utilities functions to detect GUI status.
 Push => Signal that a window is visible and update the cursor status to handle it.
 Pop => Signal that a window has been closed and if there are no windows opened then it hides the cursor since it is not needed anymore.
 */
void GUIPushWindow(GUI_t *GUI)
{
    if( !GUI->NumActiveWindows ) {
        //NOTE(Adriano):We have pushed a new window to the stack release the mouse and show
        //the cursor.
        VideoSystemGrabMouse(0);
    }
    GUI->NumActiveWindows++;
}
void GUIPopWindow(GUI_t *GUI)
{
    GUI->NumActiveWindows--;
    if( !GUI->NumActiveWindows ) {
        //NOTE(Adriano):All the windows have been closed...grab the mouse!
        VideoSystemGrabMouse(1);
    }
}
/*
 * Push or Pop a window from the stack based on the handle value.
 */
void GUIUpdateWindowStack(GUI_t *GUI,int HandleValue)
{
    if( HandleValue ) {
        GUIPushWindow(GUI);
    } else {
        GUIPopWindow(GUI);
    }
}
void GUIToggleDebugWindow(GUI_t *GUI)
{
    GUI->DebugWindowHandle = !GUI->DebugWindowHandle;
    GUIUpdateWindowStack(GUI,GUI->DebugWindowHandle);
}
void GUIToggleVideoSettingsWindow(GUI_t *GUI)
{
    GUI->VideoSettingsWindowHandle = !GUI->VideoSettingsWindowHandle;
    GUIUpdateWindowStack(GUI,GUI->VideoSettingsWindowHandle);

}
void GUIToggleLevelSelectWindow(GUI_t *GUI)
{
    GUI->LevelSelectWindowHandle = !GUI->LevelSelectWindowHandle;
    GUIUpdateWindowStack(GUI,GUI->LevelSelectWindowHandle);
}

void GUIDrawDebugWindow(GUI_t *GUI,LevelManager_t *LevelManager,Camera_t *Camera,VideoSystem_t *VideoSystem)
{
    ImVec2 ZeroSize;
    SDL_version LinkedVersion;
    SDL_version CompiledVersion;
    int MaxLengthMinutes;
    int MaxLengthSeconds;
    int CurrentLengthMinutes;
    int CurrentLengthSeconds;
    int i;
    int IsSelected;
    
    if( !GUI->DebugWindowHandle ) {
        return;
    }
    ZeroSize.x = 0.f;
    ZeroSize.y = 0.f;
    if( igBegin("Debug Settings",&GUI->DebugWindowHandle,ImGuiWindowFlags_AlwaysAutoResize) ) {
        if( LevelManagerIsLevelLoaded(LevelManager) ) {
            igText("Game:%s",LevelManager->EngineName);
            igText("Current Path:%s",LevelManager->BasePath);
            igSeparator();
            if( igCollapsingHeader_TreeNodeFlags("Settings",0) ) {
                if( GUICheckBoxWithTooltip("Show FPS",(bool *) &GUIShowFPS->IValue,GUIShowFPS->Description) ) {
                    ConfigSetNumber("GUIShowFPS",GUIShowFPS->IValue);
                }
                if( igSliderFloat("Camera Speed",&CameraSpeed->FValue,10.f,256.f,"%.2f",0) ) {
                        ConfigSetNumber("CameraSpeed",CameraSpeed->FValue);
                }
                if( igSliderFloat("Camera Mouse Sensitivity",&CameraMouseSensitivity->FValue,1.f,20.f,"%.2f",0) ) {
                        ConfigSetNumber("CameraMouseSensitivity",CameraMouseSensitivity->FValue);
                }
            }
        }
        if( igCollapsingHeader_TreeNodeFlags("Debug Statistics",0) ) {
            igText("NumActiveWindows:%i",GUI->NumActiveWindows);
            igSeparator();
            igText("OpenGL Version: %s",glGetString(GL_VERSION));
            SDL_GetVersion(&LinkedVersion);
            SDL_VERSION(&CompiledVersion);
            igText("SDL Compiled Version: %u.%u.%u",CompiledVersion.major,CompiledVersion.minor,CompiledVersion.patch);
            igText("SDL Linked Version: %u.%u.%u",LinkedVersion.major,LinkedVersion.minor,LinkedVersion.patch);
            igSeparator();
            igText("Display Informations");
            igText("Resolution:%ix%i",VidConfigWidth->IValue,VidConfigHeight->IValue);
            igText("Refresh Rate:%i",VidConfigRefreshRate->IValue);
            igSeparator();
            igText("Camera Info");
            igText("Position:%f;%f;%f",Camera->Position[0],Camera->Position[1],Camera->Position[2]);
            igText("Rotation:%f;%f;%f",Camera->Rotation[PITCH],Camera->Rotation[YAW],Camera->Rotation[ROLL]);
        }
    }
    
    if( !GUI->DebugWindowHandle ) {
        GUIUpdateWindowStack(GUI,GUI->DebugWindowHandle);
    }
    igEnd();
}

void GUIDrawHelpOverlay()
{
    ImGuiWindowFlags WindowFlags;
    ImGuiViewport *Viewport;
    ImVec2 WorkPosition;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    
    WindowFlags = /*ImGuiWindowFlags_NoDecoration |*/ ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | 
                    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
                    ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    Viewport = igGetMainViewport();
    WorkPosition = Viewport->WorkPos;
    WindowPosition.x = (WorkPosition.x + 10.f);
    WindowPosition.y = (WorkPosition.y + 10.f);
    WindowPivot.x = 0.f;
    WindowPivot.y = 0.f;
    igSetNextWindowPos(WindowPosition, ImGuiCond_Once, WindowPivot);

    if( igBegin("Help", NULL, WindowFlags) ) {
        igText("Press WASD to move the camera around");
        igText("Press F1 to open the debug settings");
        igText("Press F2 to open the video settings");
        igText("Press F3 to open the level selection window");
        igText("Press F4 to change the game path");
        igText("Press Escape to exit the program");
    }
    igEnd();
}

void GUIDrawDebugOverlay(ComTimeInfo_t *TimeInfo,Camera_t *Camera,LevelManager_t *LevelManager)
{
    ImGuiViewport *Viewport;
    ImVec2 WorkPosition;
    ImVec2 WorkSize;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    vec3 CameraPosition;
    ImGuiWindowFlags WindowFlags;
    
    WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | 
                    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 
                    ImGuiWindowFlags_NoMove;
    Viewport = igGetMainViewport();
    WorkPosition = Viewport->WorkPos;
    WorkSize = Viewport->WorkSize;
    WindowPosition.x = (WorkPosition.x + WorkSize.x - 10.f);
    WindowPosition.y = (WorkPosition.y + 10.f);
    WindowPivot.x = 1.f;
    WindowPivot.y = 0.f;

    igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);
    if( igBegin("Debug", NULL, WindowFlags) ) {
        if( GUIShowFPS->IValue ) {
            igText(TimeInfo->FPSString);
        }
    }
    igEnd(); 
}

void GUIDrawLevelTree(GUI_t *GUI,LevelManager_t *LevelManager,VideoSystem_t *VideoSystem,const Mission_t *Missions,int NumMissions)
{
    static int FailedMissionNumber = -1;
    static int FailedLevelNumber = -1;
    ImGuiTreeNodeFlags TreeNodeFlags;
    int i;
    int j;
    int DisableNode;
    int CurrentMission;
    int CurrentLevel;
    ImVec2 ButtonSize;
    
    ButtonSize.x = 120;
    ButtonSize.y = 0;
    CurrentMission = -1;
    CurrentLevel = -1;
    
    if( LevelManagerIsLevelLoaded(LevelManager) ) {
        CurrentMission = 0;
        CurrentLevel = 0;
    }
//     for( i = 0; i < NumMissions; i++ ) {
//         TreeNodeFlags = ImGuiTreeNodeFlags_None;
//         if(  CurrentMission == Missions[i].MissionNumber ) {
//             TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
//         }
//         if( igTreeNodeEx_Str(Missions[i].MissionName,TreeNodeFlags) ) {
//             for( j = 0; j < Missions[i].NumLevels; j++ ) {
//                 DisableNode = 0;
//                 TreeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
//                 if( CurrentMission == Missions[i].MissionNumber && CurrentLevel == Missions[i].Levels[j].LevelNumber ) {
//                     TreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
//                     DisableNode = 1;
//                 }
//                 if( DisableNode ) {
//                     igBeginDisabled(1);
//                 }
//                 if( igTreeNodeEx_Str(Missions[i].Levels[j].LevelName,TreeNodeFlags) ) {
//                     if (igIsMouseDoubleClicked(0) && igIsItemHovered(ImGuiHoveredFlags_None) ) {
//                         if( LevelManagerLoadLevel(LevelManager,GUI,VideoSystem,Missions[i].MissionNumber,Missions[i].Levels[j].LevelNumber) ) {
//                             //Close it if we selected a level and it was loaded properly.
//                             GUI->LevelSelectWindowHandle = 0;
//                         } else {
//                             FailedMissionNumber = i;
//                             FailedLevelNumber = j;
//                             igOpenPopup_Str("Load Level Error",0);
//                         }
//                     }
//                 }
//                 if( DisableNode ) {
//                     igEndDisabled();
//                 }
//             }
//             GUIPrepareModalWindow();
//             if( igBeginPopupModal("Load Level Error",NULL,ImGuiWindowFlags_AlwaysAutoResize) ) {
//                 assert(FailedMissionNumber != -1 && FailedLevelNumber != -1 );
//                 igText("Failed to load level \"%s\"",Missions[FailedMissionNumber].Levels[FailedLevelNumber].LevelName);
//                 if (igButton("OK", ButtonSize) ) {
//                     igCloseCurrentPopup(); 
//                 }
//                 igEndPopup();
//             }
//             igTreePop();
//         }
//     }
}
void GUIDrawLevelSelectWindow(GUI_t *GUI,LevelManager_t *LevelManager,VideoSystem_t *VideoSystem)
{
    if( !GUI->LevelSelectWindowHandle ) {
        return;
    }

    if( igBegin("Level Select",&GUI->LevelSelectWindowHandle,0) ) {
        if( !LevelManagerIsLevelLoaded(LevelManager) ) {
            igText("Level has not been loaded yet!");
        } else {
            igText(LevelManager->EngineName);
            igSeparator();
            if( LevelManagerGetGameEngine(LevelManager) == MOH_GAME_STANDARD ) {
//                 GUIDrawLevelTree(GUI,LevelManager,VideoSystem,MOHMissionsList,NumMOHMissions);
            } else {
//                 GUIDrawLevelTree(GUI,LevelManager,VideoSystem,MOHUMissionsList,NumMOHUMissions);
            }
        }
    }
    igEnd();
    if( !GUI->LevelSelectWindowHandle ) {
        GUIUpdateWindowStack(GUI,GUI->LevelSelectWindowHandle);
    }
}

void GUIDraw(GUI_t *GUI,LevelManager_t *LevelManager,Camera_t *Camera,VideoSystem_t *VideoSystem,ComTimeInfo_t *TimeInfo)
{
    int PreviousHandleValue;
    
    GUIBeginFrame();
    
    GUIDrawDebugOverlay(TimeInfo,Camera,LevelManager);
    
    if( !GUI->NumActiveWindows ) {
        GUIDrawHelpOverlay();
        GUIEndFrame();
        return;
    }
    
    FileDialogRenderList();
    ErrorMessageDialogDraw(GUI->ErrorMessageDialog);
    GUIDrawDebugWindow(GUI,LevelManager,Camera,VideoSystem);
    
    PreviousHandleValue = GUI->VideoSettingsWindowHandle;
    GUIDrawVideoSettingsWindow(&GUI->VideoSettingsWindowHandle,VideoSystem);
    if( GUI->VideoSettingsWindowHandle != PreviousHandleValue ) {
        GUIUpdateWindowStack(GUI,GUI->VideoSettingsWindowHandle);
    }
    GUIDrawLevelSelectWindow(GUI,LevelManager,VideoSystem);
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
    GUI->ErrorMessageDialog = ErrorMessageDialogInit();
    if( !GUI->ErrorMessageDialog ) {
        DPrintf("GUIInit:Failed to initialize error message dialog\n");
        free(GUI);
        return NULL;
    }
    
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
    GUI->NumActiveWindows = 0;

    return GUI;
}
