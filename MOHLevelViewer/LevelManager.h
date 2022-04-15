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


typedef struct LevelManager_s {
    char    *BasePath;
    char    MissionPath[256];
    char    EngineName[256];
    Level_t *CurrentLevel;
    LevelSettings_t Settings;
    int     IsPathSet;
    int     GameEngine;
} LevelManager_t;

void    LevelManagerInit();
int     LevelManagerGetGameEngine(LevelManager_t *LevelManager);
int     LevelManagerSetPath(LevelManager_t *LevelManager,char *Path);
void    LevelManagerDraw(LevelManager_t *LevelManager);

#endif//__LEVELMANAGER_H_
