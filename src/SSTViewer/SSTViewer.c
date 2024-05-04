// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    SSTViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SSTViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SSTViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#include "SSTViewer.h"
#include "../Common/ShaderManager.h"
#include "../Common/Config.h"

void ApplicationCheckEvents(Application_t *Application)
{
    SDL_Event Event;
    while( SDL_PollEvent(&Event) ) {
        if( Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            ConfigSetNumber("VideoWidth",Event.window.data1);
            ConfigSetNumber("VideoHeight",Event.window.data2);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F1 ) {
            GUIToggleDebugWindow(Application->GUI);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F2 ) {
            GUIToggleVideoSettingsWindow(Application->GUI);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F3 ) {
            GUIToggleLevelSelectWindow(Application->GUI);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F4 ) {
            SSTManagerToggleFileDialog(Application->SSTManager,Application->GUI,Application->Engine->VideoSystem);
        }
        if( Event.type == SDL_QUIT || (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE ) ) {
            Quit(Application);
        }
        
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

void ApplicationShutDown(Application_t *Application)
{
    if( !Application ) {
        return;
    }
    if( Application->SSTManager ) {
        SSTManagerCleanUp(Application->SSTManager);
    }
    if( Application->Camera ) {
        CameraCleanUp(Application->Camera);
    }
    if( Application->GUI ) {
        GUIFree(Application->GUI);
    }
    if( Application->Engine ) {
        EngineShutDown(Application->Engine);
    }
    CommonShutdown();
    free(Application);
}

void Quit(Application_t *Application)
{
    ApplicationShutDown(Application);
    exit(0);
}

void ApplicationDraw(Application_t *Application)
{
    if( !Application ) {
        DPrintf("ApplicationDraw:Called without a valid engine\n");
        return;
    }
    SSTManagerDraw(Application->SSTManager,Application->Camera);
    glDisable (GL_DEPTH_TEST);
    GUIDraw(Application->GUI,Application->SSTManager,Application->Camera,
            Application->Engine->VideoSystem,Application->Engine->TimeInfo);
    glEnable(GL_DEPTH_TEST);
}
void ApplicationFrame(Application_t *Application)
{
    if( !Application ) {
        DPrintf("ApplicationFrame:Called without a valid engine\n");
        return;
    }
    EngineBeginFrame(Application->Engine);
    ApplicationCheckEvents(Application);
    SSTManagerUpdate(Application->SSTManager,Application->Camera);
    ApplicationDraw(Application);
    EngineEndFrame(Application->Engine);
}

void RegisterDefaultSettings()
{
    ConfigRegister("CameraSpeed","30.f",NULL);
    ConfigRegister("CameraMouseSensitivity","1.f",NULL);

    ConfigRegister("GameBasePath","","Sets the path from which the game will be loaded,any invalid path will result in this variable to "
                                    "being set to empty.");
}

Application_t *ApplicationInit(int argc,char **argv)
{
    Application_t *Application;
    
    Application = malloc(sizeof(Application_t));
    
    if( !Application ) {
        printf("ApplicationInit:Failed to allocate memory for the application\n");
        return NULL;
    }
    
    Application->Engine = NULL;
    Application->GUI = NULL;
    Application->Camera = NULL;
    Application->SSTManager = NULL;
        
    CommonInit("SSTViewer");

    RegisterDefaultSettings();
    ConfigInit();
    
    Application->Engine = EngineInit("SST Viewer");
    
    if( !Application->Engine ) {
        printf("ApplicationInit:Failed to initialize the Engine\n");
        goto Failure;
    }
    
    VideoSystemGrabMouse(1);

    //NOTE(Adriano):Allow the game path to be set using command line argument.
    //              If the path is not valid the game will discard it.
    if( argc > 1 ) {
        ConfigSet("GameBasePath",argv[1]);
    }

    Application->GUI = GUIInit(Application->Engine->VideoSystem);
    if( !Application->GUI ) {
        printf("ApplicationInit:Failed to initialize GUI system\n");
        goto Failure;
    }
    
    Application->Camera = CameraInit();
    
    if( !Application->Camera ) {
        printf("ApplicationInit:Failed to Initialize Camera System\n");
        goto Failure;
    }
    
    Application->SSTManager = SSTManagerInit(Application->GUI,Application->Engine->VideoSystem);
    
    if( !Application->SSTManager ) {
        printf("ApplicationInit:Failed to initialize SSTManager\n");
        goto Failure;
    }
    GLSetDefaultState();
    return Application;

Failure:
    ApplicationShutDown(Application);
    return NULL;
}

#define TEST_ENGINE 0
int main(int argc,char **argv)
{
    Application_t *Application;
    
    srand(time(NULL));
    
    Application = ApplicationInit(argc,argv);
    
    if( !Application ) {
        printf("Failed to initialize Application...\n");
        return -1;
    }
    
    while( 1 ) {
        ApplicationFrame(Application);
    }
    
    return 0;
}
