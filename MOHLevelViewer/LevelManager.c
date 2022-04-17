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

#include "LevelManager.h"

Mission_t MOHMissionsList[] = {
    {
        "Rescue The G3 Officer",
        1,
        3,
        (MissionLevel_t[]) {
            {
                "Find The Downed Plane",
                1
            },
            {
                "Search The Town",
                2
            },
            {
                "Sewer Chase",
                3
            }
        }
    },
    {
        "Destroy The Mighty Railgun Greta",
        2,
        4,
        (MissionLevel_t[]) {
            {
                "Sneak Into Railstation",
                1
            },
            {
                "Find The Gift Package",
                2
            },
            {
                "Rail Canyon",
                3
            },
            {
                "Meeting Greta",
                4
            }
        }
    },
    {
        "Scuttle Das Boot U-4901",
        3,
        4,
        (MissionLevel_t[]) {
            {
                "Escape The Wolfram",
                1
            },
            {
                "The Rooftops Of Dachsmag",
                2
            },
            {
                "The Hunters Den",
                3
            },
            {
                "Dive",
                4
            }
        }
    },
    {
        "Attack Impenetrable Fort Schmerzen",
        4,
        3,
        (MissionLevel_t[]) {
            {
                "The Siegfried Forest",
                1
            },
            {
                "Officers Quarters",
                2
            },
            {
                "Mustard Gas Production",
                3
            }
        }
    },
    {
        "Sabotage The Rjukan Hydro Plant",
        5,
        4,
        (MissionLevel_t[]) {
            {
                "The Roaring Penstocks",
                1
            },
            {
                "Generators Of Destruction",
                2
            },
            {
                "Betrayal In The Telemark",
                3
            },
            {
                "Heavy Water",
                4
            }
        }
    },
    {
        "Capture The Secret German Treasure",
        7,
        3,
        (MissionLevel_t[]) {
            {
                "Mountain Pass",
                1
            },
            {
                "Merkers Upper Mine",
                2
            },
            {
                "Treasures Caverns",
                3
            }
        }
    },
    {
        "Escape The V2 Rocket Plant",
        9,
        3,
        (MissionLevel_t[]) {
            {
                "Buzz Bomb Assembly",
                1
            },
            {
                "Vengeance Production",
                2
            },
            {
                "Gotterdammerung",
                4
            }
        }
    },
    {
        "Multiplayer",
        12,
        7,
        (MissionLevel_t[]) {
            {
                "Game Werks",
                1
            },
            {
                "The Short Line",
                2
            },
            {
                "Tail Of 2 Cities",
                3
            },
            {
                "Follow Your Nose",
                4
            },
            {
                "Castle Von Trapped",
                5
            },
            {
                "Trouble Shooting",
                6
            },
            {
                "Site Seeing",
                7
            }
        }
    }
};

Mission_t MOHUMissionsList[] = {
    {
        "Occupied!",
        2,
        4,
        (MissionLevel_t[]) {
            {
                "Midnight Rendez-Vous",
                1
            },
            {
                "Amongst the Dead",
                2
            },
            {
                "Without A Trace",
                3
            },
            {
                "Tread Carefully",
                4
            }
        }
    },
    {
        "Hunting The Desert Fox",
        3,
        4,
        (MissionLevel_t[]) {
            {
                "Casablanca",
                1
            },
            {
                "Lighting The Torch",
                2
            },
            {
                "Burning Sands",
                3
            },
            {
                "Ally In The Desert",
                4
            }
        }
    },
    {
        "Undercover In Crete",
        5,
        3,
        (MissionLevel_t[]) {
            {
                "Getting The Story",
                1
            },
            {
                "What Lies At Knossos",
                2
            },
            {
                "Labyrinth",
                3
            }
        }
    },
    {
        "Wewelsburg: Dark Camelot",
        4,
        3,
        (MissionLevel_t[]) {
            {
                "Ascent To The Castle",
                1
            },
            {
                "Dark Valhalla",
                2
            },
            {
                "A Vicious Cycle",
                3
            }
        }
    },
    {
        "Last Rites At Monte Cassino",
        6,
        3,
        (MissionLevel_t[]) {
            {
                "Roundabout",
                1
            },
            {
                "Prisoners Of War",
                2
            },
            {
                "Mayhem In The Monastery",
                3
            }
        }
    },
    {
        "A Mittelwerk Saboteur",
        7,
        3,
        (MissionLevel_t[]) {
            {
                "Plans for Destruction",
                1
            },
            {
                "Sabotage!",
                2
            },
            {
                "Sidecar Shootout",
                3
            }
        }
    },
    {
        "Liberation!",
        8,
        4,
        (MissionLevel_t[]) {
            {
                "Final Uprising",
                1
            },
            {
                "Street By Street!",
                2
            },
            {
                "Operation Marketplace",
                3
            },
            {
                "The End Of The Line",
                4
            }
        }
    },
    {
        "Bonus - Panzerknacker Unleashed!",
        9,
        3,
        (MissionLevel_t[]) {
            {
                "Where Beagles Dare",
                1
            },
            {
                "Rotten To The Corps",
                2
            },
            {
                "I, Panzerknacker",
                3
            }
        }
    },
    {
        "Multiplayer",
        12,
        6,
        (MissionLevel_t[]) {
            {
                "Aztec Turtle House",
                1
            },
            {
                "The Shoot Hole IV",
                2
            },
            {
                "Metro Plex",
                3
            },
            {
                "The Missionary",
                4
            },
            {
                "Squaresville",
                5
            },
            {
                "Shostakovich Nightmare",
                6
            }
        }
    }
};
int NumMOHMissions = sizeof(MOHMissionsList) / sizeof(MOHMissionsList[0]);
int NumMOHUMissions = sizeof(MOHUMissionsList) / sizeof(MOHUMissionsList[0]);

