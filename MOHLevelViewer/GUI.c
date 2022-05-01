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

void GUIReleaseContext(ImGuiContext *Context)
{    
    igSetCurrentContext(Context);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    igDestroyContext(Context);
}
void GUIFree(GUI_t *GUI)
{
    GUIReleaseContext(GUI->DefaultContext);
    GUIReleaseContext(GUI->ProgressBar->Context);
    IGFD_Destroy(GUI->DirSelectFileDialog);
    if( GUI->ProgressBar->DialogTitle ) {
        free(GUI->ProgressBar->DialogTitle);
    }
    free(GUI->ConfigFilePath);
    free(GUI->ProgressBar);
    free(GUI);
}

/*
 * Process a new event from the SDL system only when
 * it is active.
 * Returns 0 if GUI has ignored the event 1 otherwise.
 */
int GUIProcessEvent(GUI_t *GUI,SDL_Event *Event)
{
    if( !GUI->NumActiveWindows ) {
        return 0;
    }
    ImGui_ImplSDL2_ProcessEvent(Event);
    return 1;
}

/*
 Utilities functions to detect GUI status.
 Push => Signal that a window is visible and update the cursor status to handle it.
 Pop => Signal that a window has been closed and if there are no windows opened then it hides the cursor since it is not needed anymore.
 */
