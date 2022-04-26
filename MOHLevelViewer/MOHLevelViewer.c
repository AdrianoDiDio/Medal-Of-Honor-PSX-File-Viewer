// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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
#include "MOHLevelViewer.h"
#include "ShaderManager.h"
#include "Config.h"

Color4f_t    c_Black = {   0,    0,   0,   1.f};
Color4f_t    c_Red    = {   1.f,  0,   0,   1.f};
Color4f_t    c_Green    = {   0,    1.f, 0,   1.f};
Color4f_t    c_Blue    = {   0,    0,   1.f, 1.f};
Color4f_t    c_Yellow = {  1.f,  1.f, 0,   1.f};
Color4f_t    c_White    = {   1.f,  1.f, 1.f, 1.f};
Color4f_t    c_Grey =  {   0.75, 0.75,0.75,1.f};

Config_t *VidConfigWidth;
Config_t *VidConfigHeight;
Config_t *VidConfigRefreshRate;

int StartSeconds = 0;

float Rand01()
{
    return (rand() / (float)(RAND_MAX));
}
int RandRangeI(int Min,int Max)
{
    return rand() % ( (Max + 1 - Min) + Min);
}
int vasprintf(char **Strp, const char *Fmt, va_list Ap)
{
    va_list APCopy;
    int Length;
    char *Buffer;
    int Result;

    va_copy(APCopy, Ap);
    Length = vsnprintf(NULL, 0, Fmt, APCopy);

    if (Length < 0) {
        return Length;
    }

    va_end(APCopy);
    Buffer = malloc(Length + 1);

    if (!Buffer) {
        return -1;
    }

    Result = vsnprintf(Buffer, Length + 1, Fmt, Ap);

    if (Result < 0) {
        free(Buffer);
    } else {
        *Strp = Buffer;
    }
    return Result;
}
/*
 sprintf version that takes an empty buffer and allocate the required space based on the format argument.
 It should be included in some linux version but it is not standard.
 */
int asprintf(char **Strp, const char *Fmt, ...)
{
    int Error;
    va_list Ap;

    va_start(Ap, Fmt);
    Error = vasprintf(Strp, Fmt, Ap);
    va_end(Ap);

    return Error;
}
char *StringCopy(const char *From)
{
    char *Dest;

    Dest = malloc(strlen(From) + 1);

    if ( !Dest ) {
        return NULL;
    }

    strcpy(Dest, From);

    return Dest;
}

char *StringToUpper(char *In)
{
    char *Result;
    int i;
    
    Result = StringCopy(In);
    for( i = 0; Result[i]; i++ ){
        if( Result[i] >= 'a' && Result[i] <= 'z' ){
            Result[i] -= ('a' - 'A');
        }
    }
    return Result;
}

int StringToInt(char *String)
{
    char *EndPtr;    
    long Value;
    
    Value = strtol(String, &EndPtr, 10);
    
    if( errno == ERANGE && Value == LONG_MIN ) {
        DPrintf("StringToInt %s (%lu) invalid...underflow occurred\n",String,Value);
        return 0;
    } else if( errno == ERANGE && Value == LONG_MAX ) {
        DPrintf("StringToInt %s (%lu) invalid...overflow occurred\n",String,Value);
        return 0;
    }
    return Value;
}

char *GetBaseName(char *Path)
{
    char *Out;
    Out = strrchr(Path, PATHSEPARATOR);
    if (Out == NULL) {
        Out = Path;
    } else {
        //Skip remaining dir separator.
        Out++;
    }
    return Out;
}

void CreateDirIfNotExists(char *DirName) {
    struct stat FileStat;

    if (stat(DirName, &FileStat) == -1) {
#ifdef _WIN32
        mkdir(DirName);
#else
        mkdir(DirName, 0700);
#endif
    }
}

char *SwitchExt(const char *In, const char *Ext)
{
    char *NewExt;
    int i;
    
    if ( !In || !Ext ) {
        return NULL;
    }

    NewExt = malloc(strlen(In) + 1);

    for ( i = 0; In[i] != '.'; i++ ) {
        NewExt[i] = In[i];
    }
    NewExt[i] = '\0';

    //Now append the extension to the string.
    strncat(NewExt, Ext, strlen(NewExt));
    return NewExt;
}