int LevelManagerIsLevelLoaded(LevelManager_t *LevelManager)
{
    if( !LevelManager ) {
        DPrintf("LevelManagerIsLevelLoaded:Called without a valid LevelManager\n");
        return 0;
    }
    return LevelIsLoaded(LevelManager->CurrentLevel);
}

int LevelManagerGetGameEngine(LevelManager_t *LevelManager)
{
    return LevelManager->GameEngine;
}

void LevelManagerCleanUp()
{
    if( LevelManager->CurrentLevel != NULL ) {
        LevelCleanUp(LevelManager->CurrentLevel);
    }
    if( LevelManager->BasePath ) {
        free(LevelManager->BasePath);
    }
    free(LevelManager);
}



void LevelManagerDraw(LevelManager_t *LevelManager)
{
    Level_t *Level;
    vec3 temp;

    //LevelManager has not received a valid path yet.
    if( !LevelManager->IsPathSet ) {
        return;
    }
    //Level has not been loaded in yet.
    if( !LevelManagerIsLevelLoaded(LevelManager) ) {
        return;
    }
    
    Level = LevelManager->CurrentLevel;
    
    if( LevelManager->Settings.EnableAnimatedLights ) {
        BSDUpdateAnimatedLights(Level->BSD);
        TSPUpdateAnimatedFaces(Level->TSPList,Level->BSD,0);
    }
    
    glm_perspective(glm_rad(110.f),(float) VidConf.Width/ (float) VidConf.Height,1.f, 4096.f,VidConf.PMatrixM4);

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
//      BSD2PDraw(Level);
     BSDDraw(LevelManager);
     
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
     TSPDrawList(LevelManager);
     
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
     
     glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
     
     //Emulate PSX Coordinate system...
     glm_rotate_x(VidConf.MVPMatrix,glm_rad(180.f), VidConf.MVPMatrix);
     BSDDrawSky(LevelManager);
}

int LevelManagerInitWithPath(LevelManager_t *LevelManager,GUI_t *GUI,char *Path)
{
    int GameEngine;
    if( !LevelManager ) {
        DPrintf("LevelManagerInitWithPath:Called without a valid struct\n");
        return 0;
    }
    if( !Path ) {
        DPrintf("LevelManagerInitWithPath:Called without a valid path\n");
        return 0;
    }
    LevelManager->BasePath = StringCopy(Path);
    if( !LevelInit(LevelManager->CurrentLevel,GUI,LevelManager->BasePath,1,1,&GameEngine) ) {
        GUISetProgressBarDialogTitle(GUI,"Mission 2 Level 1");
        if( !LevelInit(LevelManager->CurrentLevel,GUI,LevelManager->BasePath,2,1,&GameEngine) ) {
            DPrintf("LevelManagerInitWithPath:Invalid path...\n");
            LevelManager->IsPathSet = 0;
            return 0;
        }
    }
    LevelManager->GameEngine = GameEngine;
    sprintf(LevelManager->EngineName,"%s",GameEngine == MOH_GAME_STANDARD ? "Medal Of Honor" : "Medal of Honor:Underground");
    LevelManager->IsPathSet = 1;
    return 1;
}
void LevelManagerLoadLevel(LevelManager_t *LevelManager,GUI_t *GUI,int MissionNumber,int LevelNumber)
{
    if( !LevelManager->IsPathSet ) {
        DPrintf("LevelManagerLoadLevel:Called without a valid path set\n");
        return;
    }
    if( LevelManagerIsLevelLoaded(LevelManager) ) {
        if( LevelManager->CurrentLevel->MissionNumber == MissionNumber && LevelManager->CurrentLevel->LevelNumber == LevelNumber ) {
            DPrintf("LevelManagerLoadLevel:Attempted to load the same level...\n");
            return;
        }
    }
    LevelInit(LevelManager->CurrentLevel,GUI,LevelManager->BasePath,MissionNumber,LevelNumber,NULL);
}
void LevelManagerInit()
{
    LevelManager = malloc(sizeof(LevelManager_t));
    LevelManager->CurrentLevel = malloc(sizeof(Level_t));
    LevelManager->BasePath = NULL;
    LevelSetDefaultSettings(&LevelManager->Settings);
    memset(LevelManager->CurrentLevel,0,sizeof(Level_t));
    //No path has been provided to it yet.
    LevelManager->IsPathSet = 0;
}
