// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
    MOHLevelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MOHLevelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MOHLevelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#include "GUI.h"
#include "VRAM.h"
#include "TSP.h"
#include "MOHLevelViewer.h"

Config_t *GUIFont;
Config_t *GUIFontSize;
Config_t *GUIShowFPS;

const char* LevelMusicOptions[] = { 
    "Disable",
    "Music and Ambient Sounds",
    "Ambient Sounds Only" 
};

int NumLevelMusicOptions = sizeof(LevelMusicOptions) / sizeof(LevelMusicOptions[0]);

typedef struct VSyncSettings_s {
    char *DisplayValue;
    int Value;
} VSyncSettings_t;

const VSyncSettings_t VSyncOptions[] = { 
    
    {
        "Disable",
        0
    },
    {
        "Standard",
        1
    },
    {
        "Adaptive",
        -1
    }
};

int NumVSyncOptions = sizeof(VSyncOptions) / sizeof(VSyncOptions[0]);

void GUIReleaseContext(ImGuiContext *Context)
{    
    igSetCurrentContext(Context);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    igDestroyContext(Context);
}
void GUIFreeDialogList(GUI_t *GUI)
{
    GUIFileDialog_t *Temp;
    
    while( GUI->FileDialogList ) {
        free(GUI->FileDialogList->WindowTitle);
        free(GUI->FileDialogList->Key);
        if( GUI->FileDialogList->Filters ) {
            free(GUI->FileDialogList->Filters);
        }
        IGFD_Destroy(GUI->FileDialogList->Window);
        Temp = GUI->FileDialogList;
        GUI->FileDialogList = GUI->FileDialogList->Next;
        free(Temp);
    }
}
void GUIFree(GUI_t *GUI)
{
    GUIReleaseContext(GUI->DefaultContext);
    GUIReleaseContext(GUI->ProgressBar->Context);
    GUIFreeDialogList(GUI);
    
    if( GUI->ProgressBar->DialogTitle ) {
        free(GUI->ProgressBar->DialogTitle);
    }
    if( GUI->ErrorMessage ) {
        free(GUI->ErrorMessage);
    }
    free(GUI->ConfigFilePath);
    free(GUI->ProgressBar);
    free(GUI);
}

void GUIPrepareModalWindow()
{
    ImGuiIO *IO;
    ImVec2 Pivot; 
    ImVec2 ModalPosition;
    
    IO = igGetIO();
    Pivot.x = 0.5f;
    Pivot.y = 0.5f;
    ModalPosition.x = IO->DisplaySize.x * 0.5;
    ModalPosition.y = IO->DisplaySize.y * 0.5;
    igSetNextWindowPos(ModalPosition, ImGuiCond_Always, Pivot);
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
void GUIToggleHandle(GUI_t *GUI,int HandleValue)
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
    GUIToggleHandle(GUI,GUI->DebugWindowHandle);
}
void GUIToggleVideoSettingsWindow(GUI_t *GUI)
{
    GUI->VideoSettingsWindowHandle = !GUI->VideoSettingsWindowHandle;
    GUIToggleHandle(GUI,GUI->VideoSettingsWindowHandle);

}
void GUIToggleLevelSelectWindow(GUI_t *GUI)
{
    GUI->LevelSelectWindowHandle = !GUI->LevelSelectWindowHandle;
    GUIToggleHandle(GUI,GUI->LevelSelectWindowHandle);
}
void GUIBeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();   
}