int GetFileLength(FILE *Fp)
{
    int Length;
    int CurrentPosition;

    if ( !Fp ) {
        return -1; //Must be a valid file
    }

    CurrentPosition = ftell(Fp);
    fseek(Fp, 0, SEEK_END);
    Length = ftell(Fp);
    fseek(Fp, CurrentPosition, SEEK_SET);

    return Length;
}

char *ReadTextFile(char *File,int Length)
{
    FILE *Fp;
    int FileSize;
    char *Result;
    int Ret;
    
    Fp = fopen(File,"r");
    
    if( !Fp ) {
        DPrintf("Shader file %s not found.\n",File);
        return NULL;
    }
    FileSize = Length != 0 ? Length : GetFileLength(Fp);
    Result = malloc(FileSize + 1);
    Ret = fread(Result,1, FileSize,Fp);
    if( Ret != FileSize ) {
        DPrintf("Failed to read file %s\n",File);
        return NULL;
    }
    Result[Ret] = '\0';
    fclose(Fp);
    return Result;
}

int GetCurrentFilePosition(FILE *Fp)
{
    return ftell(Fp);
}

void SkipFileSection(int SectionSize,FILE *InFile)
{
    if( !InFile ) {
        printf("SkipFileSection: Invalid file.\n");
        return;
    }
    int CurrentSection = ftell(InFile);
    fseek(InFile,SectionSize,SEEK_CUR);
    assert((ftell(InFile) - CurrentSection) == SectionSize);
}

Vec3_t Vec3Build(float x,float y,float z)
{
    Vec3_t Temp;

    Temp.x = x;
    Temp.y = y;
    Temp.z = z;

    return Temp;
}

float Vec3Length(Vec3_t Vector)
{
    return sqrt (Vector.x*Vector.x + Vector.y*Vector.y + Vector.z*Vector.z);
}

void Vec3RotateXAxis(float Theta,Vec3_t *Vector)
{
    Vector->x = Vector->x;
    Vector->y = Vector->y*cos(Theta) - Vector->z*sin(Theta);
    Vector->z = Vector->y*sin(Theta) + Vector->z*cos(Theta);
}

void Vec3Normalize(Vec3_t *VOut)
{
    VOut->x = VOut->x / Vec3Length(*VOut);
    VOut->y = VOut->y / Vec3Length(*VOut);
    VOut->z = VOut->z / Vec3Length(*VOut);
}

void Vec3Add(Vec3_t A,Vec3_t B,Vec3_t *VOut)
{
    VOut->x = A.x + B.x;
    VOut->y = A.y + B.y;
    VOut->z = A.z + B.z;
}

void Vec3Subtract(Vec3_t A,Vec3_t B,Vec3_t *VOut)
{
    VOut->x = A.x - B.x;
    VOut->y = A.y - B.y;
    VOut->z = A.z - B.z;
}

void Vec3Cross( Vec3_t A, Vec3_t B, Vec3_t *Out ) {
    Out->x = A.y*B.z - A.z*B.y;
    Out->y = A.z*B.x - A.x*B.z;
    Out->z = A.x*B.y - A.y*B.x;
}

void Vec3Scale(Vec3_t InVec,float Amount,Vec3_t *OutVec)
{
    OutVec->x = InVec.x * Amount;
    OutVec->y = InVec.y * Amount;
    OutVec->z = InVec.z * Amount;
}


void CamFixAngles(ViewParm_t *Camera)
{
    //If needed fix it.
    if ( Camera->Angle.x > 90.0f ) {
        Camera->Angle.x = 90.0f;
    }

    if ( Camera->Angle.x < -90.0f ) {
        Camera->Angle.x = -90.0f;
    }

    if ( Camera->Angle.y > 180.0f ) {
        Camera->Angle.y -= 360.0f;
    }

    if ( Camera->Angle.y < -180.0f ) {
        Camera->Angle.y += 360.0f;
    }

}

void CamInit(ViewParm_t *Camera,BSD_t *BSD)
{
    Vec3_t PlayerRotation;


    if( !BSD ) {
        Camera->Position = Vec3Build(0.f,0.f,0.f);
        Camera->Angle = Vec3Build(0.f,0.f,0.f);
    } else {
        Camera->Position = BSDGetPlayerSpawn(BSD,&PlayerRotation);
        Camera->Angle.x = (PlayerRotation.x / 4096.f) *  360.f;
        Camera->Angle.y = (PlayerRotation.y / 4096.f) *  360.f;
        Camera->Angle.z = (PlayerRotation.z / 4096.f) *  360.f;
    }
    Camera->OldPosition = Camera->Position;
    Camera->Up = Vec3Build(0.0f,1.0f,0.0f);
    Camera->Right = Vec3Build(1.0f,0.0f,0.0f);
    Camera->Forward = Vec3Build(0.0f,0.0f,1.0f);

    CamFixAngles(Camera);
}

