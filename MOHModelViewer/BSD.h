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

#ifndef __BSD_H_
#define __BSD_H_

#include "Common.h"

#define BSD_ANIMATED_LIGHTS_TABLE_SIZE 40
#define MOH_RENDER_OBJECT_SIZE 256
#define MOH_UNDERGROUND_RENDER_OBJECT_SIZE 276

#define BSD_ANIMATED_LIGHTS_FILE_POSITION 0xD8
#define BSD_RENDER_OBJECT_STARTING_OFFSET 0x5A4
#define BSD_ENTRY_TABLE_FILE_POSITION 0x53C

typedef struct BSDEntryTable_s {
    int NodeTableOffset;
    int UnknownDataOffset;
    
    int AnimationTableOffset;
    int NumAnimationTableEntries;
    
    int AnimationDataOffset;
    int NumAnimationData;
    
    int AnimationQuaternionDataOffset;
    int NumAnimationQuaternionData;
        
    int AnimationHierarchyDataOffset;
    int NumAnimationHierarchyData;
        
    int AnimationFaceTableOffset;
    int NumAnimationFaceTableEntries;
    
    int AnimationFaceDataOffset;
    int NumAnimationFaces;
        
    int AnimationVertexTableOffset;
    int NumAnimationVertexTableEntries;

    int AnimationVertexDataOffset;
    int NumAnimationVertices;
    
    int Off8;
    int Num8;
    
} BSDEntryTable_t;
typedef struct BSD_s {
    BSDEntryTable_t EntryTable;
} BSD_t;

BSD_t   *BSDLoad(const char *FName);
void    BSDFree(BSD_t *BSD);

#endif //__BSD_H_
