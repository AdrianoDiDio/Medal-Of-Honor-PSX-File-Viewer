/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    SSTViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SSTViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SSTViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#ifndef __SSTVIEWER_H_
#define __SSTVIEWER_H_ 

#include "../Common/Common.h"
#include "../Common/Config.h"
#include "../Common/Video.h"
#include "../Common/Engine.h"
#include "Camera.h"
#include "LevelManager.h"
#include "GUI.h"

typedef struct Application_s {
    Engine_t                    *Engine;
    LevelManager_t              *LevelManager;
    Camera_t                    *Camera;
    GUI_t                       *GUI;
} Application_t;

void        Quit(Application_t *Application);

#endif //__SSTVIEWER_H_
