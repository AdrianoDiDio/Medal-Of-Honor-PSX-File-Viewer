/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
    VABWalker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VABWalker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VABWalker.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 

#ifndef __VABWALKER_H_
#define __VABWALKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sndfile.h>

#ifdef __GNUC__
#define Attribute(x) __attribute__(x)
#else
#define Attribute(x)
#endif

#define VAB_MAX_PROGRAM_NUMBER 128
#define VAB_MAX_TONE_NUMBER 16
#define VAB_MAX_NUM_VAG_OFFSET 256
#define VAB_MAX_NUM_VAG 256
typedef unsigned char Byte;

typedef enum {
    false,
    true
} bool;

void    DPrintf(char *Fmt, ...) Attribute((format(printf,1,2)));


#endif //__VABWALKER_H_