void GUIEndFrame()
{
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}
bool GUICheckBoxWithTooltip(char *Label,bool *Value,char *DescriptionFormat,...)
{
    va_list Arguments;
    int IsChecked;
    IsChecked = igCheckbox(Label,Value);
    if( DescriptionFormat != NULL && igIsItemHovered(ImGuiHoveredFlags_None) ) {
        igBeginTooltip();
        igPushTextWrapPos(igGetFontSize() * 40.0f);
        va_start(Arguments, DescriptionFormat);
        igTextV(DescriptionFormat,Arguments);
        va_end(Arguments);
        igPopTextWrapPos();
        igEndTooltip();
    }
    return IsChecked;
}
void GUIDrawDebugWindow(GUI_t *GUI,LevelManager_t *LevelManager,Camera_t *Camera,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem)
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
            if( igCollapsingHeader_TreeNodeFlags("Music Info",0) ) {
                if( igBeginCombo("Music Options",LevelMusicOptions[LevelEnableMusicTrack->IValue],0) ) {
                    for (i = 0; i < NumLevelMusicOptions; i++) {
                        IsSelected = (LevelEnableMusicTrack->IValue == i);
                        if (igSelectable_Bool(LevelMusicOptions[i], IsSelected,0,ZeroSize)) {
                            if( LevelEnableMusicTrack->IValue != i ) {
                                LevelManagerUpdateSoundSettings(LevelManager,SoundSystem,i);
                            }
                        }
                        if (IsSelected) {
                            igSetItemDefaultFocus();
                        }
                    }
                    igEndCombo();
                }
                if( !SoundSystem->MusicList ) {
                    igText("This Level Doesn't contain any music files.");
                } else {
                    if( LevelEnableMusicTrack->IValue ) {
                        SoundSystemGetSoundDuration(SoundSystem,&MaxLengthMinutes,&MaxLengthSeconds);
                        SoundSystemGetCurrentSoundTime(SoundSystem,&CurrentLengthMinutes,&CurrentLengthSeconds);
                        igText("Music Track Info:");
                        igText("Name:%s",SoundSystem->CurrentMusic->Name);
                        igText("%02i:%02i/%02i:%02i",CurrentLengthMinutes,CurrentLengthSeconds,MaxLengthMinutes,MaxLengthSeconds);
                        
                        if( igSliderInt("Sound Volume",&SoundVolume->IValue,0,128,"%i",0) ) {
                            ConfigSetNumber("SoundVolume",SoundVolume->IValue);
                        }
                    } else {
                        igText("Music is not enabled..enable it first in order to show the statistics.");
                    }
                }
            }
            if( igCollapsingHeader_TreeNodeFlags("Level Settings",0) ) {
                if( GUICheckBoxWithTooltip("WireFrame Mode",(bool *) &LevelEnableWireFrameMode->IValue,LevelEnableWireFrameMode->Description) ) {
                    ConfigSetNumber("LevelEnableWireFrameMode",LevelEnableWireFrameMode->IValue);
                }
                if( GUICheckBoxWithTooltip("Draw Level",(bool *) &LevelDrawSurfaces->IValue,LevelDrawSurfaces->Description) ) {
                    ConfigSetNumber("LevelDrawSurfaces",LevelDrawSurfaces->IValue);
                }
                if( GUICheckBoxWithTooltip("Draw Collision Data",(bool *) &LevelDrawCollisionData->IValue,LevelDrawCollisionData->Description) ) {
                    ConfigSetNumber("LevelDrawCollisionData",LevelDrawCollisionData->IValue);
                }
                if( GUICheckBoxWithTooltip("Draw BSP Tree",(bool *) &LevelDrawBSPTree->IValue,LevelDrawBSPTree->Description) ) {
                    ConfigSetNumber("LevelDrawBSPTree",LevelDrawBSPTree->IValue);
                }
                if( GUICheckBoxWithTooltip("Draw BSD nodes as Points",(bool *) &LevelDrawBSDNodesAsPoints->IValue,
                    LevelDrawBSDNodesAsPoints->Description) ) {
                    ConfigSetNumber("LevelDrawBSDNodesAsPoints",LevelDrawBSDNodesAsPoints->IValue);
                }
                if( GUICheckBoxWithTooltip("Draw BSD nodes collision volumes",(bool *) &LevelDrawBSDNodesCollisionVolumes->IValue,
                    LevelDrawBSDNodesCollisionVolumes->Description ) ) {
                        ConfigSetNumber("LevelDrawBSDNodesCollisionVolumes",LevelDrawBSDNodesCollisionVolumes->IValue);
                }
                if( GUICheckBoxWithTooltip("Draw BSD RenderObjects as Points",(bool *) &LevelDrawBSDRenderObjectsAsPoints->IValue,
                                        LevelDrawBSDRenderObjectsAsPoints->Description) ) {
                    ConfigSetNumber("LevelDrawBSDRenderObjectsAsPoints",LevelDrawBSDRenderObjectsAsPoints->IValue);
                }
                if( GUICheckBoxWithTooltip("Draw BSD RenderObjects",(bool *) &LevelDrawBSDRenderObjects->IValue,
                    LevelDrawBSDRenderObjects->Description) ) {
                    ConfigSetNumber("LevelDrawBSDRenderObjects",LevelDrawBSDRenderObjects->IValue);
                }
                if( GUICheckBoxWithTooltip("Enable BSD RenderObjects Showcase Rendering",(bool *) &LevelDrawBSDShowcase->IValue,
                                        LevelDrawBSDShowcase->Description) ) {
                    ConfigSetNumber("LevelDrawBSDShowcase",LevelDrawBSDShowcase->IValue);
                }
                if( GUICheckBoxWithTooltip("Frustum Culling",(bool *) &LevelEnableFrustumCulling->IValue,
                    LevelEnableFrustumCulling->Description ) ) {
                    ConfigSetNumber("LevelEnableFrustumCulling",LevelEnableFrustumCulling->IValue);
                }
                if( GUICheckBoxWithTooltip("Ambient Light",(bool *) &LevelEnableAmbientLight->IValue,LevelEnableAmbientLight->Description) ) {
                    ConfigSetNumber("LevelEnableAmbientLight",LevelEnableAmbientLight->IValue);
                }
                if( GUICheckBoxWithTooltip("Semi-Transparency",(bool *) &LevelEnableSemiTransparency->IValue,
                    LevelEnableSemiTransparency->Description) ) {
                    ConfigSetNumber("LevelEnableSemiTransparency",LevelEnableSemiTransparency->IValue);
                }
                if ( GUICheckBoxWithTooltip("Animated Lights",(bool *) &LevelEnableAnimatedLights->IValue,
                    LevelEnableAnimatedLights->Description) ) {
                    if( !LevelEnableAnimatedLights->IValue ) {
                        TSPUpdateAnimatedFaces(LevelManager->CurrentLevel->TSPList,LevelManager->CurrentLevel->BSD,Camera,1);
                    }
                    ConfigSetNumber("LevelEnableAnimatedLights",LevelEnableAnimatedLights->IValue);
                }
                if ( GUICheckBoxWithTooltip("Animated Surfaces",(bool *) &LevelEnableAnimatedSurfaces->IValue,
                    LevelEnableAnimatedSurfaces->Description) ) {
                    ConfigSetNumber("LevelEnableAnimatedSurfaces",LevelEnableAnimatedSurfaces->IValue);
                }
            }
            if( igCollapsingHeader_TreeNodeFlags("Exporter",0) ) {
                igText("Export the current level and objects");
                ZeroSize.x = 0.f;
                ZeroSize.y = 0.f;
                if( igButton("Export to OBJ",ZeroSize) ) {
                    LevelManagerExport(LevelManager,GUI,VideoSystem,SoundSystem,LEVEL_MANAGER_EXPORT_FORMAT_OBJ);
                }
                igSameLine(0.f,10.f);
                if( igButton("Export to Ply",ZeroSize) ) {
                    LevelManagerExport(LevelManager,GUI,VideoSystem,SoundSystem,LEVEL_MANAGER_EXPORT_FORMAT_PLY);
                }
                igSeparator();
                igText("Export current music and ambient sounds");
                if( igButton("Export to WAV",ZeroSize) ) {
                    LevelManagerExport(LevelManager,GUI,VideoSystem,SoundSystem,LEVEL_MANAGER_EXPORT_FORMAT_WAV);
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
        GUIToggleHandle(GUI,GUI->DebugWindowHandle);
    }
    igEnd();
}