void CamMouseEvent(ViewParm_t *Camera,int Dx,int Dy)
{
    Camera->Angle.x += ( Dy - (VidConf.Height/2) ) / 10.0f; // 0.001f;
    Camera->Angle.y += ( Dx - (VidConf.Width/2)) / 10.0f; // 0.001f;
    CamFixAngles(Camera);
}

void CamUpdate(ViewParm_t *Camera,int Orientation, float Sensitivity)
{
    switch ( Orientation ) {
        case DIR_FORWARD:
            Camera->Forward.x = sin(DEGTORAD(Camera->Angle.y));
            Camera->Forward.z = -cos(DEGTORAD(Camera->Angle.y));
            Camera->Forward.y = -sin(DEGTORAD(Camera->Angle.x));
            Vec3Scale(Camera->Forward,Sensitivity,&Camera->Forward);
            Vec3Add(Camera->Position,Camera->Forward,&Camera->Position);
            break;
        case DIR_BACKWARD:
            Camera->Forward.x = -sin(DEGTORAD(Camera->Angle.y));
            Camera->Forward.z = cos(DEGTORAD(Camera->Angle.y));
            Camera->Forward.y = sin(DEGTORAD(Camera->Angle.x));
            Vec3Scale(Camera->Forward,Sensitivity,&Camera->Forward);
            Vec3Add(Camera->Position,Camera->Forward,&Camera->Position);
            break;
        case DIR_UPWARD:
            Camera->Position.y += Sensitivity;
            break;
        case DIR_DOWNWARD:
            Camera->Position.y -= Sensitivity;
            break;
        case DIR_LEFTWARD:
            Camera->Right.x = -(float)(cos(DEGTORAD(Camera->Angle.y)));
            Camera->Right.z = -(float)(sin(DEGTORAD(Camera->Angle.y)));
            Vec3Scale(Camera->Right,Sensitivity,&Camera->Right);
            Vec3Add(Camera->Position,Camera->Right,&Camera->Position);
            break;
        case DIR_RIGHTWARD:
            Camera->Right.x = (float)(cos(DEGTORAD(Camera->Angle.y)));
            Camera->Right.z = (float)(sin(DEGTORAD(Camera->Angle.y)));
            Vec3Scale(Camera->Right,Sensitivity,&Camera->Right);
            Vec3Add(Camera->Position,Camera->Right,&Camera->Position);
            break;
         case LOOK_LEFT:
             Camera->Angle.y -= Sensitivity;
             if ( Camera->Angle.y < -360.f ) {
                 Camera->Angle.y += 360.f;
             }
             break;
         case LOOK_RIGHT:
             Camera->Angle.y += Sensitivity;
             if ( Camera->Angle.y > 360.f ) {
                 Camera->Angle.y -= 360.f;
             }
             break;
         case LOOK_DOWN:
             Camera->Angle.x += Sensitivity;
             break;
         case LOOK_UP:
             Camera->Angle.x -= Sensitivity;
             break;
        default:
            break;
    }
}

char *SysGetConfigPath()
{
    return SDL_GetPrefPath(NULL,"MOHLevelViewer");
}
int SysMilliseconds()
{
    struct timeval tp;
    int CTime;

    gettimeofday(&tp, NULL);

    if ( !StartSeconds ){
        StartSeconds = tp.tv_sec;
        return tp.tv_usec/1000;
    }

    CTime = (tp.tv_sec - StartSeconds)*1000 + tp.tv_usec / 1000;

    return CTime;
}

void DPrintf(char *Fmt, ...)
{
    char Temp[1000];
    va_list arglist;

    va_start(arglist, Fmt);
    vsnprintf(Temp, sizeof( Temp ), Fmt, arglist);
#ifdef _DEBUG
    fputs(Temp, stdout);
#endif
    va_end(arglist);
}