void GUIPushWindow(GUI_t *GUI)
{
    if( !GUI->NumActiveWindows ) {
        SysShowCursor();
    }
    GUI->NumActiveWindows++;
}
void GUIPopWindow(GUI_t *GUI)
{
    GUI->NumActiveWindows--;
    if( !GUI->NumActiveWindows ) {
        SysHideCursor();
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
void GUIToggleSettingsWindow(GUI_t *GUI)
{
    GUI->SettingsWindowHandle = !GUI->SettingsWindowHandle;
    GUIToggleHandle(GUI,GUI->SettingsWindowHandle);

}
void GUIToggleLevelSelectWindow(GUI_t *GUI)
{
    GUI->LevelSelectWindowHandle = !GUI->LevelSelectWindowHandle;
    GUIToggleHandle(GUI,GUI->LevelSelectWindowHandle);

}
/*
 TODO(Adriano):
 Turn this into a generic function to save file,accept a callback to a function that
 will be called when the dialog was closed with a selected path!
 Rename this function as GUIOpenDirSelectDialog(GUI,OnSuccess).
 There can be only one dialog...if for example a dialog is already opened this function
 should return -1 and tell the user that another dialog is already running.
 */
void GUISetMOHPath(GUI_t *GUI)
{
//     if( !LevelManager->IsPathSet ) {
//         //NOTE(Adriano):Default behaviour is to open the dialog at startup when there is not a valid pat set yet.
//         return;
//     }
    if( IGFD_IsOpened(GUI->DirSelectFileDialog) ) {
//         if( LevelManager->IsPathSet ) {
            //User has cancelled the path change operation.
            GUIPopWindow(GUI);
            IGFD_CloseDialog(GUI->DirSelectFileDialog);
//         }
        return;
    }
    DPrintf("Opening dialog\n");
    IGFD_OpenDialog2(GUI->DirSelectFileDialog,"Dir Select","Select dir",NULL,".",1,NULL,ImGuiFileDialogFlags_DontShowHiddenFiles);
    GUIPushWindow(GUI);
//     LevelManager->IsPathSet = 0;
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

void GUIDrawDebugWindow(GUI_t *GUI)
{
    SDL_version Version;
#if 1
//     if( !LevelManager->CurrentLevel ) {
//         return;
//     }
    if( !GUI->DebugWindowHandle ) {
        return;
    }
    if( igBegin("Debug Settings",&GUI->DebugWindowHandle,0) ) {
        if( LevelManagerIsLevelLoaded(LevelManager) ) {
            igText(LevelManager->EngineName);
            igSeparator();
            igText("Debug Settings");
            igCheckbox("WireFrame Mode",&LevelManager->Settings.WireFrame);
            igCheckbox("Show Level",&LevelManager->Settings.ShowMap);
            igCheckbox("Show Collision Data",&LevelManager->Settings.ShowCollisionData);
            igCheckbox("Show BSP Tree",&LevelManager->Settings.ShowAABBTree);
            igCheckbox("Show BSD nodes as Points",&LevelManager->Settings.ShowBSDNodes);
            igCheckbox("Show BSD RenderObjects as Points",&LevelManager->Settings.ShowBSDRenderObject);
            igCheckbox("Draw BSD RenderObjects",&LevelManager->Settings.DrawBSDRenderObjects);
            igCheckbox("Enable BSD RenderObjects ShowCase Rendering",&LevelManager->Settings.DrawBSDShowCaseRenderObject);
            igCheckbox("Frustum Culling",&LevelManager->Settings.EnableFrustumCulling);
            igCheckbox("Lighting",&LevelManager->Settings.EnableLighting);
            igCheckbox("Semi-Transparency",&LevelManager->Settings.EnableSemiTransparency);
            if( igCheckbox("Animated Lights",&LevelManager->Settings.EnableAnimatedLights) ) {
                if( !LevelManager->Settings.EnableAnimatedLights ) {
                    TSPUpdateAnimatedFaces(LevelManager->CurrentLevel->TSPList,LevelManager->CurrentLevel->BSD,1);
                }
            }
        }
       igSeparator();
       igText("Debug Statistics");
       igText("NumActiveWindows:%i",GUI->NumActiveWindows);
       igSeparator();
       igText(ComTime->FPSString);
       igText("OpenGL Version: %s",glGetString(GL_VERSION));
       SDL_GetVersion(&Version);
       igText("SDL Version: %u.%u.%u",Version.major,Version.minor,Version.patch);
       igSeparator();
       igText("Display Informations");
       igText("Resolution:%ix%i",VidConfigWidth->IValue,VidConfigHeight->IValue);
       igText("Refresh Rate:%i",VidConfigRefreshRate->IValue);
    }
    if( !GUI->DebugWindowHandle ) {
        GUIToggleHandle(GUI,GUI->DebugWindowHandle);
    }
    //If the user has closed it make sure to reset the cursor state.
//     if( !GUI->IsActive ) {
//         SysHideCursor();
//     }
    igEnd();
#endif
}

void GUIDrawHelpOverlay()
{
    int WindowFlags;
    ImGuiViewport *Viewport;
    ImVec2 WorkPosition;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    
    WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | 
                    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    Viewport = igGetMainViewport();
    WorkPosition = Viewport->WorkPos;
    WindowPosition.x = (WorkPosition.x + 10.f);
    WindowPosition.y = (WorkPosition.y + 10.f);
    WindowPivot.x = 0.f;
    WindowPivot.y = 0.f;
    igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);

    if( igBegin("Help", NULL, WindowFlags) ) {
        igText("Press F1 to enable/disable debug settings");
        igText("Press F2 to open video settings");
        igText("Press F3 to open the level selection window");
        igText("Press F4 to change the game path");
        igText("Press Escape to exit the program");
    }
    igEnd();
}

void GUISetProgressBarDialogTitle(GUI_t *GUI,char *Title)
{
    if( GUI->ProgressBar->DialogTitle ) {
        free(GUI->ProgressBar->DialogTitle);
    }
    GUI->ProgressBar->DialogTitle = (Title != NULL) ? StringCopy(Title) : "Loading...";
    //NOTE(Adriano):Forces a refresh...since changing the title disrupts the rendering process.
    GUI->ProgressBar->IsOpen = 0;
}

void GUIProgressBarBegin(GUI_t *GUI,char *Title)
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
void GUIProgressBarIncrement(GUI_t *GUI,float Increment,char *Message)
{
    ImGuiViewport *Viewport;
    ImVec2 ScreenCenter;
    ImVec2 Pivot;
    ImVec2 Size;
    ImGuiIO *IO;
    float Delta;
    
    if( !GUI ) {
        return;
    }
    
    IO = igGetIO();
    
    Viewport = igGetMainViewport();
    ImGuiViewport_GetCenter(&ScreenCenter,Viewport);
    DPrintf("Center:%f;%f\n",ScreenCenter.x,ScreenCenter.y);
    Pivot.x = 0.5f;
    Pivot.y = 0.5f;
    //Update it
    //Clear Screen init progress bar
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
    DPrintf("PBar:Increment++:%f %f\n",GUI->ProgressBar->CurrentPercentage,Increment);
    if (igBeginPopupModal(GUI->ProgressBar->DialogTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        Delta = (ComTime->Delta != 0.f) ? ComTime->Delta : 1;
        igProgressBar((GUI->ProgressBar->CurrentPercentage / 100.f) * Delta/* * IO->DeltaTime*/,Size,Message);
        igEnd();
    }
    GUIEndFrame();
    SysSwapBuffers();
}

void GUIProgressBarEnd(GUI_t *GUI)
{
    igSetCurrentContext(GUI->DefaultContext);
    GUI->ProgressBar->IsOpen = 0;
    GUI->ProgressBar->CurrentPercentage = 0.f;
}
void GUIGetMOHPath(GUI_t *GUI,LevelManager_t *LevelManager)
{
    ImVec2 MaxSize;
    ImVec2 MinSize;
    ImGuiViewport *Viewport;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    char *DirectoryPath;
    int LoadStatus;
    
    if( !IGFD_IsOpened(GUI->DirSelectFileDialog) ) {
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

    //     if( !GUI->DirSelectFileDialog ) {
//         GUI->DirSelectFileDialog = IGFD_Create();
//         GUIPushWindow(GUI);
//     }
    igSetNextWindowSize(MaxSize,0);
    igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);
    if (IGFD_DisplayDialog(GUI->DirSelectFileDialog, "Dir Select", ImGuiWindowFlags_NoCollapse  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings, 
        MinSize, MaxSize)) {
//         GUIPushWindow(GUI);
        if (IGFD_IsOk(GUI->DirSelectFileDialog)) {
                DirectoryPath = IGFD_GetFilePathName(GUI->DirSelectFileDialog);
                DPrintf("Selected directory %s\n",DirectoryPath);
                LoadStatus = LevelManagerInitWithPath(LevelManager,GUI,DirectoryPath);
                if( !LoadStatus ) {
                    igOpenPopup_Str("Wrong Folder",0);
                    GUIPushWindow(GUI);
                } else {
                    //Close it if we managed to load it.
                    ConfigSet("GameBasePath",DirectoryPath);
                    GUIPopWindow(GUI);
                    IGFD_CloseDialog(GUI->DirSelectFileDialog);
                }
                if (DirectoryPath) { 
                    free(DirectoryPath);
                }
        } else {
//             if( LevelManager->IsPathSet ) {
                //User has cancelled the path change operation.
                GUIPopWindow(GUI);
                IGFD_CloseDialog(GUI->DirSelectFileDialog);
//             }
        }
    }
}
void GUIDrawSettingsWindow(GUI_t *GUI)
{
#if 1
    int i;
//     if( !LevelManager->CurrentLevel ) {
//         return;
//     }
    if( !GUI->SettingsWindowHandle ) {
        return;
    }
    ImVec2 Size;
    Size.x = Size.y = 0.f;
    int PreviewIndex = VidConf.CurrentVideoMode != -1 ? VidConf.CurrentVideoMode : 0;
    if( igBegin("Settings",&GUI->SettingsWindowHandle,0) ) {
        igText("Video Settings");
        igSeparator();
        //NOTE(Adriano):Only in Fullscreen mode we can select the video mode we want.
        if( VidConfigFullScreen->IValue ) {
            igText("Video Mode");
            if( igBeginCombo("##Resolution", VidConf.VideoModeList[PreviewIndex].Description, 0) ) {
                for( i = 0; i < VidConf.NumVideoModes; i++ ) {
                    int IsSelected = ((VidConf.VideoModeList[i].Width == VidConf.Width) && 
                        (VidConf.VideoModeList[i].Height == VidConf.Height)) ? 1 : 0;
                    if( igSelectable_Bool(VidConf.VideoModeList[i].Description,IsSelected,0,Size ) ) {
                        SysSetCurrentVideoSettings(i);
                    }
                    if( IsSelected ) {
                        igSetItemDefaultFocus();
                    }
                }
                igEndCombo();
            }
            igSeparator();
        }
        if( igCheckbox("Fullscreen Mode",&VidConfigFullScreen->IValue) ) {
            SysSetCurrentVideoSettings(-1);
        }
    }
    igEnd();
    if( !GUI->SettingsWindowHandle ) {
        GUIToggleHandle(GUI,GUI->SettingsWindowHandle);
    }
#endif
}

void GUIDrawLevelTree(GUI_t *GUI,LevelManager_t *LevelManager,Mission_t *Missions,int NumMissions)
{
    int TreeNodeFlags;
    int i;
    int j;
    int DisableNode;
    int CurrentMission;
    int CurrentLevel;
    
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
                        LevelManagerLoadLevel(LevelManager,GUI,Missions[i].MissionNumber,Missions[i].Levels[j].LevelNumber);
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
void GUIDrawLevelSelectWindow(GUI_t *GUI,LevelManager_t *LevelManager)
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
                GUIDrawLevelTree(GUI,LevelManager,MOHMissionsList,NumMOHMissions);
            } else {
                GUIDrawLevelTree(GUI,LevelManager,MOHUMissionsList,NumMOHUMissions);
            }
        }
    }
    igEnd();
    if( !GUI->LevelSelectWindowHandle ) {
        GUIToggleHandle(GUI,GUI->LevelSelectWindowHandle);
    }
}

