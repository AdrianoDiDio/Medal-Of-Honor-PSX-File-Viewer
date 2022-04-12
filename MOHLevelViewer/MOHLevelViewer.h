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


#include <stdio.h>
#include <stdlib.h>
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
#include "cimgui.h"
#include "cimgui_impl.h"
#include "ImGuiFileDialog.h"

#include "Types.h"
#include "ShaderManager.h"
#include "VAO.h"
#include "GUI.h"
#include "TIM.h"
#include "VRAM.h"
#include "Font.h"
#include "BSD.h"
#include "TSP.h"
#include "BSDTwoPlayers.h"

#ifdef __linux__
#define PATHSEPARATOR '/'
#else
#define PATHSEPARATOR '\\'
#endif

#ifdef __GNUC__
#define Attribute(x) __attribute__(x)
#else
#define Attribute(x)
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

#define MAX_FPS 60

typedef enum
{
    MOH_GAME_STANDARD,
    MOH_GAME_UNDERGROUND
} MOHGame;

typedef struct VidDriver_s
{
const char *Title;
    int    Width;
    int    Height;
    bool   Fullscreen;
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
} VidDriver_t;

typedef struct ComTimeInfo_s
{
    int	  Fps;
    float Delta;
    //Frame to frame time
    //updated each second.
    float LastFpsTime;
    float LastLoopTime;
    float Optimal_Time;
    float UpdateLength;
    char  FpsString[256];
    char  FpsSimpleString[256];
} ComTimeInfo_t;



typedef struct ViewParm_s
{
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

typedef enum
{
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
    LevelSettings_t Settings;
    // At any time there can only be 4 tsp to be rendered.
    int     TSPNumberRenderList[4];
} Level_t;

typedef struct LevelManager_s {
    char    *BasePath;
    char    MissionPath[256];
    char    EngineName[256];
    Level_t *CurrentLevel;
    LevelSettings_t Settings;
    int     IsPathSet;
    int     GameEngine;
} LevelManager_t;

VidDriver_t VidConf;
ViewParm_t Camera;
ComTimeInfo_t *ComTime;
LevelManager_t *LevelManager;
GUI_t *GUI;
SDL_Window *VideoSurface;
SDL_Renderer *SDLRenderer;

void    DumpLevel(Level_t* Level);
void    DPrintf(char *Fmt, ...) Attribute((format(printf,1,2)));
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
void    CamUpdate(ViewParm_t *Camera,int Orientation, float Sensibility);
void    CamUpdateVectors(ViewParm_t *Camera);
int     LevelGetGameEngine();
int     LevelManagerSetPath(LevelManager_t *LevelManager,char *Path);
float   Rand01();
int     RandRangeI(int Min,int Max);
void    SysShowCursor();
void    SysHideCursor();
void    Quit();

#endif //__MOHLEVELVIEWER_H_