bool VidInitSDL()
{
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        return false;
    } else {
        DPrintf("SDL Subsystem initialized.\n");
        return true;
    }
    return false;
}

void VidGetAvailableVideoModes()
{
    int NumAvailableVideoModes;
    SDL_DisplayMode Mode;
    int i;
    
    //NOTE(Adriano):We are forcing this to display 0.
    NumAvailableVideoModes = SDL_GetNumDisplayModes(0);
    VidConf.VideoModeList = malloc(NumAvailableVideoModes * sizeof(VideoMode_t));
    //Pickup the maximum supported resolution as the default one.
    VidConf.CurrentVideoMode = 0;
    for( i = 0; i < NumAvailableVideoModes; i++ ) {
        SDL_GetDisplayMode(0,i,&Mode);
        if( Mode.w == 800 && Mode.h == 600 && Mode.refresh_rate == 120) {
            VidConf.CurrentVideoMode = i;
        }
        VidConf.VideoModeList[i].Width = Mode.w;
        VidConf.VideoModeList[i].Height = Mode.h;
        VidConf.VideoModeList[i].RefreshRate = Mode.refresh_rate;
        VidConf.VideoModeList[i].BPP = SDL_BITSPERPIXEL(Mode.format);
        asprintf(&VidConf.VideoModeList[i].Description, "%ix%i@%iHz",VidConf.VideoModeList[i].Width,VidConf.VideoModeList[i].Height,
                 VidConf.VideoModeList[i].RefreshRate);
    }
    VidConf.NumVideoModes = NumAvailableVideoModes;
}

SDL_DisplayMode *SDLGetCurrentDisplayMode()
{
    static SDL_DisplayMode Result;
    VideoMode_t *CurrentMode;
    int NumModes;
    int i;
    
    NumModes = SDL_GetNumDisplayModes(0);
    CurrentMode = &VidConf.VideoModeList[VidConf.CurrentVideoMode];
    for( i = 0; i < NumModes; i++ ) {
        SDL_GetDisplayMode(0,i,&Result);
        if( Result.w == CurrentMode->Width && Result.h == CurrentMode->Height &&
            SDL_BITSPERPIXEL(Result.format) == CurrentMode->BPP && Result.refresh_rate == CurrentMode->RefreshRate ) {
                return &Result;
            }
    }
    return NULL;
}
void SysSetCurrentVideoSettings()
{
    if( SDL_GetWindowFlags (VideoSurface) & SDL_WINDOW_FULLSCREEN ) {
        //Was fullscreen reset it...
        SDL_SetWindowFullscreen(VideoSurface,0);
    }
    SDL_SetWindowSize(VideoSurface,VidConf.VideoModeList[VidConf.CurrentVideoMode].Width,VidConf.VideoModeList[VidConf.CurrentVideoMode].Height);
    if( VidConf.FullScreen ) {
        SDL_SetWindowDisplayMode(VideoSurface,SDLGetCurrentDisplayMode());
        SDL_SetWindowFullscreen(VideoSurface,SDL_WINDOW_FULLSCREEN);
    }
}

bool VidOpenWindow()
{
    //Make sure we have an OpenGL context.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef _DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    VideoSurface = SDL_CreateWindow(VidConf.Title,SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                     VidConf.VideoModeList[VidConf.CurrentVideoMode].Width, VidConf.VideoModeList[VidConf.CurrentVideoMode].Height, 
                     SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    
    SysSetCurrentVideoSettings();
    Context = SDL_GL_CreateContext(VideoSurface);
    VidConf.DPIScale = 1.f;
    if( !SDL_GetDisplayDPI(0, NULL, &VidConf.DPIScale, NULL) ) {
        VidConf.DPIScale /= 96.f;
    }
        
    GUI = GUIInit(VideoSurface,Context);
    VidConf.Initialized = true;
    SDL_GL_SetSwapInterval(1);
    
    return true;
}
void SysVidShutdown()
{
    int i;
    for( i = 0; i < VidConf.NumVideoModes; i++ ) {
        free(VidConf.VideoModeList[i].Description);
    }
    free(VidConf.VideoModeList);
    SDL_GL_DeleteContext(Context);
    SDL_DestroyWindow(VideoSurface);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}
void SysHideCursor()
{
    SDL_ShowCursor(false);
}

void SysShowCursor()
{
    SDL_ShowCursor(true);
}

