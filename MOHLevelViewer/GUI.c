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

void GUIDrawDebugWindow(GUI_t *GUI,LevelManager_t *LevelManager)
{
    ImVec2 ButtonSize;
    SDL_version Version;
    
    if( !GUI->DebugWindowHandle ) {
        return;
    }
    if( igBegin("Debug Settings",&GUI->DebugWindowHandle,ImGuiWindowFlags_AlwaysAutoResize) ) {
        if( LevelManagerIsLevelLoaded(LevelManager) ) {
            igText(LevelManager->EngineName);
            igText(LevelManager->BasePath);
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
        igText("Export the current level and objects");
        ButtonSize.x = 0.f;
        ButtonSize.y = 0.f;
        if( igButton("Export to OBJ",ButtonSize) ) {
            LevelManagerExport(LevelManager,GUI,LEVEL_MANAGER_EXPORT_TYPE_OBJ);
        }
        igSameLine(0.f,10.f);
        if( igButton("Export to Ply",ButtonSize) ) {
            LevelManagerExport(LevelManager,GUI,LEVEL_MANAGER_EXPORT_TYPE_PLY);
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
    igEnd();
}

void GUIDrawHelpOverlay()
{
    int WindowFlags;
    ImGuiViewport *Viewport;
    ImVec2 WorkPosition;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    
    WindowFlags = /*ImGuiWindowFlags_NoDecoration |*/ ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | 
                    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    Viewport = igGetMainViewport();
    WorkPosition = Viewport->WorkPos;
    WindowPosition.x = (WorkPosition.x + 10.f);
    WindowPosition.y = (WorkPosition.y + 10.f);
    WindowPivot.x = 0.f;
    WindowPivot.y = 0.f;
    igSetNextWindowPos(WindowPosition, ImGuiCond_Once, WindowPivot);

    if( igBegin("Help", NULL, WindowFlags) ) {
        igText("Press F1 to enable/disable debug settings");
        igText("Press F2 to open video settings");
        igText("Press F3 to open the level selection window");
        igText("Press F4 to change the game path");
        igText("Press Escape to exit the program");
    }
    igEnd();
}

void GUIDrawFPSOverlay()
{
    ImGuiViewport *Viewport;
    ImVec2 WorkPosition;
    ImVec2 WorkSize;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    int WindowFlags;
    
    WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | 
                    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    Viewport = igGetMainViewport();
    WorkPosition = Viewport->WorkPos;
    WorkSize = Viewport->WorkSize;
    WindowPosition.x = (WorkPosition.x + WorkSize.x - 10.f);
    WindowPosition.y = (WorkPosition.y + 10.f);
    WindowPivot.x = 1.f;
    WindowPivot.y = 0.f;
    igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);

    if( igBegin("FPS", NULL, WindowFlags) ) {
        igText(ComTime->FPSString);
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
    float Delta;
    
    if( !GUI ) {
        return;
    }
        
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
        if( igCheckbox("Fullscreen Mode",(bool *) &VidConfigFullScreen->IValue) ) {
            DPrintf("VidConfigFullScreen:%i\n",VidConfigFullScreen->IValue);
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

void GUISetErrorMessage(GUI_t *GUI,char *Message)
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
                if( FileDialog->OnDirSelected ) {
                    FileDialog->OnDirSelected(FileDialog,GUI,DirectoryPath,FileName,UserData);
                }
                if( DirectoryPath ) {
                    free(DirectoryPath);
                }
                if( FileName ) {
                    free(FileName);
                }
        } else {
            if( FileDialog->OnDirSelectionCancelled ) {
                FileDialog->OnDirSelectionCancelled(FileDialog,GUI);
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
void GUIDraw(GUI_t *GUI,LevelManager_t *LevelManager)
{
    ImVec2 ButtonSize;
    ImVec2 ModalPosition;
    ImVec2 Pivot;
    ImGuiIO *IO;
    
    GUIBeginFrame();
    
    if( GUIShowFPS->IValue ) {
        GUIDrawFPSOverlay();
    }
    
    if( !GUI->NumActiveWindows ) {
        GUIDrawHelpOverlay();
        GUIEndFrame();
        return;
    }
    
    GUIRenderFileDialogs(GUI);
    if( GUI->ErrorMessage ) {
        IO = igGetIO();
        ButtonSize.x = 120;
        ButtonSize.y = 0;
        Pivot.x = 0.5f;
        Pivot.y = 0.5f;
        ModalPosition.x = IO->DisplaySize.x * 0.5;
        ModalPosition.y = IO->DisplaySize.y * 0.5;
        igSetNextWindowPos(ModalPosition, ImGuiCond_Always, Pivot);
        if( igBeginPopupModal("Error",NULL,ImGuiWindowFlags_AlwaysAutoResize) ) {
            igText(GUI->ErrorMessage);
            if (igButton("OK", ButtonSize) ) {
                GUIPopWindow(GUI);
                igCloseCurrentPopup(); 
            }
            igEndPopup();
        }
    }
    GUIDrawDebugWindow(GUI,LevelManager);
    GUIDrawSettingsWindow(GUI);
    GUIDrawLevelSelectWindow(GUI,LevelManager);
//     igShowDemoWindow(NULL);
    GUIEndFrame();
}

int GUIFileDialogIsOpen(GUIFileDialog_t *Dialog)
{
    if( !Dialog ) {
        DPrintf("GUIFileDialogIsOpen:Invalid dialog data\n");
        return 0;
    }
    
    return IGFD_IsOpened(Dialog->Window);
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
 OnDirSelected and OnDirSelectionCancelled are two callback that can be set to NULL if we are not interested in the result.
 NOTE that setting them to NULL or the cancel callback to NULL doesn't close the dialog.
 */
GUIFileDialog_t *GUIFileDialogRegister(GUI_t *GUI,char *WindowTitle,char *Filters,DirSelectedCallback_t OnDirSelected,
                                       DirSelectionCancelledCallback_t OnDirSelectionCancelled)
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
    FileDialog->OnDirSelected = OnDirSelected;
    FileDialog->OnDirSelectionCancelled = OnDirSelectionCancelled;
    FileDialog->Next = GUI->FileDialogList;
    GUI->FileDialogList = FileDialog;
    GUI->NumRegisteredFileDialog++;
    
    return FileDialog;
}
void GUIFileDialogSetTitle(GUIFileDialog_t *FileDialog,char *Title)
{
    if(!FileDialog) {
        DPrintf("GUIFileDialogSetCallbacks:Invalid dialog\n");
        return;
    }
    if( !Title ) {
        DPrintf("GUIFileDialogSetCallbacks:Invalid title\n");
        return;
    }
    if( FileDialog->WindowTitle ) {
        free(FileDialog->WindowTitle);
    }
    FileDialog->WindowTitle = StringCopy(Title);
}
/*

*/
void GUIFileDialogSetCallbacks(GUIFileDialog_t *FileDialog,DirSelectedCallback_t OnDirSelected,DirSelectionCancelledCallback_t OnDirSelectionCancelled)
{
    if(!FileDialog) {
        DPrintf("GUIFileDialogSetCallbacks:Invalid dialog\n");
        return;
    }
    FileDialog->OnDirSelected = OnDirSelected;
    FileDialog->OnDirSelectionCancelled = OnDirSelectionCancelled;
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
    if( GUIFont->Value[0] ) {
        Font = ImFontAtlas_AddFontFromFileTTF(IO->Fonts,GUIFont->Value,floor(GUIFontSize->FValue * VidConf.DPIScale),NULL,NULL);
        if( !Font ) {
            DPrintf("GUIContextInit:Invalid font file...using default\n");
            ConfigSet("GUIFont","");
        }
    }
    Style = igGetStyle();
    Style->WindowTitleAlign.x = 0.5f;
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
    GUI->ErrorMessage = NULL;
    asprintf(&GUI->ConfigFilePath,"%simgui.ini",SysGetConfigPath());
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
    GUIContextInit(GUI->ProgressBar->Context,Window,GLContext,GUI->ConfigFilePath);
    GUIContextInit(GUI->DefaultContext,Window,GLContext,GUI->ConfigFilePath);
    
    GUI->NumActiveWindows = 0;

    return GUI;
}
