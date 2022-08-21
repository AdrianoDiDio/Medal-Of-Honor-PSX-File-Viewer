// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
    MOHModelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MOHModelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MOHModelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#include "MOHModelViewer.h"
#include "ShaderManager.h"

Color4f_t    c_Black = {   0,    0,   0,   1.f};
Color4f_t    c_Red    = {   1.f,  0,   0,   1.f};
Color4f_t    c_Green    = {   0,    1.f, 0,   1.f};
Color4f_t    c_Blue    = {   0,    0,   1.f, 1.f};
Color4f_t    c_Yellow = {  1.f,  1.f, 0,   1.f};
Color4f_t    c_White    = {   1.f,  1.f, 1.f, 1.f};
Color4f_t    c_Grey =  {   0.75, 0.75,0.75,1.f};

Byte HighNibble(Byte In)
{
    return (In >> 0x4) & 0xF;
}

Byte LowNibble(Byte In)
{
    return In & 0xF;
}

int SignExtend(int Temp)
{
    if ( Temp & 0x8000 ) {
        Temp |= 0xffff0000;
    }
    return Temp;
}

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

char *StringToUpper(const char *In)
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

int StringToInt(const char *String)
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

char *GetBaseName(const char *Path)
{
    char *Out;
    Out = strrchr(Path, PATH_SEPARATOR);
    if (Out == NULL) {
        return strdup(Path);
    } else {
        //Skip remaining dir separator.
        Out++;
    }
    return strdup(Out);
}

