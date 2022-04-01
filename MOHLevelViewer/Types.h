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
#ifndef __TYPES_H_
#define __TYPES_H_ 

typedef unsigned char Byte;

// typedef enum 
// {
//     false,
//     true
// } bool;

typedef union  {
    unsigned short AsShort;
    Byte AsBytes[2];
}ShortByteUnion;


typedef union  {
    int   AsInt;
    short AsShort[2];
} IntShortUnion;

typedef struct Color4f_s
{
    float r;
    float g;
    float b;
    float a;
} Color4f_t;

typedef union {
    int Color;
    Byte rgba[4];
} Color1i;

typedef struct Vec3_s
{
    float x;
    float y;
    float z;
} Vec3_t;

#endif //__TYPES_H_