void SysCenterCursor()
{
    SDL_WarpMouseInWindow(VideoSurface,VidConf.Width/2,VidConf.Height/2);
}

void SysSwapBuffers()
{
    if( !VidConf.Initialized ){
        return;
    }
    SDL_GL_SwapWindow(VideoSurface);
}

int SysGetCurrentVideoWidth()
{
    assert(VidConf.CurrentVideoMode != -1);
    return VidConf.VideoModeList[VidConf.CurrentVideoMode].Width;
}
int SysGetCurrentVideoHeight()
{
    assert(VidConf.CurrentVideoMode != -1);
    return VidConf.VideoModeList[VidConf.CurrentVideoMode].Height;
}

void InitSDL(const char *Title,int Width,int Height,bool FullScreen)
{
    GLenum GlewError;
    if ( !VidConf.Initialized ) {
        DPrintf("Vidconf isn't initialized...\n");
    }

    VidConf.Title = Title == NULL ? "Unnamed" : Title;
    DPrintf("Title:%s\n",Title);
    VidConf.Width = Width;
    VidConf.Height = Height;
    VidConf.FullScreen = FullScreen;
    VidConf.Resizable = false;

    if ( !VidInitSDL() ) {
        DPrintf("Failed on initializing SDL.\n");
    }
    
    VidGetAvailableVideoModes();
    
    VidConf.Width = SysGetCurrentVideoWidth();
    VidConf.Height = SysGetCurrentVideoHeight();

    if ( !VidOpenWindow() ) {
        DPrintf("Failed on opening a new window.\n");
    }
    //Needed in order to load the core 3.3 profile.
    glewExperimental = GL_TRUE;
    GlewError = glewInit();
    if (GlewError != GLEW_OK) {
        DPrintf( "Failed to init GLEW\n");
        SDL_DestroyWindow(VideoSurface);
        SDL_Quit();
        return;
    }

    
    VidConf.Driver = StringCopy("SDL");
//     SysHideCursor();
    return;
}

void SysVidInit(int Width, int Height, bool FullScreen)
{
    InitSDL("MOH Level Viewer",Width,Height,FullScreen);
}

void SysCheckKeyEvents()
{
    SDL_Event Event;
    float CamSpeed = 80.f;
    while( SDL_PollEvent(&Event) ) {
        
        if( Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            VidConf.Width = Event.window.data1;
            VidConf.Height = Event.window.data2;
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F1 ) {
            GUIToggleDebugWindow(GUI);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F2 ) {
            GUIToggleSettingsWindow(GUI);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F3 ) {
            GUIToggleLevelSelectWindow(GUI);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F4 ) {
            GUISetMOHPath(GUI);
        }
        if( Event.type == SDL_QUIT || (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE ) ) {
            Quit();
        }
        
        if( !GUIProcessEvent(GUI,&Event) ) {
            switch( Event.type ) {
                case SDL_MOUSEMOTION:
                    CamMouseEvent(&Camera,Event.motion.x,Event.motion.y);
                    SysCenterCursor();
                    break;
                case SDL_KEYDOWN:
                    if( Event.key.keysym.sym == SDLK_ESCAPE ) {
                        Quit();
                    }
                    if( Event.key.keysym.sym == SDLK_w ) {
                        CamUpdate(&Camera, DIR_FORWARD, CamSpeed * ComTime->Delta);
                    }
                    if( Event.key.keysym.sym == SDLK_s ) {
                        CamUpdate(&Camera, DIR_BACKWARD, CamSpeed * ComTime->Delta);
                    }
                    if( Event.key.keysym.sym == SDLK_a ) {
                        CamUpdate(&Camera, DIR_LEFTWARD, CamSpeed * ComTime->Delta);
                    }
                    if( Event.key.keysym.sym == SDLK_d ) {
                        CamUpdate(&Camera, DIR_RIGHTWARD, CamSpeed * ComTime->Delta);
                    }
                    if( Event.key.keysym.sym == SDLK_SPACE ) {
                        CamUpdate(&Camera, DIR_UPWARD, CamSpeed * ComTime->Delta);
                    }
                    if( Event.key.keysym.sym == SDLK_z ) {
                        CamUpdate(&Camera, DIR_DOWNWARD, CamSpeed * ComTime->Delta);
                    }
    //                 if( Event.key.keysym.sym == SDLK_LEFT ) {
    //                     CamUpdate(&Camera, LOOK_LEFT, CamSpeed * ComTime->Delta);
    //                 }
    //                 if( Event.key.keysym.sym == SDLK_RIGHT ) {
    //                     CamUpdate(&Camera, LOOK_RIGHT, CamSpeed * ComTime->Delta);
    //                 }
    //                 if( Event.key.keysym.sym == SDLK_UP ) {
    //                     CamUpdate(&Camera, LOOK_UP, CamSpeed * ComTime->Delta);
    //                 }
    //                 if( Event.key.keysym.sym == SDLK_DOWN ) {
    //                     CamUpdate(&Camera, LOOK_DOWN, CamSpeed * ComTime->Delta);
    //                 }
                    CamFixAngles(&Camera);
                    break;
                case SDL_QUIT:
                    Quit();
                    break;
                default:
                    break;
            }
        }
    }
    
}

