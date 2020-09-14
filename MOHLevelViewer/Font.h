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

#ifndef __FONT_H_
#define __FONT_H_

#define MOH_FONT_TEXTURE_VRAM_PAGE 15
#define NUM_MOH_FONT_CHARS 50
#define NUM_MOH_FONT_CHAR_PER_COLUMN 25
#define MOH_FONT_CHAR_WIDTH 10
#define MOH_FONT_CHAR_HEIGHT 7
#define MOH_FONT_CHAR_STARTING_TEXTURE_X 224
#define MOH_FONT_CHAR_STARTING_TEXTURE_Y 0

struct Level_s;
typedef struct Level_s Level_t;

typedef struct Font_s {
    Vao_t *Characters[NUM_MOH_FONT_CHARS];
} Font_t;

Font_t *FontInit();
void FontDrawString(Level_t *Level,char *String,float x,float y);
#endif //__FONT_H_
