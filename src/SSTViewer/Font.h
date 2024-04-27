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

#ifndef __FONT_H_
#define __FONT_H_

#include "../Common/Common.h"
#include "../Common/VRAM.h"
#include "../Common/VAO.h"
#include "Camera.h"

#define MOH_FONT_TEXTURE_VRAM_PAGE 15
#define NUM_MOH_FONT_CHARS 50
#define NUM_MOH_FONT_CHAR_PER_COLUMN 25
#define MOH_FONT_CHAR_WIdTH 10
#define MOH_FONT_CHAR_HEIGHT 7
#define MOH_FONT_CHAR_STARTING_TEXTURE_X 224
#define MOH_FONT_CHAR_STARTING_TEXTURE_Y 0

typedef struct Font_s {
    VAO_t *Characters[NUM_MOH_FONT_CHARS];
} Font_t;

Font_t *FontInit(const VRAM_t *VRAM);
void FontDrawString(Font_t *Font,const VRAM_t *VRAM,const char *String,float x,float y,Color4f_t Color);
void FontFree(Font_t *Font);
#endif //__FONT_H_
