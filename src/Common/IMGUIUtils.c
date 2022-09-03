// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
    Medal-Of-Honor-PSX-File-Viewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Medal-Of-Honor-PSX-File-Viewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Medal-Of-Honor-PSX-File-Viewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#include "IMGUIUtils.h"

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

void ProgressBarDestroy(ProgressBar_t *ProgressBar)
{
    if( !ProgressBar ) {
        return;
    }
    GUIReleaseContext(ProgressBar->Context);
    if( ProgressBar->DialogTitle ) {
        free(ProgressBar->DialogTitle);
    }
    free(ProgressBar);
}
void ProgressBarEnd(ProgressBar_t *ProgressBar,VideoSystem_t *VideoSystem)
{
    int Width;
    int Height;
    
    igSetCurrentContext(ProgressBar->OldContext);
    ProgressBar->IsOpen = 0;
    ProgressBar->CurrentPercentage = 0.f;
    //NOTE(Adriano):Make sure to update the current window size.
    VideoSystemGetCurrentWindowSize(VideoSystem,&Width,&Height);
    if( Width != VidConfigWidth->IValue ) {
        ConfigSetNumber("VideoWidth",Width);
    }
    if( Height != VidConfigHeight->IValue ) {
        ConfigSetNumber("VideoHeight",Height);
    }
}
void ProgressBarBegin(ProgressBar_t *ProgressBar,const char *Title)
{
    ProgressBar->OldContext = igGetCurrentContext();
    igSetCurrentContext(ProgressBar->Context);
    ProgressBar->IsOpen = 0;
    ProgressBar->CurrentPercentage = 0.f;
    ProgressBarSetDialogTitle(ProgressBar,Title);
}
void ProgressBarReset(ProgressBar_t *ProgressBar)
{
    if(!ProgressBar) {
        return;
    }
    ProgressBar->CurrentPercentage = 0;
}
void ProgressBarSetDialogTitle(ProgressBar_t *ProgressBar,const char *Title)
{
    if( ProgressBar->DialogTitle ) {
        free(ProgressBar->DialogTitle);
    }
    ProgressBar->DialogTitle = (Title != NULL) ? StringCopy(Title) : "Loading...";
    //NOTE(Adriano):Forces a refresh...since changing the title disrupts the rendering process.
    ProgressBar->IsOpen = 0;
}
/*
    This function can be seen as a complete rendering loop.
    Each time we increment the progress bar, we check for any pending event that the GUI
    can handle and then we clear the display, show the current progress and swap buffers.
 */
void ProgressBarIncrement(ProgressBar_t *ProgressBar,VideoSystem_t *VideoSystem,float Increment,const char *Message)
{
    SDL_Event Event;
    ImGuiViewport *Viewport;
    ImVec2 ScreenCenter;
    ImVec2 Pivot;
    ImVec2 Size;
    
    if( !ProgressBar ) {
        return;
    }
    
   SDL_PumpEvents();
    //NOTE(Adriano):
    //Process any window event that could be generated while showing the progress bar.
    //We need to make sure not to process any mouse/keyboad event otherwise when the progress bar ends the
    //queue may be empty and the GUI won't respond to events properly...
    while( SDL_PeepEvents(&Event, 1, SDL_GETEVENT, SDL_WINDOWEVENT, SDL_WINDOWEVENT) > 0 ) {
        ImGui_ImplSDL2_ProcessEvent(&Event);
    }
    
    //NOTE(Adriano):Since we are checking for events these function have now an updated view of the current window size.
    Viewport = igGetMainViewport();
    
    ImGuiViewport_GetCenter(&ScreenCenter,Viewport);

    Pivot.x = 0.5f;
    Pivot.y = 0.5f;

    glClear(GL_COLOR_BUFFER_BIT );
    

    GUIBeginFrame();
    
    if( !ProgressBar->IsOpen ) {
        igOpenPopup_Str(ProgressBar->DialogTitle,0);
        ProgressBar->IsOpen = 1;
    }
    
    Size.x = 0.f;
    Size.y = 0.f;
    igSetNextWindowPos(ScreenCenter, ImGuiCond_Always, Pivot);
    ProgressBar->CurrentPercentage += Increment;
    if (igBeginPopupModal(ProgressBar->DialogTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        igProgressBar((ProgressBar->CurrentPercentage / 100.f),Size,Message);
        igEnd();
    }
    GUIEndFrame();
    VideoSystemSwapBuffers(VideoSystem);
}

ProgressBar_t *ProgressBarInitialize(VideoSystem_t *VideoSystem)
{
    ProgressBar_t *ProgressBar;
    
    ProgressBar = malloc(sizeof(ProgressBar_t));
    if( !ProgressBar ) {
        return NULL;
    }
    ProgressBar->Context = igCreateContext(NULL);
    ProgressBar->DialogTitle = NULL;
    GUIContextInit(ProgressBar->Context,VideoSystem,NULL);
    return ProgressBar;
}

void GUILoadCommonSettings()
{
    GUIFont = ConfigGet("GUIFont");
    GUIFontSize = ConfigGet("GUIFontSize");
    GUIShowFPS = ConfigGet("GUIShowFPS");
}
