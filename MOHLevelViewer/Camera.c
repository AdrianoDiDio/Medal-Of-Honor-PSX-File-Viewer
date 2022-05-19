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
void CameraFixAngles(Camera_t *Camera)
{
    //If needed fix it.
    if ( Camera->Rotation.x > 90.0f ) {
        Camera->Rotation.x = 90.0f;
    }

    if ( Camera->Rotation.x < -90.0f ) {
        Camera->Rotation.x = -90.0f;
    }

    if ( Camera->Rotation.y > 180.0f ) {
        Camera->Rotation.y -= 360.0f;
    }

    if ( Camera->Rotation.y < -180.0f ) {
        Camera->Rotation.y += 360.0f;
    }

}

void CameraOnMouseEvent(Camera_t *Camera,int Dx,int Dy)
{
    Camera->Rotation.x += ( ( Dy - (VidConfigHeight->IValue/2) ) / 10.f ) * CameraMouseSensitivity->FValue; // 0.001f;
    Camera->Rotation.y += ( ( Dx - (VidConfigWidth->IValue/2)) / 10.f ) * CameraMouseSensitivity->FValue; // 0.001f;
    CameraFixAngles(Camera);
}

void CameraUpdate(Camera_t *Camera,int Orientation, float Delta)
{
    float CamSpeed;
    
    CamSpeed = CameraSpeed->FValue * Delta * 128.f;

    Camera->Forward.x = sin(DEGTORAD(Camera->Rotation.y));
    Camera->Forward.z = -cos(DEGTORAD(Camera->Rotation.y));
    Camera->Forward.y = -sin(DEGTORAD(Camera->Rotation.x));
    
    Camera->Right.x = -(float)(cos(DEGTORAD(Camera->Rotation.y)));
    Camera->Right.z = -(float)(sin(DEGTORAD(Camera->Rotation.y)));
    
    switch ( Orientation ) {
        case DIR_FORWARD:
            Vec3Scale(Camera->Forward,CamSpeed,&Camera->Forward);
            Vec3Add(Camera->Position,Camera->Forward,&Camera->Position);
            break;
        case DIR_BACKWARD:
            Vec3Scale(Camera->Forward,CamSpeed,&Camera->Forward);
            Vec3Subtract(Camera->Position,Camera->Forward,&Camera->Position);
            break;
        case DIR_UPWARD:
            Camera->Position.y += CamSpeed;
            break;
        case DIR_DOWNWARD:
            Camera->Position.y -= CamSpeed;
            break;
        case DIR_LEFTWARD:
            Vec3Scale(Camera->Right,CamSpeed,&Camera->Right);
            Vec3Add(Camera->Position,Camera->Right,&Camera->Position);
            break;
        case DIR_RIGHTWARD:
            Vec3Scale(Camera->Right,CamSpeed,&Camera->Right);
            Vec3Subtract(Camera->Position,Camera->Right,&Camera->Position);
            break;
         case LOOK_LEFT:
             Camera->Rotation.y -= CamSpeed;
             if ( Camera->Rotation.y < -360.f ) {
                 Camera->Rotation.y += 360.f;
             }
             break;
         case LOOK_RIGHT:
             Camera->Rotation.y += CamSpeed;
             if ( Camera->Rotation.y > 360.f ) {
                 Camera->Rotation.y -= 360.f;
             }
             break;
         case LOOK_DOWN:
             Camera->Rotation.x += CamSpeed;
             break;
         case LOOK_UP:
             Camera->Rotation.x -= CamSpeed;
             break;
        default:
            break;
    }
}
void CameraBeginFrame(Camera_t *Camera)
{
    vec3 Axis;
    glm_mat4_identity(Camera->ViewMatrix);
    Axis[0] = 1;
    Axis[1] = 0;
    Axis[2] = 0;
    glm_mat4_identity(Camera->ViewMatrix);
    glm_rotate(Camera->ViewMatrix,glm_rad(Camera->Rotation.x), Axis);
    Axis[0] = 0;
    Axis[1] = 1;
    Axis[2] = 0;
    glm_rotate(Camera->ViewMatrix,glm_rad(Camera->Rotation.y), Axis);
    Axis[0] = 0;
    Axis[1] = 0;
    Axis[2] = 1;
    glm_rotate(Camera->ViewMatrix,glm_rad(Camera->Rotation.z), Axis);
    Axis[0] = -Camera->Position.x;
    Axis[1] = -Camera->Position.y;
    Axis[2] = -Camera->Position.z;
    glm_translate(Camera->ViewMatrix,Axis);
}
void CameraSetRotation(Camera_t *Camera,Vec3_t Rotation)
{
    if( !Camera ) {
        DPrintf("CameraSetRotation:Invalid camera\n");
        return;
    }
    Camera->Rotation = Rotation;
}
void CameraSetPosition(Camera_t *Camera,Vec3_t Position)
{
    if( !Camera ) {
        DPrintf("CameraSetPosition:Invalid camera\n");
        return;
    }
    Camera->Position = Position;
}
Camera_t *CameraInit()
{
    Camera_t *Camera;
    
    Camera = malloc(sizeof(Camera_t));
    if( !Camera ) {
        printf("CameraInit:Failed to allocate memory for struct\n");
        return NULL;
    }

    Camera->Position = Vec3Build(0.f,0.f,0.f);
    Camera->Rotation = Vec3Build(0.f,0.f,0.f);

    Camera->Up = Vec3Build(0.0f,1.0f,0.0f);
    Camera->Right = Vec3Build(1.0f,0.0f,0.0f);
    Camera->Forward = Vec3Build(0.0f,0.0f,1.0f);

    CameraFixAngles(Camera);
    
    CameraSpeed = ConfigGet("CameraSpeed");
    CameraMouseSensitivity = ConfigGet("CameraMouseSensitivity");
    return Camera;
}
