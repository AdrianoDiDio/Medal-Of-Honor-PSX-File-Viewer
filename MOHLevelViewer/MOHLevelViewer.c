/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
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

Color_t    c_Black = {   0,    0,   0,   1.f};
Color_t    c_Red    = {   1.f,  0,   0,   1.f};
Color_t    c_Green    = {   0,    1.f, 0,   1.f};
Color_t    c_Blue    = {   0,    0,   1.f, 1.f};
Color_t    c_Yellow = {  1.f,  1.f, 0,   1.f};
Color_t    c_White    = {   1.f,  1.f, 1.f, 1.f};
Color_t    c_Grey =  {   0.75, 0.75,0.75,1.f};

int StartSeconds = 0;

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
    return Result;
}

int GetCurrentFilePosition(FILE *Fp)
{
    return ftell(Fp);
}

void SkipFileSection(FILE *InFile,int SectionSize)
{
    if( !InFile ) {
        printf("SkipFileSection: Invalid file.\n");
        return;
    }
    int CurrentSection = ftell(InFile);
    fseek(InFile,SectionSize,SEEK_CUR);
    assert((ftell(InFile) - CurrentSection) == SectionSize);
}

Vec3_t Vec3_Build(float x,float y,float z)
{
    Vec3_t Temp;

    Temp.x = x;
    Temp.y = y;
    Temp.z = z;

    return Temp;
}

float Vec_Length(Vec3_t Vector)
{
    return sqrt (Vector.x*Vector.x + Vector.y*Vector.y + Vector.z*Vector.z);
}

void Vec_RotateXAxis(float Theta,Vec3_t *Vector)
{
    Vector->x = Vector->x;
    Vector->y = Vector->y*cos(Theta) - Vector->z*sin(Theta);
    Vector->z = Vector->y*sin(Theta) + Vector->z*cos(Theta);
}

void Vec_Normalize(Vec3_t *VOut)
{
    VOut->x = VOut->x / Vec_Length(*VOut);
    VOut->y = VOut->y / Vec_Length(*VOut);
    VOut->z = VOut->z / Vec_Length(*VOut);
}

void Vec_Add(Vec3_t A,Vec3_t B,Vec3_t *VOut)
{
    VOut->x = A.x + B.x;
    VOut->y = A.y + B.y;
    VOut->z = A.z + B.z;
}

void Vec_Subtract(Vec3_t A,Vec3_t B,Vec3_t *VOut)
{
    VOut->x = A.x - B.x;
    VOut->y = A.y - B.y;
    VOut->z = A.z - B.z;
}

void Vec_Cross( Vec3_t A, Vec3_t B, Vec3_t *Out ) {
    Out->x = A.y*B.z - A.z*B.y;
    Out->y = A.z*B.x - A.x*B.z;
    Out->z = A.x*B.y - A.y*B.x;
}

void Vec_Scale(Vec3_t InVec,float Amount,Vec3_t *OutVec)
{
    OutVec->x = InVec.x * Amount;
    OutVec->y = InVec.y * Amount;
    OutVec->z = InVec.z * Amount;
}

void Cam_Init(ViewParm_t *Camera)
{
    Camera->Position = BSDGetPlayerSpawn(Level->BSD);

    Camera->OldPosition = Camera->Position;

    Camera->Angle.x = 0.0;
    Camera->Angle.y = 0.0;
    Camera->Angle.z = 0.0;

    Camera->Up = Vec3_Build(0.0f,1.0f,0.0f);
    Camera->Right = Vec3_Build(1.0f,0.0f,0.0f);
    Camera->Forward = Vec3_Build(0.0f,0.0f,1.0f);
    
}

void Cam_FixAngles(ViewParm_t *Camera)
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

void Cam_MouseEvent(ViewParm_t *Camera,int Dx,int Dy)
{
    Camera->Angle.x += ( Dy - (VidConf.Height/2) ) / 10.0f; // 0.001f;
    Camera->Angle.y += ( Dx - (VidConf.Width/2)) / 10.0f; // 0.001f;
    Cam_FixAngles(Camera);
}

