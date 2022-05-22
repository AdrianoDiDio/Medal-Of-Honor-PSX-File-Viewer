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
#include "Camera.h" 
#include "MOHLevelViewer.h"


Config_t *CameraSpeed;
Config_t *CameraMouseSensitivity;

void CameraCleanUp(Camera_t *Camera)
{
    if( !Camera ) {
        return;
    }
    free(Camera);
}

void CameraUpdateVectors(Camera_t *Camera)
{
    Camera->Forward[0] =  sin(DEGTORAD(Camera->Rotation[YAW])) * cos(DEGTORAD(Camera->Rotation[PITCH]));
    Camera->Forward[1] =  -sin(DEGTORAD(Camera->Rotation[PITCH]));
    Camera->Forward[2] = -cos(DEGTORAD(Camera->Rotation[YAW])) * cos(DEGTORAD(Camera->Rotation[PITCH]));
    
    glm_vec3_cross(GLM_YUP,Camera->Forward,Camera->Right);
/*    
    Camera->Right[0] = -(float)(cos(DEGTORAD(Camera->Rotation[YAW])));
    Camera->Right[1] = 0;
    Camera->Right[2] = -(float)(sin(DEGTORAD(Camera->Rotation[YAW])));*/
    
    glm_vec3_normalize(Camera->Forward);
    glm_vec3_normalize(Camera->Right);
}
void CameraOnAngleUpdate(Camera_t *Camera)
{
    //If needed fix it.
    if ( Camera->Rotation[PITCH] > 89.0f ) {
        Camera->Rotation[PITCH] = 89.0f;
    }

    if ( Camera->Rotation[PITCH] < -89.0f ) {
        Camera->Rotation[PITCH] = -89.0f;
    }

    if ( Camera->Rotation[YAW] > 180.0f ) {
        Camera->Rotation[YAW] -= 360.0f;
    }

    if ( Camera->Rotation[YAW] < -180.0f ) {
        Camera->Rotation[YAW] += 360.0f;
    }
    CameraUpdateVectors(Camera);
}
void CameraLostFocus(Camera_t *Camera)
{
    Camera->HasFocus = 0;
}
void CameraOnMouseEvent(Camera_t *Camera,int Dx,int Dy)
{
    if( !Camera->HasFocus ) {
        Dx = VidConfigWidth->IValue/2;
        Dy = VidConfigHeight->IValue/2;
        Camera->HasFocus = 1;
    }
    Camera->Rotation[PITCH] += ( ( Dy - (VidConfigHeight->IValue/2) ) / 10.f ) * CameraMouseSensitivity->FValue; // 0.001f;
    Camera->Rotation[YAW] += ( ( Dx - (VidConfigWidth->IValue/2)) / 10.f ) * CameraMouseSensitivity->FValue; // 0.001f;
    CameraOnAngleUpdate(Camera);
}

void CameraUpdate(Camera_t *Camera,int Orientation, float Delta)
{
    float CamSpeed;
    vec3 Forward;
    vec3 Right;

    
    CamSpeed = CameraSpeed->FValue * Delta * 128.f;

    switch ( Orientation ) {
        case CAMERA_DIRECTION_FORWARD:
            glm_vec3_scale(Camera->Forward,CamSpeed,Forward);
            glm_vec3_add(Camera->Position,Forward,Camera->Position);
            break;
        case CAMERA_DIRECTION_BACKWARD:
            glm_vec3_scale(Camera->Forward,CamSpeed,Forward);
            glm_vec3_sub(Camera->Position,Forward,Camera->Position);
            break;
        case CAMERA_DIRECTION_UPWARD:
            Camera->Position[1] += CamSpeed;
            break;
        case CAMERA_DIRECTION_DOWNWARD:
            Camera->Position[1] -= CamSpeed;
            break;
        case CAMERA_DIRECTION_LEFTWARD:
            glm_vec3_scale(Camera->Right,CamSpeed,Right);
            glm_vec3_add(Camera->Position,Right,Camera->Position);
            break;
        case CAMERA_DIRECTION_RIGHTWARD:
            glm_vec3_scale(Camera->Right,CamSpeed,Right);
            glm_vec3_sub(Camera->Position,Right,Camera->Position);
            break;
         case CAMERA_LOOK_LEFT:
             Camera->Rotation[YAW] -= CamSpeed;
             CameraOnAngleUpdate(Camera);
             break;
         case CAMERA_LOOK_RIGHT:
             Camera->Rotation[YAW] += CamSpeed;
             CameraOnAngleUpdate(Camera);
             break;
         case CAMERA_LOOK_DOWN:
             Camera->Rotation[PITCH] += CamSpeed;
             CameraOnAngleUpdate(Camera);
             break;
         case CAMERA_LOOK_UP:
             Camera->Rotation[PITCH] -= CamSpeed;
             CameraOnAngleUpdate(Camera);
             break;
        default:
            break;
    }
}
void CameraBeginFrame(Camera_t *Camera)
{
    vec3 Direction;
    glm_mat4_identity(Camera->ViewMatrix);
    glm_vec3_add(Camera->Position,Camera->Forward,Direction);
    glm_lookat(Camera->Position,Direction,GLM_YUP,Camera->ViewMatrix);
}
void CameraSetRotation(Camera_t *Camera,Vec3_t Rotation)
{
    if( !Camera ) {
        DPrintf("CameraSetRotation:Invalid camera\n");
        return;
    }
    Camera->Rotation[PITCH] = Rotation.x;
    Camera->Rotation[YAW] = Rotation.y;
    Camera->Rotation[ROLL] = Rotation.z;
    
    CameraOnAngleUpdate(Camera);
}
void CameraSetPosition(Camera_t *Camera,Vec3_t Position)
{
    if( !Camera ) {
        DPrintf("CameraSetPosition:Invalid camera\n");
        return;
    }
    Camera->Position[0] = Position.x;
    Camera->Position[1] = Position.y;
    Camera->Position[2] = Position.z;
}
Camera_t *CameraInit()
{
    Camera_t *Camera;
    
    Camera = malloc(sizeof(Camera_t));
    if( !Camera ) {
        printf("CameraInit:Failed to allocate memory for struct\n");
        return NULL;
    }
    
    Camera->HasFocus = 0;
    glm_vec3_zero(Camera->Position);
    glm_vec3_zero(Camera->Rotation);

    glm_vec3_zero(Camera->Right);
    glm_vec3_zero(Camera->Forward);

    CameraOnAngleUpdate(Camera);
    CameraSpeed = ConfigGet("CameraSpeed");
    CameraMouseSensitivity = ConfigGet("CameraMouseSensitivity");
    return Camera;
}
