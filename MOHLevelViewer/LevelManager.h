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

#ifndef __LEVELMANAGER_H_
#define __LEVELMANAGER_H_

#include "GUI.h"
#include "Level.h"
#include "Config.h"

typedef struct MissionLevel_s {
    char *LevelName;
    int LevelNumber;
} MissionLevel_t;
typedef struct Mission_s {
    char *MissionName;
    int   MissionNumber;
    int   NumLevels;
    MissionLevel_t *Levels;
} Mission_t;

typedef struct LevelManager_s {
    char    *BasePath;
    char    MissionPath[256];
    char    EngineName[256];
    GUIFileDialog_t *FileDialog;
    Level_t *CurrentLevel;
    LevelSettings_t Settings;
    int     IsPathSet;
    int     GameEngine;
} LevelManager_t;

void    LevelManagerInit();
void    LevelManagerLoadLevel(LevelManager_t *LevelManager,GUI_t *GUI,int MissionNumber,int LevelNumber);
int     LevelManagerIsLevelLoaded(LevelManager_t *LevelManager);
int     LevelManagerGetGameEngine(LevelManager_t *LevelManager);
int     LevelManagerInitWithPath(LevelManager_t *LevelManager,GUI_t *GUI,char *Path);
void    LevelManagerDraw(LevelManager_t *LevelManager);
void    LevelManagerToggleFileDialog(LevelManager_t *LevelManager,GUI_t *GUI);
void    LevelManagerCleanUp();

extern Mission_t MOHMissionsList[];
extern int NumMOHMissions;
extern Mission_t MOHUMissionsList[];
extern int NumMOHUMissions;

extern Config_t *LevelManagerBasePath;
#endif//__LEVELMANAGER_H_