void GUIDrawHelpOverlay()
{
    int WindowFlags;
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

void GUIDrawDebugOverlay(ComTimeInfo_t *TimeInfo)
{
    ImGuiViewport *Viewport;
    ImVec2 WorkPosition;
    ImVec2 WorkSize;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    int WindowFlags;
    
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

    
    if( GUIShowFPS->IValue ) {
        igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);
        if( igBegin("FPS", NULL, WindowFlags) ) {
            igText(TimeInfo->FPSString);
        }
        igEnd(); 
    }
}

void GUISetProgressBarDialogTitle(GUI_t *GUI,const char *Title)
{
    if( GUI->ProgressBar->DialogTitle ) {
        free(GUI->ProgressBar->DialogTitle);
    }
    GUI->ProgressBar->DialogTitle = (Title != NULL) ? StringCopy(Title) : "Loading...";
    //NOTE(Adriano):Forces a refresh...since changing the title disrupts the rendering process.
    GUI->ProgressBar->IsOpen = 0;
}

void GUIProgressBarBegin(GUI_t *GUI,const char *Title)
{
    igSetCurrentContext(GUI->ProgressBar->Context);
    GUI->ProgressBar->IsOpen = 0;
    GUI->ProgressBar->CurrentPercentage = 0.f;
    GUISetProgressBarDialogTitle(GUI,Title);
}
void GUIProgressBarReset(GUI_t *GUI)
{
    if(!GUI) {
        return;
    }
    GUI->ProgressBar->CurrentPercentage = 0;
}
/*
    This function can be seen as a complete rendering loop.
    Each time we increment the progress bar, we check for any pending event that the GUI
    can handle and then we clear the display, show the current progress and swap buffers.
 */
