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
        DPrintf("LevelIsLoaded:Invalid level\n");
        return 0;
    }
    return (Level->MissionNumber != 0 && Level->LevelNumber != 0);
}


void LevelUnload(Level_t *Level)
{
    if( !Level ) {
        return;
    }
    DPrintf("LevelCleanUp:Deallocating previous allocated Level struct\n");
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
    if( SoundValue > 3 || SoundValue < 0 ) {
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
    vec4 temp;
    mat4 ModelViewMatrix;
    
    if( !Level ) {
        DPrintf("LevelDraw:Invalid Level\n");
    }
    
    glm_mat4_identity(VidConf.ModelViewMatrix);
         
    temp[0] = 1;
    temp[1] = 0;
    temp[2] = 0;
    glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera->Rotation.x), temp);
    temp[0] = 0;
    temp[1] = 1;
    temp[2] = 0;
    glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera->Rotation.y), temp);
    temp[0] = 0;
    temp[1] = 0;
    temp[2] = 1;
    glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera->Rotation.z), temp);
     
    glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
     
     //Emulate PSX Coordinate system...
     glm_rotate_x(VidConf.MVPMatrix,glm_rad(180.f), VidConf.MVPMatrix);
     BSDDrawSky(LevelManager->CurrentLevel->BSD,LevelManager->CurrentLevel->VRAM);
     
     temp[0] = 1;
     temp[1] = 0;
     temp[2] = 0;
     glm_mat4_identity(VidConf.ModelViewMatrix);
     glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera->Rotation.x), temp);
     temp[0] = 0;
     temp[1] = 1;
     temp[2] = 0;
     glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera->Rotation.y), temp);
     temp[0] = 0;
     temp[1] = 0;
     temp[2] = 1;
     glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera->Rotation.z), temp);
     temp[0] = -Camera->Position.x;
     temp[1] = -Camera->Position.y;
     temp[2] = -Camera->Position.z;
     glm_translate(VidConf.ModelViewMatrix,temp);
     
     glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
     
     //Emulate PSX Coordinate system...
     glm_rotate_x(VidConf.MVPMatrix,glm_rad(180.f), VidConf.MVPMatrix);
          
//      BSD2PDraw(Level);
     BSDDraw(LevelManager->CurrentLevel->BSD,LevelManager->CurrentLevel->VRAM,Camera);

     
     temp[0] = 1;
     temp[1] = 0;
     temp[2] = 0;
     glm_mat4_identity(VidConf.ModelViewMatrix);
     glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera->Rotation.x), temp);
     temp[0] = 0;
     temp[1] = 1;
     temp[2] = 0;
     glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera->Rotation.y), temp);
     temp[0] = 0;
     temp[1] = 0;
     temp[2] = 1;
     glm_rotate(VidConf.ModelViewMatrix,glm_rad(Camera->Rotation.z), temp);
     temp[0] = -Camera->Position.x;
     temp[1] = -Camera->Position.y;
     temp[2] = -Camera->Position.z;
     glm_translate(VidConf.ModelViewMatrix,temp);
     
     glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
     
     //Emulate PSX Coordinate system...
     glm_rotate_x(VidConf.MVPMatrix,glm_rad(180.f), VidConf.MVPMatrix);
     
     glm_frustum_planes(VidConf.MVPMatrix,Camera->FrustumPlaneList);
     glm_frustum_corners(VidConf.MVPMatrix,Camera->FrustumCornerList);
     TSPDrawList(LevelManager->CurrentLevel->TSPList,LevelManager->CurrentLevel->VRAM,Camera);

}

