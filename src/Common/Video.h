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
#ifndef __VIDEO_H_
#define __VIDEO_H_

#include "Common.h"
#include "Config.h"

typedef struct VideoMode_s {
    int Width;
    int Height;
    int RefreshRate;
    int BPP;
    char *Description;
} VideoMode_t;

typedef struct VideoSystem_s {
    char        *WindowTitle;
    VideoMode_t *VideoModeList;
    float   DPIScale;
    int     NumVideoModes;
    int     CurrentVideoMode;
    SDL_Window *Window;
    SDL_GLContext GLContext;
} VideoSystem_t;

extern Config_t *VidConfigWidth;
extern Config_t *VidConfigHeight;
extern Config_t *VidConfigRefreshRate;
extern Config_t *VidConfigFullScreen;
extern Config_t *VidConfigVSync;

VideoSystem_t   *VideoSystemInit(const char *WindowTitle);
void            VideoSystemSetVideoSettings(VideoSystem_t *VideoSystem,int PreferredModeIndex);
void            VideoSystemSetWindowResizable(VideoSystem_t *VideoSystem,int Resizable);
void            VideoSystemGetCurrentWindowSize(VideoSystem_t *VideoSystem,int *Width,int *Height);
int             VideoSystemSetSwapInterval(int Value);
void            VideoSystemCenterMouse(VideoSystem_t *VideoSystem);
void            VideoSystemSwapBuffers(VideoSystem_t *VideoSystem);
void            VideoSystemGrabMouse(bool Grab);
void            VideoSystemShutdown(VideoSystem_t *VideoSystem);
#endif//__VIDEO_H_
