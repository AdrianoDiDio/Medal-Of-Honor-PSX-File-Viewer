// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    GFXModelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GFXModelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GFXModelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/
#include "Camera.h" 
#include "GFXModelViewer.h"

Config_t *CameraMouseSensitivity;
Config_t *CameraSpeed;

void CameraCleanUp(Camera_t *Camera)
{
    if( !Camera ) {
        return;
    }
    free(Camera);
}

void CameraOnAngleUpdate(Camera_t *Camera)
{
    float ThetaMax;
    ThetaMax = M_PI / 2.0f;
    if (Camera->Position.Theta > ThetaMax) {
        Camera->Position.Theta = ThetaMax;
    }
    if (Camera->Position.Theta < -ThetaMax) {
        Camera->Position.Theta = -ThetaMax;
    }
}
void CameraZoom(Camera_t *Camera,float Distance)
{
    Camera->Position.Radius += Distance * CameraMouseSensitivity->FValue * 10.f;
}
void CameraOnMouseEvent(Camera_t *Camera,int Dx,int Dy)
{
    Camera->Position.Phi += Dx * CameraMouseSensitivity->FValue * 0.02;
    Camera->Position.Theta += Dy * CameraMouseSensitivity->FValue * 0.02;

    CameraOnAngleUpdate(Camera);
}
void CameraUpdateEyeVector(Camera_t *Camera)
{
    Camera->Eye[0] = Camera->ViewPoint[0] + Camera->Position.Radius * cos(Camera->Position.Theta) * cos(Camera->Position.Phi);
    Camera->Eye[1] = Camera->ViewPoint[1] + Camera->Position.Radius * sin(Camera->Position.Theta);
    Camera->Eye[2] = Camera->ViewPoint[2] + Camera->Position.Radius * cos(Camera->Position.Theta) * sin(Camera->Position.Phi);
}
void CameraGetNormalizedDirection(Camera_t *Camera,vec3 Direction)
{
    glm_vec3_sub(Camera->ViewPoint,Camera->Eye,Direction);
    glm_vec3_normalize(Direction);
}
void CameraGetForwardVector(Camera_t *Camera,vec3 Forward)
{
    vec3 Direction;
    CameraGetNormalizedDirection(Camera,Direction);
    glm_vec3_add(Direction,Camera->Eye,Forward);
}
void CameraUpdateViewMatrix(Camera_t *Camera)
{
    vec3 Forward;
    glm_mat4_identity(Camera->ViewMatrix);
    CameraUpdateEyeVector(Camera);
    CameraGetForwardVector(Camera,Forward);
    glm_lookat(Camera->Eye,Forward,GLM_YUP,Camera->ViewMatrix);
}
void CameraUpdate(Camera_t *Camera,int Orientation, float Delta)
{
    float CamSpeed;
    vec3 Direction;
    vec3 Right;

    
    CamSpeed = CameraSpeed->FValue * Delta * 128.f;
    CameraGetNormalizedDirection(Camera,Direction);
    glm_vec3_cross(GLM_YUP,Direction,Right);    
    glm_vec3_normalize(Right);
    glm_vec3_scale(Right,CamSpeed,Right);
    switch( Orientation ) {
        case CAMERA_DIRECTION_UPWARD:
            Camera->ViewPoint[1] += CamSpeed;
            break;
        case CAMERA_DIRECTION_DOWNWARD:
            Camera->ViewPoint[1] -= CamSpeed;
            break;
        case CAMERA_DIRECTION_LEFTWARD:
            glm_vec3_add(Camera->ViewPoint,Right,Camera->ViewPoint);
            break;
        case CAMERA_DIRECTION_RIGHTWARD:
            glm_vec3_sub(Camera->ViewPoint,Right,Camera->ViewPoint);
            break;
    }
}
void CameraCheckKeyEvents(Camera_t *Camera,const bool *KeyState,float Delta)
{
    if( KeyState[SDL_SCANCODE_A] ) {
        CameraUpdate(Camera,CAMERA_DIRECTION_LEFTWARD,Delta);
    }
    if( KeyState[SDL_SCANCODE_D] ) {
        CameraUpdate(Camera,CAMERA_DIRECTION_RIGHTWARD,Delta);
    }
    if( KeyState[SDL_SCANCODE_SPACE] ) {
        CameraUpdate(Camera,CAMERA_DIRECTION_UPWARD,Delta);
    }
    if( KeyState[SDL_SCANCODE_Z] ) {
        CameraUpdate(Camera,CAMERA_DIRECTION_DOWNWARD,Delta);
    }
}
void CameraBeginFrame(Camera_t *Camera)
{
    //NOTE(Adriano):Update it even if not focused in order to have a valid matrix available to all subsystems.
    CameraUpdateViewMatrix(Camera);
} 

void CameraReset(Camera_t *Camera)
{
    if( !Camera ) {
        DPrintf("CameraReset:Invalid camera\n");
        return;
    }
    glm_vec3_zero(Camera->ViewPoint);
    Camera->Position.Radius = 350.f;
    Camera->Position.Theta = 0.f;
    Camera->Position.Phi = 0.f;
    CameraOnAngleUpdate(Camera);
}
Camera_t *CameraInit()
{
    Camera_t *Camera;
    
    Camera = malloc(sizeof(Camera_t));
    if( !Camera ) {
        printf("CameraInit:Failed to allocate memory for struct\n");
        return NULL;
    }
    CameraReset(Camera);
    CameraSpeed = ConfigGet("CameraSpeed");
    CameraMouseSensitivity = ConfigGet("CameraMouseSensitivity");
    return Camera;
}