Vec3_t LevelGetPlayerSpawn(Level_t *Level,int SpawnIndex,Vec3_t *Rotation)
{
    Vec3_t Position;
    Position = BSDGetPlayerSpawn(Level->BSD,SpawnIndex,Rotation);
    return Position;
}
void LevelLoadSettings()
{
    LevelEnableWireFrameMode = ConfigGet("LevelEnableWireFrameMode");
    LevelDrawCollisionData = ConfigGet("LevelDrawCollisionData");
    LevelDrawBSPTree = ConfigGet("LevelDrawBSPTree");
    LevelDrawSurfaces = ConfigGet("LevelDrawSurfaces");
    LevelDrawBSDNodesAsPoints = ConfigGet("LevelDrawBSDNodesAsPoints");
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
bool LevelInit(Level_t *Level,GUI_t *GUI,SoundSystem_t *SoundSystem,char *BasePath,int MissionNumber,int LevelNumber,int *GameEngine)
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

    
    //Attempt to load the level...
    if( !Level ){
        DPrintf("LevelInit:Fatal error...called without a valid level...\n");
        return false;
    }
    GUIProgressBarReset(GUI);
    GUIProgressBarIncrement(GUI,5,"Unloading Previous Level");
    if( LevelIsLoaded(Level) ) {
        LevelUnload(Level);
    }
    
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

    BasePercentage = 5.f;
    GUIProgressBarIncrement(GUI,BasePercentage,"Loading all images");
    //Step.1 Load all the tims from taf.
    //0 is hardcoded...for the images it doesn't make any difference between 0 and 1
    //but if we need to load all the level sounds then 0 means Standard Mode while 1 American (All voices are translated to english!).
    snprintf(Buffer,sizeof(Buffer),"%s%c%i_%i0.TAF",Level->MissionPath,PATH_SEPARATOR,Level->MissionNumber,Level->LevelNumber);
    Level->ImageList = TIMGetAllImages(Buffer);

    if( !Level->ImageList ) {
        DPrintf("LevelInit:Failed to load TAF file %s\n",Buffer);
        return false;
    }
    BasePercentage += 5;
    GUIProgressBarIncrement(GUI,BasePercentage,"Early Loading BSD File");
    //Step.2 Partially load the BSD file in order to get the TSP info.
    BSDFile = BSDEarlyInit(&Level->BSD,Level->MissionPath,Level->MissionNumber,Level->LevelNumber);
    if( !BSDFile ) {
        DPrintf("LevelInit:Failed to load BSD file\n");
        return false;
    }
    NumStepsLeft = (Level->BSD->TSPInfo.NumTSP) + 7;
    Increment = (100.f - BasePercentage)  / NumStepsLeft;
//     assert(1!=1);
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
        GUIProgressBarIncrement(GUI,Increment,Buffer);
        TSP = TSPLoad(Buffer,i);
        if( !TSP ) {
            DPrintf("LevelInit:Failed to load TSP File %s\n",Buffer);
            return false;
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
    GUIProgressBarIncrement(GUI,Increment,"Loading BSD");
    //Step.4 Resume loading the BSD after we successfully loaded the TSP.
    DPrintf("LevelInit: Detected game %s\n",LocalGameEngine == MOH_GAME_STANDARD ? "MOH" : "MOH:Underground");
    BSDLoad(Level->BSD,LocalGameEngine,IsMultiplayer,BSDFile);
    GUIProgressBarIncrement(GUI,Increment,"Loading VRAM");
    Level->VRAM = VRAMInit(Level->ImageList);
    GUIProgressBarIncrement(GUI,Increment,"Loading Font");
    Level->Font = FontInit(Level->VRAM);
    GUIProgressBarIncrement(GUI,Increment,"Generating VAOs");
    TSPCreateNodeBBoxVAO(Level->TSPList);
    TSPCreateCollisionVAO(Level->TSPList);
    BSDCreateVAOs(Level->BSD,LocalGameEngine,Level->VRAM);
    GUIProgressBarIncrement(GUI,Increment,"Fixing Objects Position");
    BSDFixRenderObjectPosition(Level);
    GUIProgressBarIncrement(GUI,Increment,"Loading Music");
    SoundSystemLoadLevelMusic(SoundSystem,Level->MissionPath,MissionNumber,LevelNumber,LocalGameEngine);
    if( LevelEnableMusicTrack->IValue ) {
        PlayAmbientMusic = (LevelEnableMusicTrack->IValue == 2) ? 1 : 0;
        SoundSystemPlayMusic(SoundSystem,PlayAmbientMusic);
    }
    DPrintf("LevelInit:Allocated level struct\n");
    GUIProgressBarIncrement(GUI,100,"Ready");
    return true;
    
}
