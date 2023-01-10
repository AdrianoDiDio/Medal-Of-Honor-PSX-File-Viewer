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

#ifndef __ENGINE_H_
#define __ENGINE_H_

#include "Video.h"
#include "Sound.h"

typedef struct ComTimeInfo_s {
    int	  FPS;
    float Delta;
    //Frame to frame time
    //updated each second.
    float LastFPSTime;
    float LastLoopTime;
    float OptimalTime;
    float UpdateLength;
    char  FPSString[256];
    char  FPSSimpleString[256];
} ComTimeInfo_t;

typedef struct Engine_s {
   ComTimeInfo_t            *TimeInfo;
   VideoSystem_t            *VideoSystem;
   SoundSystem_t            *SoundSystem;
   const Byte               *KeyState;
} Engine_t;


Engine_t    *EngineInit(const char *WindowTitle);
void        EngineBeginFrame(Engine_t *Engine);
void        EngineEndFrame(Engine_t *Engine);
void        EngineShutDown(Engine_t *Engine);


#endif//__ENGINE_H_
