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
#include "Sound.h"
#include "GUI.h"
#include "TSP.h"
#include "BSD.h"
#include "BSDTwoPlayers.h"
#include "VRAM.h"
#include "TIM.h"
#include "Font.h"

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

bool    LevelInit(Level_t *Level,GUI_t *GUI,VideoSystem_t *VideoSystem,
                  SoundSystem_t *SoundSystem,char *BasePath,int MissionNumber,int LevelNumber,int *GameEngine);
void    LevelDraw(Level_t *Level,Camera_t *Camera,mat4 ProjectionMatrix);
Vec3_t  LevelGetPlayerSpawn(Level_t *Level,int SpawnIndex,Vec3_t *Rotation);
void    LevelUpdate(Level_t *Level,Camera_t *Camera);
int     LevelIsLoaded(Level_t *Level);
void    LevelSetMusicTrackSettings(Level_t *Level,SoundSystem_t *SoundSystem,int GameEngine,int SoundValue);
void    LevelUnload(Level_t *Level);
void    LevelCleanUp(Level_t *Level);

extern Config_t *LevelEnableWireFrameMode;
extern Config_t *LevelDrawCollisionData;
extern Config_t *LevelDrawBSPTree;
extern Config_t *LevelDrawSurfaces;
extern Config_t *LevelDrawBSDNodesAsPoints;
extern Config_t *LevelDrawBSDRenderObjectsAsPoints;
extern Config_t *LevelDrawBSDRenderObjects;
extern Config_t *LevelDrawBSDShowcase;
extern Config_t *LevelEnableFrustumCulling;
extern Config_t *LevelEnableAmbientLight;
extern Config_t *LevelEnableSemiTransparency;
extern Config_t *LevelEnableAnimatedLights;
extern Config_t *LevelEnableAnimatedSurfaces;
extern Config_t *LevelEnableMusicTrack;
#endif//__LEVEL_H_
