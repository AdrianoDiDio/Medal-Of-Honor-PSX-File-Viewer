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
#ifndef __CAMERA_H_
#define __CAMERA_H_

#include "Common.h"
#include "Config.h"

typedef enum {
    LOOK_LEFT,
    LOOK_RIGHT,
    LOOK_UP,
    LOOK_DOWN,
    DIR_FORWARD,
    DIR_BACKWARD,
    DIR_UPWARD,
    DIR_DOWNWARD,
    DIR_LEFTWARD,
    DIR_RIGHTWARD
} CameraDirection_t;

typedef struct Camera_s {
    Vec3_t Position;
    Vec3_t Rotation;
    //Movements...
    Vec3_t	Up;
    Vec3_t	Right;
    Vec3_t	Forward;
    
    mat4    ViewMatrix;
} Camera_t;

extern Config_t *CameraSpeed;
extern Config_t *CameraMouseSensitivity;

Camera_t    *CameraInit();
void        CameraBeginFrame(Camera_t *Camera);
void        CameraSetPosition(Camera_t *Camera,Vec3_t Position);
void        CameraSetRotation(Camera_t *Camera,Vec3_t Rotation);
void        CameraOnMouseEvent(Camera_t *Camera,int Dx,int Dy);
void        CameraUpdate(Camera_t *Camera,int Orientation, float Delta);
void        CameraCleanUp(Camera_t *Camera);
#endif//__CAMERA_H_
