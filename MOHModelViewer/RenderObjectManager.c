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
#include "RenderObjectManager.h"
#include "MOHModelViewer.h"

int RenderObjectManagerLoadBSD(RenderObjectManager_t *RenderObjectManager,const char *File)
{
   if( !RenderObjectManager ) {
       DPrintf("RenderObjectManagerLoadBSD:Invalid RenderObjectManager\n");
       return 0;
   }
   if( !File ) {
       DPrintf("RenderObjectManagerLoadBSD:Invalid file name\n");
       return 0;
   }
   return 1;
}
RenderObjectManager_t *RenderObjectManagerInit()
{
    RenderObjectManager_t *RenderObjectManager;
    
    RenderObjectManager = malloc(sizeof(RenderObjectManager_t));
    RenderObjectManager->BSDList = NULL;
    return RenderObjectManager;
}