void GUIProgressBarIncrement(GUI_t *GUI,VideoSystem_t *VideoSystem,float Increment,const char *Message)
{
    SDL_Event Event;
    ImGuiViewport *Viewport;
    ImVec2 ScreenCenter;
    ImVec2 Pivot;
    ImVec2 Size;
    
    if( !GUI ) {
        return;
    }
    
    //NOTE(Adriano):Process any window event that could be generated during load.
    while( SDL_PollEvent(&Event) ) {
        ImGui_ImplSDL2_ProcessEvent(&Event);
    }
    //NOTE(Adriano):Since we are checking for events these function have now an updated view of the current window size.
    Viewport = igGetMainViewport();
    
    ImGuiViewport_GetCenter(&ScreenCenter,Viewport);

    Pivot.x = 0.5f;
    Pivot.y = 0.5f;

    glClear(GL_COLOR_BUFFER_BIT );
    

    GUIBeginFrame();
    
    if( !GUI->ProgressBar->IsOpen ) {
        igOpenPopup_Str(GUI->ProgressBar->DialogTitle,0);
        GUI->ProgressBar->IsOpen = 1;
    }
    
    Size.x = 0.f;
    Size.y = 0.f;
    igSetNextWindowPos(ScreenCenter, ImGuiCond_Always, Pivot);
    GUI->ProgressBar->CurrentPercentage += Increment;
    if (igBeginPopupModal(GUI->ProgressBar->DialogTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        igProgressBar((GUI->ProgressBar->CurrentPercentage / 100.f),Size,Message);
        igEnd();
    }
    GUIEndFrame();
    VideoSystemSwapBuffers(VideoSystem);
}

void GUIProgressBarEnd(GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    int Width;
    int Height;
    
    igSetCurrentContext(GUI->DefaultContext);
    GUI->ProgressBar->IsOpen = 0;
    GUI->ProgressBar->CurrentPercentage = 0.f;
    VideoSystemGetCurrentWindowSize(VideoSystem,&Width,&Height);
    if( Width != VidConfigWidth->IValue ) {
        ConfigSetNumber("VideoWidth",Width);
    }
    if( Height != VidConfigHeight->IValue ) {
        ConfigSetNumber("VideoHeight",Height);
    }
}
int GUIGetVSyncOptionValue()
{
    int i;
    for (i = 0; i < NumVSyncOptions; i++) {
        if( VSyncOptions[i].Value == VidConfigVSync->IValue ) {
            return i;
        }
    }
    return 0;
}
void GUIDrawVideoSettingsWindow(GUI_t *GUI,VideoSystem_t *VideoSystem)
{
    int OldValue;
    int IsSelected;
    int i;
    int CurrentVSyncOption;

    if( !GUI->VideoSettingsWindowHandle ) {
        return;
    }
    ImVec2 ZeroSize;
    ZeroSize.x = ZeroSize.y = 0.f;
    int PreviewIndex = VideoSystem->CurrentVideoMode != -1 ? VideoSystem->CurrentVideoMode : 0;
    if( igBegin("Video Settings",&GUI->VideoSettingsWindowHandle,ImGuiWindowFlags_AlwaysAutoResize) ) {
        CurrentVSyncOption = GUIGetVSyncOptionValue();
        if( igBeginCombo("VSync Options",VSyncOptions[CurrentVSyncOption].DisplayValue,0) ) {
            for (i = 0; i < NumVSyncOptions; i++) {
                IsSelected = (CurrentVSyncOption == i);
                if (igSelectable_Bool(VSyncOptions[i].DisplayValue, IsSelected,0,ZeroSize)) {
                    if( CurrentVSyncOption != i ) {
                        OldValue = VidConfigVSync->IValue;
                        if( VideoSystemSetSwapInterval(VSyncOptions[i].Value) < 0 ) {
                            VideoSystemSetSwapInterval(OldValue);
                        }
                    }
                }
          
            }
            igEndCombo();
        }
        igSeparator();
        //NOTE(Adriano):Only in Fullscreen mode we can select the video mode we want.
        if( VidConfigFullScreen->IValue ) {
            igText("Video Mode");
            if( igBeginCombo("##Resolution", VideoSystem->VideoModeList[PreviewIndex].Description, 0) ) {
                for( i = 0; i < VideoSystem->NumVideoModes; i++ ) {
                    int IsSelected = ((VideoSystem->VideoModeList[i].Width == VidConfigWidth->IValue) && 
                        (VideoSystem->VideoModeList[i].Height == VidConfigHeight->IValue)) ? 1 : 0;
                    if( igSelectable_Bool(VideoSystem->VideoModeList[i].Description,IsSelected,0,ZeroSize ) ) {
                        VideoSystemSetVideoSettings(VideoSystem,i);
                    }
                    if( IsSelected ) {
                        igSetItemDefaultFocus();
                    }
                }
                igEndCombo();
            }
            igSeparator();
        }
        if( igCheckbox("Fullscreen Mode",(bool *) &VidConfigFullScreen->IValue) ) {
            DPrintf("VidConfigFullScreen:%i\n",VidConfigFullScreen->IValue);
            VideoSystemSetVideoSettings(VideoSystem,-1);
        }
    }
    igEnd();
    if( !GUI->VideoSettingsWindowHandle ) {
        GUIToggleHandle(GUI,GUI->VideoSettingsWindowHandle);
    }
}

void GUIDrawLevelTree(GUI_t *GUI,LevelManager_t *LevelManager,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem,
                      const Mission_t *Missions,int NumMissions)
{
    static int FailedMissionNumber = -1;
    static int FailedLevelNumber = -1;
    int TreeNodeFlags;
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
        CurrentMission = LevelManager->CurrentLevel->MissionNumber;
        CurrentLevel = LevelManager->CurrentLevel->LevelNumber;
    }
    for( i = 0; i < NumMissions; i++ ) {
        TreeNodeFlags = ImGuiTreeNodeFlags_None;
        if(  CurrentMission == Missions[i].MissionNumber ) {
            TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }
        if( igTreeNodeEx_Str(Missions[i].MissionName,TreeNodeFlags) ) {
            for( j = 0; j < Missions[i].NumLevels; j++ ) {
                DisableNode = 0;
                TreeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
                if( CurrentMission == Missions[i].MissionNumber && CurrentLevel == Missions[i].Levels[j].LevelNumber ) {
                    TreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
                    DisableNode = 1;
                }
                if( DisableNode ) {
                    igBeginDisabled(1);
                }
                if( igTreeNodeEx_Str(Missions[i].Levels[j].LevelName,TreeNodeFlags) ) {
                    if (igIsMouseDoubleClicked(0) && igIsItemHovered(ImGuiHoveredFlags_None) ) {
                        if( LevelManagerLoadLevel(LevelManager,GUI,VideoSystem,SoundSystem,
                                              Missions[i].MissionNumber,Missions[i].Levels[j].LevelNumber) ) {
                            //Close it if we selected a level and it was loaded properly.
                            GUI->LevelSelectWindowHandle = 0;
                        } else {
                            FailedMissionNumber = i;
                            FailedLevelNumber = j;
                            igOpenPopup_Str("Load Level Error",0);
                        }
                    }
                }
                if( DisableNode ) {
                    igEndDisabled();
                }
            }
            GUIPrepareModalWindow();
            if( igBeginPopupModal("Load Level Error",NULL,ImGuiWindowFlags_AlwaysAutoResize) ) {
                assert(FailedMissionNumber != -1 && FailedLevelNumber != -1 );
                igText("Failed to load level \"%s\"",Missions[FailedMissionNumber].Levels[FailedLevelNumber].LevelName);
                if (igButton("OK", ButtonSize) ) {
                    igCloseCurrentPopup(); 
                }
                igEndPopup();
            }
            igTreePop();
        }
    }
}
void GUIDrawLevelSelectWindow(GUI_t *GUI,LevelManager_t *LevelManager,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem)
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
                GUIDrawLevelTree(GUI,LevelManager,VideoSystem,SoundSystem,MOHMissionsList,NumMOHMissions);
            } else {
                GUIDrawLevelTree(GUI,LevelManager,VideoSystem,SoundSystem,MOHUMissionsList,NumMOHUMissions);
            }
        }
    }
    igEnd();
    if( !GUI->LevelSelectWindowHandle ) {
        GUIToggleHandle(GUI,GUI->LevelSelectWindowHandle);
    }
}

