/*
===========================================================================
    Copyright (C) 2018-2023 Adriano Di Dio.
    
    Medal-Of-Honor-PSX-File-Viewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Medal-Of-Honor-PSX-File-Viewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Medal-Of-Honor-PSX-File-Viewer.  If not, see <http://www.gnu.org/licenses/>.
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

typedef struct Color4f_s
{
    float r;
    float g;
    float b;
    float a;
} Color4f_t;

typedef union {
    unsigned int c;
    Byte rgba[4];
} Color1i_t;

typedef struct Color3b_s {
    Byte r;
    Byte g;
    Byte b;
} Color3b_t;

#endif //__TYPES_H_
