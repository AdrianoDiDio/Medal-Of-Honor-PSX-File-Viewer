/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
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
 
#include "MOHLevelViewer.h"
 
/*
    FONT DESCRIPTION

Each taf file contains the data of the used font.
It starts at 224;0 and ends at 244;0.
This means that we have 2 columns where each column has a width of 10.
Each Font Sprite has an eight of 7.
24 row * 7 height = 168
*/
void FontInit(Level_t *Level)
{
    if( !Level ) {
        DPrintf("FontInit: Invalid Level\n");
        return;
    }
}
