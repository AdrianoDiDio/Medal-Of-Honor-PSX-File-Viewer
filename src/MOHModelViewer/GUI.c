// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    MOHModelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MOHModelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MOHModelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#include "GUI.h"
#include "../Common/VRAM.h"
#include "MOHModelViewer.h"

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
    ImGui_ImplSDL2_ProcessEvent(Event);
}

void GUIDrawDebugWindow(GUI_t *GUI,Camera_t *Camera,VideoSystem_t *VideoSystem)
{
    SDL_version LinkedVersion;
    SDL_version CompiledVersion;
    
    if( !GUI->DebugWindowHandle ) {
        return;
    }

    if( igBegin("Debug Settings",&GUI->DebugWindowHandle,ImGuiWindowFlags_AlwaysAutoResize) ) {
        if( igCollapsingHeader_TreeNodeFlags("Debug Statistics",ImGuiTreeNodeFlags_DefaultOpen) ) {
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

void GUIDrawMainWindow(GUI_t *GUI,RenderObjectManager_t *RenderObjectManager,VideoSystem_t *VideoSystem,Camera_t *Camera)
{
    BSDRenderObjectPack_t *PackIterator;
    RenderObject_t *RenderObjectIterator;
    RenderObject_t *CurrentRenderObject;
    BSDAnimationFrame_t *CurrentFrame;
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
    TreeNodeFlags = RenderObjectManager->BSDList != NULL ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
    if( igCollapsingHeader_TreeNodeFlags("Animated RenderObjects List",TreeNodeFlags) ) {
        for(PackIterator = RenderObjectManager->BSDList; PackIterator; PackIterator = PackIterator->Next) {
            TreeNodeFlags = ImGuiTreeNodeFlags_None;
            if(  PackIterator == RenderObjectManagerGetSelectedBSDPack(RenderObjectManager) ) {
                TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
            }
            if( igTreeNodeEx_Str(PackIterator->Name,TreeNodeFlags) ) {
                igSameLine(0,-1);
                igText(PackIterator->GameVersion == MOH_GAME_STANDARD ? "MOH" : "MOH:Underground");
                igSameLine(0,-1);
                //NOTE(Adriano):We do not allow for duplicated BSD however since we support both MOH and MOH:Underground then
                //this could be confusing since the ID would for example be 2_1 for both versions when loading Mission 2 Level 1.
                //In order to solve this we just append the GameVersion in order to obtain the final Id which will be
                //in this example 2_1.BSD0 or 2_1.BSD1 thus solving any potential conflict.
                sprintf(DeleteButtonId,"Remove##%i",PackIterator->GameVersion);
                if( igSmallButton(DeleteButtonId) ) {
                    if( RenderObjectManagerDeleteBSDPack(RenderObjectManager,PackIterator->Name,PackIterator->GameVersion) ) {
                        igTreePop();
                        break;
                    }
                    ErrorMessageDialogSet(GUI->ErrorMessageDialog,"Failed to remove BSD pack from list");
                }
                for( RenderObjectIterator = PackIterator->RenderObjectList; RenderObjectIterator; 
                    RenderObjectIterator = RenderObjectIterator->Next ) {
                    TreeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
                    DisableNode = 0;
                    sprintf(SmallBuffer,"%i(%s)",RenderObjectIterator->Id,RenderObjectIterator->IsStatic ? "Static" : "Animated");
                    if( RenderObjectIterator == RenderObjectManagerGetSelectedRenderObject(RenderObjectManager) ) {
                        TreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
                        DisableNode = 1;
                    }
                    if( DisableNode ) {
                        igBeginDisabled(1);
                    }
                    if( igTreeNodeEx_Str(SmallBuffer,TreeNodeFlags) ) {
                        if (igIsMouseDoubleClicked_Nil(0) && igIsItemHovered(ImGuiHoveredFlags_None) ) {
                            RenderObjectManagerSetSelectedRenderObject(RenderObjectManager,PackIterator,RenderObjectIterator);
                        }
                    }
                    if( DisableNode ) {
                        igEndDisabled();
                    }
                }
                igTreePop();
            }
        }
    }
    if( igCollapsingHeader_TreeNodeFlags("Current RenderObject Informations",ImGuiTreeNodeFlags_DefaultOpen) ) {
        CurrentRenderObject = RenderObjectManagerGetSelectedRenderObject(RenderObjectManager);
        if( !CurrentRenderObject ) {
            igText("No RenderObject selected.");
        } else {
            igText("Id:%i",CurrentRenderObject->Id);
            igText("Type:%i",CurrentRenderObject->Type);
            igText("Scale:%f;%f;%f",CurrentRenderObject->Scale[0],CurrentRenderObject->Scale[1],CurrentRenderObject->Scale[2]);
            igText("References RenderObject Id:%i",CurrentRenderObject->ReferencedRenderObjectId);
            if( !CurrentRenderObject->IsStatic ) {
                CurrentFrame = RenderObjectGetCurrentFrame(CurrentRenderObject);
                igText("Current Animation Index:%i",CurrentRenderObject->CurrentAnimationIndex);
                igText("Current Frame Index:%i/%i",CurrentRenderObject->CurrentFrameIndex,
                   CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].NumFrames);
                igText("Current Frame Interpolation Index:%i (From %i to %i)",CurrentFrame->FrameInterpolationIndex,
                   HighNibble(CurrentFrame->FrameInterpolationIndex),
                   LowNibble(CurrentFrame->FrameInterpolationIndex));
                igText("NumAnimations:%i",CurrentRenderObject->NumAnimations);
                sprintf(SmallBuffer,"Animation %i",CurrentRenderObject->CurrentAnimationIndex + 1);
                if( igBeginCombo("Animation Pose",SmallBuffer,0) ) {
                    for (i = 0; i < CurrentRenderObject->NumAnimations; i++) {
                        IsSelected = (CurrentRenderObject->CurrentAnimationIndex == i);
                        sprintf(SmallBuffer,"Animation %i",i + 1);
                        if( !CurrentRenderObject->AnimationList[i].NumFrames ) {
                            igBeginDisabled(1);
                        }
                        if (igSelectable_Bool(SmallBuffer, IsSelected,0,ZeroSize)) {
                            if( CurrentRenderObject->CurrentAnimationIndex != i ) {
                                if( !RenderObjectSetAnimationPose(CurrentRenderObject,i,0,0) ) {
                                    ErrorMessageDialogSet(GUI->ErrorMessageDialog,"Failed to set animation pose");
                                }
                            }
                        }
                        if( IsSelected ) {
                            igSetItemDefaultFocus();
                        }
                        if( !CurrentRenderObject->AnimationList[i].NumFrames ) {
                            igEndDisabled();
                        }
                    }
                    igEndCombo();
                }
                sprintf(SmallBuffer,"Frame %i",CurrentRenderObject->CurrentFrameIndex + 1);
                if( igBeginCombo("Animation Frame List",SmallBuffer,0) ) {
                    for( i = 0; i < CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].NumFrames; i++ ) {
                        IsSelected = (CurrentRenderObject->CurrentFrameIndex == i);
                        sprintf(SmallBuffer,"Frame %i",i + 1);
                        if (igSelectable_Bool(SmallBuffer, IsSelected,0,ZeroSize)) {
                            if( CurrentRenderObject->CurrentFrameIndex != i ) {
                                if( !RenderObjectSetAnimationPose(CurrentRenderObject,CurrentRenderObject->CurrentAnimationIndex,i,0) ) {
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
                if( igCollapsingHeader_TreeNodeFlags("Quaternion List",ImGuiTreeNodeFlags_None) ) {
                    igSeparator();
                    igText("Changes to an input-field can be undo by pressing CTRL-Z.\n");
                    igText("Note that numbers are in fixed point math where 4096 is equals to 1\n");
                    if( igButton("Reset",ZeroSize) ) {
                        RenderObjectResetFrameQuaternionList(CurrentFrame);
                        RenderObjectSetAnimationPose(CurrentRenderObject,CurrentRenderObject->CurrentAnimationIndex,
                                                                CurrentRenderObject->CurrentFrameIndex,1);

                    }
                    TableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Borders | 
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_NoHostExtendX;
                    if( igBeginTable("Quaternion List",5,TableFlags,ZeroSize,0.f) ) {
                        igTableSetupColumn("Quaternion",0,0.f,0);
                        igTableSetupColumn("x",0,0.f,0);
                        igTableSetupColumn("y",0,0.f,0);
                        igTableSetupColumn("z",0,0.f,0);
                        igTableSetupColumn("w",0,0.f,0);

                        igTableHeadersRow();
                        InputTextFlags = ImGuiInputTextFlags_CharsDecimal;
                        if( RenderObjectManagerIsAnimationPlaying(RenderObjectManager) ) {
                            InputTextFlags |= ImGuiInputTextFlags_ReadOnly;
                        }
                        for( i = 0; i < CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].
                            Frame[CurrentRenderObject->CurrentFrameIndex].NumQuaternions; i++ ) {
                            Changed = 0;
                            igTableNextRow(0,0.f);
                            igTableSetColumnIndex(0);
                            igAlignTextToFramePadding(); 
                            igText("Quaternion %i",i);
                            igTableSetColumnIndex(1);
                            igPushID_Int(5 * i + 1); 
                            Changed |= igInputScalar("##Q1", ImGuiDataType_S16, 
                                                    (short *) &CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].
                                                    Frame[CurrentRenderObject->CurrentFrameIndex].
                                                    CurrentQuaternionList[i].x,NULL,NULL,NULL,InputTextFlags);
                            igPopID();
                            igTableSetColumnIndex(2);
                            igPushID_Int(5 * i + 2); 
                            Changed |= igInputScalar("##Q2", ImGuiDataType_S16, 
                                                    (short *) &CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].
                                                    Frame[CurrentRenderObject->CurrentFrameIndex].
                                                    CurrentQuaternionList[i].y,NULL,NULL,NULL,InputTextFlags);
                            igPopID();
                            igTableSetColumnIndex(3);
                            igPushID_Int(5 * i + 3);
                            Changed |= igInputScalar("##Q3", ImGuiDataType_S16, 
                                                    (short *) &CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].
                                                    Frame[CurrentRenderObject->CurrentFrameIndex].
                                                    CurrentQuaternionList[i].z,NULL,NULL,NULL,InputTextFlags);
                            igPopID();
                            igTableSetColumnIndex(4);
                            igPushID_Int(5 * i + 4);
                            Changed |= igInputScalar("##Q4", ImGuiDataType_S16, 
                                                    (short *) &CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].
                                                    Frame[CurrentRenderObject->CurrentFrameIndex].
                                                    CurrentQuaternionList[i].w,NULL,NULL,NULL,InputTextFlags);
                            igPopID();
                            if( Changed ) {
                                RenderObjectSetAnimationPose(CurrentRenderObject,CurrentRenderObject->CurrentAnimationIndex,
                                                                CurrentRenderObject->CurrentFrameIndex,1);
                            }
                        }
                        igEndTable();
                    }
                }
                igSeparator();
                if( igButton("Play Animation",ZeroSize) ) {
                    if( !RenderObjectManagerIsAnimationPlaying(RenderObjectManager) ) {
                        CurrentRenderObject->CurrentFrameIndex = 0;
                        RenderObjectManagerSetAnimationPlay(RenderObjectManager,1);
                    }
                    
                }
                igSameLine(0,-1);
                if( igButton("Stop Animation",ZeroSize) ) {
                    RenderObjectManagerSetAnimationPlay(RenderObjectManager,0);
                }
                igSeparator();
                igText("Export selected model");
                if( igButton("Export current pose to Ply",ZeroSize) ) {
                    RenderObjectManagerExportSelectedModel(RenderObjectManager,GUI,VideoSystem,RENDER_OBJECT_MANAGER_EXPORT_FORMAT_PLY,false);
                }
                igSameLine(0.f,10.f);
                if( igButton("Export current animation to Ply",ZeroSize) ) {
                    RenderObjectManagerExportSelectedModel(RenderObjectManager,GUI,VideoSystem,RENDER_OBJECT_MANAGER_EXPORT_FORMAT_PLY,true);
                }
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
            RenderObjectManagerOpenFileDialog(Application->RenderObjectManager,Application->GUI,Application->Engine->VideoSystem);
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
    GUIDrawMainWindow(Application->GUI,Application->RenderObjectManager,Application->Engine->VideoSystem,Application->Camera);
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
