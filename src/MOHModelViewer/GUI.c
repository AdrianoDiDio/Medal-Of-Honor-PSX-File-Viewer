// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
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

Config_t *GUIFont;
Config_t *GUIFontSize;
Config_t *GUIShowFPS;

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
    ProgressBarDestroy(GUI->ProgressBar);
    GUIFreeDialogList(GUI);
    
    if( GUI->ErrorMessage ) {
        free(GUI->ErrorMessage);
    }
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

void GUIProcessEvent(GUI_t *GUI,SDL_Event *Event)
{
    ImGui_ImplSDL2_ProcessEvent(Event);
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
    MaxSize.x = Viewport->WorkSize.x;
    MaxSize.y = Viewport->WorkSize.y;
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
                if( IsSelected ) {
                    igSetItemDefaultFocus();
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

void GUIDrawMainWindow(GUI_t *GUI,RenderObjectManager_t *RenderObjectManager,VideoSystem_t *VideoSystem,Camera_t *Camera)
{
    BSDRenderObjectPack_t *PackIterator;
    BSDRenderObject_t *RenderObjectIterator;
    BSDRenderObject_t *CurrentRenderObject;
    BSDAnimationFrame_t *CurrentFrame;
    ImVec2 ZeroSize;
    int IsSelected;
    int TreeNodeFlags;
    int DisableNode;
    char SmallBuffer[64];
    char DeleteButtonId[32];
    int i;
    
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
                    GUISetErrorMessage(GUI,"Failed to remove BSD pack from list");
                }
                for( RenderObjectIterator = PackIterator->RenderObjectList; RenderObjectIterator; 
                    RenderObjectIterator = RenderObjectIterator->Next ) {
                    TreeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
                    DisableNode = 0;
                    sprintf(SmallBuffer,"%i",RenderObjectIterator->Id);
                    if( RenderObjectIterator == RenderObjectManagerGetSelectedRenderObject(RenderObjectManager) ) {
                        TreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
                        DisableNode = 1;
                    }
                    if( DisableNode ) {
                        igBeginDisabled(1);
                    }
                    if( igTreeNodeEx_Str(SmallBuffer,TreeNodeFlags) ) {
                        if (igIsMouseDoubleClicked(0) && igIsItemHovered(ImGuiHoveredFlags_None) ) {
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
            CurrentFrame = &CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].
                Frame[CurrentRenderObject->CurrentFrameIndex];
            igText("Id:%i",CurrentRenderObject->Id);
            igText("Type:%i",CurrentRenderObject->Type);
            igText("Scale:%f;%f;%f",CurrentRenderObject->Scale[0],CurrentRenderObject->Scale[1],CurrentRenderObject->Scale[2]);
            igText("References RenderObject Id:%i",CurrentRenderObject->ReferencedRenderObjectId);
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
                    if (igSelectable_Bool(SmallBuffer, IsSelected,0,ZeroSize)) {
                        if( CurrentRenderObject->CurrentAnimationIndex != i ) {
                            if( !BSDRenderObjectSetAnimationPose(CurrentRenderObject,i,0) ) {
                                GUISetErrorMessage(GUI,"Failed to set animation pose");
                            }
                        }
                    }
                    if( IsSelected ) {
                        igSetItemDefaultFocus();
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
                            if( !BSDRenderObjectSetAnimationPose(CurrentRenderObject,CurrentRenderObject->CurrentAnimationIndex,i) ) {
                                GUISetErrorMessage(GUI,"Failed to set animation pose");
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
            igText("Export current pose");
            if( igButton("Export to Ply",ZeroSize) ) {
                RenderObjectManagerExportCurrentPose(RenderObjectManager,GUI,VideoSystem,RENDER_OBJECT_MANAGER_EXPORT_FORMAT_PLY);
            }
        }
    }
    igEnd();
}
void GUIDrawMenuBar(Engine_t *Engine)
{
    if( !igBeginMainMenuBar() ) {
        return;
    }
    if (igBeginMenu("File",true)) {
        if( igMenuItem_Bool("Open",NULL,false,true) ) {
            RenderObjectManagerOpenFileDialog(Engine->RenderObjectManager,Engine->VideoSystem);
        }
        if( igMenuItem_Bool("Exit",NULL,false,true) ) {
            Quit(Engine);
        }
        igEndMenu();
    }
    if (igBeginMenu("Settings",true)) {
        if( igMenuItem_Bool("Video",NULL,Engine->GUI->VideoSettingsWindowHandle,true) ) {
            Engine->GUI->VideoSettingsWindowHandle = 1;
        }
        igEndMenu();
    }
    if (igBeginMenu("View",true)) {
        if( igMenuItem_Bool("Debug Window",NULL,Engine->GUI->DebugWindowHandle,true) ) {
            Engine->GUI->DebugWindowHandle = 1;
        }
        igEndMenu();
    }
    igEndMainMenuBar();
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
void GUIDraw(Engine_t *Engine)
{
    
    GUIBeginFrame();
    GUIDrawDebugOverlay(Engine->TimeInfo);
    GUIDrawMenuBar(Engine);
    GUIRenderFileDialogs(Engine->GUI);
    GUIDrawErrorMessage(Engine->GUI);
    GUIDrawMainWindow(Engine->GUI,Engine->RenderObjectManager,Engine->VideoSystem,Engine->Camera);
    GUIDrawDebugWindow(Engine->GUI,Engine->Camera,Engine->VideoSystem);
    GUIDrawVideoSettingsWindow(Engine->GUI,Engine->VideoSystem);
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
void GUIFileDialogOpenWithUserData(GUIFileDialog_t *FileDialog,void *UserData)
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
}
void GUIFileDialogOpen(GUIFileDialog_t *FileDialog)
{
    GUIFileDialogOpenWithUserData(FileDialog,NULL);
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

    GUI->NumRegisteredFileDialog = 0;
    
    GUI->FileDialogList = NULL;

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