void GUIDraw(GUI_t *GUI,LevelManager_t *LevelManager)
{
    ImVec2 ButtonSize;
    ImVec2 ModalPosition;
    ImVec2 Pivot;
    ImGuiIO *IO;
    
    if( !GUI->NumActiveWindows ) {
        GUIBeginFrame();
        GUIDrawHelpOverlay();
        GUIEndFrame();
        return;
    }
    GUIBeginFrame();
    //TODO(Adriano):Handle this as a separate window with his own handle instead of relying on the
    //LevelManager itself...on init it should be open and then closed when done.
//     if( !LevelManager->IsPathSet ) {
    GUIGetMOHPath(GUI,LevelManager);
//     }
    IO = igGetIO();
    ButtonSize.x = 120;
    ButtonSize.y = 0;
    Pivot.x = 0.5f;
    Pivot.y = 0.5f;
    ModalPosition.x = IO->DisplaySize.x * 0.5;
    ModalPosition.y = IO->DisplaySize.y * 0.5;
    igSetNextWindowPos(ModalPosition, ImGuiCond_Always, Pivot);
    if( igBeginPopupModal("Wrong Folder",NULL,ImGuiWindowFlags_AlwaysAutoResize) ) {
        igText("Selected path doesn't seems to contain any game file...\nPlease select a folder containing MOH or MOH:Undergound");
        if (igButton("OK", ButtonSize) ) {
            GUIPopWindow(GUI);
            igCloseCurrentPopup(); 
        }
        igEndPopup();
    }
    GUIDrawDebugWindow(GUI);
    GUIDrawSettingsWindow(GUI);
    GUIDrawLevelSelectWindow(GUI,LevelManager);
//     igShowDemoWindow(NULL);
    GUIEndFrame();
//     if( !GUI->IsActive ) {
//         GUIBeginFrame();
//         GUIDrawHelpOverlay();
//         GUIEndFrame();
//         return;
//     }
//     GUIBeginFrame();
//     GUIDrawDebugWindow(GUI);
//     GUIEndFrame();
}