bool ComUpdateDelta()
{
    long Now;
    int TimeSlice;
    
    Now = SysMilliseconds();
    TimeSlice = /*floor*/((1/MAX_FPS) * 1000);

    ComTime->UpdateLength = Now - ComTime->LastLoopTime;
    ComTime->OptimalTime = 1000 / MAX_FPS;
    ComTime->UpdateLength = Now - ComTime->LastLoopTime;
    ComTime->Delta = ComTime->UpdateLength / ( ComTime->OptimalTime);

    if( ComTime->UpdateLength < TimeSlice ) {
        return false;
    }

    // update the frame counter
    ComTime->LastFPSTime += ComTime->UpdateLength;
    ComTime->FPS++;
    ComTime->LastLoopTime = Now;
    // update our FPS counter if a second has passed since
    // we last recorded
    if (ComTime->LastFPSTime >= 1000 ) {
        sprintf(ComTime->FPSString,"FPS:%i\nMs: %.2f ms\nLast FPS Time:%f\nDelta:%f",
                ComTime->FPS,
                1000.f/(float)ComTime->FPS,
                ComTime->LastFPSTime,ComTime->Delta);
        sprintf(ComTime->FPSSimpleString,"FPS %i Ms %.2f ms",
                ComTime->FPS,
                1000.f/(float)ComTime->FPS);
        DPrintf("%s\n",ComTime->FPSString);
        DPrintf("Current Camera Position:%f;%f;%f\n",Camera.Position.x,Camera.Position.y,Camera.Position.z);
        ComTime->LastFPSTime = 0;
        ComTime->FPS = 0;
    }
    return true;
}


/*
Projection Matrix.
Where:
t = l => XYMax
r => xyMax
n => zNear.
f => zFar.
Complete Matrix:
2n/r-l	0		r+l/r-l	   0
0		2n/t-b	t+b/t-b	   0
0		0		-(f+n)/f-n -2fn/f-n
0		0		-1		   0
----------------------------------------
TODO:
Horizontal and Vertical field-of-view calculation.
Assuming that V and H are respectively vertical and horizontal field of view in degrees.
H = 2 * arctan(tan(V/2)* ratio);
V = 2 * arctan(tan(H/2)* 1/ratio);

*/
void GLSetProjectionMatrix()
{

    //FIXME:Make those value adjustable.
    float Fov = 110.0f; // In Degrees
    float  XYMax;
    float  XMin,YMin;
    float	Width, Height, Depth;
    float	zNear = 1.0f;
    float  zFar = 4096.0f;


    XYMax = zNear * tan( Fov * PI_OVER_360 );
    XMin = YMin = -XYMax;

    Width  = XYMax - XMin;
    Height = XYMax - YMin;

    Depth = zFar - zNear;

    VidConf.PMatrix[0] = 2 * zNear / Width;
    VidConf.PMatrix[1] = 0;
    VidConf.PMatrix[2] = 0;
    VidConf.PMatrix[3] = 0;

    VidConf.PMatrix[4] = 0;
    VidConf.PMatrix[5] = 2 * zNear / Height;
    VidConf.PMatrix[6] = 0;
    VidConf.PMatrix[7] = 0;

    //DPrintf("%f-%f\n",(XYMax + XMin )  / Width,( XYMax + YMin ) / Height);
    VidConf.PMatrix[8] = ( XYMax + XMin )  / Width;//This should be 0.
    VidConf.PMatrix[9] = ( XYMax + YMin ) / Height;//This should be 0.
    //DPrintf("%f\n",-(zFar + zNear) / Depth);
    VidConf.PMatrix[10] = -(zFar + zNear) / Depth;
    VidConf.PMatrix[11] = -1;

    VidConf.PMatrix[12] = 0;
    VidConf.PMatrix[13] = 0;
    VidConf.PMatrix[14] = -2 * (zFar * zNear) / Depth;
    VidConf.PMatrix[15] = 0;
}

