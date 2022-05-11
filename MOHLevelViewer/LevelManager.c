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
int NumMOHMissions = sizeof(MOHMissionsList) / sizeof(MOHMissionsList[0]);
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
int NumMOHUMissions = sizeof(MOHUMissionsList) / sizeof(MOHUMissionsList[0]);

Config_t *LevelManagerBasePath;

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

void LevelManagerFreeExporterData(GUIFileDialog_t *FileDialog)
{
    LevelManagerExporter_t *Exporter;
    Exporter = (LevelManagerExporter_t *) GUIFileDialogGetUserData(FileDialog);
    if( Exporter ) {
        free(Exporter);
    }
}
void LevelManagerCleanUp()
{
    SoundSystemCleanUp(LevelManager->SoundSystem);
    if( LevelManager->CurrentLevel != NULL ) {
        LevelCleanUp(LevelManager->CurrentLevel);
    }
    if( LevelManager->BasePath ) {
        free(LevelManager->BasePath);
    }
    //If the user didn't close the dialog free the user data that we passed to it.
    if( GUIFileDialogIsOpen(LevelManager->ExportFileDialog) ) {
        LevelManagerFreeExporterData(LevelManager->ExportFileDialog);
    }
    free(LevelManager);
}
void LevelManagerExportToObj(LevelManager_t *LevelManager,GUI_t *GUI,char *Directory)
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
    asprintf(&ObjectFile,"%s%c%s",Directory,PATHSEPARATOR,FileName);
    
    DPrintf("LevelManagerExportToObj:Dumping it...%s\n",ObjectFile);
    OutFile = fopen(ObjectFile,"w");
    if( !OutFile ) {
        DPrintf("LevelManagerExportToObj:Failed to open %s for writing\n",ObjectFile);
        return;
    }
    GUISetProgressBarDialogTitle(GUI,"Exporting to Obj...");
    GUIProgressBarIncrement(GUI,5,"Writing material file.");
    sprintf(MaterialNameTag,"mtllib vram.mtl\n");
    fwrite(MaterialNameTag,strlen(MaterialNameTag),1,OutFile);
    GUIProgressBarIncrement(GUI,35,"Writing TSP data.");
    TSPDumpDataToFile(LevelManager->CurrentLevel->TSPList,OutFile);
    GUIProgressBarIncrement(GUI,55,"Writing BSD data.");
    BSDDumpDataToFile(LevelManager->CurrentLevel->BSD,OutFile);
    GUIProgressBarIncrement(GUI,95,"Exporting VRAM.");
    VRAMDumpDataToFile(LevelManager->CurrentLevel->VRAM,Directory);
    GUIProgressBarIncrement(GUI,100,"Done.");
    free(EngineName);
    free(FileName);
    free(ObjectFile);
    fclose(OutFile);
}

void LevelManagerExportToPly(LevelManager_t *LevelManager,GUI_t *GUI,char *Directory)
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
    asprintf(&LevelFileName,"%s-MSN%iLVL%i_Level.ply",EngineName,LevelManager->CurrentLevel->MissionNumber,LevelManager->CurrentLevel->LevelNumber);
    asprintf(&ObjectFileName,"%s-MSN%iLVL%i_Objects.ply",EngineName,LevelManager->CurrentLevel->MissionNumber,LevelManager->CurrentLevel->LevelNumber);
    asprintf(&PlyLevelFile,"%s%c%s",Directory,PATHSEPARATOR,LevelFileName);
    asprintf(&PlyObjectFile,"%s%c%s",Directory,PATHSEPARATOR,ObjectFileName);
    asprintf(&TextureFile,"%s%cvram.png",Directory,PATHSEPARATOR);
    DPrintf("LevelManagerExportToPly:Dumping it...%s / %s\n",PlyLevelFile,PlyObjectFile);
    PlyLevelOutFile = fopen(PlyLevelFile,"w");
    if( !PlyLevelOutFile ) {
        DPrintf("LevelManagerExportToPly:Failed to open %s for writing\n",PlyLevelFile);
        return;
    }
    PlyObjectOutFile = fopen(PlyObjectFile,"w");
    if( !PlyObjectOutFile ) {
        DPrintf("LevelManagerExportToPly:Failed to open %s for writing\n",PlyObjectFile);
        return;
    }
    GUISetProgressBarDialogTitle(GUI,"Exporting to Ply...");
    GUIProgressBarIncrement(GUI,5,"Writing TSP data.");
    TSPDumpDataToPlyFile(LevelManager->CurrentLevel->TSPList,PlyLevelOutFile);
    GUIProgressBarIncrement(GUI,55,"Writing BSD data.");
    BSDDumpDataToPlyFile(LevelManager->CurrentLevel->BSD,PlyObjectOutFile);
    GUIProgressBarIncrement(GUI,95,"Exporting VRAM.");
    VRAMSave(LevelManager->CurrentLevel->VRAM,TextureFile);
    GUIProgressBarIncrement(GUI,100,"Done.");
    free(EngineName);
    free(PlyLevelFile);
    free(PlyObjectFile);
    free(LevelFileName);
    free(ObjectFileName);
    free(TextureFile);
    fclose(PlyLevelOutFile);
    fclose(PlyObjectOutFile);

}


