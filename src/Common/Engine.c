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
#include "Common.h"
#include "Engine.h"
#include "ShaderManager.h"

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
    EngineQuitSDL();
    free(Engine);
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
void EngineBeginFrame(Engine_t *Engine)
{
    if( !Engine ) {
        DPrintf("EngineBeginFrame:Called without a valid engine\n");
        return;
    }
//     EngineCheckEvents(Engine);
    ComUpdateDelta(Engine->TimeInfo);
    glViewport(0,0,VidConfigWidth->IValue,VidConfigHeight->IValue);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void EngineEndFrame(Engine_t *Engine)
{
    if( !Engine ) {
        DPrintf("EngineFrame:Called without a valid engine\n");
        return;
    }
    VideoSystemSwapBuffers(Engine->VideoSystem);
}

int EngineInitSDL()
{
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ) {
        return false;
    }
    return true;
}
Engine_t *EngineInit()
{
    Engine_t *Engine;
    
    Engine = malloc(sizeof(Engine_t));
    
    if( !Engine ) {
        printf("EngineInit:Failed to allocate memory for engine\n");
        return NULL;
    }
    
    Engine->TimeInfo = NULL;
    Engine->VideoSystem = NULL;


    if( !EngineInitSDL() ) {
        printf("EngineInit:Failed to initialize SDL subsystems.\n");
        goto Failure;
    }
    
    Engine->VideoSystem = VideoSystemInit();
    
    if( !Engine->VideoSystem ) {
        printf("EngineInit:Failed to Initialize Video system...\n");
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
    
    return Engine;

Failure:
    EngineShutDown(Engine);
    return NULL;
} 
