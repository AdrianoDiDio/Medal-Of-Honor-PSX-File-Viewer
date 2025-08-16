// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2025 Adriano Di Dio.
    
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
#ifndef __RSC_H_
#define __RSC_H_

#include "Common.h"

typedef enum
{
    RSC_OK = 0,
    RSC_INVALID_DATA = -1,
    RSC_FILE_NOT_FOUND = -2, 
} RSCError_t;
typedef struct RSCHeader_s {
    char DirName[64];
    int  NumEntry;
    int  Unknown;
} RSCHeader_t;
typedef struct RSCEntry_s {
    char    Name[64];
    int     Index;
    int     Length;
    int     Offset;
    int     Pad;
    Byte    *Data;
} RSCEntry_t;

typedef struct RSC_s {
    RSCHeader_t         Header;
    RSCEntry_t          *EntryList;
    struct RSC_s        *Next;
} RSC_t;

RSC_t           *RSCLoad(const char *FileName);
void            RSCAppend(RSC_t **RSCList,RSC_t *RSC);
int             RSCOpen(const RSC_t *RSC,const char *FileName,RSCEntry_t *OutEntry);
char            *RSCGetBaseName(const char *RSCPath);
int             RSCGetDirectoryFileCount(const RSC_t *RSC,const char *Directory);
RSCEntry_t      *RSCGetDirectoryEntries(const RSC_t *RSC,const char *Directory,int *NumEntry);
int             RSCFree(RSC_t *RSC);
#endif//__RSC_H_
