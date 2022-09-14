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

#include "Level.h"
#include "MOHLevelViewer.h"

Config_t *LevelEnableWireFrameMode;
Config_t *LevelDrawCollisionData;
Config_t *LevelDrawBSPTree;
Config_t *LevelDrawSurfaces;
Config_t *LevelDrawBSDNodesAsPoints;
Config_t *LevelDrawBSDNodesCollisionVolumes;
Config_t *LevelDrawBSDRenderObjectsAsPoints;
Config_t *LevelDrawBSDRenderObjects;
Config_t *LevelDrawBSDShowcase;
Config_t *LevelEnableFrustumCulling;
Config_t *LevelEnableAmbientLight;
Config_t *LevelEnableSemiTransparency;
Config_t *LevelEnableAnimatedLights;
Config_t *LevelEnableAnimatedSurfaces;
Config_t *LevelEnableMusicTrack;

int LevelIsLoaded(Level_t *Level)
{
    if( !Level ) {
        return 0;
    }
    return (Level->MissionNumber != 0 && Level->LevelNumber != 0);
}


void LevelUnload(Level_t *Level)
{
    if( !Level ) {
        return;
    }
    DPrintf("LevelUnload:Deallocating previously allocated Level struct\n");
    if( Level->BSD ) {
        BSDFree(Level->BSD);
    }
    if( Level->TSPList ) {
        TSPFreeList(Level->TSPList);
    }
    if( Level->ImageList ) {
        TIMImageListFree(Level->ImageList);
    }
    if( Level->VRAM ) {
        VRAMFree(Level->VRAM);
    }
    if( Level->Font ) {
        FontFree(Level->Font);
    }
    Level->MissionNumber = 0;
    Level->LevelNumber = 0;
    Level->BSD = NULL;
    Level->TSPList = NULL;
    Level->ImageList = NULL;
    Level->VRAM = NULL;
    Level->Font = NULL;
}
void LevelCleanUp(Level_t *Level)
{
    if( !Level ) {
        return;
    }
    LevelUnload(Level);
    free(Level);
}

void LevelSetMusicTrackSettings(Level_t *Level,SoundSystem_t *SoundSystem,int GameEngine,int SoundValue)
{
    int IsAmbient;
    if( SoundValue < 0 || SoundValue > 2 ) {
        SoundValue = 1;
    }
    if( !SoundValue ) {
        SoundSystemStopMusic(SoundSystem);
    } else {
        IsAmbient = (SoundValue == 2) ? 1 : 0;
        SoundSystemPlayMusic(SoundSystem,IsAmbient);
    }
    ConfigSetNumber("LevelEnableMusicTrack",SoundValue);
}
void LevelUpdate(Level_t *Level,Camera_t *Camera)
{
    int DynamicData;
    
    if( !Level ) {
        return;
    }
    if( LevelEnableAnimatedSurfaces->IValue ) {

        BSDClearNodesFlag(Level->BSD);
    
        while( (DynamicData = BSDGetCurrentCameraNodeDynamicData(Level->BSD,Camera) ) != -1 ) {
            TSPUpdateDynamicFaces(Level->TSPList,Camera,DynamicData);
        }
    }
    if( LevelEnableAnimatedLights->IValue ) {
        BSDUpdateAnimatedLights(Level->BSD);
        TSPUpdateAnimatedFaces(Level->TSPList,Level->BSD,Camera,0);
    }
}
void LevelDraw(Level_t *Level,Camera_t *Camera,mat4 ProjectionMatrix)
{
    if( !Level ) {
        DPrintf("LevelDraw:Invalid Level\n");
        return;
    }
             
    BSDDrawSky(Level->BSD,Level->VRAM,Camera,ProjectionMatrix);
    BSDDraw(Level->BSD,Level->VRAM,Camera,ProjectionMatrix);
    TSPDrawList(Level->TSPList,Level->VRAM,Camera,ProjectionMatrix);
}

