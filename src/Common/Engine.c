// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2023 Adriano Di Dio.
    
    Medal-Of-Honor-PSX-File-Viewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Medal-Of-Honor-PSX-File-Viewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Medal-Of-Honor-PSX-File-Viewer.  If not, see <http://www.gnu.org/licenses/>.
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
Engine_t *EngineInit(const char *WindowTitle)
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
    
    Engine->VideoSystem = VideoSystemInit(WindowTitle);
    
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
    
    ShaderManagerInit();
    
    return Engine;

Failure:
    EngineShutDown(Engine);
    return NULL;
} 