/*
 Suppressed messages:
 Id = 131204 => Message:Texture state usage warning
 */
void GLDebugOutput(GLenum Source, GLenum Type, unsigned int Id, GLenum Severity, GLsizei Length, const char *Message, const void *UserParam)
{
    if( Id == 131204 ) {
        return;
    }
    DPrintf("---------------\n");
    DPrintf("Debug message Id: %i\n",Id);


    switch (Source) {
        case GL_DEBUG_SOURCE_API:             
            DPrintf("Source: API"); 
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   
            DPrintf("Source: Window System"); 
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: 
            DPrintf("Source: Shader Compiler"); 
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     
            DPrintf("Source: API"); 
            break;
        case GL_DEBUG_SOURCE_APPLICATION:     
            DPrintf("Source: Application"); 
            break;
        case GL_DEBUG_SOURCE_OTHER:           
            DPrintf("Source: Other"); 
            break;
    }
    DPrintf("\n");

    switch (Type) {
        case GL_DEBUG_TYPE_ERROR:               
            DPrintf("Type: Error");
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: 
            DPrintf("Type: Deprecated Behaviour");
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  
            DPrintf("Type: Undefined Behaviour"); 
            break; 
        case GL_DEBUG_TYPE_PORTABILITY:         
            DPrintf("Type: Portability");
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:         
            DPrintf("Type: Performance");
            break;
        case GL_DEBUG_TYPE_MARKER:              
            DPrintf("Type: Marker");
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          
            DPrintf("Type: Push Group");
            break;
        case GL_DEBUG_TYPE_POP_GROUP:           
            DPrintf("Type: Pop Group"); 
            break;
        case GL_DEBUG_TYPE_OTHER:               
            DPrintf("Type: Other"); 
            break;
    } 
    DPrintf("\n");
    
    switch (Severity) {
        case GL_DEBUG_SEVERITY_HIGH:         
            DPrintf("Severity: High"); 
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:       
            DPrintf("Severity: Medium"); break;
        case GL_DEBUG_SEVERITY_LOW:          
            DPrintf("Severity: Low");
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            DPrintf("Severity: Notification"); 
            break;
    } 
    DPrintf("\n");
    
    DPrintf("Message:%s\n",Message);
    DPrintf("---------------\n");
    if( Severity == GL_DEBUG_SEVERITY_HIGH ) {
        assert(1!=1);
    }
}

void GLSetDefaultState()
{
//     glShadeModel( GL_SMOOTH );

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    glClearDepth( 1.0f );

    glDepthFunc( GL_LEQUAL );

    glEnable( GL_DEPTH_TEST );
    
#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(GLDebugOutput, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif

//     glEnableClientState( GL_VERTEX_ARRAY );
}

void InitGLView()
{
    GLSetDefaultState();
}

