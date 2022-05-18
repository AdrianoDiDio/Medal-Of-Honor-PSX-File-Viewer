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
    VideoMode_t *VideoModeList;
    float   DPIScale;
    int     NumVideoModes;
    int     CurrentVideoMode;
    
    mat4 ModelViewMatrix;
    mat4 PMatrixM4;
    mat4    MVPMatrix;
    
    SDL_Window *Window;
    SDL_GLContext GLContext;
} VideoSystem_t;

extern Config_t *VidConfigWidth;
extern Config_t *VidConfigHeight;
extern Config_t *VidConfigRefreshRate;
extern Config_t *VidConfigFullScreen;

VideoSystem_t   *VideoSystemInit();
void            VideoSystemSetVideoSettings(VideoSystem_t *VideoSystem,int PreferredModeIndex);
void            VideoSystemSwapBuffers(VideoSystem_t *VideoSystem);
void            VideoSystemShutdown(VideoSystem_t *VideoSystem);
#endif//__VIDEO_H_