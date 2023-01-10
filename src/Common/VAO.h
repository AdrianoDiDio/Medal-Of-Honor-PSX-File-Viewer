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
#ifndef __VAO_H_
#define __VAO_H_ 

#include "Common.h"

#define BUFFER_OFFSET(i) ((GLvoid*)(i * sizeof(GLfloat)))
#define BUFFER_INT_OFFSET(i) ((GLvoid*)(i * sizeof(GLint)))

typedef struct VAO_s
{
    unsigned int VAOId[1];
    unsigned int VBOId[1];
    unsigned int IBOId[1];
    int          CurrentSize;
    int          Stride;
    int          Size;
    int          Count;
    struct VAO_s *Next;
} VAO_t;

// 3D
VAO_t *VAOInitXYZUVRGB(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,int ColorOffset,int Count);
VAO_t *VAOInitXYZUVRGBCLUTColorModeInteger(int *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,int ColorOffset,int CLUTOffset,
                                           int ColorModeOffset,int Count);
VAO_t *VAOInitXYZUV(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,int Count);
VAO_t *VAOInitXYZRGB(float *Data,int DataSize,int Stride,int VertexOffset,int ColorOffset,int DynamicDraw);
VAO_t *VAOInitXYZ(float *Data,int DataSize,int Stride,int VertexOffset,int Count);
// 3D Indexed
VAO_t *VAOInitXYZRGBIBO(float *Data,int DataSize,int Stride,unsigned short *Index,int IndexSize,int VertexOffset,int ColorOffset);
VAO_t *VAOInitXYZIBO(float *Data,int DataSize,int Stride,int *Index,int IndexSize,int Count);
// 2D
VAO_t *VAOInitXYUVRGB(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,int ColorOffset,bool StaticDraw);
VAO_t *VAOInitXYUV(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,bool StaticDraw);
VAO_t *VAOInitXYRGB(float *Data,int DataSize,int Stride,int VertexOffset,int ColorOffset,bool StaticDraw);
void VAOUpdate(VAO_t *VAO,int *Data,int DataSize,int NumElements);
void VAOFree(VAO_t *VAO);
#endif //__VAO_H_