void LevelGetPlayerSpawn(Level_t *Level,int SpawnIndex,vec3 Position,vec3 Rotation)
{
    BSDGetPlayerSpawn(Level->BSD,SpawnIndex,Position,Rotation);
}
void LevelLoadSettings()
{
    LevelEnableWireFrameMode = ConfigGet("LevelEnableWireFrameMode");
    LevelDrawCollisionData = ConfigGet("LevelDrawCollisionData");
    LevelDrawBSPTree = ConfigGet("LevelDrawBSPTree");
    LevelDrawSurfaces = ConfigGet("LevelDrawSurfaces");
    LevelDrawBSDNodesAsPoints = ConfigGet("LevelDrawBSDNodesAsPoints");
    LevelDrawBSDNodesCollisionVolumes = ConfigGet("LevelDrawBSDNodesCollisionVolumes");
    LevelDrawBSDRenderObjectsAsPoints = ConfigGet("LevelDrawBSDRenderObjectsAsPoints");
    LevelDrawBSDRenderObjects = ConfigGet("LevelDrawBSDRenderObjects");
    LevelDrawBSDShowcase = ConfigGet("LevelDrawBSDShowcase");
    LevelEnableFrustumCulling = ConfigGet("LevelEnableFrustumCulling");
    LevelEnableAmbientLight = ConfigGet("LevelEnableAmbientLight");
    LevelEnableSemiTransparency = ConfigGet("LevelEnableSemiTransparency");
    LevelEnableAnimatedLights = ConfigGet("LevelEnableAnimatedLights");
    LevelEnableAnimatedSurfaces = ConfigGet("LevelEnableAnimatedSurfaces");
    LevelEnableMusicTrack = ConfigGet("LevelEnableMusicTrack");
}
Level_t *LevelInit(GUI_t *GUI,VideoSystem_t *VideoSystem,SoundSystem_t *SoundSystem,const char *BasePath,int MissionNumber,int LevelNumber,
               int *GameEngine)
{
    FILE *BSDFile;
    char Buffer[512];
    int i;
    TSP_t *TSP;
    int LocalGameEngine;
    float BasePercentage;
    int PlayAmbientMusic;
    int NumStepsLeft;
    float Increment;
    int IsMultiplayer;
    Level_t *Level;
    
    Level = malloc(sizeof(Level_t));
    //Attempt to load the level...
    if( !Level ){
        printf("LevelInit:Fatal error...couldn't allocate memory for level struct...\n");
        goto Failure;
    }
    ProgressBarReset(GUI->ProgressBar);
    
    Level->Font = NULL;
    Level->BSD = NULL;
    Level->VRAM = NULL;
    Level->TSPList = NULL;
    Level->ImageList = NULL;
    Level->MissionNumber = MissionNumber;
    Level->LevelNumber = LevelNumber;
        
    LevelLoadSettings();

    snprintf(Level->MissionPath,sizeof(Level->MissionPath),"%s%cDATA%cMSN%i%cLVL%i",BasePath,PATH_SEPARATOR,PATH_SEPARATOR,
             Level->MissionNumber,PATH_SEPARATOR,Level->LevelNumber);
    DPrintf("LevelInit:Working directory:%s\n",BasePath);
    DPrintf("LevelInit:Loading level %s Mission %i Level %i\n",Level->MissionPath,Level->MissionNumber,Level->LevelNumber);

    BasePercentage = 0.f;
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,BasePercentage,"Loading all images");
    
    //Step.1 Load all the tims from taf.
    //0 is hardcoded...for the images it doesn't make any difference between 0 and 1
    //but if we need to load all the level sounds then 0 means Standard Mode while 1 American (All voices are translated to english!).
    snprintf(Buffer,sizeof(Buffer),"%s%c%i_%i0.TAF",Level->MissionPath,PATH_SEPARATOR,Level->MissionNumber,Level->LevelNumber);
    Level->ImageList = TIMGetAllImages(Buffer);

    if( !Level->ImageList ) {
        DPrintf("LevelInit:Failed to load TAF file %s\n",Buffer);
        goto Failure;
    }
    BasePercentage += 10;
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,BasePercentage,"Early Loading BSD File");
    
    //Step.2 Partially load the BSD file in order to get the TSP info.
    BSDFile = BSDEarlyInit(&Level->BSD,Level->MissionPath,Level->MissionNumber,Level->LevelNumber);
    if( !BSDFile ) {
        DPrintf("LevelInit:Failed to load BSD file\n");
        goto Failure;
    }
    NumStepsLeft = (Level->BSD->TSPInfo.NumTSP) + 7;
    Increment = (100.f - BasePercentage)  / NumStepsLeft;

    //Read the TSP FILES
    //Step.3 Load all the TSP file based on the data read from the BSD file.
    //Note that we are going to load all the tsp file since we do not know 
    //where in the bsd file it signals to stream/load the next tsp.
    for( i = Level->BSD->TSPInfo.StartingComparment - 1; i < Level->BSD->TSPInfo.TargetInitialCompartment; i++ ) {
       Level->TSPNumberRenderList[i] = i;
    }
    
    for( i = Level->BSD->TSPInfo.StartingComparment; i <= Level->BSD->TSPInfo.NumTSP; i++ ) {
        snprintf(Buffer,sizeof(Buffer),"%s%cTSP0%c%i_%i_C%i.TSP",Level->MissionPath,PATH_SEPARATOR,PATH_SEPARATOR,
                 Level->MissionNumber,Level->LevelNumber,i);
        ProgressBarIncrement(GUI->ProgressBar,VideoSystem,Increment,Buffer);
        TSP = TSPLoad(Buffer,i);
        if( !TSP ) {
            DPrintf("LevelInit:Failed to load TSP File %s\n",Buffer);
            goto Failure;
        }
        TSP->Next = Level->TSPList;
        Level->TSPList = TSP;
    }
    LocalGameEngine = TSPIsVersion3(Level->TSPList) ? MOH_GAME_UNDERGROUND : MOH_GAME_STANDARD;
    if( GameEngine ) {
        *GameEngine = LocalGameEngine;
    }
    //NOTE(Adriano):This is required due to the different BSD RenderObject ID mapping that multiplayer levels use.
    IsMultiplayer = MissionNumber == 12 ? 1 : 0;
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,Increment,"Loading BSD");
    //Step.4 Resume loading the BSD after we successfully loaded the TSP.
    DPrintf("LevelInit: Detected game %s\n",LocalGameEngine == MOH_GAME_STANDARD ? "MOH" : "MOH:Underground");
    if( !BSDLoad(Level->BSD,LocalGameEngine,IsMultiplayer,BSDFile) ) {
        goto Failure;
    }
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,Increment,"Loading VRAM");
    Level->VRAM = VRAMInit(Level->ImageList);
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,Increment,"Loading Font");
    Level->Font = FontInit(Level->VRAM);
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,Increment,"Generating VAOs");
    TSPCreateVAOs(Level->TSPList);
    BSDCreateVAOs(Level->BSD,LocalGameEngine,Level->VRAM);
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,Increment,"Fixing Objects Position");
    BSDFixRenderObjectPosition(Level);
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,Increment,"Loading Music");
    SoundSystemLoadLevelMusic(SoundSystem,Level->MissionPath,MissionNumber,LevelNumber,LocalGameEngine);
    if( LevelEnableMusicTrack->IValue ) {
        PlayAmbientMusic = (LevelEnableMusicTrack->IValue == 2) ? 1 : 0;
        SoundSystemPlayMusic(SoundSystem,PlayAmbientMusic);
    }
    DPrintf("LevelInit:Allocated level struct\n");
    ProgressBarIncrement(GUI->ProgressBar,VideoSystem,100,"Ready");
    return Level;
Failure:
    LevelCleanUp(Level);
    return NULL;
}