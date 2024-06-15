// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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
#ifndef __GFX_H_
#define __GFX_H_

#include "../Common/VRAM.h"
#include "../Common/VAO.h"

typedef struct GFXHeader_s {
    int NumVertices;
    int NumNormals;
    int NumFaces;
    int NumUnk3;
    int NumUnk4;
    int Pad;
} GFXHeader_t;

typedef struct GFXOffsetTable_s {
    short Offset0;
    short Pad0;
    short Offset1;
    short Pad1;
    short Offset2;
    short Pad2;
    int   Pad3;
} GFXOffsetTable_t;

typedef struct GFXVertex_s {
    short x;
    short y;
    short z;
    short Pad;
} GFXVertex_t;

typedef struct GFXFace_s {
    short           Unk0;
    unsigned short  Vert0; // => Vertex 0
    unsigned short  Vert1; // => Vertex 1
    unsigned short  Vert2; // => Vertex 2
    unsigned short  Norm0; // => Normal 0
    unsigned short  Norm1; // => Normal 1
    unsigned short  Norm2; // => Normal 2
    unsigned char   U1;
    unsigned char   V1;
    unsigned char   U2;
    unsigned char   V2;
    unsigned char   U3;
    unsigned char   V3;
    Color1i_t       RGB0;
    Color1i_t       RGB1;
    Color1i_t       RGB2;
    unsigned short  TSB;
    short           CBA;
} GFXFace_t;

typedef struct GFX_s {
    GFXHeader_t         Header;
    GFXOffsetTable_t    OffsetTable;
    GFXVertex_t         *Vertex;
    GFXVertex_t         *Normal;
    GFXFace_t           *Face;
    GFXVertex_t         *AnimationData;
    int                 NumAnimations;
    
    int                 RotationX;
    int                 RotationY;
    int                 RotationZ;
    
    VAO_t               *VAO;
    
    struct GFX_s        *Next;
} GFX_t;

GFX_t *GFXRead(void* GFXFileBuffer);
void GFXFree(GFX_t *GFX);
void GFXPrepareVAO(GFX_t *GFX);
void GFXRender(GFX_t *GFX,VRAM_t *VRAM,mat4 ProjectionMatrix);
#endif //__GFX_H_
