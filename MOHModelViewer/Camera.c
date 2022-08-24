// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
    MOHModelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MOHModelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MOHModelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/
#include "Camera.h" 
#include "MOHModelViewer.h"

Config_t *CameraMouseSensitivity;

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

void CameraUpdateViewMatrix(Camera_t *Camera)
{
    vec3 Direction;
    glm_mat4_identity(Camera->ViewMatrix);
    Camera->Eye[0] = Camera->Position.Radius * cos(Camera->Position.Theta) * cos(Camera->Position.Phi);
    Camera->Eye[1] = Camera->Position.Radius * sin(Camera->Position.Theta);
    Camera->Eye[2] = Camera->Position.Radius * cos(Camera->Position.Theta) * sin(Camera->Position.Phi);
    
    glm_vec3_negate_to(Camera->Eye,Direction);
    glm_vec3_normalize(Direction);
    glm_vec3_add(Direction,Camera->Eye,Direction);
    glm_lookat(Camera->Eye,Direction,GLM_YUP,Camera->ViewMatrix);
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
    Camera->Position.Radius = 150.f;
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
    CameraMouseSensitivity = ConfigGet("CameraMouseSensitivity");
    return Camera;
}
