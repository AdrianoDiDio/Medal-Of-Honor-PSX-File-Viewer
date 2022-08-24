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
#ifndef __CAMERA_H_
#define __CAMERA_H_

#include "Common.h"
#include "Config.h"

#define PITCH 0
#define YAW 1
#define ROLL 2

typedef enum {
    CAMERA_LOOK_LEFT,
    CAMERA_LOOK_RIGHT,
    CAMERA_LOOK_UP,
    CAMERA_LOOK_DOWN,
    CAMERA_DIRECTION_FORWARD,
    CAMERA_DIRECTION_BACKWARD,
    CAMERA_DIRECTION_UPWARD,
    CAMERA_DIRECTION_DOWNWARD,
    CAMERA_DIRECTION_LEFTWARD,
    CAMERA_DIRECTION_RIGHTWARD
} CameraDirection_t;

typedef struct PolarCoordinate_s {
    float Radius;
    float Theta;
    float Phi;
} PolarCoordinate_t;

typedef struct Camera_s {
    PolarCoordinate_t   Position;
    vec3                Eye;
    mat4                ViewMatrix;
    
} Camera_t;

extern Config_t *CameraSpeed;
extern Config_t *CameraMouseSensitivity;

Camera_t    *CameraInit();
void        CameraBeginFrame(Camera_t *Camera);
void        CameraReset(Camera_t *Camera);
void        CameraOnMouseEvent(Camera_t *Camera,int Dx,int Dy);
void        CameraZoom(Camera_t *Camera,float Distance);
void        CameraCleanUp(Camera_t *Camera);
#endif//__CAMERA_H_
