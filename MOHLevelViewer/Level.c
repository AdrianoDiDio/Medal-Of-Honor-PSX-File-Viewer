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
#include "BSD.h"
#include "TSP.h"
#include "BSDTwoPlayers.h"
#include "GUI.h"
#include "MOHLevelViewer.h"
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
void LevelSetDefaultSettings(LevelSettings_t *LevelSettings)
{
    LevelSettings->ShowMap = true;
    LevelSettings->ShowBSDNodes = true;
    LevelSettings->ShowBSDRenderObject = true;
    LevelSettings->DrawBSDRenderObjects = true;
    LevelSettings->DrawBSDShowCaseRenderObject = false;
    LevelSettings->EnableFrustumCulling = true;
    LevelSettings->EnableLighting = true;
    LevelSettings->EnableSemiTransparency = true;
    LevelSettings->EnableAnimatedLights = true;
    LevelSettings->WireFrame = false;
    LevelSettings->ShowAABBTree = false;
    LevelSettings->ShowCollisionData = false;
}

bool LevelInit(Level_t *Level,GUI_t *GUI,char *BasePath,int MissionNumber,int LevelNumber,int *GameEngine)
{
    FILE *BSDFile;
    char Buffer[512];
    int i;
    TSP_t *TSP;
    int LocalGameEngine;
    float BasePercentage;
    
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

    snprintf(Level->MissionPath,sizeof(Level->MissionPath),"%s/DATA/MSN%i/LVL%i",BasePath,Level->MissionNumber,Level->LevelNumber);
    DPrintf("LevelInit:Working directory:%s\n",BasePath);
    DPrintf("LevelInit:Loading level %s Mission %i Level %i\n",Level->MissionPath,Level->MissionNumber,Level->LevelNumber);

    BasePercentage = 10.f;
    GUIProgressBarIncrement(GUI,BasePercentage,"Loading all images");
    //Step.1 Load all the tims from taf.
    //0 is hardcoded...for the images it doesn't make any difference between 0 and 1
    //but if we need to load all the level sounds then 0 means Standard Mode while 1 American (All voices are translated to english!).
    snprintf(Buffer,sizeof(Buffer),"%s/%i_%i0.TAF",Level->MissionPath,Level->MissionNumber,Level->LevelNumber);
    Level->ImageList = TIMGetAllImages(Buffer);

    if( !Level->ImageList ) {
        DPrintf("LevelInit:Failed to load TAF file %s\n",Buffer);
        return false;
    }
    BasePercentage += 10;
    GUIProgressBarIncrement(GUI,BasePercentage,"Early Loading BSD File");
    //Step.2 Partially load the BSD file in order to get the TSP info.
    BSDFile = BSDEarlyInit(Level);
    if( !BSDFile ) {
        DPrintf("LevelInit:Failed to load BSD file\n");
        return false;
    }
    float NumStepsLeft = (Level->BSD->TSPInfo.NumTSP) + 5;
    float Increment = (100.f - BasePercentage) / NumStepsLeft;
    //Read the TSP FILES
    //Step.3 Load all the TSP file based on the data read from the BSD file.
    //Note that we are going to load all the tsp file since we do not know 
    //where in the bsd file it signals to stream/load the next tsp.
    for( i = Level->BSD->TSPInfo.StartingComparment - 1; i < Level->BSD->TSPInfo.TargetInitialCompartment; i++ ) {
       Level->TSPNumberRenderList[i] = i;
    }
    
    for( i = Level->BSD->TSPInfo.StartingComparment; i <= Level->BSD->TSPInfo.NumTSP; i++ ) {
        snprintf(Buffer,sizeof(Buffer),"%s/TSP0/%i_%i_C%i.TSP",Level->MissionPath,Level->MissionNumber,Level->LevelNumber,i);
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
    GUIProgressBarIncrement(GUI,Increment,"Loading BSD");
    //Step.4 Resume loading the BSD after we successfully loaded the TSP.
    DPrintf("LevelInit: Detected game %s\n",LocalGameEngine == MOH_GAME_STANDARD ? "MOH" : "MOH:Underground");
    BSDLoad(Level,LocalGameEngine,BSDFile);
    GUIProgressBarIncrement(GUI,Increment,"Loading VRAM");
    Level->VRAM = VRAMInit(Level->ImageList);
    GUIProgressBarIncrement(GUI,Increment,"Loading Font");
    Level->Font = FontInit(Level->VRAM);
    GUIProgressBarIncrement(GUI,Increment,"Generating VAOs");
    TSPCreateNodeBBoxVAO(Level->TSPList);
    TSPCreateCollisionVAO(Level->TSPList);
    BSDCreateVAOs(Level->BSD,Level->VRAM);
    GUIProgressBarIncrement(GUI,Increment,"Fixing Objects Position");
    BSDFixRenderObjectPosition(Level);
    CamInit(&Camera,Level->BSD);
    DPrintf("LevelInit:Allocated level struct\n");
//     GUIProgressBarIncrement(GUI,99,"Done");
    return true;
    
}
