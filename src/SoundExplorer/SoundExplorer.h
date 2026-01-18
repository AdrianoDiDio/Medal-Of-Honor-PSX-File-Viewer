/*
===========================================================================
    Copyright (C) 2018-2026 Adriano Di Dio.
    
    SoundExplorer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SoundExplorer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SoundExplorer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 

#ifndef __SOUND_EXPLORER_H_
#define __SOUND_EXPLORER_H_

#include "../Common/Common.h"
#include "../Common/Config.h"
#include "../Common/Engine.h"
#include "GUI.h"
#include "SoundManager.h"


typedef struct Application_s {
    Engine_t                    *Engine;
    GUI_t                       *GUI;
    SoundManager_t              *SoundManager;
} Application_t;

void        Quit(Application_t *Application);

#endif //__SOUND_EXPLORER_H_
