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
#ifndef __MOHLEVELVIEWER_H_
#define __MOHLEVELVIEWER_H_ 

#include "../Common/Common.h"
#include "../Common/Config.h"
#include "../Common/Video.h"
#include "Camera.h"
#include "BSD.h"
#include "Level.h"
#include "LevelManager.h"
#include "GUI.h"

#define MAX_FPS 120


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
   ComTimeInfo_t    *TimeInfo;
   VideoSystem_t    *VideoSystem;
   SoundSystem_t    *SoundSystem;
   Camera_t         *Camera;
   LevelManager_t   *LevelManager;
   GUI_t            *GUI;
   const Byte       *KeyState;
} Engine_t;

Engine_t    *EngineInit(int argc,char **argv);
void        EngineShutDown(Engine_t *Engine);
char        *AppGetConfigPath();
void        Quit(Engine_t *Engine);

#endif //__MOHLEVELVIEWER_H_