void LevelManagerOnExportDirSelected(GUIFileDialog_t *FileDialog,GUI_t *GUI,char *Directory,char *File,void *UserData)
{
    LevelManagerExporter_t *Exporter;
    LevelManager_t *LevelManager;
    Exporter = (LevelManagerExporter_t *) UserData;
    LevelManager = Exporter->LevelManager;
        
    GUIProgressBarBegin(GUI,"Exporting...");

    switch( Exporter->OutputFormat ) {
        case LEVEL_MANAGER_EXPORT_FORMAT_OBJ:
            LevelManagerExportToObj(LevelManager,GUI,Directory);
            break;
        case LEVEL_MANAGER_EXPORT_FORMAT_PLY:
            LevelManagerExportToPly(LevelManager,GUI,Directory);
            break;
        default:
            DPrintf("LevelManagerOnExportDirSelected:Invalid output format\n");
            break;
    }
        
    GUIProgressBarEnd(GUI);
    GUIFileDialogClose(GUI,FileDialog);
    free(Exporter);
}

void LevelManagerOnExportDirCancelled(GUIFileDialog_t *FileDialog,GUI_t *GUI)
{
    LevelManagerFreeExporterData(FileDialog);
    GUIFileDialogClose(GUI,FileDialog);
}

void LevelManagerOnDirSelected(GUIFileDialog_t *FileDialog,GUI_t *GUI,char *Path,char *File,void *UserData)
{
    int LoadStatus;
    LoadStatus = LevelManagerInitWithPath(LevelManager,GUI,Path);
    if( !LoadStatus ) {
        GUISetErrorMessage(GUI,"Selected path doesn't seems to contain any game file...\nPlease select a folder containing MOH or MOH:Undergound.");
    } else {
        //Close it if we managed to load it.
        ConfigSet("GameBasePath",Path);
        GUIFileDialogClose(GUI,FileDialog);
    }
}
void LevelManagerOnDirSelectionCancelled(GUIFileDialog_t *FileDialog,GUI_t *GUI)
{
    if( LevelManager->IsPathSet ) {
        GUIFileDialogClose(GUI,FileDialog);
    }
}

void LevelManagerExport(LevelManager_t* LevelManager,GUI_t *GUI,int OutputFormat)
{
    LevelManagerExporter_t *Exporter;
    
    if( !LevelManager ) {
        DPrintf("LevelManagerExport:Invalid LevelManager\n");
        return;
    }
    if( !GUI ) {
        DPrintf("LevelManagerExport:Invalid GUI data\n");
        return;
    }
    Exporter = malloc(sizeof(LevelManager_t));
    if( !Exporter ) {
        DPrintf("LevelManagerExport:Couldn't allocate data for the exporter\n");
        return;
    }
    Exporter->LevelManager = LevelManager;
    Exporter->OutputFormat = OutputFormat;

    GUIFileDialogSetTitle(LevelManager->ExportFileDialog,"Export");
    GUIFileDialogOpenWithUserData(GUI,LevelManager->ExportFileDialog,Exporter);
}

