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

#ifndef __LEVEL_H_
#define __LEVEL_H_

#include "Common.h"
#include "GUI.h"
#include "TSP.h"
#include "BSD.h"
#include "BSDTwoPlayers.h"
#include "VRAM.h"
#include "TIM.h"
#include "Font.h"

typedef struct LevelSettings_s {
    bool WireFrame;
    bool ShowCollisionData;
    bool ShowAABBTree;
    bool ShowMap;
    bool ShowBSDNodes;
    bool ShowBSDRenderObject;
    bool DrawBSDRenderObjects;
    bool DrawBSDShowCaseRenderObject;
    bool EnableFrustumCulling;
    bool EnableLighting;
    bool EnableSemiTransparency;
    bool EnableAnimatedLights;
    bool EnableAnimatedSurfaces;
} LevelSettings_t;

typedef struct Level_s {
    char    BasePath[256];
    char    MissionPath[256];
    char    EngineName[256];
    int     MissionNumber;
    int     LevelNumber;
    BSD_t  *BSD;
    BSD2P_t *BSDTwoP;
    TSP_t  *TSPList;
    TIMImage_t *ImageList;
    VRAM_t *VRAM;
    Font_t *Font;
    // At any time there can only be 4 tsp to be rendered.
    int     TSPNumberRenderList[4];
} Level_t;

bool    LevelInit(Level_t *Level,GUI_t *GUI,char *BasePath,int MissionNumber,int LevelNumber,int *GameEngine);
void    LevelSetDefaultSettings(LevelSettings_t *LevelSettings);
int     LevelIsLoaded(Level_t *Level);
void    LevelUnload(Level_t *Level);
void    LevelCleanUp(Level_t *Level);


#endif//__LEVEL_H_