void CreateDirIfNotExists(const char *DirName) {
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
    int StrippedFileNameLength;
    char *Temp;
    
    if ( !In || !Ext ) {
        return NULL;
    }

    Temp = strrchr(In,'.');
    if( Temp ) {
        StrippedFileNameLength = Temp - In;
    } else {
        StrippedFileNameLength = strlen(In);
    }
    NewExt = malloc(StrippedFileNameLength + strlen(Ext) + 1);
    if( !NewExt ) {
        return NULL;
    }
    strncpy(NewExt,In,StrippedFileNameLength);
    NewExt[StrippedFileNameLength] = '\0';
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

char *ReadTextFile(const char *File,int Length)
{
    FILE *Fp;
    int FileSize;
    char *Result;
    int Ret;
    
    Fp = fopen(File,"rb");
    
    if( !Fp ) {
        DPrintf("ReadTextFile:File %s was not found.\n",File);
        return NULL;
    }
    FileSize = Length != 0 ? Length : GetFileLength(Fp);
    Result = malloc(FileSize + 1);
    
    if( !Result ) {
        DPrintf("ReadTextFile:Failed to allocate buffer\n");
        fclose(Fp);
        return NULL;
    }
    Ret = fread(Result,1, FileSize,Fp);
    fclose(Fp);
    
    if( Ret != FileSize ) {
        DPrintf("Failed to read file %s\n",File);
        free(Result);
        return NULL;
    }
    Result[Ret] = '\0';
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

char *SysGetConfigPath()
{
    return SDL_GetPrefPath(NULL,"MOHModelViewer");
}
int SysMilliseconds()
{
    return SDL_GetTicks64();
}

void DPrintf(const char *Fmt, ...)
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

void SysHideCursor()
{
    SDL_ShowCursor(false);
}

void SysShowCursor()
{
    SDL_ShowCursor(true);
}

void EngineCheckEvents(Engine_t *Engine)
{
    SDL_Event Event;
    BSDRenderObject_t *CurrentRenderObject;
    while( SDL_PollEvent(&Event) ) {
        if( Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            ConfigSetNumber("VideoWidth",Event.window.data1);
            ConfigSetNumber("VideoHeight",Event.window.data2);
        }
        if( Event.type == SDL_QUIT || (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE ) ) {
            Quit(Engine);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_m ) {
            CurrentRenderObject = RenderObjectManagerGetSelectedRenderObject(Engine->RenderObjectManager);
            if( CurrentRenderObject != NULL ) {
                int NextPose = (CurrentRenderObject->CurrentAnimationIndex + 1);
                BSDRenderObjectSetAnimationPose(CurrentRenderObject,NextPose % CurrentRenderObject->NumAnimations);
            }
        }
        if( Event.type == SDL_MOUSEMOTION ) {
//             CameraOnMouseEvent(Engine->Camera,Event.motion.xrel,Event.motion.yrel);
        }
        GUIProcessEvent(Engine->GUI,&Event);
    }
}

void ComUpdateDelta(ComTimeInfo_t *TimeInfo)
{
    long Now;
    
    Now = SysMilliseconds();

    TimeInfo->UpdateLength = Now - TimeInfo->LastLoopTime;
    TimeInfo->Delta = TimeInfo->UpdateLength * 0.001f;


    // Update the frame counter
    TimeInfo->LastFPSTime += TimeInfo->UpdateLength;
    TimeInfo->FPS++;
    TimeInfo->LastLoopTime = Now;
    // Update our FPS counter if a second has passed since
    // we last recorded
    if (TimeInfo->LastFPSTime >= 1000 ) {
        sprintf(TimeInfo->FPSString,"FPS:%i\nMs: %.2f ms\nLast FPS Time:%f\nDelta:%f",
                TimeInfo->FPS,
                1000.f/(float)TimeInfo->FPS,
                TimeInfo->LastFPSTime,TimeInfo->Delta);
        sprintf(TimeInfo->FPSSimpleString,"FPS %i Ms %.2f ms",
                TimeInfo->FPS,
                1000.f/(float)TimeInfo->FPS);
        DPrintf("%s\n",TimeInfo->FPSString);
        TimeInfo->LastFPSTime = 0;
        TimeInfo->FPS = 0;
    }
}

/*
 Suppressed messages:
 Id = 131204 => Message:Texture state usage warning.
 Id = 131185 => Buffer Usage Hint.
 */
void GLDebugOutput(GLenum Source, GLenum Type, unsigned int Id, GLenum Severity, GLsizei Length, const char *Message, const void *UserParam)
{
    if( Id == 131204 || Id == 131185) {
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
            DPrintf("Source: Third Party"); 
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
            DPrintf("Severity: Medium"); 
            break;
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
}

void Quit(Engine_t *Engine)
{
    EngineShutDown(Engine);
    ShaderManagerFree();
    ConfigFree();
    exit(0);
}
void EngineQuitSDL()
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Quit();
}
void EngineShutDown(Engine_t *Engine)
{
    if( !Engine ) {
        return;
    }
    if( Engine->TimeInfo ) {
        free(Engine->TimeInfo);
    }
    if( Engine->VideoSystem ) {
        VideoSystemShutdown(Engine->VideoSystem);
    }
    if( Engine->RenderObjectManager ) {
        RenderObjectManagerCleanUp(Engine->RenderObjectManager);
    }
    if( Engine->GUI ) {
        GUIFree(Engine->GUI);
    }
    if( Engine->Camera ) {
        CameraCleanUp(Engine->Camera);
    }
    EngineQuitSDL();
    free(Engine);
}

void EngineDraw(Engine_t *Engine)
{
    if( !Engine ) {
        DPrintf("EngineDraw:Called without a valid engine\n");
        return;
    }
    glViewport(0,0,VidConfigWidth->IValue,VidConfigHeight->IValue);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    RenderObjectManagerDrawAll(Engine->RenderObjectManager,Engine->Camera);
    
    glDisable (GL_DEPTH_TEST);
    GUIDraw(Engine->GUI,Engine->RenderObjectManager,Engine->Camera,Engine->VideoSystem,Engine->TimeInfo,Engine->KeyState);
    glEnable(GL_DEPTH_TEST);
}
void EngineFrame(Engine_t *Engine)
{
    if( !Engine ) {
        DPrintf("EngineFrame:Called without a valid engine\n");
        return;
    }
    ComUpdateDelta(Engine->TimeInfo);
    EngineCheckEvents(Engine);
    CameraBeginFrame(Engine->Camera);
    EngineDraw(Engine);
    VideoSystemSwapBuffers(Engine->VideoSystem);
}

int EngineInitSDL()
{
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ) {
        return false;
    }
    return true;
}
Engine_t *EngineInit(int argc,char **argv)
{
    Engine_t *Engine;
    
    Engine = malloc(sizeof(Engine_t));
    
    if( !Engine ) {
        printf("EngineInit:Failed to allocate memory for engine\n");
        return NULL;
    }
    
    Engine->TimeInfo = NULL;
    Engine->VideoSystem = NULL;
    Engine->GUI = NULL;
    Engine->Camera = NULL;
    Engine->RenderObjectManager = NULL;
    ConfigInit();

    if( !EngineInitSDL() ) {
        printf("EngineInit:Failed to initialize SDL subsystems.\n");
        goto Failure;
    }
    
    Engine->VideoSystem = VideoSystemInit();
    
    if( !Engine->VideoSystem ) {
        printf("EngineInit:Failed to Initialize Video system...\n");
        goto Failure;
    }
    
    Engine->GUI = GUIInit(Engine->VideoSystem);
    
    if( !Engine->GUI ) {
        printf("EngineInit:Failed to initialize GUI system\n");
        goto Failure;
    }
    
    Engine->Camera = CameraInit();
    
    if( !Engine->Camera ) {
        printf("EngineInit:Failed to Initialize Camera System\n");
        goto Failure;
    }
    
    Engine->KeyState = SDL_GetKeyboardState(NULL);
        
    Engine->TimeInfo = malloc(sizeof(ComTimeInfo_t));
    
    if( !Engine->TimeInfo ) {
        printf("EngineInit:Failed to allocate memory for time info\n");
        goto Failure;
    }
    memset(Engine->TimeInfo,0,sizeof(ComTimeInfo_t));
    
    GLSetDefaultState();
    ShaderManagerInit();
    

    
    Engine->RenderObjectManager = RenderObjectManagerInit(Engine->GUI);
    
    if( !Engine->RenderObjectManager ) {
        printf("EngineInit:Failed to initialize RenderObjectManager\n");
        goto Failure;
    }
    
    if( argc > 1 ) {
        RenderObjectManagerLoadPack(Engine->RenderObjectManager,Engine->GUI,Engine->VideoSystem,argv[1]);
    }
    return Engine;

Failure:
    EngineShutDown(Engine);
    return NULL;
}
#define TEST_ENGINE 0
int main(int argc,char **argv)
{
    Engine_t *Engine;
    
    srand(time(NULL));
    
//     BSDRenderObject_t *RenderObjectList;
//     BSDRenderObject_t *Iterator;
//     DPrintf("Processing %s\n",argv[1]);
//     RenderObjectList = BSDLoadAllAnimatedRenderObjects(argv[1]);
//     for( Iterator = RenderObjectList; Iterator; Iterator = Iterator->Next ) {
//         DPrintf("Setting pose 0 for RenderObject %u\n",Iterator->Id);
//         BSDRenderObjectSetAnimationPose(Iterator,0);
//     }
    
    Engine = EngineInit(argc,argv);
    
    if( !Engine ) {
        printf("Failed to initialize engine...\n");
        EngineShutDown(Engine);
        return -1;
    }
//     BSDRenderObjectGenerateVAO(RenderObjectList);

    while( 1 ) {
        EngineFrame(Engine);
    }
//     BSDFreeRenderObjectList(RenderObjectList);
    return 0;
}