void LevelManagerDraw(LevelManager_t *LevelManager)
{
    Level_t *Level;
    vec3 temp;
    int DynamicData;
    
    //LevelManager has not received a valid path yet.
    if( !LevelManager->IsPathSet ) {
        return;
    }
    //Level has not been loaded in yet.
    if( !LevelManagerIsLevelLoaded(LevelManager) ) {
        return;
    }
    
    Level = LevelManager->CurrentLevel;
    
    if( LevelEnableAnimatedSurfaces->IValue ) {

        BSDClearNodesFlag(Level->BSD);
    
        while( (DynamicData = BSDGetCurrentCameraNodeDynamicData(Level->BSD) ) != -1 ) {
            TSPUpdateDynamicFaces(Level->TSPList,DynamicData);
        }
    }
    if( LevelEnableAnimatedLights->IValue ) {
        BSDUpdateAnimatedLights(Level->BSD);
        TSPUpdateAnimatedFaces(Level->TSPList,Level->BSD,0);
    }
    
    glm_perspective(glm_rad(110.f),(float) VidConfigWidth->IValue/ (float) VidConfigHeight->IValue,1.f, 4096.f,VidConf.PMatrixM4);

         
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
    GUIProgressBarBegin(GUI,"Loading Mission 1 Level 1");
    if( LevelManager->BasePath ) {
        free(LevelManager->BasePath);
    }
    LevelManager->BasePath = StringCopy(Path);
    LevelManager->IsPathSet = 0;
    if( !LevelInit(LevelManager->CurrentLevel,GUI,LevelManager->SoundSystem,LevelManager->BasePath,1,1,&GameEngine) ) {
        if( GUI != NULL ) {
            GUISetProgressBarDialogTitle(GUI,"Mission 2 Level 1");
        }
        if( !LevelInit(LevelManager->CurrentLevel,GUI,LevelManager->SoundSystem,LevelManager->BasePath,2,1,&GameEngine) ) {
            DPrintf("LevelManagerInitWithPath:Invalid path...\n");
        } else {
            LevelManager->IsPathSet = 1;
        }
    } else {
        LevelManager->IsPathSet = 1;
    }
    if( LevelManager->IsPathSet != 0 ) {
        LevelManager->GameEngine = GameEngine;
        sprintf(LevelManager->EngineName,"%s",GameEngine == MOH_GAME_STANDARD ? "Medal Of Honor" : "Medal of Honor:Underground");
    }
    GUIProgressBarEnd(GUI);
    return LevelManager->IsPathSet;
}
void LevelManagerLoadLevel(LevelManager_t *LevelManager,GUI_t *GUI,int MissionNumber,int LevelNumber)
{
    char *Buffer;
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
    asprintf(&Buffer,"Loading Mission %i Level %i...",MissionNumber,LevelNumber);
    GUIProgressBarBegin(GUI,Buffer);
    LevelInit(LevelManager->CurrentLevel,GUI,LevelManager->SoundSystem,LevelManager->BasePath,MissionNumber,LevelNumber,NULL);
    GUIProgressBarEnd(GUI);
    free(Buffer);
}

void LevelManagerToggleFileDialog(LevelManager_t *LevelManager,GUI_t *GUI)
{
    if( GUIFileDialogIsOpen(LevelManager->FileDialog) ) {
        if( LevelManager->IsPathSet ) {
            GUIFileDialogClose(GUI,LevelManager->FileDialog);
        }
    } else {
        DPrintf("Opening dialog!!!\n");
        GUIFileDialogOpen(GUI,LevelManager->FileDialog);
    }

}
void LevelManagerInit(GUI_t *GUI)
{
    int OpenDialog;
    LevelManager = malloc(sizeof(LevelManager_t));
    if( !LevelManager ) {
        DPrintf("LevelManagerInit:Failed to allocate memory for struct\n");
        return;
    }
    LevelManager->SoundSystem = SoundSystemInit();
    LevelManager->CurrentLevel = malloc(sizeof(Level_t));
    if( !LevelManager->CurrentLevel ) {
        DPrintf("LevelManagerInit:Failed to allocate memory for level struct\n");
        return;
    }
    LevelManager->FileDialog = GUIFileDialogRegister(GUI,"Select Directory",NULL,
                                                     LevelManagerOnDirSelected,LevelManagerOnDirSelectionCancelled);
    LevelManager->ExportFileDialog = GUIFileDialogRegister(GUI,"Export Level",NULL,LevelManagerOnExportDirSelected,LevelManagerOnExportDirCancelled);

    LevelManager->BasePath = NULL;
    //No path has been provided to it yet.
    LevelManager->IsPathSet = 0;
    memset(LevelManager->CurrentLevel,0,sizeof(Level_t));
    
    LevelManagerBasePath = ConfigGet("GameBasePath");
    OpenDialog = 0;
    if( LevelManagerBasePath->Value[0] ) {
        if( !LevelManagerInitWithPath(LevelManager,GUI,LevelManagerBasePath->Value) ) {
            ConfigSet("GameBasePath","");
            OpenDialog = 1;
        }
    } else {
        OpenDialog = 1;
    }
    if( OpenDialog ) {
        LevelManagerToggleFileDialog(LevelManager,GUI);
//         GUIDirSelectDialogOpen(GUI,LevelManagerOnDirSelected,NULL);
    }
}