void GUISetErrorMessage(GUI_t *GUI,const char *Message)
{
    if( !GUI ) {
        DPrintf("GUISetErrorMessage:Invalid GUI struct\n");
        return;
    }
    if( !Message ) {
        DPrintf("GUISetErrorMessage:Invalid Message.");
        return;
    }
    
    if( GUI->ErrorMessage ) {
        free(GUI->ErrorMessage);
    }
    
    GUI->ErrorMessage = StringCopy(Message);
    igOpenPopup_Str("Error",0);
    GUIPushWindow(GUI);
}
void GUIFileDialogRender(GUI_t *GUI,GUIFileDialog_t *FileDialog)
{
    ImVec2 MaxSize;
    ImVec2 MinSize;
    ImGuiViewport *Viewport;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    char *DirectoryPath;
    char *FileName;
    void *UserData;
    
    if( !FileDialog ) {
        return;
    }
    if( !IGFD_IsOpened(FileDialog->Window) ) {
        return;
    }
    Viewport = igGetMainViewport();
    WindowPosition.x = Viewport->WorkPos.x;
    WindowPosition.y = Viewport->WorkPos.y;
    WindowPivot.x = 0.f;
    WindowPivot.y = 0.f;
    MaxSize.x = Viewport->Size.x;
    MaxSize.y = Viewport->Size.y;
    MinSize.x = -1;
    MinSize.y = -1;

    igSetNextWindowSize(MaxSize,0);
    igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);
    if (IGFD_DisplayDialog(FileDialog->Window, FileDialog->Key, 
        ImGuiWindowFlags_NoCollapse  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings, MinSize, MaxSize)) {
        if (IGFD_IsOk(FileDialog->Window)) {
                FileName = IGFD_GetFilePathName(FileDialog->Window);
                DirectoryPath = IGFD_GetCurrentPath(FileDialog->Window);
                UserData = IGFD_GetUserDatas(FileDialog->Window);
                if( FileDialog->OnElementSelected ) {
                    FileDialog->OnElementSelected(FileDialog,GUI,DirectoryPath,FileName,UserData);
                }
                if( DirectoryPath ) {
                    free(DirectoryPath);
                }
                if( FileName ) {
                    free(FileName);
                }
        } else {
            if( FileDialog->OnDialogCancelled ) {
                FileDialog->OnDialogCancelled(FileDialog,GUI);
            }
        }
    }

}
void GUIRenderFileDialogs(GUI_t *GUI)
{
    GUIFileDialog_t *Iterator;
    
    for( Iterator = GUI->FileDialogList; Iterator; Iterator = Iterator->Next ) {
        GUIFileDialogRender(GUI,Iterator);
    }
}
void GUIDraw(GUI_t *GUI,LevelManager_t *LevelManager,Camera_t *Camera,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem,ComTimeInfo_t *TimeInfo)
{
    ImVec2 ButtonSize;
    
    GUIBeginFrame();
    
    GUIDrawDebugOverlay(TimeInfo);
    
    if( !GUI->NumActiveWindows ) {
        GUIDrawHelpOverlay();
        GUIEndFrame();
        return;
    }
    
    GUIRenderFileDialogs(GUI);
    if( GUI->ErrorMessage ) {
        ButtonSize.x = 120;
        ButtonSize.y = 0;
        GUIPrepareModalWindow();
        if( igBeginPopupModal("Error",NULL,ImGuiWindowFlags_AlwaysAutoResize) ) {
            igText(GUI->ErrorMessage);
            if (igButton("OK", ButtonSize) ) {
                GUIPopWindow(GUI);
                igCloseCurrentPopup(); 
            }
            igEndPopup();
        }
    }
    GUIDrawDebugWindow(GUI,LevelManager,Camera,VideoSystem,SoundSystem);
    GUIDrawVideoSettingsWindow(GUI,VideoSystem);
    GUIDrawLevelSelectWindow(GUI,LevelManager,VideoSystem,SoundSystem);
//     igShowDemoWindow(NULL);
    GUIEndFrame();
}

