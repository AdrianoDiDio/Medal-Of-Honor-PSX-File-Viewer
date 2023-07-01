// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2023 Adriano Di Dio.
    
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
#include "Video.h"

Config_t *VidConfigWidth;
Config_t *VidConfigHeight;
Config_t *VidConfigRefreshRate;
Config_t *VidConfigFullScreen;
Config_t *VidConfigVSync;

void VideoSystemShutdown(VideoSystem_t *VideoSystem)
{
    int i;
    for( i = 0; i < VideoSystem->NumVideoModes; i++ ) {
        free(VideoSystem->VideoModeList[i].Description);
    }
    free(VideoSystem->WindowTitle);
    free(VideoSystem->VideoModeList);
    if( VideoSystem->GLContext ) {
        SDL_GL_DeleteContext(VideoSystem->GLContext);
    }
    if( VideoSystem->Window) {
        SDL_DestroyWindow(VideoSystem->Window);
    }
    free(VideoSystem);
}

void VideoSystemSetWindowResizable(VideoSystem_t *VideoSystem,int Resizable)
{
    SDL_SetWindowResizable(VideoSystem->Window,Resizable);
}
void VideoSystemCenterMouse(VideoSystem_t *VideoSystem)
{
    SDL_WarpMouseInWindow(VideoSystem->Window,VidConfigWidth->IValue/2,VidConfigHeight->IValue/2);
}

void VideoSystemGetCurrentWindowSize(VideoSystem_t *VideoSystem,int *Width,int *Height)
{
    SDL_GetWindowSize(VideoSystem->Window,Width,Height);
}

void VideoSystemGrabMouse(bool Grab)
{
    SDL_SetRelativeMouseMode(Grab);
}
/*
 * Retrieves the selected video mode from the SDL mode list using the CurrentVideoMode field inside the VideoSystem_t struct 
 * that was set manually or by the VideoSystemSetFullScreenVideoMode function.
 */
SDL_DisplayMode *SDLGetCurrentDisplayMode(VideoSystem_t *VideoSystem)
{
    static SDL_DisplayMode Result;
    VideoMode_t *CurrentMode;
    int NumModes;
    int i;
    
    NumModes = SDL_GetNumDisplayModes(0);
    CurrentMode = &VideoSystem->VideoModeList[VideoSystem->CurrentVideoMode];
    for( i = 0; i < NumModes; i++ ) {
        SDL_GetDisplayMode(0,i,&Result);
        if( Result.w == CurrentMode->Width && Result.h == CurrentMode->Height &&
            SDL_BITSPERPIXEL(Result.format) == CurrentMode->BPP && Result.refresh_rate == CurrentMode->RefreshRate ) {
                return &Result;
            }
    }
    return NULL;
}

/*
 Given a target resolution from the settings, if we are going to init the windows as fullscreen
 we need to pick an exact match in the SDL video mode list.
 If PreferredModeIndex is equals to -1 then it will use the config values otherwise the one
 from the selected video mode in the mode array.
 This function will set the CurrentVideoMode field in the VideoSystem_t struct.
 */
