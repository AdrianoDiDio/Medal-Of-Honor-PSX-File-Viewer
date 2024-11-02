// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    GFXModelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GFXModelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GFXModelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#include "GUI.h"
#include "../Common/VRAM.h"
#include "GFXModelViewer.h"

void GUIFree(GUI_t *GUI)
{
    GUIReleaseContext(GUI->DefaultContext);
    ProgressBarDestroy(GUI->ProgressBar);
    FileDialogListFree();
    ErrorMessageDialogFree(GUI->ErrorMessageDialog);
    free(GUI->ConfigFilePath);
    free(GUI);
}

bool GUIIsMouseFree()
{
    ImGuiIO *IO;
    IO = igGetIO();
    return !IO->WantCaptureMouse;
}
bool GUIIsKeyboardFree()
{
    ImGuiIO *IO;
    IO = igGetIO();
    return !IO->WantCaptureKeyboard;
}
void GUIProcessEvent(GUI_t *GUI,SDL_Event *Event)
{
    ImGui_ImplSDL3_ProcessEvent(Event);
}

void GUIDrawDebugWindow(GUI_t *GUI,Camera_t *Camera,VideoSystem_t *VideoSystem)
{
    int LinkedVersion;
    int CompiledVersion;
    
    if( !GUI->DebugWindowHandle ) {
        return;
    }

    if( igBegin("Debug Settings",&GUI->DebugWindowHandle,ImGuiWindowFlags_AlwaysAutoResize) ) {
        if( igCollapsingHeader_TreeNodeFlags("Debug Statistics",ImGuiTreeNodeFlags_DefaultOpen) ) {
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
        }
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

    
    if( GUIShowFPS->IValue ) {
        igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);
        if( igBegin("FPS", NULL, WindowFlags) ) {
            igText(TimeInfo->FPSString);
        }
        igEnd(); 
    }
}

