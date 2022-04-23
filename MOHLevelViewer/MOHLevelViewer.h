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
#include "BSD.h"
#include "Level.h"
#include "LevelManager.h"
#include "GUI.h"

#define MAX_FPS 120

//TODO(Adriano):Instance them and pass the pointer around without relying on any global variable...


typedef enum
{
    MOH_GAME_STANDARD,
    MOH_GAME_UNDERGROUND
} MOHGame;

typedef struct VideoMode_s {
    int Width;
    int Height;
    int RefreshRate;
    int BPP;
    
    char *Description;
} VideoMode_t;

typedef struct VidDriver_s {
const char *Title;
    int    Width;
    int    Height;
    bool   FullScreen;
    void  *GLLibHandle;
    //..Use this one.
    char   *Driver;
    bool   Initialized;
    bool   Resizable;
    bool	UseCompatibilityRenderer; //If true use OpenGL 3 compatibility profile for older systems.
    float PMatrix[16]; //TODO:Remove this and put it into the right context...
    mat4    PMatrixM4;
    mat4    ModelMatrix;
    mat4    ViewMatrix;
    mat4    ModelViewMatrix;
    mat4    MVPMatrix;
    float   DPIScale;
    VideoMode_t *VideoModeList;
    int     NumVideoModes;
    int     CurrentVideoMode;
} VidDriver_t;

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



typedef struct ViewParm_s {
    Vec3_t Position;
    Vec3_t Angle;
    Vec3_t  OldPosition;
    //Movements...
    Vec3_t	Up;
    Vec3_t	Right;
    Vec3_t	Forward;
    vec4    FrustumPlaneList[6];
    vec4    FrustumCornerList[8];
} ViewParm_t;

typedef enum {
    LOOK_LEFT,
    LOOK_RIGHT,
    LOOK_UP,
    LOOK_DOWN,
    DIR_FORWARD,
    DIR_BACKWARD,
    DIR_UPWARD,
    DIR_DOWNWARD,
    DIR_LEFTWARD,
    DIR_RIGHTWARD
} Direction_t;

VidDriver_t VidConf;
ViewParm_t Camera;
ComTimeInfo_t *ComTime;
LevelManager_t *LevelManager;
GUI_t *GUI;
SDL_Window *VideoSurface;
SDL_GLContext Context;

void    DumpLevel(Level_t* Level);
void    DPrintf(char *Fmt, ...) Attribute((format(printf,1,2)));
int     asprintf(char **Strp, const char *Fmt, ...);
char   *StringCopy(const char *From);
int     GetFileLength(FILE *Fp);
char   *ReadTextFile(char *File,int Length);
int     GetCurrentFilePosition(FILE *Fp);
void    SkipFileSection(int SectionSize,FILE *InFile);
void    CreateDirIfNotExists(char *DirName);
char   *SwitchExt(const char *In, const char *Ext);
char   *GetBaseName(char *Path);
bool    GLInitCompatibilityProfile(const char *Driver);
Vec3_t  Vec3Build(float x,float y,float z);
void    Vec3RotateXAxis(float Theta,Vec3_t *Vector);
void    GLSet3D();
void    CamInit(ViewParm_t *Camera,BSD_t *BSD);
void    CamUpdate(ViewParm_t *Camera,int Orientation, float Sensibility);
void    CamUpdateVectors(ViewParm_t *Camera);
float   Rand01();
int     RandRangeI(int Min,int Max);
void    SysSwapBuffers();
void    SysShowCursor();
void    SysHideCursor();
void    SysSetCurrentVideoSettings();
void    Quit();

#endif //__MOHLEVELVIEWER_H_
