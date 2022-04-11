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

void GUIToggle(GUI_t *GUI)
{
    GUI->IsActive = !GUI->IsActive;
    if( GUI->IsActive ) {
        SysShowCursor();
    } else {
        SysHideCursor();
    }
}
/*
 * Process a new event from the SDL system only when
 * it is active.
 * Returns 0 if GUI has ignored the event 1 otherwise.
 */
int GUIProcessEvent(GUI_t *GUI,SDL_Event *Event)
{
    if( !GUI->IsActive ) {
        return 0;
    }
    ImGui_ImplSDL2_ProcessEvent(Event);
    return 1;
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
    if( igBegin("Debug Settings",(bool *) &GUI->IsActive,0) ) {
//             if( Level->IsPathSet && Level->Loaded ) {
        igText(Level->EngineName);
        igSeparator();
//             }
        igText("Debug Settings");
        igCheckbox("WireFrame Mode",&Level->Settings.WireFrame);
        igCheckbox("Show Level",&Level->Settings.ShowMap);
        igCheckbox("Show Collision Data",&Level->Settings.ShowCollisionData);
        igCheckbox("Show BSP Tree",&Level->Settings.ShowAABBTree);
        igCheckbox("Show BSD nodes as Points",&Level->Settings.ShowBSDNodes);
        igCheckbox("Show BSD RenderObjects as Points",&Level->Settings.ShowBSDRenderObject);
        igCheckbox("Draw BSD RenderObjects",&Level->Settings.DrawBSDRenderObjects);
        igCheckbox("Enable BSD RenderObjects ShowCase Rendering",&Level->Settings.DrawBSDShowCaseRenderObject);
        igCheckbox("Frustum Culling",&Level->Settings.EnableFrustumCulling);
        igCheckbox("Lighting",&Level->Settings.EnableLighting);
        igCheckbox("Semi-Transparency",&Level->Settings.EnableSemiTransparency);
        if( igCheckbox("Animated Lights",&Level->Settings.EnableAnimatedLights) ) {
            if( !Level->Settings.EnableAnimatedLights ) {
                TSPUpdateAnimatedFaces(Level->TSPList,Level->BSD,1);
            }
        }
        igSeparator();
    }
    //If the user has closed it make sure to reset the cursor state.
    if( !GUI->IsActive ) {
        SysHideCursor();
    }
    igEnd();
}
void GUIDraw(GUI_t *GUI)
{
    if( !GUI->IsActive ) {
        return;
    }
    GUIBeginFrame();
    GUIDrawDebugWindow(GUI);
    GUIEndFrame();
}
GUI_t *GUIInit(SDL_Window *Window,SDL_GLContext *GLContext)
{
    GUI_t *GUI;
    ImGuiIO *IO;
    
    GUI = malloc(sizeof(GUI_t));
    
    GUI->Context = igCreateContext(NULL);
    GUI->IsActive = 0;
    IO = igGetIO();
    ImGui_ImplSDL2_InitForOpenGL(VideoSurface, &GLContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    igStyleColorsDark(NULL);
    ImFontAtlas_AddFontFromFileTTF(IO->Fonts,"Fonts/DroidSans.ttf",16.0,NULL,NULL);
    IO->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    return GUI;
}
