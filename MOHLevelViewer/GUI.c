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
#include "Level.h"
#include "LevelManager.h"

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
       igSeparator();
       igText(ComTime->FPSString);
       igText("OpenGL Version: %s",glGetString(GL_VERSION));
       SDL_GetVersion(&Version);
       igText("SDL Version: %u.%u.%u",Version.major,Version.minor,Version.patch);
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
    GUI->ProgressBar->CurrentPercentage = 0;
    GUISetProgressBarDialogTitle(GUI,Title);
}
void GUIProgressBarReset(GUI_t *GUI)
{
    GUI->ProgressBar->CurrentPercentage = 0;
}
void GUIProgressBarIncrement(GUI_t *GUI,int Increment,char *Message)
{
    ImGuiViewport *Viewport;
    ImVec2 ScreenCenter;
    ImVec2 Pivot;
    ImVec2 Size;

    
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
    if (igBeginPopupModal(GUI->ProgressBar->DialogTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        igProgressBar((GUI->ProgressBar->CurrentPercentage / 100.f) * ComTime->Delta/* * IO->DeltaTime*/,Size,Message);
        igEnd();
    }
    GUIEndFrame();
    SysSwapBuffers();
}

void GUIProgressBarEnd(GUI_t *GUI)
{
    igSetCurrentContext(GUI->DefaultContext);
    GUI->ProgressBar->IsOpen = 0;
    GUI->ProgressBar->CurrentPercentage = 0;
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
                GUIProgressBarBegin(GUI,"Loading Mission 1 Level 1");
                LoadStatus = LevelManagerInitWithPath(LevelManager,GUI,DirectoryPath);
                GUIProgressBarEnd(GUI);
                if( !LoadStatus ) {
                    igOpenPopup_Str("Wrong Folder",0);
                    GUIPushWindow(GUI);
                } else {
                    //Close it if we managed to load it.
                    GUIPopWindow(GUI);
                    IGFD_CloseDialog(GUI->DirSelectFileDialog);
                }
                if (DirectoryPath) { 
                    free(DirectoryPath);
                }
        }
    } else {
        IGFD_OpenDialog2(GUI->DirSelectFileDialog,"Dir Select","Select dir",NULL,".",1,NULL,ImGuiFileDialogFlags_DontShowHiddenFiles);
    }
}
void GUIDrawSettingsWindow(GUI_t *GUI)
{
#if 1
//     if( !LevelManager->CurrentLevel ) {
//         return;
//     }
    if( !GUI->SettingsWindowHandle ) {
        return;
    }
    if( igBegin("Settings",&GUI->SettingsWindowHandle,0) ) {
        igText("Video Settings");
        igSeparator();
        
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
    char *Buffer;
    
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
                        asprintf(&Buffer,"Loading Mission %i Level %i...",Missions[i].MissionNumber,Missions[i].Levels[j].LevelNumber);
                        GUIProgressBarBegin(GUI,Buffer);
                        LevelManagerLoadLevel(LevelManager,GUI,Missions[i].MissionNumber,Missions[i].Levels[j].LevelNumber);
                        GUIProgressBarEnd(GUI);
                        free(Buffer);

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
        GUIDrawHelpOverlay(GUI);
        GUIEndFrame();
        return;
    }
    GUIBeginFrame();
    if( !LevelManager->IsPathSet ) {
        GUIGetMOHPath(GUI,LevelManager);
    }
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

void GUIContextInit(ImGuiContext *Context,SDL_Window *Window,SDL_GLContext *GLContext)
{
    ImGuiIO *IO;
    ImGuiStyle *Style;

    IO = igGetIO();
    igSetCurrentContext(Context);
    ImGui_ImplSDL2_InitForOpenGL(VideoSurface, &GLContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    igStyleColorsDark(NULL);
    ImFontAtlas_AddFontFromFileTTF(IO->Fonts,"Fonts/DroidSans.ttf",floor(14.f * VidConf.DPIScale),NULL,NULL);
    Style = igGetStyle();
    ImGuiStyle_ScaleAllSizes(Style,VidConf.DPIScale);
    IO->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
}
GUI_t *GUIInit(SDL_Window *Window,SDL_GLContext *GLContext)
{
    GUI_t *GUI;

    GUI = malloc(sizeof(GUI_t));
    memset(GUI,0,sizeof(GUI_t));
    GUI->ProgressBar = malloc(sizeof(GUIProgressBar_t));
    GUI->DefaultContext = igCreateContext(NULL);
    GUI->ProgressBar->Context = igCreateContext(NULL);
    GUI->ProgressBar->DialogTitle = NULL;
    GUIContextInit(GUI->ProgressBar->Context,Window,GLContext);
    GUIContextInit(GUI->DefaultContext,Window,GLContext);
    
//     GUI->DebugWindowHandle = 0;

    GUI->DirSelectFileDialog = IGFD_Create();
    GUI->NumActiveWindows = 1;

    return GUI;
}
