// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    GFXModelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GFXModelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GFXModelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#include "GFXModelViewer.h"
#include "../Common/ShaderManager.h"

void ApplicationCheckEvents(Application_t *Application)
{
    SDL_Event Event;
    GFX_t *CurrentGFX;
    int NextFrame;
    int NextPose;
    
    while( SDL_PollEvent(&Event) ) {
        if( Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            ConfigSetNumber("VideoWidth",Event.window.data1);
            ConfigSetNumber("VideoHeight",Event.window.data2);
        }
        if( Event.type == SDL_QUIT || (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE ) ) {
            Quit(Application);
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_m ) {
            CurrentGFX = GFXObjectManagerGetCurrentGFX(Application->GFXObjectManager);
            if( CurrentGFX != NULL ) {
                NextFrame = (CurrentGFX->CurrentFrameIndex + 1) % 
                    CurrentGFX->Animation[CurrentGFX->CurrentAnimationIndex].NumFrames;
                GFXSetAnimationPose(CurrentGFX,CurrentGFX->CurrentAnimationIndex,NextFrame);
            }
        }
        if( Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_n ) {
            CurrentGFX = GFXObjectManagerGetCurrentGFX(Application->GFXObjectManager);
            if( CurrentGFX != NULL ) {
                 NextPose = (CurrentGFX->CurrentAnimationIndex + 1) % CurrentGFX->Header.NumAnimationIndex;
                 while( !GFXSetAnimationPose(CurrentGFX,NextPose,0) ) {
                    NextPose = (NextPose + 1 ) % CurrentGFX->Header.NumAnimationIndex;
                }
            }
        }
        GUIProcessEvent(Application->GUI,&Event);
        if( GUIIsMouseFree() ) {
            if( Event.type == SDL_MOUSEWHEEL) {
                CameraZoom(Application->Camera,-Event.wheel.y);
            }
            if( Event.type == SDL_MOUSEMOTION && Event.motion.state & SDL_BUTTON_LMASK ) {
                CameraOnMouseEvent(Application->Camera,Event.motion.xrel,Event.motion.yrel);
            }
        }
    }
    if( GUIIsKeyboardFree() ) {
        CameraCheckKeyEvents(Application->Camera,Application->Engine->KeyState,Application->Engine->TimeInfo->Delta);
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
    if( Application->GFXObjectManager ) {
        GFXObjectManagerCleanUp(Application->GFXObjectManager);
    }
    if( Application->GUI ) {
        GUIFree(Application->GUI);
    }
    if( Application->Camera ) {
        CameraCleanUp(Application->Camera);
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
    GFXObjectManagerDraw(Application->GFXObjectManager,Application->Camera);
    glDisable (GL_DEPTH_TEST);
    GUIDraw(Application);
    glEnable(GL_DEPTH_TEST);
}
void ApplicationFrame(Application_t *Application)
{
    if( !Application ) {
        DPrintf("ApplicationFrame:Called without a valid Application\n");
        return;
    }
    EngineBeginFrame(Application->Engine);
    ApplicationCheckEvents(Application);
    CameraBeginFrame(Application->Camera);
    GFXObjectManagerUpdate(Application->GFXObjectManager);
    ApplicationDraw(Application);
    EngineEndFrame(Application->Engine);
}
void RegisterDefaultSettings()
{
    ConfigRegister("CameraSpeed","30.f",NULL);
    ConfigRegister("CameraMouseSensitivity","1.f",NULL);
    
    ConfigRegister("EnableWireFrameMode","0","Draw the model surfaces as lines");
    ConfigRegister("EnableAmbientLight","1","When enabled the texture color is interpolated with the surface color to simulate lights on \n"
                                                    "surfaces");

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
    Application->GFXObjectManager = NULL;

    CommonInit("GFXModelViewer");
    RegisterDefaultSettings();
    ConfigInit();
    
    Application->Engine = EngineInit("GFX Model Viewer");
    
    if( !Application->Engine ) {
        printf("ApplicationInit:Failed to initialize the Engine\n");
        goto Failure;
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
        
    Application->GFXObjectManager = GFXObjectManagerInit(Application->GUI);
    
    if( !Application->GFXObjectManager ) {
        printf("ApplicationInit:Failed to initialize GFXObjectManager\n");
        goto Failure;
    }
    
    if( argc > 1 ) {
        GFXObjectManagerLoadPack(Application->GFXObjectManager,Application->GUI,Application->Engine->VideoSystem,argv[1]);
    }
    GLSetDefaultState();
    return Application;

Failure:
    ApplicationShutDown(Application);
    return NULL;
}

int main(int argc,char **argv)
{
    Application_t *Application;
    
    srand(time(NULL));
    
    Application = ApplicationInit(argc,argv);
    
    if( !Application ) {
        printf("Failed to initialize application...\n");
        return -1;
    }

    while( 1 ) {
        ApplicationFrame(Application);
    }
    return 0;
}
