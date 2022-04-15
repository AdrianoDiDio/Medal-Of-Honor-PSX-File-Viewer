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

#include "MOHLevelViewer.h"

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

int LevelManagerSetPath(LevelManager_t *LevelManager,char *Path)
{
    int GameEngine;
    if( !LevelManager ) {
        DPrintf("LevelManagerSetPath:Called without a valid struct\n");
        return 0;
    }
    if( !Path ) {
        DPrintf("LevelManagerSetPath:Called without a valid path\n");
        return 0;
    }
    LevelManager->BasePath = StringCopy(Path);

    if( !LevelInit(LevelManager->CurrentLevel,LevelManager->BasePath,1,1,&GameEngine) ) {
        if( !LevelInit(LevelManager->CurrentLevel,LevelManager->BasePath,2,1,&GameEngine) ) {
            DPrintf("LevelManagerSetPath:Invalid path...\n");
            LevelManager->IsPathSet = 0;
            return 0;
        }
    }
    LevelManager->GameEngine = GameEngine;
    sprintf(LevelManager->EngineName,"Engine %s",GameEngine == MOH_GAME_STANDARD ? "Medal Of Honor" : "Medal of Honor:Underground");
    LevelManager->IsPathSet = 1;
    return 1;
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