void Cam_Update(ViewParm_t *Camera,int Orientation, float Sensibility)
{
    switch ( Orientation ) {
        case DIR_FORWARD:
            Camera->Forward.x = sin(DEGTORAD(Camera->Angle.y));
            Camera->Forward.z = -cos(DEGTORAD(Camera->Angle.y));
            Camera->Forward.y = -sin(DEGTORAD(Camera->Angle.x));
            Vec_Scale(Camera->Forward,Sensibility,&Camera->Forward);
            Vec_Add(Camera->Position,Camera->Forward,&Camera->Position);
            break;
        case DIR_BACKWARD:
            Camera->Forward.x = -sin(DEGTORAD(Camera->Angle.y));
            Camera->Forward.z = cos(DEGTORAD(Camera->Angle.y));
            Camera->Forward.y = sin(DEGTORAD(Camera->Angle.x));
            Vec_Scale(Camera->Forward,Sensibility,&Camera->Forward);
            Vec_Add(Camera->Position,Camera->Forward,&Camera->Position);
            break;
        case DIR_UPWARD:
            Camera->Position.y += Sensibility;
            break;
        case DIR_DOWNWARD:
            Camera->Position.y -= Sensibility;
            break;
        case DIR_LEFTWARD:
            Camera->Right.x = -(float)(cos(DEGTORAD(Camera->Angle.y)));
            Camera->Right.z = -(float)(sin(DEGTORAD(Camera->Angle.y)));
            Vec_Scale(Camera->Right,Sensibility,&Camera->Right);
            Vec_Add(Camera->Position,Camera->Right,&Camera->Position);
            break;
        case DIR_RIGHTWARD:
            Camera->Right.x = (float)(cos(DEGTORAD(Camera->Angle.y)));
            Camera->Right.z = (float)(sin(DEGTORAD(Camera->Angle.y)));
            Vec_Scale(Camera->Right,Sensibility,&Camera->Right);
            Vec_Add(Camera->Position,Camera->Right,&Camera->Position);
            break;
         case LOOK_LEFT:
             Camera->Angle.y -= Sensibility;
             if ( Camera->Angle.y < -360.f ) {
                 Camera->Angle.y += 360.f;
             }
             break;
         case LOOK_RIGHT:
             Camera->Angle.y += Sensibility;
             if ( Camera->Angle.y > 360.f ) {
                 Camera->Angle.y -= 360.f;
             }
             break;
         case LOOK_DOWN:
             Camera->Angle.x += Sensibility;
             break;
         case LOOK_UP:
             Camera->Angle.x -= Sensibility;
             break;
        default:
            break;
    }
}

int Sys_Milliseconds()
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

char *String_Copy(const char *From)
{
    char *Dest;

    Dest = malloc(strlen(From) + 1);

    if ( !Dest ) {
        return NULL;
    }

    strcpy(Dest, From);

    return Dest;
}


bool Vid_InitSDL()
{
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        return false;
    } else {
        DPrintf("SDL Subsystem initialized.\n");
        return true;
    }
    return false;
}

bool Vid_OpenWindow()
{
    //Make sure we have an OpenGL context.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    VideoSurface = SDL_CreateWindow(VidConf.Title,SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                     VidConf.Width, VidConf.Height, SDL_WINDOW_OPENGL);
//     SDL_SetWindowTitle(VideoSurface,);
    SDL_GL_CreateContext(VideoSurface);
    VidConf.Initialized = true;
    SDL_GL_SetSwapInterval(1);
    
    return true;
}

void Sys_HideCursor()
{
    SDL_ShowCursor(false);
}

void Sys_ShowCursor()
{
    SDL_ShowCursor(true);
}

void Sys_CenterCursor()
{
    SDL_WarpMouseInWindow(VideoSurface,VidConf.Width/2,VidConf.Height/2);
}

void Sys_SwapBuffers()
{
    if( !VidConf.Initialized ){
        return;
    }
    SDL_GL_SwapWindow(VideoSurface);
}