int GUIFileDialogIsOpen(GUIFileDialog_t *FileDialog)
{
    if( !FileDialog ) {
        DPrintf("GUIFileDialogIsOpen:Invalid dialog data\n");
        return 0;
    }
    
    return IGFD_IsOpened(FileDialog->Window);
}

void *GUIFileDialogGetUserData(GUIFileDialog_t *FileDialog)
{
    if( !FileDialog ) {
        DPrintf("GUIFileDialogIsOpen:Invalid dialog data\n");
        return 0;
    } 
    return IGFD_GetUserDatas(FileDialog->Window);
}
void GUIFileDialogOpenWithUserData(GUI_t *GUI,GUIFileDialog_t *FileDialog,void *UserData)
{
    if( !FileDialog ) {
        DPrintf("GUIFileDialogOpen:Invalid dialog data\n");
        return;
    }
    
    if( IGFD_IsOpened(FileDialog->Window) ) {
        return;
    }
    IGFD_OpenDialog2(FileDialog->Window,FileDialog->Key,FileDialog->WindowTitle,FileDialog->Filters,".",1,
                     UserData,ImGuiFileDialogFlags_DontShowHiddenFiles);
    GUIPushWindow(GUI);
}
void GUIFileDialogOpen(GUI_t *GUI,GUIFileDialog_t *FileDialog)
{
    GUIFileDialogOpenWithUserData(GUI,FileDialog,NULL);
}

