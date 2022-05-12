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
#ifndef __COMMON_H_
#define __COMMON_H_ 


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>

#include <png.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <cglm/cglm.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
// #define IMGUI_USER_CONFIG "IMGUISettings.h"
#include "cimgui.h"
#include "cimgui_impl.h"
#include "ImGuiFileDialog.h"

#include "Types.h" 

#ifdef __linux__
#define PATH_SEPARATOR '/'
#else
#define PATH_SEPARATOR '\\'
#endif

#ifdef __GNUC__
#define Attribute(x) __attribute__(x)
#else
#define Attribute(x)
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#ifndef PI_OVER_360
#define PI_OVER_360 0.0087266462599716478846184538424431
#endif

#define DEGTORAD( x ) ( (x * M_PI) / 180.0)
#define RADTODEG( x ) ( (x * 180.0) / M_PI)

#define Square( x ) ( ( x ) * ( x ) )

#define GetProcAddr(Name) SDL_GL_GetProcAddress(Name);

#ifndef MAX
#define MAX( x, y ) ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )
#define MIN( x, y ) ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#endif
#endif//__COMMON_H_
