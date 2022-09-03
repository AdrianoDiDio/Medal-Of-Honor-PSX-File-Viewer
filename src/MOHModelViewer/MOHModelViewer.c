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
#include "../Common/ShaderManager.h"

Color4f_t    c_Black = {   0,    0,   0,   1.f};
Color4f_t    c_Red    = {   1.f,  0,   0,   1.f};
Color4f_t    c_Green    = {   0,    1.f, 0,   1.f};
Color4f_t    c_Blue    = {   0,    0,   1.f, 1.f};
Color4f_t    c_Yellow = {  1.f,  1.f, 0,   1.f};
Color4f_t    c_White    = {   1.f,  1.f, 1.f, 1.f};
Color4f_t    c_Grey =  {   0.75, 0.75,0.75,1.f};


void EngineCheckEvents(Engine_t *Engine)
{
    SDL_Event Event;
    BSDRenderObject_t *CurrentRenderObject;
    int NextFrame;
    int NextPose;
    
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
                NextFrame = (CurrentRenderObject->CurrentFrameIndex + 1) % 
                    CurrentRenderObject->AnimationList[CurrentRenderObject->CurrentAnimationIndex].NumFrames;
                BSDRenderObjectSetAnimationPose(CurrentRenderObject,CurrentRenderObject->CurrentAnimationIndex,NextFrame);
            }
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_n ) {
            CurrentRenderObject = RenderObjectManagerGetSelectedRenderObject(Engine->RenderObjectManager);
            if( CurrentRenderObject != NULL ) {
                NextPose = (CurrentRenderObject->CurrentAnimationIndex + 1) % 
                    CurrentRenderObject->NumAnimations;
                while( !BSDRenderObjectSetAnimationPose(CurrentRenderObject,NextPose,0) ) {
                    NextPose = (NextPose + 1 ) % CurrentRenderObject->NumAnimations;
                }                
            }
        }
        GUIProcessEvent(Engine->GUI,&Event);
        if( GUIIsMouseFree() ) {
            if( Event.type == SDL_MOUSEWHEEL) {
                CameraZoom(Engine->Camera,-Event.wheel.y);
            }
            if( Event.type == SDL_MOUSEMOTION && Event.motion.state & SDL_BUTTON_LMASK ) {
                CameraOnMouseEvent(Engine->Camera,Event.motion.xrel,Event.motion.yrel);
            }
        }
    }
    if( GUIIsKeyboardFree() ) {
        CameraCheckKeyEvents(Engine->Camera,Engine->KeyState,Engine->TimeInfo->Delta);
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
        sprintf(TimeInfo->FPSString,"FPS:%i\nMs: %.3f ms\nLast FPS Time:%.3f\nDelta:%.3f",
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
    
    RenderObjectManagerDraw(Engine->RenderObjectManager,Engine->Camera);
    
    glDisable (GL_DEPTH_TEST);
    GUIDraw(Engine);
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
    RenderObjectManagerUpdate(Engine->RenderObjectManager);
    EngineDraw(Engine);
    VideoSystemSwapBuffers(Engine->VideoSystem);
}
void RegisterDefaultSettings()
{
    ConfigRegister("VideoWidth","800",NULL);
    ConfigRegister("VideoHeight","600",NULL);
    ConfigRegister("VideoRefreshRate","60",NULL);
    ConfigRegister("VideoFullScreen","0",NULL);
    ConfigRegister("VideoVSync","-1","Enable or disable vsync.\nPossible values are:-1 enable adaptive VSync (where supported),\n"
                    "0 Disables it and 1 enables standard VSync.");
    
    ConfigRegister("CameraSpeed","30.f",NULL);
    ConfigRegister("CameraMouseSensitivity","1.f",NULL);
    
    ConfigRegister("GUIFont","Fonts/DroidSans.ttf","Sets the file to be used as the GUI font,if not valid the application will use the default one");
    ConfigRegister("GUIFontSize","14.f",NULL);
    ConfigRegister("GUIShowFPS","1",NULL);
    
    ConfigRegister("EnableWireFrameMode","0","Draw the model surfaces as lines");

    ConfigRegister("EnableAmbientLight","1","When enabled the texture color is interpolated with the surface color to simulate lights on \n"
                                                    "surfaces");

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
    
    RegisterDefaultSettings();
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
