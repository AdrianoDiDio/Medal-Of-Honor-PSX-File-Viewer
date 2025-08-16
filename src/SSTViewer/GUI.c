// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2025 Adriano Di Dio.
    
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
int GUIProcessEvent(GUI_t *GUI, const SDL_Event *Event)
{
    if( !GUIIsActive(GUI) ) {
        return 0;
    }
    ImGui_ImplSDL3_ProcessEvent(Event);
    return 1;
}

int GUIIsActive(const GUI_t *GUI)
{
    return GUI->NumActiveWindows > 0;
}

/*
 Utilities functions to detect GUI status.
 Push => Signal that a window is visible and update the cursor status to handle it.
 Pop => Signal that a window has been closed and if there are no windows opened then it hides the cursor since it is not needed anymore.
 */
void GUIPushWindow(GUI_t *GUI, VideoSystem_t *VideoSystem)
{
    if( !GUI->NumActiveWindows ) {
        //NOTE(Adriano):We have pushed a new window to the stack release the mouse and show
        //the cursor.
        VideoSystemGrabMouse(VideoSystem, 0);
    }
    GUI->NumActiveWindows++;
}
void GUIPopWindow(GUI_t *GUI, VideoSystem_t *VideoSystem)
{
    GUI->NumActiveWindows--;
    if( !GUI->NumActiveWindows ) {
        //NOTE(Adriano):All the windows have been closed...grab the mouse!
        VideoSystemGrabMouse(VideoSystem, 1);
    }
}
/*
 * Push or Pop a window from the stack based on the handle value.
 */
void GUIUpdateWindowStack(GUI_t *GUI,VideoSystem_t *VideoSystem,int HandleValue)
{
    if( HandleValue ) {
        GUIPushWindow(GUI,VideoSystem);
    } else {
        GUIPopWindow(GUI,VideoSystem);
    }
}
void GUIToggleDebugWindow(GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    GUI->DebugWindowHandle = !GUI->DebugWindowHandle;
    GUIUpdateWindowStack(GUI,VideoSystem,GUI->DebugWindowHandle);
}
void GUIToggleVideoSettingsWindow(GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    GUI->VideoSettingsWindowHandle = !GUI->VideoSettingsWindowHandle;
    GUIUpdateWindowStack(GUI,VideoSystem,GUI->VideoSettingsWindowHandle);

}
void GUIToggleScriptSelectWindow(GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    GUI->ScriptSelectWindowHandle = !GUI->ScriptSelectWindowHandle;
    GUIUpdateWindowStack(GUI,VideoSystem,GUI->ScriptSelectWindowHandle);
}

void GUIDrawDebugWindow(GUI_t *GUI,SSTManager_t *SSTManager, const Camera_t *Camera,VideoSystem_t *VideoSystem)
{
    ImVec2 ZeroSize;
    int LinkedVersion;
    int CompiledVersion;
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
        if( SSTManagerAreScriptsLoaded(SSTManager) ) {
            igText("Game:%s",SSTManager->EngineName);
            igText("Current Path:%s",SSTManager->BasePath);
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
                if( GUICheckBoxWithTooltip("Draw GFX Models",(bool *) &SSTDrawGFXModels->IValue,SSTDrawGFXModels->Description) ) {
                    ConfigSetNumber("SSTDrawGFXModels",SSTDrawGFXModels->IValue);
                }
            }
        }
        if( igCollapsingHeader_TreeNodeFlags("Debug Statistics",0) ) {
            igText("NumActiveWindows:%i",GUI->NumActiveWindows);
            igSeparator();
            igText("OpenGL Version: %s",glGetString(GL_VERSION));
            LinkedVersion = SDL_GetVersion();
            CompiledVersion = SDL_VERSION;
            igText("SDL Compiled Version: %u.%u.%u",SDL_VERSIONNUM_MAJOR(CompiledVersion),SDL_VERSIONNUM_MINOR(CompiledVersion),
                   SDL_VERSIONNUM_MICRO(CompiledVersion));
            igText("SDL Linked Version: %u.%u.%u",SDL_VERSIONNUM_MAJOR(LinkedVersion),SDL_VERSIONNUM_MINOR(LinkedVersion),
                   SDL_VERSIONNUM_MICRO(LinkedVersion));
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
        GUIUpdateWindowStack(GUI,VideoSystem,GUI->DebugWindowHandle);
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

void GUIDrawDebugOverlay(const ComTimeInfo_t *TimeInfo,Camera_t *Camera,SSTManager_t *SSTManager)
{
    ImGuiViewport *Viewport;
    ImVec2 WorkPosition;
    ImVec2 WorkSize;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
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

void GUIDrawScriptList(GUI_t *GUI,SSTManager_t *SSTManager,VideoSystem_t *VideoSystem)
{
    static int FailedMissionNumber = -1;
    static int FailedLevelNumber = -1;
    ImGuiTreeNodeFlags TreeNodeFlags;
    SST_t *ScriptIterator;
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
    
    if( !SSTManagerAreScriptsLoaded(SSTManager) ) {
        return;
    }
    for( ScriptIterator = SSTManager->ScriptList; ScriptIterator; ScriptIterator = ScriptIterator->Next ) {
        TreeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
        if( ScriptIterator == SSTManager->ActiveScript ) {
            TreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
        }
        if( igTreeNodeEx_Str(ScriptIterator->Name,TreeNodeFlags) ) {
            if (igIsMouseDoubleClicked_Nil(0) && igIsItemHovered(ImGuiHoveredFlags_None) ) {
                SSTManagerActivateScript(SSTManager,ScriptIterator);
            }
        }
    }
}
void GUIDrawScriptSelectWindow(GUI_t *GUI,SSTManager_t *SSTManager,VideoSystem_t *VideoSystem)
{
    if( !GUI->ScriptSelectWindowHandle ) {
        return;
    }

    if( igBegin("Script Select",&GUI->ScriptSelectWindowHandle,0) ) {
        if( !SSTManagerAreScriptsLoaded(SSTManager) ) {
            igText("No script have been loaded yet!");
        } else {
            igText(SSTManager->EngineName);
            igSeparator();
            GUIDrawScriptList(GUI,SSTManager,VideoSystem);
        }
    }
    igEnd();
    if( !GUI->ScriptSelectWindowHandle ) {
        GUIUpdateWindowStack(GUI,VideoSystem,GUI->ScriptSelectWindowHandle);
    }
}

void GUIDraw(GUI_t *GUI,SSTManager_t *SSTManager,Camera_t *Camera,VideoSystem_t *VideoSystem,ComTimeInfo_t *TimeInfo)
{
    int PreviousHandleValue;
    
    GUIBeginFrame();
    
    GUIDrawDebugOverlay(TimeInfo,Camera,SSTManager);
    
    if( !GUI->NumActiveWindows ) {
        GUIDrawHelpOverlay();
        GUIEndFrame();
        return;
    }
    
    FileDialogRenderList();
    ErrorMessageDialogDraw(GUI->ErrorMessageDialog);
    GUIDrawDebugWindow(GUI,SSTManager,Camera,VideoSystem);
    
    PreviousHandleValue = GUI->VideoSettingsWindowHandle;
    GUIDrawVideoSettingsWindow(&GUI->VideoSettingsWindowHandle,VideoSystem);
    if( GUI->VideoSettingsWindowHandle != PreviousHandleValue ) {
        GUIUpdateWindowStack(GUI,VideoSystem,GUI->VideoSettingsWindowHandle);
    }
    GUIDrawScriptSelectWindow(GUI,SSTManager,VideoSystem);
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