void InitSDL(const char *Title,int Width,int Height,bool Fullscreen)
{
    GLenum GlewError;
    if ( !VidConf.Initialized ) {
        DPrintf("Vidconf isn't initialized...\n");
    }

    VidConf.Title = Title == NULL ? "Unnamed" : Title;
    DPrintf("Title:%s\n",Title);
    VidConf.Width = Width;
    VidConf.Height = Height;
    VidConf.Fullscreen = Fullscreen;
    VidConf.Resizable = false;

    if ( !Vid_InitSDL() ) {
        DPrintf("Failed on initializing SDL.\n");
    }
    
    if ( !Vid_OpenWindow() ) {
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

    
    VidConf.Driver = String_Copy("SDL");
    Sys_HideCursor();
    return;
}

void Sys_VidInit(int Width, int Height, bool Fullscreen)
{
    InitSDL("MOH Level Viewer",Width,Height,Fullscreen);
}

void Sys_CheckKeyEvents()
{
    SDL_Event Event;
    float CamSpeed = 40.f;
    while( SDL_PollEvent(&Event) ) {
        switch( Event.type ) {
            case SDL_MOUSEMOTION:
                Cam_MouseEvent(&Camera,Event.motion.x,Event.motion.y);
                Sys_CenterCursor();
                break;
            case SDL_KEYDOWN:
                if( Event.key.keysym.sym == SDLK_ESCAPE ) {
                    Quit();
                }
                if( Event.key.keysym.sym == SDLK_q ) {
                    //
                    // Toggle between WireFrame and Normal mode.
                    //
                    Level->Settings.WireFrame = !Level->Settings.WireFrame;
                }
                if( Event.key.keysym.sym == SDLK_c ) {
                    //
                    // Toggle to show Collision data or not.
                    //
                    Level->Settings.ShowCollisionData = !Level->Settings.ShowCollisionData;
                }
                if( Event.key.keysym.sym == SDLK_b ) {
                    //
                    // Toggle to show AABB data or not.
                    //
                    Level->Settings.ShowAABBTree = !Level->Settings.ShowAABBTree;
                }
                if( Event.key.keysym.sym == SDLK_l ) {
                    //
                    // Toggle to show level data or not.
                    //
                    Level->Settings.ShowMap = !Level->Settings.ShowMap;
                }
                if( Event.key.keysym.sym == SDLK_n ) {
                    //
                    // Toggle to show bsd nodes or not.
                    //
                    Level->Settings.ShowBSDNodes = !Level->Settings.ShowBSDNodes;
                }
                if( Event.key.keysym.sym == SDLK_r ) {
                    //
                    // Toggle to show bsd render object as points.
                    //
                    Level->Settings.ShowBSDRenderObject = !Level->Settings.ShowBSDRenderObject;
                }
                if( Event.key.keysym.sym == SDLK_p ) {
                    //
                    // Toggle to render bsd RenderObject.
                    //
                    Level->Settings.DrawBSDRenderObjects = !Level->Settings.DrawBSDRenderObjects;
                }
                if( Event.key.keysym.sym == SDLK_i ) {
                    //
                    // Toggle to render all the available RenderObject.
                    //
                    Level->Settings.DrawBSDShowCaseRenderObject = !Level->Settings.DrawBSDShowCaseRenderObject;
                }
                if( Event.key.keysym.sym == SDLK_f ) {
                    //
                    // Toggle Frustum Culling
                    //
                    Level->Settings.EnableFrustumCulling = !Level->Settings.EnableFrustumCulling;
                }
                if( Event.key.keysym.sym == SDLK_g ) {
                    //
                    // Toggle Level Lighting
                    //
                    Level->Settings.EnableLighting = !Level->Settings.EnableLighting;
                }
                if( Event.key.keysym.sym == SDLK_w ) {
                    Cam_Update(&Camera, DIR_FORWARD, CamSpeed * ComTime->Delta);
                }
                if( Event.key.keysym.sym == SDLK_s ) {
                    Cam_Update(&Camera, DIR_BACKWARD, CamSpeed * ComTime->Delta);
                }
                if( Event.key.keysym.sym == SDLK_a ) {
                    Cam_Update(&Camera, DIR_LEFTWARD, CamSpeed * ComTime->Delta);
                }
                if( Event.key.keysym.sym == SDLK_d ) {
                    Cam_Update(&Camera, DIR_RIGHTWARD, CamSpeed * ComTime->Delta);
                }
                if( Event.key.keysym.sym == SDLK_SPACE ) {
                    Cam_Update(&Camera, DIR_UPWARD, CamSpeed * ComTime->Delta);
                }
                if( Event.key.keysym.sym == SDLK_z ) {
                    Cam_Update(&Camera, DIR_DOWNWARD, CamSpeed * ComTime->Delta);
                }
//                 if( Event.key.keysym.sym == SDLK_LEFT ) {
//                     Cam_Update(&Camera, LOOK_LEFT, CamSpeed * ComTime->Delta);
//                 }
//                 if( Event.key.keysym.sym == SDLK_RIGHT ) {
//                     Cam_Update(&Camera, LOOK_RIGHT, CamSpeed * ComTime->Delta);
//                 }
//                 if( Event.key.keysym.sym == SDLK_UP ) {
//                     Cam_Update(&Camera, LOOK_UP, CamSpeed * ComTime->Delta);
//                 }
//                 if( Event.key.keysym.sym == SDLK_DOWN ) {
//                     Cam_Update(&Camera, LOOK_DOWN, CamSpeed * ComTime->Delta);
//                 }
                Cam_FixAngles(&Camera);
                break;
            case SDL_QUIT:
                Quit();
                break;
            default:
                break;
        }
    }
    
}

bool Com_UpdateDelta()
{
    long now = Sys_Milliseconds();
    ComTime->UpdateLength = now - ComTime->LastLoopTime;
    int TimeSlice = /*floor*/((1/MAX_FPS) * 1000);

    ComTime->Optimal_Time = 1000 / MAX_FPS;
    ComTime->UpdateLength = now - ComTime->LastLoopTime;
    ComTime->Delta = ComTime->UpdateLength / ( ComTime->Optimal_Time);

    if( ComTime->UpdateLength < TimeSlice ) {
        return false;
    }

    // update the frame counter
    ComTime->LastFpsTime += ComTime->UpdateLength;
    ComTime->Fps++;
    ComTime->LastLoopTime = now;
    // update our FPS counter if a second has passed since
    // we last recorded
    if (ComTime->LastFpsTime >= 1000 ) {
        sprintf(ComTime->FpsString,"FPS:%i | Ms: %.2f ms | Last FPS Time:%f | Delta:%f",
                ComTime->Fps,
                1000.f/(float)ComTime->Fps,
                ComTime->LastFpsTime,ComTime->Delta);
        sprintf(ComTime->FpsSimpleString,"FPS %i Ms %.2f ms",
                ComTime->Fps,
                1000.f/(float)ComTime->Fps);
        DPrintf("%s\n",ComTime->FpsString);
        DPrintf("Current Camera Position:%f;%f;%f\n",Camera.Position.x,Camera.Position.y,Camera.Position.z);
        ComTime->LastFpsTime = 0;
        ComTime->Fps = 0;
    }
    return true;
}

void GL_DrawVec3Box(BBox_t Box)
{
#if 0
    glBegin(GL_LINE_LOOP);
    glVertex3f(Box.Min.x, Box.Min.y, Box.Min.z);
    glVertex3f(Box.Min.x, Box.Min.y, Box.Max.z);
    glVertex3f(Box.Max.x, Box.Min.y, Box.Max.z);
    glVertex3f(Box.Max.x, Box.Min.y, Box.Min.z);
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    glVertex3f(Box.Min.x, Box.Max.y, Box.Min.z);
    glVertex3f(Box.Min.x, Box.Max.y, Box.Max.z);
    glVertex3f(Box.Max.x, Box.Max.y, Box.Max.z);
    glVertex3f(Box.Max.x, Box.Max.y, Box.Min.z);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex3f(Box.Min.x, Box.Min.y, Box.Min.z);
    glVertex3f(Box.Min.x, Box.Max.y, Box.Min.z);
    glVertex3f(Box.Min.x, Box.Min.y, Box.Max.z);
    glVertex3f(Box.Min.x, Box.Max.y, Box.Max.z);

    glVertex3f(Box.Max.x, Box.Min.y, Box.Min.z);
    glVertex3f(Box.Max.x, Box.Max.y, Box.Min.z);
    glVertex3f(Box.Max.x, Box.Min.y, Box.Max.z);
    glVertex3f(Box.Max.x, Box.Max.y, Box.Max.z);
    glEnd();
#endif
}

void GL_DrawPoint(Vec3_t Position,Color_t Color)
{
#if 0
    glColor4f(Color.r,Color.g,Color.b,Color.a);
    //DPrintf("Drawing at %f);%f\n",Position.x,Position.y);
    glDisable( GL_TEXTURE_2D );
    glPointSize(2);
    glBegin(GL_POINTS);
    glVertex2f(Position.x,Position.y/*,Position.z*/);
    glEnd();
    glColor3f(1,1,1);
    glEnable(GL_TEXTURE_2D);
#endif
}



/*
    The coordinate system used by PSX was
    (0,0) => Upper left corner
    (Width,Height) => Lower right corner
    or maybe not?
*/
void GL_Set2D()
{
#if 0
   glViewport( 0, 0, VidConf.Width, VidConf.Height );
   glScissor( 0, 0,  VidConf.Width, VidConf.Height );
//    glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, VidConf.Width,VidConf.Height,0,-1,1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glDisable (GL_DEPTH_TEST);
   glDisable (GL_CULL_FACE);
   glDisable (GL_BLEND);
   glEnable (GL_ALPHA_TEST);
#endif

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
void GL_SetProjectionMatrix()
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

void GL_Set3D()
{
#if 0
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glDisable (GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);

    GL_SetProjectionMatrix();

    //glLoadIdentity();
    glDepthMask(GL_TRUE);
    glMatrixMode( GL_PROJECTION );

    glViewport(0,0,VidConf.Width,VidConf.Height);

    glLoadMatrixf( VidConf.PMatrix );

    glMatrixMode( GL_MODELVIEW );

    glLoadIdentity( );
#endif
}

void GL_SetDefaultState()
{
//     glShadeModel( GL_SMOOTH );

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    glClearDepth( 1.0f );

    glDepthFunc( GL_LEQUAL );

    glEnable( GL_DEPTH_TEST );

//     glEnableClientState( GL_VERTEX_ARRAY );
}

void InitGLView()
{
    GL_SetDefaultState();
    //GL_Set2D();
    GL_Set3D();
}


void GLFrame()
{
    float y;
    float VerticalSpacing = 10.f;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm_perspective(glm_rad(110.f),(float) VidConf.Width/ (float) VidConf.Height,1.f, 4096.f,VidConf.PMatrixM4);
// 
// #if 0
//     GL_Set3D();
//     glRotatef(Camera.Angle.x, 1.0f, 0.0f, 0.0f);
//     glRotatef(Camera.Angle.y, 0.0f, 1.0f, 0.0f);
//     glRotatef(Camera.Angle.z, 0.0f, 0.0f, 1.0f);
//     glTranslatef(-Camera.Position.x, -Camera.Position.y, -Camera.Position.z);
// #endif
     vec3 temp;
     temp[0] = 1;
     temp[1] = 0;
     temp[2] = 0;
     glm_mat4_identity(VidConf.ModelViewMatrix);
     glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera.Angle.x), temp);
     temp[0] = 0;
     temp[1] = 1;
     temp[2] = 0;
     glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera.Angle.y), temp);
     temp[0] = 0;
     temp[1] = 0;
     temp[2] = 1;
     glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera.Angle.z), temp);
     temp[0] = -Camera.Position.x;
     temp[1] = -Camera.Position.y;
     temp[2] = -Camera.Position.z;
     glm_translate(VidConf.ModelViewMatrix,temp);
     
     glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
     
     //Emulate PSX Coordinate system...
     glm_rotate_x(VidConf.MVPMatrix,glm_rad(180.f), VidConf.MVPMatrix);
     
     glm_frustum_planes(VidConf.MVPMatrix,Camera.FrustumPlaneList);
     glm_frustum_corners(VidConf.MVPMatrix,Camera.FrustumCornerList);
     
     /* TEMP! */
     BSDCheckCompartmentTrigger(Level,Camera.Position);
     DrawTSPList(Level);
     BSD2PDraw(Level);
     BSDDraw(Level);
     
     glm_mat4_identity(VidConf.MVPMatrix);
    
    // 2D Drawing
    glm_mat4_identity(VidConf.PMatrixM4);
    glm_ortho(0,VidConf.Width,VidConf.Height,0,-1,1,VidConf.PMatrixM4);
    glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
    y = 100;
    FontDrawString(Level,ComTime->FpsSimpleString,10,y,c_White);
    y += VerticalSpacing;
    FontDrawString(Level,"Press ESC to exit",10,y,c_White);
    y += VerticalSpacing;
    FontDrawString(Level,"Press c to show or hide collision data",10,y,Level->Settings.ShowCollisionData ? c_Yellow : c_Red);
    y += VerticalSpacing;
    FontDrawString(Level,"Press b to show or hide BSP tree data",10,y,Level->Settings.ShowAABBTree ? c_Yellow : c_Red);
    y += VerticalSpacing;
    FontDrawString(Level,"Press i to show or hide BSD showcase",10,y,Level->Settings.DrawBSDShowCaseRenderObject ? c_Yellow : c_Red);
    y += VerticalSpacing;
    FontDrawString(Level,"Press l to show or hide the level",10,y,Level->Settings.ShowMap ? c_Yellow : c_Red);
    y += VerticalSpacing;
    FontDrawString(Level,"Press n to show or hide BSD nodes as points",10,y,Level->Settings.ShowBSDNodes ? c_Yellow : c_Red);
    y += VerticalSpacing;
    FontDrawString(Level,"Press p to show or hide BSD RenderObject",10,y,Level->Settings.DrawBSDRenderObjects ? c_Yellow : c_Red);
    y += VerticalSpacing;
    FontDrawString(Level,"Press r to show or hide BSD RenderObject as points",10,y,Level->Settings.ShowBSDRenderObject ? c_Yellow : c_Red);
    y += VerticalSpacing;
    FontDrawString(Level,"Press q to enable or disable wireframe mode",10,y,Level->Settings.WireFrame ? c_Yellow : c_Red);
    y += VerticalSpacing;
    FontDrawString(Level,"Press f to enable or disable frustum culling",10,y,Level->Settings.EnableFrustumCulling ? c_Yellow : c_Red);
    y += VerticalSpacing;
    FontDrawString(Level,"Press g to enable or disable level Lighting",10,y,Level->Settings.EnableLighting ? c_Yellow : c_Red);
    y += VerticalSpacing;
    FontDrawString(Level,"Press w a s d to move camera around",10,y,c_White);
