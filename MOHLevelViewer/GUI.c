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

#include "MOHLevelViewer.h"

void GUIFree(GUI_t *GUI)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    igDestroyContext(GUI->Context);
    //IGFD_Destroy(Dialog)
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
    char Buffer[256];
#if 1
//     if( !LevelManager->CurrentLevel ) {
//         return;
//     }
    if( !GUI->DebugWindowHandle ) {
        return;
    }
    
    if( igBegin("Debug Settings",(bool *) &GUI->DebugWindowHandle,0) ) {
        if( LevelManager->CurrentLevel ) {
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
       sprintf(Buffer,"NumActiveWindows:%i\n",GUI->NumActiveWindows);
       igText(Buffer);
       igText(ComTime->FPSString);
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
int OvOpen = 1;
void GUIDrawHelpOverlay()
{
    int WindowFlags;
    ImGuiViewport *Viewport;
    ImVec2 WorkPosition;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    
    WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    Viewport = igGetMainViewport();
    WorkPosition = Viewport->WorkPos;
    WindowPosition.x = (WorkPosition.x + 10.f);
    WindowPosition.y = (WorkPosition.y + 10.f);
    WindowPivot.x = 0.f;
    WindowPivot.y = 0.f;
    igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);

    if( igBegin("Help", NULL, WindowFlags) ) {
        igText("Press F1 to enable/disable debug settings");
    }
    igEnd();
}



void GUIGetMOHPath(GUI_t *GUI,LevelManager_t *LevelManager)
{
    ImGuiIO* IO;
    ImVec2 MaxSize;
    ImVec2 MinSize;
    ImGuiViewport *Viewport;
    ImVec2 WorkPosition;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    char *DirectoryPath;
    
    Viewport = igGetMainViewport();
    WorkPosition = Viewport->WorkPos;
    WindowPosition.x = 0;
    WindowPosition.y = 0;
    WindowPivot.x = 0.f;
    WindowPivot.y = 0.f;
    
    IO = igGetIO();
    MaxSize.x = Viewport->Size.x;
    MaxSize.y = Viewport->Size.y;
    MinSize.x = MaxSize.x*0.25f;
    MinSize.y = MaxSize.y*0.25f;
    
//     if( !GUI->DirSelectFileDialog ) {
//         GUI->DirSelectFileDialog = IGFD_Create();
//         GUIPushWindow(GUI);
//     }
    igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);
    if (IGFD_DisplayDialog(GUI->DirSelectFileDialog, "Dir Select", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize, MinSize, MaxSize)) {
//         GUIPushWindow(GUI);
        if (IGFD_IsOk(GUI->DirSelectFileDialog)) {
                DirectoryPath = IGFD_GetFilePathName(GUI->DirSelectFileDialog);
                DPrintf("Selected directory %s\n",DirectoryPath);
                if( !LevelManagerSetPath(LevelManager,DirectoryPath) ) {
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
    if( igBegin("Settings",(bool *) &GUI->SettingsWindowHandle,0) ) {
        igText("Video Settings");
        igSeparator();
    }
    igEnd();
    if( !GUI->SettingsWindowHandle ) {
        GUIToggleHandle(GUI,GUI->SettingsWindowHandle);
    }
#endif
}

void GUIDraw(GUI_t *GUI,LevelManager_t *LevelManager)
{
    ImVec2 ButtonSize;
    ImVec2 ModalPosition;
    ImVec2 Pivot;
    ImGuiIO *IO;
    
    if( !GUI->NumActiveWindows ) {
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
    GUIEndFrame();
//     if( !GUI->IsActive ) {
//         GUIBeginFrame();
//         GUIDrawHelpOverlay();
//         GUIEndFrame();
//         return;
//     }
//     GUIBeginFrame();
//     GUIDrawDebugWindow(GUI);igShowDemoWindow(NULL);
//     GUIEndFrame();
}
GUI_t *GUIInit(SDL_Window *Window,SDL_GLContext *GLContext)
{
    GUI_t *GUI;
    ImGuiIO *IO;
    ImGuiStyle *Style;
    GUI = malloc(sizeof(GUI_t));
    memset(GUI,0,sizeof(GUI_t));
    GUI->Context = igCreateContext(NULL);
//     GUI->DebugWindowHandle = 0;
    IO = igGetIO();
    ImGui_ImplSDL2_InitForOpenGL(VideoSurface, &GLContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    igStyleColorsDark(NULL);
    ImFontAtlas_AddFontFromFileTTF(IO->Fonts,"Fonts/DroidSans.ttf",floor(16.f * VidConf.DPIScale),NULL,NULL);
    Style = igGetStyle();
    ImGuiStyle_ScaleAllSizes(Style,VidConf.DPIScale);
    IO->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    GUI->DirSelectFileDialog = IGFD_Create();
    GUI->NumActiveWindows = 1;
    return GUI;
}
