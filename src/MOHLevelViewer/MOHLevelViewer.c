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
#include "../Common/ShaderManager.h"
#include "../Common/Config.h"

Color4f_t    c_Black = {   0,    0,   0,   1.f};
Color4f_t    c_Red    = {   1.f,  0,   0,   1.f};
Color4f_t    c_Green    = {   0,    1.f, 0,   1.f};
Color4f_t    c_Blue    = {   0,    0,   1.f, 1.f};
Color4f_t    c_Yellow = {  1.f,  1.f, 0,   1.f};
Color4f_t    c_White    = {   1.f,  1.f, 1.f, 1.f};
Color4f_t    c_Grey =  {   0.75, 0.75,0.75,1.f};

// char *AppGetConfigPath()
// {
//     return SDL_GetPrefPath(NULL,"MOHLevelViewer");
// }

void EngineCheckEvents(Engine_t *Engine)
{
    SDL_Event Event;
    while( SDL_PollEvent(&Event) ) {
        if( Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            ConfigSetNumber("VideoWidth",Event.window.data1);
            ConfigSetNumber("VideoHeight",Event.window.data2);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F1 ) {
            GUIToggleDebugWindow(Engine->GUI);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F2 ) {
            GUIToggleVideoSettingsWindow(Engine->GUI);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F3 ) {
            GUIToggleLevelSelectWindow(Engine->GUI);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F4 ) {
            LevelManagerToggleFileDialog(Engine->LevelManager,Engine->GUI,Engine->VideoSystem,Engine->SoundSystem);
        }
        if( Event.type == SDL_QUIT || (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE ) ) {
            Quit(Engine);
        }
        
        if( !GUIProcessEvent(Engine->GUI,&Event) ) {
            if( Event.type == SDL_MOUSEMOTION ) {
                CameraOnMouseEvent(Engine->Camera,Event.motion.xrel,Event.motion.yrel);
            }
        }
    }
    //NOTE(Adriano):If the GUI is closed and we pumped all the events then
    //              check if any key is down and update the camera.
    if( !GUIIsActive(Engine->GUI) ) {
        CameraCheckKeyEvents(Engine->Camera,Engine->KeyState,Engine->TimeInfo->Delta);
    }
}