void GLFrame()
{
    glViewport(0,0,VidConf.Width,VidConf.Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    LevelManagerDraw(LevelManager);
    glDisable (GL_DEPTH_TEST);
    GUIDraw(GUI,LevelManager);
    glEnable(GL_DEPTH_TEST);
}



void DumpLevel(Level_t* Level)
{
    char *EngineName;
    char *EngineDir;
    char *OutDir;
    char *PlyDir;
    char *ObjectFile;
    char *PlyLevelFile;
    char *PlyObjectFile;
    char FileName[256];
    char PlyLevelFileName[256];
    char PlyObjectFileName[256];
    char MaterialNameTag[64];
    char MissionDir[64];
    char ExportDir[64];
    FILE *OutFile;
    FILE *PlyLevelOutFile;
    FILE *PlyObjectOutFile;


    sprintf(FileName,"MSN%iLVL%i.obj",Level->MissionNumber,Level->LevelNumber);
    sprintf(PlyLevelFileName,"MSN%iLVL%i_Level.ply",Level->MissionNumber,Level->LevelNumber);
    sprintf(PlyObjectFileName,"MSN%iLVL%i_Objects.ply",Level->MissionNumber,Level->LevelNumber);

    sprintf(MissionDir,"MSN%iLVL%i",Level->MissionNumber,Level->LevelNumber);
    sprintf(ExportDir,"Export");
    asprintf(&EngineName,"%s",(LevelManagerGetGameEngine(LevelManager) == MOH_GAME_STANDARD) ? "MOH" : "MOHUndergound");
    asprintf(&EngineDir,"%s%c%s",ExportDir,PATHSEPARATOR,EngineName);
    asprintf(&OutDir,"%s%c%s%c",EngineDir,PATHSEPARATOR,MissionDir,PATHSEPARATOR);
    asprintf(&PlyDir,"%s%cPly%c",OutDir,PATHSEPARATOR,PATHSEPARATOR);

    CreateDirIfNotExists(ExportDir);
    CreateDirIfNotExists(EngineDir);
    CreateDirIfNotExists(OutDir);
    CreateDirIfNotExists(PlyDir);
    
    asprintf(&ObjectFile,"%s%s", OutDir,FileName);
    asprintf(&PlyLevelFile,"%s%s", PlyDir,PlyLevelFileName);
    asprintf(&PlyObjectFile,"%s%s", PlyDir,PlyObjectFileName);

    DPrintf("Dumping it...%s\n",ObjectFile);
    OutFile = fopen(ObjectFile,"w");
    PlyLevelOutFile = fopen(PlyLevelFile,"w");
    PlyObjectOutFile = fopen(PlyObjectFile,"w");

    sprintf(MaterialNameTag,"mtllib vram.mtl\n");
    fwrite(MaterialNameTag,strlen(MaterialNameTag),1,OutFile);
    TSPDumpDataToFile(Level->TSPList,OutFile);
    TSPDumpDataToPlyFile(Level->TSPList,PlyLevelOutFile);

    BSDDumpDataToFile(Level->BSD,OutFile);
    BSDDumpDataToPlyFile(Level->BSD,PlyObjectOutFile);
    VRAMDumpDataToFile(Level->VRAM,OutDir);
    
    free(EngineName);
    free(EngineDir);
    free(OutDir);
    free(PlyDir);
    free(ObjectFile);
    free(PlyLevelFile);
    free(PlyObjectFile);
    
    fclose(OutFile);
    fclose(PlyLevelOutFile);
    fclose(PlyObjectOutFile);
}

/*
    Late level initialization for all the subsystems that
    requires a valid OpenGL context.
*/
void LevelLateInit()
{
    ShaderManagerInit();
}

void Quit()
{
    LevelManagerCleanUp();
    GUIFree(GUI);
//     LevelCleanUp(Level);
    ShaderManagerFree();
    free(VidConf.Driver);
    free(ComTime);
    SysVidShutdown();
    SDL_Quit();
    exit(0);
}

/*
 * Requires 3 things:
    - MOH DATA Location
    - MISSION NUMBER
    - LEVEL NUMBER
*/
#define _ENABLEVIDEOOUT 1
int main(int argc,char **argv)
{
    srand(time(NULL));
//     if( argc != 4 ) {
//         printf("%s <MOH Directory> <Mission Number> <Level Number> will load level files from that mission.\n",argv[0]);
//         return -1;
//     }
    LevelManagerInit();
    
    ConfigInit();
    
//     VidConfigWidth = ConfigRegister("VidWidth","1920");
//     VidConfigHeight = ConfigRegister("VidHeight","800");
//     if( !LevelInit(argv[1],argv[2],argv[3]) ) {
//         printf("Couldn't load data.\n");
//         return -1;
//     }

#if _ENABLEVIDEOOUT
    SysVidInit(1920,800,false);
    ComTime = malloc(sizeof(ComTimeInfo_t));
    memset(ComTime,0,sizeof(ComTimeInfo_t));
    InitGLView();
    ShaderManagerInit();
//     CamInit(&Camera);
//     LevelLateInit();
    /* TEMP! */
    while( 1 ) {
        SysCheckKeyEvents();
        do {
        } while( !ComUpdateDelta() );
        GLFrame();
        glFlush();
        SysSwapBuffers();
    }
#else
    #ifdef _DEBUG
    Quit();
    #endif
#endif
// 
    return 0;
}