void VideoSystemSetFullScreenVideoMode(VideoSystem_t *VideoSystem,int PreferredModeIndex)
{
    int i;
    int ClosestDistance;
    int Delta;
    int DistanceX;
    int DistanceY;
    int TargetWidth;
    int TargetHeight;
    int TargetRefreshRate;
    int BestMode;
    
    ClosestDistance = 9999;
    BestMode = -1;
    
    if( PreferredModeIndex != -1 ) {
        TargetWidth = VideoSystem->VideoModeList[PreferredModeIndex].Width;
        TargetHeight = VideoSystem->VideoModeList[PreferredModeIndex].Height;
        TargetRefreshRate = VideoSystem->VideoModeList[PreferredModeIndex].RefreshRate;
    } else {
        TargetWidth = VidConfigWidth->IValue;
        TargetHeight = VidConfigHeight->IValue;
        TargetRefreshRate = VidConfigRefreshRate->IValue;
    }
    DPrintf("VideoSystemSetFullScreenVideoMode:Users wants to go with %ix%i Fullscreen\n",VidConfigWidth->IValue,VidConfigHeight->IValue);
    for( i = 0; i < VideoSystem->NumVideoModes; i++ ) {
        //Ideally we want to match the one in the config!
        if( VideoSystem->VideoModeList[i].RefreshRate != TargetRefreshRate ) {
            continue;
        }
        DistanceX = TargetWidth - VideoSystem->VideoModeList[i].Width;
        DistanceY = TargetHeight - VideoSystem->VideoModeList[i].Height;

        Delta = Square(DistanceX) + Square(DistanceY);
        
        if( Delta < ClosestDistance ) {
            ClosestDistance = Delta;
            BestMode = i;
        }
    }
    if( BestMode == -1 ) {
        //User did not set any preference...pick the highest one from the list.
        BestMode = 0;
    }
    DPrintf("VideoSystemSetFullScreenVideoMode:Users obtained %ix%i RefreshRate:%i\n",VideoSystem->VideoModeList[BestMode].Width,
            VideoSystem->VideoModeList[BestMode].Height,VideoSystem->VideoModeList[BestMode].RefreshRate);
    VideoSystem->CurrentVideoMode = BestMode;

}

void VideoSystemSetVideoSettings(VideoSystem_t *VideoSystem,int PreferredModeIndex)
{
    SDL_DisplayMode *SelectedMode;
    if( SDL_GetWindowFlags(VideoSystem->Window) & SDL_WINDOW_FULLSCREEN ) {
        //Was fullscreen reset it...
        SDL_SetWindowFullscreen(VideoSystem->Window,0);
    }
    SDL_SetWindowSize(VideoSystem->Window,VidConfigWidth->IValue,VidConfigHeight->IValue);
    if( VidConfigFullScreen->IValue ) {
        VideoSystemSetFullScreenVideoMode(VideoSystem,PreferredModeIndex);
        SelectedMode = SDLGetCurrentDisplayMode(VideoSystem);
        if( SDL_SetWindowDisplayMode(VideoSystem->Window, SelectedMode) < 0 ) {
            ConfigSetNumber("VideoFullScreen",0);
            return;
        }
        SDL_SetWindowFullscreen(VideoSystem->Window,SDL_WINDOW_FULLSCREEN);
        ConfigSetNumber("VideoWidth",SelectedMode->w);
        ConfigSetNumber("VideoHeight",SelectedMode->h);
        ConfigSetNumber("VideoRefreshRate",SelectedMode->refresh_rate);
    }
    //Update the value and save changes on the file.
    ConfigSetNumber("VideoFullScreen",VidConfigFullScreen->IValue);

}
void VideoSystemGetAvailableVideoModes(VideoSystem_t *VideoSystem)
{
    int NumAvailableVideoModes;
    SDL_DisplayMode Mode;
    int i;
    
    if( !VideoSystem ) {
        DPrintf("VideoSystemGetAvailableVideoModes:Called without valid VideoSystem data\n");
        return;
    }
    
    //NOTE(Adriano):We are forcing this to display 0.
    NumAvailableVideoModes = SDL_GetNumDisplayModes(0);
    VideoSystem->VideoModeList = malloc(NumAvailableVideoModes * sizeof(VideoMode_t));
    if( !VideoSystem->VideoModeList ) {
        DPrintf("VideoSystemGetAvailableVideoModes:Unable to allocate memory for video mode list.\n");
        return;
    }
    //Pickup the maximum supported resolution as the default one.
    VideoSystem->CurrentVideoMode = 0;
    for( i = 0; i < NumAvailableVideoModes; i++ ) {
        SDL_GetDisplayMode(0,i,&Mode);
        VideoSystem->VideoModeList[i].Width = Mode.w;
        VideoSystem->VideoModeList[i].Height = Mode.h;
        VideoSystem->VideoModeList[i].RefreshRate = Mode.refresh_rate;
        VideoSystem->VideoModeList[i].BPP = SDL_BITSPERPIXEL(Mode.format);
        asprintf(&VideoSystem->VideoModeList[i].Description, "%ix%i@%iHz",VideoSystem->VideoModeList[i].Width,VideoSystem->VideoModeList[i].Height,
                 VideoSystem->VideoModeList[i].RefreshRate);
    }
    VideoSystem->NumVideoModes = NumAvailableVideoModes;
}

