/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
    TIMViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TIMViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TIMViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 

#ifndef __TIM_VIEWER_H_
#define __TIM_VIEWER_H_

#include "../Common/Common.h"
#include "../Common/Config.h"
#include "../Common/Engine.h"
#include "GUI.h"
#include "ImageManager.h"


typedef struct Application_s {
    Engine_t                    *Engine;
    GUI_t                       *GUI;
    ImageManager_t              *ImageManager;
} Application_t;

void        Quit(Application_t *Application);

#endif //__TIM_VIEWER_H_