void GUIFileDialogClose(GUI_t *GUI,GUIFileDialog_t *FileDialog)
{
    if( !FileDialog ) {
        DPrintf("GUIFileDialogClose:Invalid dialog data\n");
        return;
    }
    
    if( !IGFD_IsOpened(FileDialog->Window) ) {
        return;
    }
    IGFD_CloseDialog(FileDialog->Window);
    GUIPopWindow(GUI);
}
/*
 Register a new file dialog.
 Filters can be NULL if we want a dir selection dialog or have a value based on ImGuiFileDialog documentation if we want to
 select a certain type of file.
 OnElementSelected and OnDialogCancelled are two callback that can be set to NULL if we are not interested in the result.
 NOTE that setting them to NULL or the cancel callback to NULL doesn't close the dialog.
 */
GUIFileDialog_t *GUIFileDialogRegister(GUI_t *GUI,const char *WindowTitle,const char *Filters,FileDialogSelectCallback_t OnElementSelected,
                                       FileDialogCancelCallback_t OnDialogCancelled)
{
    GUIFileDialog_t *FileDialog;
    
    if( !WindowTitle) {
        DPrintf("GUIFileDialogRegister:Invalid Window Title\n");
        return NULL;
    }

    FileDialog = malloc(sizeof(GUIFileDialog_t));
    
    if( !FileDialog ) {
        DPrintf("GUIFileDialogRegister:Couldn't allocate struct data.\n");
        return NULL;
    }
    asprintf(&FileDialog->Key,"FileDialog%i",GUI->NumRegisteredFileDialog);
    FileDialog->WindowTitle = StringCopy(WindowTitle);
    if( Filters ) {
        FileDialog->Filters = StringCopy(Filters);
    } else {
        FileDialog->Filters = NULL;
    }
    FileDialog->Window = IGFD_Create();
    FileDialog->OnElementSelected = OnElementSelected;
    FileDialog->OnDialogCancelled = OnDialogCancelled;
    FileDialog->Next = GUI->FileDialogList;
    GUI->FileDialogList = FileDialog;
    GUI->NumRegisteredFileDialog++;
    
    return FileDialog;
}
void GUIFileDialogSetTitle(GUIFileDialog_t *FileDialog,const char *Title)
{
    if(!FileDialog) {
        DPrintf("GUIFileDialogSetTitle:Invalid dialog\n");
        return;
    }
    if( !Title ) {
        DPrintf("GUIFileDialogSetTitle:Invalid title\n");
        return;
    }
    if( FileDialog->WindowTitle ) {
        free(FileDialog->WindowTitle);
    }
    FileDialog->WindowTitle = StringCopy(Title);
}

