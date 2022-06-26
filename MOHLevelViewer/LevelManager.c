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

#include "LevelManager.h"
#include "MOHLevelViewer.h"

const Mission_t MOHMissionsList[] = {
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
int NumMOHMissions = sizeof(MOHMissionsList) / sizeof(MOHMissionsList[0]);
const Mission_t MOHUMissionsList[] = {
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
        6,
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
        4,
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
int NumMOHUMissions = sizeof(MOHUMissionsList) / sizeof(MOHUMissionsList[0]);

Config_t *LevelManagerBasePath;

int LevelManagerIsLevelLoaded(const LevelManager_t *LevelManager)
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


void LevelManagerFreeDialogData(GUIFileDialog_t *FileDialog)
{
    LevelManagerDialogData_t *DialogData;
    DialogData = (LevelManagerDialogData_t *) GUIFileDialogGetUserData(FileDialog);
    if( DialogData ) {
        free(DialogData);
    }
}

void LevelManagerCloseDialog(GUI_t *GUI,GUIFileDialog_t *FileDialog)
{
    LevelManagerFreeDialogData(FileDialog);
    GUIFileDialogClose(GUI,FileDialog);
}

void LevelManagerCleanUp(LevelManager_t *LevelManager)
{
    if( LevelManager->CurrentLevel != NULL ) {
        LevelCleanUp(LevelManager->CurrentLevel);
    }
    if( LevelManager->BasePath ) {
        free(LevelManager->BasePath);
    }
    //If the user didn't close the dialog free the user data that we passed to it.
    if( GUIFileDialogIsOpen(LevelManager->ExportFileDialog) ) {
        LevelManagerFreeDialogData(LevelManager->ExportFileDialog);
    }
    if( GUIFileDialogIsOpen(LevelManager->FileDialog) ) {
        LevelManagerFreeDialogData(LevelManager->FileDialog);
    }
    free(LevelManager);
}
void LevelManagerDrawString(const LevelManager_t *LevelManager,const char *String,float x,float y,Color4f_t Color)
{
    if( !LevelManager ) {
        DPrintf("LevelManagerDrawString:Called without a valid level manager.\n");
        return;
    }
    if( !LevelManagerIsLevelLoaded(LevelManager) ) {
        DPrintf("LevelManagerDrawString:Called without a valid level\n");
        return;
    }
    FontDrawString(LevelManager->CurrentLevel->Font,LevelManager->CurrentLevel->VRAM,String,x,y,Color);
}
void LevelManagerUpdateSoundSettings(LevelManager_t *LevelManager,SoundSystem_t *SoundSystem,int SoundValue)
{
    LevelSetMusicTrackSettings(LevelManager->CurrentLevel,SoundSystem,LevelManager->GameEngine,SoundValue);
}
void LevelManagerExportToObj(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem,const char *Directory)
{
    char *EngineName;
    char *ObjectFile;
    char *FileName;
    char MaterialNameTag[64];
    FILE *OutFile;
    
    if( !LevelManager ) {
        DPrintf("LevelManagerExportToObj:Invalid user data\n");
        return;
    }
    
    if( !LevelManager->IsPathSet ) {
        DPrintf("LevelManagerExportToObj:Game path is not set!\n");
        return;
    }
    asprintf(&EngineName,"%s",(LevelManager->GameEngine == MOH_GAME_STANDARD) ? "MOH" : "MOHUndergound");
    asprintf(&FileName,"%s-MSN%iLVL%i.obj",EngineName,LevelManager->CurrentLevel->MissionNumber,LevelManager->CurrentLevel->LevelNumber);
    asprintf(&ObjectFile,"%s%c%s",Directory,PATH_SEPARATOR,FileName);
    
    DPrintf("LevelManagerExportToObj:Dumping it...%s\n",ObjectFile);
    OutFile = fopen(ObjectFile,"w");
    if( !OutFile ) {
        DPrintf("LevelManagerExportToObj:Failed to open %s for writing\n",ObjectFile);
        return;
    }
    GUISetProgressBarDialogTitle(GUI,"Exporting to Obj...");
    GUIProgressBarIncrement(GUI,VideoSystem,5,"Writing material file.");
    sprintf(MaterialNameTag,"mtllib vram.mtl\n");
    fwrite(MaterialNameTag,strlen(MaterialNameTag),1,OutFile);
    GUIProgressBarIncrement(GUI,VideoSystem,35,"Writing TSP data.");
    TSPDumpDataToObjFile(LevelManager->CurrentLevel->TSPList,LevelManager->CurrentLevel->VRAM,OutFile);
    GUIProgressBarIncrement(GUI,VideoSystem,55,"Writing BSD data.");
    BSDDumpDataToObjFile(LevelManager->CurrentLevel->BSD,LevelManager->CurrentLevel->VRAM,LevelManager->GameEngine,OutFile);
    GUIProgressBarIncrement(GUI,VideoSystem,95,"Exporting VRAM.");
    VRAMDumpDataToFile(LevelManager->CurrentLevel->VRAM,Directory);
    GUIProgressBarIncrement(GUI,VideoSystem,100,"Done.");
    free(EngineName);
    free(FileName);
    free(ObjectFile);
    fclose(OutFile);
}
void LevelManagerExportMusicToWav(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem,
                                  const char *Directory)
{
    char *EngineName;
    
    if( !LevelManager ) {
        DPrintf("LevelManagerExportMusicToWav:Invalid user data\n");
        return;
    }
    
    if( !LevelManager->IsPathSet ) {
        DPrintf("LevelManagerExportMusicToWav:Game path is not set!\n");
        return;
    }
    asprintf(&EngineName,"%s",(LevelManager->GameEngine == MOH_GAME_STANDARD) ? "MOH" : "MOHUndergound");
    
    GUISetProgressBarDialogTitle(GUI,"Exporting to Wav...");
    SoundSystemDumpMusicToWav(SoundSystem,EngineName,Directory);
    GUIProgressBarIncrement(GUI,VideoSystem,100,"Done.");
    free(EngineName);
}
void LevelManagerExportToPly(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem,
                             const char *Directory)
{
    char *EngineName;
    char *PlyLevelFile;
    char *PlyObjectFile;
    char *LevelFileName;
    char *ObjectFileName;
    char *TextureFile;
    FILE *PlyLevelOutFile;
    FILE *PlyObjectOutFile;
    
    if( !LevelManager ) {
        DPrintf("LevelManagerExportToPly:Invalid user data\n");
        return;
    }
    
    if( !LevelManager->IsPathSet ) {
        DPrintf("LevelManagerExportToPly:Game path is not set!\n");
        return;
    }
    asprintf(&EngineName,"%s",(LevelManager->GameEngine == MOH_GAME_STANDARD) ? "MOH" : "MOHUndergound");
    asprintf(&LevelFileName,"%s-MSN%iLVL%i_Level.ply",EngineName,LevelManager->CurrentLevel->MissionNumber,
             LevelManager->CurrentLevel->LevelNumber);
    asprintf(&ObjectFileName,"%s-MSN%iLVL%i_Objects.ply",EngineName,LevelManager->CurrentLevel->MissionNumber,
             LevelManager->CurrentLevel->LevelNumber);
    asprintf(&PlyLevelFile,"%s%c%s",Directory,PATH_SEPARATOR,LevelFileName);
    asprintf(&PlyObjectFile,"%s%c%s",Directory,PATH_SEPARATOR,ObjectFileName);
    asprintf(&TextureFile,"%s%cvram.png",Directory,PATH_SEPARATOR);
    DPrintf("LevelManagerExportToPly:Dumping it...%s / %s\n",PlyLevelFile,PlyObjectFile);
    PlyLevelOutFile = fopen(PlyLevelFile,"w");
    if( !PlyLevelOutFile ) {
        DPrintf("LevelManagerExportToPly:Failed to open %s for writing\n",PlyLevelFile);
        return;
    }
    PlyObjectOutFile = fopen(PlyObjectFile,"w");
    if( !PlyObjectOutFile ) {
        DPrintf("LevelManagerExportToPly:Failed to open %s for writing\n",PlyObjectFile);
        fclose(PlyLevelOutFile);
        return;
    }
    GUISetProgressBarDialogTitle(GUI,"Exporting to Ply...");
    GUIProgressBarIncrement(GUI,VideoSystem,5,"Writing TSP data.");
    TSPDumpDataToPlyFile(LevelManager->CurrentLevel->TSPList,LevelManager->CurrentLevel->VRAM,PlyLevelOutFile);
    GUIProgressBarIncrement(GUI,VideoSystem,55,"Writing BSD data.");
    BSDDumpDataToPlyFile(LevelManager->CurrentLevel->BSD,LevelManager->CurrentLevel->VRAM,LevelManager->GameEngine,PlyObjectOutFile);
    GUIProgressBarIncrement(GUI,VideoSystem,95,"Exporting VRAM.");
    VRAMSave(LevelManager->CurrentLevel->VRAM,TextureFile);
    GUIProgressBarIncrement(GUI,VideoSystem,100,"Done.");
    free(EngineName);
    free(PlyLevelFile);
    free(PlyObjectFile);
    free(LevelFileName);
    free(ObjectFileName);
    free(TextureFile);
    fclose(PlyLevelOutFile);
    fclose(PlyObjectOutFile);

}


void LevelManagerOnExportDirSelected(GUIFileDialog_t *FileDialog,GUI_t *GUI,const char *Directory,const char *File,void *UserData)
{
    LevelManagerDialogData_t *Exporter;
    LevelManager_t *LevelManager;
    Exporter = (LevelManagerDialogData_t *) UserData;
    LevelManager = Exporter->LevelManager;
        
    GUIProgressBarBegin(GUI,"Exporting...");

    switch( Exporter->OutputFormat ) {
        case LEVEL_MANAGER_EXPORT_FORMAT_OBJ:
            LevelManagerExportToObj(LevelManager,GUI,Exporter->VideoSystem,Directory);
            break;
        case LEVEL_MANAGER_EXPORT_FORMAT_PLY:
            LevelManagerExportToPly(LevelManager,GUI,Exporter->VideoSystem,Directory);
            break;
        case LEVEL_MANAGER_EXPORT_FORMAT_WAV:
            LevelManagerExportMusicToWav(LevelManager,GUI,Exporter->VideoSystem,Exporter->SoundSystem,Directory);
            break;
        default:
            DPrintf("LevelManagerOnExportDirSelected:Invalid output format\n");
            break;
    }
        
    GUIProgressBarEnd(GUI,Exporter->VideoSystem);
    GUIFileDialogClose(GUI,FileDialog);
    free(Exporter);
}

void LevelManagerOnExportDirCancelled(GUIFileDialog_t *FileDialog,GUI_t *GUI)
{
    LevelManagerCloseDialog(GUI,FileDialog);
}

/*
 * Initialize the game from the config string.
 * Returns 0 if the config value is not valid 1 otherwise.
 * Note that the config key is cleared if it was not valid.
 */
int LevelManagerLoadFromConfig(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem)
{
    if( !LevelManagerBasePath->Value[0] ) {
        return 0;
    }
    if( !LevelManagerInitWithPath(LevelManager,GUI,VideoSystem,SoundSystem,LevelManagerBasePath->Value) ) {
        ConfigSet("GameBasePath","");
        return 0;
    }
    return 1;
}
void LevelManagerOnDirSelected(GUIFileDialog_t *FileDialog,GUI_t *GUI,const char *Path,const char *File,void *UserData)
{
    LevelManagerDialogData_t *LevelManagerDialogData;
    int LoadStatus;
    
    LevelManagerDialogData = (LevelManagerDialogData_t *) UserData;
    LoadStatus = LevelManagerInitWithPath(LevelManagerDialogData->LevelManager,GUI,LevelManagerDialogData->VideoSystem,
                                          LevelManagerDialogData->SoundSystem,Path);

    if( !LoadStatus ) {
        GUISetErrorMessage(GUI,"Selected path doesn't seems to contain any game file...\n"
        "Please select a folder containing MOH or MOH:Undergound.");
    } else {
        //Close it if we managed to load it.
        ConfigSet("GameBasePath",Path);
        LevelManagerCloseDialog(GUI,FileDialog);
    }
}
void LevelManagerOnDirSelectionCancelled(GUIFileDialog_t *FileDialog,GUI_t *GUI)
{
    LevelManagerDialogData_t *LevelManagerDialogData;
    LevelManagerDialogData = (LevelManagerDialogData_t *) GUIFileDialogGetUserData(FileDialog);
    if( LevelManagerDialogData->LevelManager->IsPathSet ) {
        LevelManagerCloseDialog(GUI,FileDialog);
    }
}

void LevelManagerExport(LevelManager_t* LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem,int OutputFormat)
{
    LevelManagerDialogData_t *Exporter;
    
    if( !LevelManager ) {
        DPrintf("LevelManagerExport:Invalid LevelManager\n");
        return;
    }
    if( !GUI ) {
        DPrintf("LevelManagerExport:Invalid GUI data\n");
        return;
    }
    Exporter = malloc(sizeof(LevelManagerDialogData_t));
    if( !Exporter ) {
        DPrintf("LevelManagerExport:Couldn't allocate data for the exporter\n");
        return;
    }
    Exporter->LevelManager = LevelManager;
    Exporter->VideoSystem = VideoSystem;
    Exporter->SoundSystem = SoundSystem;
    Exporter->OutputFormat = OutputFormat;

    GUIFileDialogSetTitle(LevelManager->ExportFileDialog,"Export");
    GUIFileDialogOpenWithUserData(GUI,LevelManager->ExportFileDialog,Exporter);
}

void LevelManagerSpawnCamera(const LevelManager_t *LevelManager,Camera_t *Camera)
{
    vec3 Position;
    vec3 Rotation;
    
    //LevelManager has not received a valid path yet.
    if( !LevelManager->IsPathSet ) {
        return;
    }
    //Level has not been loaded in yet.
    if( !LevelManagerIsLevelLoaded(LevelManager) ) {
        return;
    }
    LevelGetPlayerSpawn(LevelManager->CurrentLevel,0,Position,Rotation);
    CameraSetPosition(Camera,Position);
    CameraSetRotation(Camera,Rotation);
}
void LevelManagerUpdate(LevelManager_t *LevelManager,Camera_t *Camera)
{    
    //LevelManager has not received a valid path yet.
    if( !LevelManager->IsPathSet ) {
        return;
    }
    //Level has not been loaded in yet.
    if( !LevelManagerIsLevelLoaded(LevelManager) ) {
        return;
    }
    if( LevelManager->HasToSpawnCamera ) {
        LevelManagerSpawnCamera(LevelManager,Camera);
        LevelManager->HasToSpawnCamera = 0;
    }
    LevelUpdate(LevelManager->CurrentLevel,Camera);
}
void LevelManagerDraw(LevelManager_t *LevelManager,Camera_t *Camera)
{
    mat4 ProjectionMatrix;
    //LevelManager has not received a valid path yet.
    if( !LevelManager->IsPathSet ) {
        return;
    }
    //Level has not been loaded in yet.
    if( !LevelManagerIsLevelLoaded(LevelManager) ) {
        return;
    }
    
    glm_perspective(glm_rad(110.f),(float) VidConfigWidth->IValue / (float) VidConfigHeight->IValue,1.f, 4096.f,ProjectionMatrix);     
    
    LevelDraw(LevelManager->CurrentLevel,Camera,ProjectionMatrix);
}

int LevelManagerInitWithPath(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem,const char *Path)
{
    Level_t *Level;
    int Loaded;
    int GameEngine;
    char *Buffer;
    
    if( !LevelManager ) {
        DPrintf("LevelManagerInitWithPath:Called without a valid struct\n");
        return 0;
    }
    if( !Path ) {
        DPrintf("LevelManagerInitWithPath:Called without a valid path\n");
        return 0;
    }
    GUIProgressBarBegin(GUI,"Loading Mission 1 Level 1");
    Loaded = 0;
    for( int i = 1; i <= 2; i++ ) {
        asprintf(&Buffer,"Loading Mission %i Level 1",i);
        GUISetProgressBarDialogTitle(GUI,Buffer);
        Level = LevelInit(GUI,VideoSystem,SoundSystem,Path,i,1,&GameEngine);
        free(Buffer);
        if( Level ) {
            if( LevelManager->BasePath ) {
                free(LevelManager->BasePath);
            }
            if( LevelManagerIsLevelLoaded(LevelManager) ) {
                LevelCleanUp(LevelManager->CurrentLevel);
            }
            Loaded = 1;
            LevelManager->IsPathSet = Loaded;
            LevelManager->BasePath = StringCopy(Path);
            LevelManager->CurrentLevel = Level;
            LevelManager->GameEngine = GameEngine;
            sprintf(LevelManager->EngineName,"%s",GameEngine == MOH_GAME_STANDARD ? "Medal Of Honor" : "Medal of Honor:Underground");
            LevelManager->HasToSpawnCamera = 1;
            break;
        }
    }
    GUIProgressBarEnd(GUI,VideoSystem);
    return Loaded;
}
int LevelManagerLoadLevel(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem,
                           int MissionNumber,int LevelNumber)
{
    Level_t *Level;
    char *Buffer;

    if( !LevelManager->IsPathSet ) {
        DPrintf("LevelManagerLoadLevel:Called without a valid path set\n");
        return 0;
    }
    if( LevelManagerIsLevelLoaded(LevelManager) ) {
        if( LevelManager->CurrentLevel->MissionNumber == MissionNumber && LevelManager->CurrentLevel->LevelNumber == LevelNumber ) {
            DPrintf("LevelManagerLoadLevel:Attempted to load the same level...\n");
            return 0;
        }
    }
    asprintf(&Buffer,"Loading Mission %i Level %i...",MissionNumber,LevelNumber);
    GUIProgressBarBegin(GUI,Buffer);
    Level = LevelInit(GUI,VideoSystem,SoundSystem,LevelManager->BasePath,MissionNumber,LevelNumber,NULL);
    GUIProgressBarEnd(GUI,VideoSystem);
    if( !Level ) {
        printf("LevelManagerLoadLevel:Couldn't load mission %i level %i...\n",MissionNumber,LevelNumber);
        free(Buffer);
        return 0;
    }
    LevelCleanUp(LevelManager->CurrentLevel);
    LevelManager->CurrentLevel = Level;
    LevelManager->HasToSpawnCamera = 1;
    free(Buffer);
    return 1;
}

void LevelManagerToggleFileDialog(LevelManager_t *LevelManager,GUI_t *GUI,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem)
{
    LevelManagerDialogData_t *DialogData;
    
    if( GUIFileDialogIsOpen(LevelManager->FileDialog) ) {
        if( LevelManager->IsPathSet ) {
            LevelManagerCloseDialog(GUI,LevelManager->FileDialog);
        }
    } else {
        DialogData = malloc(sizeof(LevelManagerDialogData_t));
        if( !DialogData ) {
            DPrintf("LevelManagerToggleFileDialog:Couldn't allocate data for the exporter\n");
            return;
        }
        DialogData->LevelManager = LevelManager;
        DialogData->VideoSystem = VideoSystem;
        DialogData->SoundSystem = SoundSystem;
        GUIFileDialogOpenWithUserData(GUI,LevelManager->FileDialog,DialogData);
    }
}

LevelManager_t *LevelManagerInit(GUI_t *GUI,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem)
{
    LevelManager_t *LevelManager;

    LevelManager = malloc(sizeof(LevelManager_t));
    
    if( !LevelManager ) {
        printf("LevelManagerInit:Failed to allocate memory for struct\n");
        return NULL;
    }
    LevelManager->CurrentLevel = NULL;
    LevelManager->HasToSpawnCamera = 0;
    LevelManager->FileDialog = GUIFileDialogRegister(GUI,"Select Directory",NULL,
                                                     LevelManagerOnDirSelected,LevelManagerOnDirSelectionCancelled);
    LevelManager->ExportFileDialog = GUIFileDialogRegister(GUI,"Export Level",NULL,
                                                           LevelManagerOnExportDirSelected,LevelManagerOnExportDirCancelled);

    LevelManager->BasePath = NULL;
    //No path has been provided to it yet.
    LevelManager->IsPathSet = 0;
    
    LevelManagerBasePath = ConfigGet("GameBasePath");

    if( !LevelManagerLoadFromConfig(LevelManager,GUI,VideoSystem,SoundSystem) ) {
        LevelManagerToggleFileDialog(LevelManager,GUI,VideoSystem,SoundSystem);
    }
    return LevelManager;
}