//     FontDrawString(Level,"a b c d e f g h i j k l m n o p q r s t u v w x y z",0,VidConf.Height / 2);
//     FontDrawString(Level,"A B C D E F G H I J K L M N O P Q R S T U V W X Y Z",0,(VidConf.Height / 2 ) + 10);
//     FontDrawString(Level,"0 1 2 3 4 5 6 7 8 9 10",0,(VidConf.Height / 2 ) + 20);
//     FontDrawString(Level,"? ! \" ' ",0,(VidConf.Height / 2 ) + 30);
}

void SetDefaultSettings(Level_t *Level)
{
    Level->Settings.ShowMap = true;
    Level->Settings.ShowBSDNodes = true;
    Level->Settings.ShowBSDRenderObject = true;
    Level->Settings.DrawBSDRenderObjects = true;
    Level->Settings.DrawBSDShowCaseRenderObject = false;
    Level->Settings.EnableFrustumCulling = true;
    Level->Settings.EnableLighting = true;
}
bool LevelInit(char *Directory,char *MissionNumber,char *LevelNumber)
{
    TSP_t *TSP;
    char Buffer[512];
    int i;
    
    if( Level != NULL ) {
        free(Level);
    }
    Level = malloc(sizeof(Level_t));
    Level->Font = NULL;
    Level->VRam = NULL;
    Level->TSPList = NULL;
    Level->ImageList = NULL;
    SetDefaultSettings(Level);
    Level->MissionNumber = StringToInt(MissionNumber);
    Level->LevelNumber = StringToInt(LevelNumber);

    strcpy(Level->BasePath,Directory);
    snprintf(Level->MissionPath,sizeof(Level->MissionPath),"%s/DATA/MSN%i/LVL%i",Directory,Level->MissionNumber,Level->LevelNumber);
    
    DPrintf("Working directory:%s\n",Directory);
    DPrintf("Loading level %s Mission %i Level %i\n",Level->MissionPath,Level->MissionNumber,Level->LevelNumber);

    //Step.1 Load all the tims from taf.
    //0 is hardcoded...for the images it doesn't make any difference between 0 and 1
    //but if we need to load all the level sounds then 0 means Standard Mode while 1 American (All voices are translated to english!).
    snprintf(Buffer,sizeof(Buffer),"%s/%i_%i0.TAF",Level->MissionPath,Level->MissionNumber,Level->LevelNumber);
//     snprintf(Buffer,sizeof(Buffer),"%s/DATA/TWOPLAYR/PLAYERS/B/GI_P1.TAF",Level->BasePath);
    Level->ImageList = GetAllTimImages(Buffer);
    //Step.2 Load the BSD file.
    snprintf(Buffer,sizeof(Buffer),"%s/%i_%i.BSD",Level->MissionPath,Level->MissionNumber,Level->LevelNumber);
    Level->BSD = BSDLoad(Buffer,Level->MissionNumber);
    if( !Level->BSD ) {
        DPrintf("Couldn't load BSD...aborting.\n");
        return false;
    }
    //Step.3 Load all the TSP file based on the data read from the BSD file.
    //Note that we are going to load all the tsp file since we do not know 
    //where in the bsd file it signals to stream/load the next tsp.
    for( i = Level->BSD->TSPInfo.StartingComparment; i <= Level->BSD->TSPInfo.TargetInitialCompartment; i++ ) {
       Level->TSPNumberRenderList[i] = i;
    }
    for( i = Level->BSD->TSPInfo.StartingComparment; i <= Level->BSD->TSPInfo.NumTSP; i++ ) {
        snprintf(Buffer,sizeof(Buffer),"%s/TSP0/%i_%i_C%i.TSP",Level->MissionPath,Level->MissionNumber,Level->LevelNumber,i);
        TSP = TSPLoad(Buffer,i);
        TSP->Next = Level->TSPList;
        Level->TSPList = TSP;
    }
    //TESTING PURPOSES ONLY!
    snprintf(Buffer,sizeof(Buffer),"%s/DATA/TWOPLAYR/PLAYERS/B/GI_P1.BSD",Level->BasePath);
    Level->BSDTwoP = BSD2PLoad(Buffer,Level->MissionNumber);
    if( !Level->BSDTwoP ) {
        DPrintf("Couldn't load BSD2P...aborting.\n");
        return false;
    }
    return true;
    
}