void GUIDrawMainWindow(GUI_t *GUI,GFXObjectManager_t *GFXObjectManager,VideoSystem_t *VideoSystem,Camera_t *Camera)
{
    GFX_t *GFX;
    ImVec2 ZeroSize;
    int IsSelected;
    int DisableNode;
    char SmallBuffer[64];
    char DeleteButtonId[32];
    int i;
    int Changed;
    ImGuiTableFlags TableFlags;
    ImGuiInputTextFlags InputTextFlags;
    ImGuiTreeNodeFlags TreeNodeFlags;

    
    if( !igBegin("Main Window", NULL, ImGuiWindowFlags_AlwaysAutoResize) ) {
        return;
    }
    
    ZeroSize.x = 0.f;
    ZeroSize.y = 0.f;
    
    if( igCollapsingHeader_TreeNodeFlags("Help",ImGuiTreeNodeFlags_DefaultOpen) ) {
        igText("Press and Hold The Left Mouse Button to Rotate the Camera");
        igText("Scroll the Mouse Wheel to Zoom the Camera In and Out");
        igText("Press A and S to strafe the Camera left-right,Spacebar and Z to move it up-down");
        igText("Press and Hold M to change to the next animation frame using the current Pose");
        igText("Press N to change to the next animation pose");
        igText("Press Escape to exit the program");
    }
    if( igCollapsingHeader_TreeNodeFlags("Camera",ImGuiTreeNodeFlags_DefaultOpen) ) {
        igText("Camera Spherical Position(Radius,Theta,Phi):%.3f;%.3f;%.3f",Camera->Position.Radius,Camera->Position.Theta,Camera->Position.Phi);
        igText("Camera View Point:%.3f;%.3f;%.3f",Camera->ViewPoint[0],Camera->ViewPoint[1],Camera->ViewPoint[2]);

        if( igButton("Reset Camera Position",ZeroSize) ) {
            CameraReset(Camera);
        }
        if( igSliderFloat("Camera Speed",&CameraSpeed->FValue,10.f,256.f,"%.2f",0) ) {
            ConfigSetNumber("CameraSpeed",CameraSpeed->FValue);
        }
        if( igSliderFloat("Camera Mouse Sensitivity",&CameraMouseSensitivity->FValue,1.f,20.f,"%.2f",0) ) {
            ConfigSetNumber("CameraMouseSensitivity",CameraMouseSensitivity->FValue);
        }
    }
    if( igCollapsingHeader_TreeNodeFlags("Settings",ImGuiTreeNodeFlags_DefaultOpen) ) {
        if( GUICheckBoxWithTooltip("WireFrame Mode",(bool *) &EnableWireFrameMode->IValue,EnableWireFrameMode->Description) ) {
            ConfigSetNumber("EnableWireFrameMode",EnableWireFrameMode->IValue);
        }
        if( GUICheckBoxWithTooltip("Ambient Light",(bool *) &EnableAmbientLight->IValue,EnableAmbientLight->Description) ) {
            ConfigSetNumber("EnableAmbientLight",EnableAmbientLight->IValue);
        }
        if( GUICheckBoxWithTooltip("Show FPS",(bool *) &GUIShowFPS->IValue,GUIShowFPS->Description) ) {
            ConfigSetNumber("GUIShowFPS",GUIShowFPS->IValue);
        }
    }
    TreeNodeFlags = GFXObjectManager->GFXPack != NULL ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
    if( igCollapsingHeader_TreeNodeFlags("GFX Informations",ImGuiTreeNodeFlags_DefaultOpen) ) {
        GFX = GFXObjectManagerGetCurrentGFX(GFXObjectManager);
        if( !GFX ) {
            igText("No GFX selected.");
        } else {
            igText("Name:%s",GFXObjectManager->GFXPack->Name);
            igText("Rotation:%i;%i;%i",GFX->RotationX,GFX->RotationY,GFX->RotationZ);
            igText("Current Animation Index:%i",GFX->CurrentAnimationIndex);
            igText("Current Frame Index:%i/%i",GFX->CurrentFrameIndex,
                   GFX->Animation[GFX->CurrentAnimationIndex].NumFrames);
            igText("NumAnimations:%i",GFX->Header.NumAnimationIndex);
            sprintf(SmallBuffer,"Animation %i",GFX->CurrentAnimationIndex + 1);
            if( igBeginCombo("Animation Pose",SmallBuffer,0) ) {
                for (i = 0; i < GFX->Header.NumAnimationIndex; i++) {
                    IsSelected = (GFX->CurrentAnimationIndex == i);
                    sprintf(SmallBuffer,"Animation %i",i + 1);
                    if( !GFX->Animation[i].NumFrames ) {
                        igBeginDisabled(1);
                    }
                    if (igSelectable_Bool(SmallBuffer, IsSelected,0,ZeroSize)) {
                        if( GFX->CurrentAnimationIndex != i ) {
                            if( !GFXSetAnimationPose(GFX,i,0) ) {
                                ErrorMessageDialogSet(GUI->ErrorMessageDialog,"Failed to set animation pose");
                            }
                        }
                    }
                    if( IsSelected ) {
                        igSetItemDefaultFocus();
                    }
                    if( !GFX->Animation[i].NumFrames ) {
                        igEndDisabled();
                    }
                }
                igEndCombo();
            }
            sprintf(SmallBuffer,"Frame %i",GFX->CurrentFrameIndex + 1);
            if( igBeginCombo("Animation Frame List",SmallBuffer,0) ) {
                for( i = 0; i < GFX->Animation[GFX->CurrentAnimationIndex].NumFrames; i++ ) {
                    IsSelected = (GFX->CurrentFrameIndex == i);
                    sprintf(SmallBuffer,"Frame %i",i + 1);
                    if (igSelectable_Bool(SmallBuffer, IsSelected,0,ZeroSize)) {
                        if( GFX->CurrentFrameIndex != i ) {
                            if( !GFXSetAnimationPose(GFX,GFX->CurrentAnimationIndex,i) ) {
                                ErrorMessageDialogSet(GUI->ErrorMessageDialog,"Failed to set animation pose");
                            }
                        }
                    }
                    if( IsSelected ) {
                        igSetItemDefaultFocus();
                    }
                }
                igEndCombo();
            }
            igSeparator();
            if( igButton("Play Animation",ZeroSize) ) {
                if( !GFXObjectManagerIsAnimationPlaying(GFXObjectManager) ) {
                    GFX->CurrentFrameIndex = 0;
                    GFXObjectManagerSetAnimationPlay(GFXObjectManager,1);
                }
                
            }
            igSameLine(0,-1);
            if( igButton("Stop Animation",ZeroSize) ) {
                GFXObjectManagerSetAnimationPlay(GFXObjectManager,0);
            }
            igSeparator();
            igText("Export selected model");
            if( igButton("Export current pose to Ply",ZeroSize) ) {
                GFXObjectManagerExportSelectedModel(GFXObjectManager,GUI,VideoSystem,GFX_OBJECT_MANAGER_EXPORT_FORMAT_PLY,false);
            }
            igSameLine(0.f,10.f);
            if( igButton("Export current animation to Ply",ZeroSize) ) {
                GFXObjectManagerExportSelectedModel(GFXObjectManager,GUI,VideoSystem,GFX_OBJECT_MANAGER_EXPORT_FORMAT_PLY,true);
            }
        }
    }
    igEnd();
}
void GUIDrawMenuBar(Application_t *Application)
{
    if( !igBeginMainMenuBar() ) {
        return;
    }
    if (igBeginMenu("File",true)) {
        if( igMenuItem_Bool("Open",NULL,false,true) ) {
            GFXObjectManagerOpenFileDialog(Application->GFXObjectManager,Application->GUI,Application->Engine->VideoSystem);
        }
        if( igMenuItem_Bool("Exit",NULL,false,true) ) {
            Quit(Application);
        }
        igEndMenu();
    }
    if (igBeginMenu("Settings",true)) {
        if( igMenuItem_Bool("Video",NULL,Application->GUI->VideoSettingsWindowHandle,true) ) {
            Application->GUI->VideoSettingsWindowHandle = 1;
        }
        igEndMenu();
    }
    if (igBeginMenu("View",true)) {
        if( igMenuItem_Bool("Debug Window",NULL,Application->GUI->DebugWindowHandle,true) ) {
            Application->GUI->DebugWindowHandle = 1;
        }
        igEndMenu();
    }
    igEndMainMenuBar();
}

void GUIDraw(Application_t *Application)
{
    
    GUIBeginFrame();
    GUIDrawDebugOverlay(Application->Engine->TimeInfo);
    GUIDrawMenuBar(Application);
    FileDialogRenderList();
    ErrorMessageDialogDraw(Application->GUI->ErrorMessageDialog);
    GUIDrawMainWindow(Application->GUI,Application->GFXObjectManager,Application->Engine->VideoSystem,Application->Camera);
    GUIDrawDebugWindow(Application->GUI,Application->Camera,Application->Engine->VideoSystem);
    GUIDrawVideoSettingsWindow(&Application->GUI->VideoSettingsWindowHandle,Application->Engine->VideoSystem);
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