void ComUpdateDelta(ComTimeInfo_t *TimeInfo,Camera_t *Camera)
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
        DPrintf("Current Camera Position:%f;%f;%f\n",Camera->Position[0],Camera->Position[1],Camera->Position[2]);
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
    if( Engine->LevelManager ) {
        LevelManagerCleanUp(Engine->LevelManager);
    }
    if( Engine->GUI ) {
        GUIFree(Engine->GUI);
    }
    if( Engine->TimeInfo ) {
        free(Engine->TimeInfo);
    }
    if( Engine->VideoSystem ) {
        VideoSystemShutdown(Engine->VideoSystem);
    }
    if( Engine->SoundSystem ) {
        SoundSystemCleanUp(Engine->SoundSystem);
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
    LevelManagerDraw(Engine->LevelManager,Engine->Camera);
    glDisable (GL_DEPTH_TEST);
    GUIDraw(Engine->GUI,Engine->LevelManager,Engine->Camera,Engine->VideoSystem,Engine->SoundSystem,Engine->TimeInfo);
    glEnable(GL_DEPTH_TEST);
}
void EngineFrame(Engine_t *Engine)
{
    if( !Engine ) {
        DPrintf("EngineFrame:Called without a valid engine\n");
        return;
    }
    ComUpdateDelta(Engine->TimeInfo,Engine->Camera);
    EngineCheckEvents(Engine);
    CameraBeginFrame(Engine->Camera);
    LevelManagerUpdate(Engine->LevelManager,Engine->Camera);
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

    ConfigRegister("GameBasePath","","Sets the path from which the game will be loaded,any invalid path will result in this variable to "
                                    "being set to empty.");
    ConfigRegister("GUIFont","Fonts/DroidSans.ttf","Sets the file to be used as the GUI font,if not valid the application will use the default one");
    ConfigRegister("GUIFontSize","14.f",NULL);
    ConfigRegister("GUIShowFPS","1",NULL);

    ConfigRegister("LevelEnableWireFrameMode","0","Draw the level surfaces as lines");
    ConfigRegister("LevelDrawCollisionData","0","Draw the level collision data");
    ConfigRegister("LevelDrawBSPTree","0","When enabled draws the BSP tree for the current level.Red box represents a splitter while \n"
                                        "a yellow one a leaf (containing actual level data).");
    ConfigRegister("LevelDrawSurfaces","1","Draw the level surfaces");
    ConfigRegister("LevelDrawBSDNodesAsPoints","1","When enabled draws all the BSD nodes as points.");
    ConfigRegister("LevelDrawBSDNodesCollisionVolumes","0","When enabled draws all the BSD nodes collision volumes.");
    ConfigRegister("LevelDrawBSDRenderObjectsAsPoints","1","When enabled draws all the BSD RenderObjects as Points");
    ConfigRegister("LevelDrawBSDRenderObjects","1","When enabled draws all the supported render objects");
    ConfigRegister("LevelDrawBSDShowcase","0","When enabled draws all the loaded RenderObjects near the player spawn.");
    ConfigRegister("LevelEnableFrustumCulling","1","When enabled helps to skip non visibile nodes from the BSP tree improving the rendering speed");
    ConfigRegister("LevelEnableAmbientLight","1","When enabled the texture color is interpolated with the surface color to simulate lights on \n"
                                                    "surfaces");
    ConfigRegister("LevelEnableSemiTransparency","1","When enabled draw transparent surfaces as non-opaque");
    ConfigRegister("LevelEnableAnimatedLights","1","When enabled some surfaces will interpolate their color to simulate an animated surface.\n"
                                       "NOTE that this will only work if \"LevelEnableAmbientLight\" is enabled");
    ConfigRegister("LevelEnableAnimatedSurfaces","1","When enabled this will make some surfaces change their texture when the camera gets near.\n"
                                       "For example when hovering the camera near an explosive charge an overlay will pulse near the charge.");
    
    ConfigRegister("LevelEnableMusicTrack","1","When enabled sound will be played in background.\n"
                                       "There are 3 possible values:0 Disabled,1 Enable Music And Ambient sounds,2 Enable ambient sounds only.");
    ConfigRegister("SoundVolume","128","Sets the sound volume, the value must be in range 0-128, values outside that range will be clamped.");
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
    
    Engine->LevelManager = NULL;
    Engine->GUI = NULL;
    Engine->Camera = NULL;
    Engine->TimeInfo = NULL;
    Engine->VideoSystem = NULL;
    Engine->SoundSystem = NULL;
    
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
    VideoSystemGrabMouse(1);
    Engine->SoundSystem = SoundSystemInit();
    
    if( !Engine->SoundSystem ) {
        printf("EngineInit:Failed to initialize Audio system\n");
        goto Failure;
    }
    
    Engine->KeyState = SDL_GetKeyboardState(NULL);
    
    Engine->Camera = CameraInit();
    
    if( !Engine->Camera ) {
        printf("EngineInit:Failed to Initialize Camera System\n");
        goto Failure;
    }
    
    Engine->TimeInfo = malloc(sizeof(ComTimeInfo_t));
    if( !Engine->TimeInfo ) {
        printf("EngineInit:Failed to allocate memory for time info\n");
        goto Failure;
    }
    memset(Engine->TimeInfo,0,sizeof(ComTimeInfo_t));
    
    GLSetDefaultState();
    ShaderManagerInit();
    
    //NOTE(Adriano):Allow the game path to be set using command line argument.
    //              If the path is not valid the game will discard it.
    if( argc > 1 ) {
        ConfigSet("GameBasePath",argv[1]);
    }

    Engine->GUI = GUIInit(Engine->VideoSystem);
    if( !Engine->GUI ) {
        printf("EngineInit:Failed to initialize GUI system\n");
        goto Failure;
    }
    
    Engine->LevelManager = LevelManagerInit(Engine->GUI,Engine->VideoSystem,Engine->SoundSystem);
    
    if( !Engine->LevelManager ) {
        printf("EngineInit:Failed to initialize LevelManager\n");
        goto Failure;
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
    
    Engine = EngineInit(argc,argv);
    
    if( !Engine ) {
        printf("Failed to initialize engine...\n");
        EngineShutDown(Engine);
        return -1;
    }
    
#if TEST_ENGINE
    //NOTE(Adriano):Quick test to check if everything is working.
    int StartTime;
    if( argc != 3 ) {
        printf("Engine test failed...Game Path were not specified\n");
        printf("Engine test requires 2 arguments <MOH Path> <MOH:Underground Path>\n");
        Quit(Engine);
        return -1;
    }
    StartTime = SysMilliseconds();
    LevelManagerInitWithPath(Engine->LevelManager,Engine->GUI,Engine->VideoSystem,Engine->SoundSystem,argv[1]);
    for( int i = 0; i < NumMOHMissions; i++ ) {
        //NOTE(Adriano):LevelManagerInitWithPath loads the first level...make sure to skip it.
        for( int j = (MOHMissionsList[i].MissionNumber == 1 ? 1 : 0); j < MOHMissionsList[i].NumLevels; j++ ) {
            assert( LevelManagerLoadLevel(Engine->LevelManager,Engine->GUI,Engine->VideoSystem,Engine->SoundSystem,
                           MOHMissionsList[i].MissionNumber,MOHMissionsList[i].Levels[j].LevelNumber));
        }
    }
    LevelManagerInitWithPath(Engine->LevelManager,Engine->GUI,Engine->VideoSystem,Engine->SoundSystem,argv[2]);
    for( int i = 0; i < NumMOHUMissions; i++ ) {
        //NOTE(Adriano):LevelManagerInitWithPath loads the first level...make sure to skip it.
        for( int j = (MOHUMissionsList[i].MissionNumber == 2 ? 1 : 0); j < MOHUMissionsList[i].NumLevels; j++ ) {
            assert(LevelManagerLoadLevel(Engine->LevelManager,Engine->GUI,Engine->VideoSystem,Engine->SoundSystem,
                           MOHUMissionsList[i].MissionNumber,MOHUMissionsList[i].Levels[j].LevelNumber) );
        }
    }
    DPrintf("Engine Test Completed...took %i ms to load all the levels\n",(SysMilliseconds() - StartTime));
    Quit(Engine);
#endif
    while( 1 ) {
        EngineFrame(Engine);
    }
    
    return 0;
}