void GUIContextInit(ImGuiContext *Context,SDL_Window *Window,SDL_GLContext *GLContext,char *ConfigFilePath)
{
    ImGuiIO *IO;
    ImGuiStyle *Style;
    ImFont *Font;
    
    IO = igGetIO();
    igSetCurrentContext(Context);
    ImGui_ImplSDL2_InitForOpenGL(VideoSurface, &GLContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    igStyleColorsDark(NULL);
    //TODO(Adriano):Declare a new config key to select the GUI font...if this function fail then app will use the default one.
    if( GUIFont->Value[0] ) {
        Font = ImFontAtlas_AddFontFromFileTTF(IO->Fonts,GUIFont->Value,floor(GUIFontSize->FValue * VidConf.DPIScale),NULL,NULL);
        if( !Font ) {
            DPrintf("GUIContextInit:Invalid font file...using default\n");
            ConfigSet("GUIFont","");
        }
    }
    Style = igGetStyle();
    ImGuiStyle_ScaleAllSizes(Style,VidConf.DPIScale);
    IO->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    IO->IniFilename = ConfigFilePath;
}
GUI_t *GUIInit(SDL_Window *Window,SDL_GLContext *GLContext)
{
    GUI_t *GUI;

    GUI = malloc(sizeof(GUI_t));
    
    if( !GUI ) {
        DPrintf("GUIInit:Failed to allocate memory for struct\n");
        return NULL;
    }
    
    memset(GUI,0,sizeof(GUI_t));
    GUI->ProgressBar = malloc(sizeof(GUIProgressBar_t));
    asprintf(&GUI->ConfigFilePath,"%simgui.ini",SysGetConfigPath());
    if( !GUI->ProgressBar ) {
        DPrintf("GUIInit:Failed to allocate memory for ProgressBar struct\n");
        free(GUI);
        return NULL;
    }
    
    GUIFont = ConfigGet("GUIFont");
    GUIFontSize = ConfigGet("GUIFontSize");
    
    GUI->DefaultContext = igCreateContext(NULL);
    GUI->ProgressBar->Context = igCreateContext(NULL);
    GUI->ProgressBar->DialogTitle = NULL;
    GUIContextInit(GUI->ProgressBar->Context,Window,GLContext,GUI->ConfigFilePath);
    GUIContextInit(GUI->DefaultContext,Window,GLContext,GUI->ConfigFilePath);
    
//     GUI->DebugWindowHandle = 0;

    GUI->DirSelectFileDialog = IGFD_Create();
    GUI->NumActiveWindows = 0;

    return GUI;
}
