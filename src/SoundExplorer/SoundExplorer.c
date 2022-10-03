/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
    SoundExplorer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SoundExplorer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SoundExplorer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#include "SoundExplorer.h"

void ApplicationCheckEvents(Application_t *Application)
{
    SDL_Event Event;
    
    while( SDL_PollEvent(&Event) ) {
        if( Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            ConfigSetNumber("VideoWidth",Event.window.data1);
            ConfigSetNumber("VideoHeight",Event.window.data2);
        }
        if( Event.type == SDL_QUIT || (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE ) ) {
            Quit(Application);
        }
        GUIProcessEvent(Application->GUI,&Event);
    }
}

void ApplicationShutDown(Application_t *Application)
{
    if( !Application ) {
        return;
    }
    if( Application->Engine ) {
        EngineShutDown(Application->Engine);
    }
    if( Application->GUI ) {
        GUIFree(Application->GUI);
    }
    if( Application->SoundManager ) {
        SoundManagerCleanUp(Application->SoundManager);
    }
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
    ApplicationDraw(Application);
    EngineEndFrame(Application->Engine);
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

    CommonInit("SoundExplorer");

    ConfigInit();
    
    Application->Engine = EngineInit("Sound Explorer");
    
    if( !Application->Engine ) {
        printf("ApplicationInit:Failed to initialize the Engine\n");
        goto Failure;
    }
    
    Application->GUI = GUIInit(Application->Engine->VideoSystem);
    
    if( !Application->GUI ) {
        printf("ApplicationInit:Failed to initialize GUI system\n");
        goto Failure;
    }
  
    Application->SoundManager = SoundManagerInit(Application->GUI);
    
    if( !Application->SoundManager ) {
        printf("ApplicationInit:Failed to initialize SoundManager\n");
        goto Failure;
    }
    
    if( argc > 1 ) {
        SoundManagerLoadAudioFile(Application->SoundManager,Application->GUI,Application->Engine->VideoSystem,argv[1]);
    }
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
        ApplicationShutDown(Application);
        return -1;
    }

    while( 1 ) {
        ApplicationFrame(Application);
    }
    return 0;
}