void GUIFileDialogSetOnElementSelectedCallback(GUIFileDialog_t *FileDialog,FileDialogSelectCallback_t OnElementSelected)
{
    if(!FileDialog) {
        DPrintf("GUIFileDialogSetOnElementSelectedCallback:Invalid dialog\n");
        return;
    }
    FileDialog->OnElementSelected = OnElementSelected;
}
void GUIFileDialogSetOnDialogCancelledCallback(GUIFileDialog_t *FileDialog,FileDialogCancelCallback_t OnDialogCancelled)
{
    if(!FileDialog) {
        DPrintf("GUIFileDialogSetOnDialogCancelledCallback:Invalid dialog\n");
        return;
    }
    FileDialog->OnDialogCancelled = OnDialogCancelled;
}

void GUIContextInit(ImGuiContext *Context,VideoSystem_t *VideoSystem,const char *ConfigFilePath)
{
    ImGuiIO *IO;
    ImGuiStyle *Style;
    ImFont *Font;
    ImFontConfig *FontConfig;
    
    IO = igGetIO();
    igSetCurrentContext(Context);
    ImGui_ImplSDL2_InitForOpenGL(VideoSystem->Window, &VideoSystem->GLContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    igStyleColorsDark(NULL);
    if( GUIFont->Value[0] ) {
        Font = ImFontAtlas_AddFontFromFileTTF(IO->Fonts,GUIFont->Value,floor(GUIFontSize->FValue * VideoSystem->DPIScale),NULL,NULL);
        if( !Font ) {
            DPrintf("GUIContextInit:Invalid font file...using default\n");
            ConfigSet("GUIFont","");
        }
    } else {
        FontConfig = ImFontConfig_ImFontConfig();
        FontConfig->OversampleH = 1;
        FontConfig->OversampleV = 1;
        FontConfig->PixelSnapH = true;
        FontConfig->SizePixels = floor(GUIFontSize->FValue * VideoSystem->DPIScale);
        ImFontAtlas_AddFontDefault(IO->Fonts,FontConfig);
        ImFontConfig_destroy(FontConfig);
    }
    Style = igGetStyle();
    Style->WindowTitleAlign.x = 0.5f;
    ImGuiStyle_ScaleAllSizes(Style,VideoSystem->DPIScale);
    IO->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    IO->IniFilename = ConfigFilePath;
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
    GUI->ProgressBar = malloc(sizeof(GUIProgressBar_t));
    GUI->ErrorMessage = NULL;
    
    ConfigPath = SysGetConfigPath();
    asprintf(&GUI->ConfigFilePath,"%simgui.ini",ConfigPath);
    free(ConfigPath);
    
    if( !GUI->ProgressBar ) {
        DPrintf("GUIInit:Failed to allocate memory for ProgressBar struct\n");
        free(GUI);
        return NULL;
    }
    GUI->NumRegisteredFileDialog = 0;
    
    GUI->FileDialogList = NULL;

    GUIFont = ConfigGet("GUIFont");
    GUIFontSize = ConfigGet("GUIFontSize");
    GUIShowFPS = ConfigGet("GUIShowFPS");
    
    GUI->DefaultContext = igCreateContext(NULL);
    GUI->ProgressBar->Context = igCreateContext(NULL);
    GUI->ProgressBar->DialogTitle = NULL;
    GUIContextInit(GUI->ProgressBar->Context,VideoSystem,GUI->ConfigFilePath);
    GUIContextInit(GUI->DefaultContext,VideoSystem,GUI->ConfigFilePath);
    
    GUI->NumActiveWindows = 0;

    return GUI;
}
