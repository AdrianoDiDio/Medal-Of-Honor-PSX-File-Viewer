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

#include "Common.h"
#include "Config.h"
#include "Video.h"
#include "Camera.h"
#include "BSD.h"
#include "Level.h"
#include "LevelManager.h"
#include "GUI.h"

#define MAX_FPS 120


typedef enum
{
    MOH_GAME_STANDARD,
    MOH_GAME_UNDERGROUND
} MOHGame_t;

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
   Camera_t         *Camera;
   LevelManager_t   *LevelManager;
   GUI_t            *GUI;
   const Byte       *KeyState;
} Engine_t;

void        DumpLevel(Level_t* Level);
void        DPrintf(char *Fmt, ...) Attribute((format(printf,1,2)));
int         asprintf(char **Strp, const char *Fmt, ...);
char        *StringCopy(const char *From);
int         StringToInt(char *String);
int         GetFileLength(FILE *Fp);
char        *ReadTextFile(char *File,int Length);
int         GetCurrentFilePosition(FILE *Fp);
void        SkipFileSection(int SectionSize,FILE *InFile);
void        CreateDirIfNotExists(char *DirName);
char        *SwitchExt(const char *In, const char *Ext);
char        *GetBaseName(char *Path);
bool        GLInitCompatibilityProfile(const char *Driver);
void        GLSet3D();
Engine_t    *EngineInit(int argc,char **argv);
void        EngineShutDown(Engine_t *Engine);
float       Rand01();
int         RandRangeI(int Min,int Max);
Byte        HighNibble(Byte In);
Byte        LowNibble(Byte In);
int         SignExtend(int Temp);
int         SysMilliseconds();
char        *SysGetConfigPath();
void        SysShowCursor();
void        SysHideCursor();
void        Quit(Engine_t *Engine);

#endif //__MOHLEVELVIEWER_H_
