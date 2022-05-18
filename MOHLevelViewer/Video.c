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
#include "Video.h"
#include "MOHLevelViewer.h"

Config_t *VidConfigWidth;
Config_t *VidConfigHeight;
Config_t *VidConfigRefreshRate;
Config_t *VidConfigFullScreen;

void VideoSystemShutdown(VideoSystem_t *VideoSystem)
{
    int i;
    for( i = 0; i < VideoSystem->NumVideoModes; i++ ) {
        free(VideoSystem->VideoModeList[i].Description);
    }
    free(VideoSystem->VideoModeList);
    SDL_GL_DeleteContext(VideoSystem->GLContext);
    SDL_DestroyWindow(VideoSystem->Window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Quit();
}
SDL_DisplayMode *SDLGetCurrentDisplayModeV2(VideoSystem_t *VideoSystem)
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
    ConfigSetNumber("VideoWidth",VideoSystem->VideoModeList[BestMode].Width);
    ConfigSetNumber("VideoHeight",VideoSystem->VideoModeList[BestMode].Height);
    ConfigSetNumber("VideoRefreshRate",VideoSystem->VideoModeList[BestMode].RefreshRate);
    VideoSystem->CurrentVideoMode = BestMode;

}

void VideoSystemSetVideoSettings(VideoSystem_t *VideoSystem,int PreferredModeIndex)
{
    if( SDL_GetWindowFlags(VideoSystem->Window) & SDL_WINDOW_FULLSCREEN ) {
        //Was fullscreen reset it...
        SDL_SetWindowFullscreen(VideoSystem->Window,0);
    }
    SDL_SetWindowSize(VideoSystem->Window,VidConfigWidth->IValue,VidConfigHeight->IValue);
    DPrintf("Going fullscreen:%i\n",VidConfigFullScreen->IValue);
    if( VidConfigFullScreen->IValue ) {
        VideoSystemSetFullScreenVideoMode(VideoSystem,PreferredModeIndex);
        if( SDL_SetWindowDisplayMode(VideoSystem->Window,SDLGetCurrentDisplayModeV2(VideoSystem) ) < 0 ) {
            ConfigSetNumber("VideoFullScreen",0);
            VideoSystemSetVideoSettings(VideoSystem,PreferredModeIndex);
            return;
        }
        SDL_SetWindowFullscreen(VideoSystem->Window,SDL_WINDOW_FULLSCREEN);
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
void VideoSystemOpenWindow(VideoSystem_t *VideoSystem)
{
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
    VideoSystem->Window = SDL_CreateWindow("MOH Level Viewer",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                     VidConfigWidth->IValue, VidConfigHeight->IValue, 
                     SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    
    VideoSystemSetVideoSettings(VideoSystem,-1);
    VideoSystem->GLContext = SDL_GL_CreateContext(VideoSystem->Window);
    VideoSystem->DPIScale = 1.f;
    if( !SDL_GetDisplayDPI(0, NULL, &VideoSystem->DPIScale, NULL) ) {
        VideoSystem->DPIScale /= 96.f;
    }        
    SDL_GL_SetSwapInterval(1);
}

bool VideoSystemInitSDL()
{
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ) {
        return false;
    }
    return true;
}


VideoSystem_t *VideoSystemInit()
{
    VideoSystem_t *VideoSystem;
    int GlewError;
    
    if( !VideoSystemInitSDL() ) {
        printf("VideoSystemInit:Failed to initialize SDL subsystems\n");
        return NULL;
    }
    
    VideoSystem = malloc(sizeof(VideoSystem_t));
    if( !VideoSystem ) {
        printf("VideoSystemInit:Failed to allocate memory for VideoSystem struct\n");
        return NULL;
    }
    
    VideoSystemGetAvailableVideoModes(VideoSystem);
    VideoSystemOpenWindow(VideoSystem);
    glewExperimental = GL_TRUE;
    GlewError = glewInit();
    if (GlewError != GLEW_OK) {
        DPrintf( "Failed to init GLEW\n");
        return NULL;
    }
    SysHideCursor();
    return VideoSystem;
}