/*
    Late level initialization for all the subsystems that
    requires a valid OpenGL context.
*/
void LevelLateInit()
{
    ShaderManagerInit();
    Level->VRam = VRamInit(Level->ImageList);
    Level->Font = FontInit();
//     DPrintf("OpenGL Version:%s\n",glGetString(GL_VERSION));
    /* TEMP! */
    
    TSPCreateVAO(Level->TSPList);
    TSPCreateNodeBBoxVAO(Level->TSPList);
    TSPCreateCollisionVAO(Level->TSPList);
    BSDVAOPointList(Level->BSD);
    BSD2PVAOPointList(Level->BSDTwoP);
    BSDVAOObjectList(Level->BSD);
    BSDVAOTexturedObjectList(Level->BSD);
    BSDSpawnNodes(Level->BSD);
    BSDSpawnShowCase(Level->BSD);
    BSDFixRenderObjectPosition(Level);

}
void LevelCleanUp()
{
    BSDFree(Level->BSD);
    BSD2PFree(Level->BSDTwoP);
    TSPFreeList(Level->TSPList);
    TimImageListFree(Level->ImageList);
    free(Level->VRam);
    FontFree(Level->Font);
    free(Level);
}

void Quit()
{
    LevelCleanUp();
    ShaderManagerFree();
    free(ComTime);
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
    if( argc != 4 ) {
        printf("%s <MOH Directory> <Mission Number> <Level Number> will load level files from that mission.\n",argv[0]);
        return -1;
    }
    if( !LevelInit(argv[1],argv[2],argv[3]) ) {
        printf("Couldn't load data.\n");
        return -1;
    }

#if _ENABLEVIDEOOUT
    Sys_VidInit(1366,768,false);
    ComTime = malloc(sizeof(ComTimeInfo_t));
    memset(ComTime,0,sizeof(ComTimeInfo_t));
    InitGLView();
    Cam_Init(&Camera);
    LevelLateInit();
    /* TEMP! */
    while( 1 ) {
        Sys_CheckKeyEvents();
        do {
        } while( !Com_UpdateDelta() );
        GLFrame();
        glFlush();
        Sys_SwapBuffers();
    }
#else
    #ifdef _DEBUG
    Quit();
    #endif
#endif
// 
    return 0;
}