void VideoSystemSwapBuffers(VideoSystem_t *VideoSystem)
{
    SDL_GL_SwapWindow(VideoSystem->Window);
}

int VideoSystemSetSwapInterval(int Value)
{
    if( Value < -1 || Value > 1 ) {
        Value = 1;
    }
    ConfigSetNumber("VideoVSync",Value);
    return SDL_GL_SetSwapInterval(VidConfigVSync->IValue);
}
int VideoSystemOpenWindow(VideoSystem_t *VideoSystem)
{
    int Result;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef _DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    VideoSystem->Window = SDL_CreateWindow(VideoSystem->WindowTitle,SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                     VidConfigWidth->IValue, VidConfigHeight->IValue, 
                     SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    
    VideoSystemSetVideoSettings(VideoSystem,-1);
    VideoSystem->GLContext = SDL_GL_CreateContext(VideoSystem->Window);
    VideoSystem->DPIScale = 1.f;
    if( !SDL_GetDisplayDPI(0, NULL, &VideoSystem->DPIScale, NULL) ) {
        VideoSystem->DPIScale /= 96.f;
    }
    if( VidConfigVSync->IValue < -1 || VidConfigVSync->IValue > 1 ) {
        ConfigSetNumber("VideoVSync",1);
    }
    Result = VideoSystemSetSwapInterval(VidConfigVSync->IValue);
    if( Result == -1 ) {
        DPrintf("VideoSystemOpenWindow:Failed to set VSync using value %i...trying default one.\n",VidConfigVSync->IValue);
        Result = VideoSystemSetSwapInterval(1);
        if( Result == -1 ) {
            printf("VideoSystemOpenWindow:Cannot set vsync...\n");
            return 0;
        }
    }
    return 1;
}

void VideoSystemLoadConfigs()
{        
    VidConfigWidth = ConfigGet("VideoWidth");
    VidConfigHeight = ConfigGet("VideoHeight");
    VidConfigRefreshRate = ConfigGet("VideoRefreshRate");
    VidConfigFullScreen = ConfigGet("VideoFullScreen");
    VidConfigVSync = ConfigGet("VideoVSync");
}

VideoSystem_t *VideoSystemInit(const char *WindowTitle)
{
    VideoSystem_t *VideoSystem;
    int GlewError;
        
    VideoSystem = malloc(sizeof(VideoSystem_t));
    if( !VideoSystem ) {
        printf("VideoSystemInit:Failed to allocate memory for VideoSystem struct\n");
        return NULL;
    }
    VideoSystem->WindowTitle = NULL;
    VideoSystem->VideoModeList = NULL;
    VideoSystem->GLContext = NULL;
    VideoSystem->Window = NULL;
    if( WindowTitle ) {
        VideoSystem->WindowTitle = StringCopy(WindowTitle);
    } else {
        asprintf(&VideoSystem->WindowTitle,"Unknown Application");
    }
    VideoSystemLoadConfigs();
    VideoSystemGetAvailableVideoModes(VideoSystem);
    if( !VideoSystemOpenWindow(VideoSystem) ) {
        printf("VideoSystemInit:Failed to open window\n");
        goto Error;
    }
    glewExperimental = GL_TRUE;
    GlewError = glewInit();
    if (GlewError != GLEW_OK) {
        DPrintf("VideoSystemInit:Failed to init GLEW\n");
        goto Error;
    }
    SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
    return VideoSystem;
Error:
    VideoSystemShutdown(VideoSystem);
}
