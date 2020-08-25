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
#ifndef __VAO_H_
#define __VAO_H_ 

#define BUFFER_OFFSET(i) ((GLvoid*)(i * sizeof(GL_FLOAT)))

typedef struct Vao_s
{
    unsigned int VaoID[1];
    unsigned int VboID[1];
    int          TextureID;
    short        TSB;
//     float       *Data;
    
    struct Vao_s *Next;
} Vao_t;

Vao_t *VaoInitXYZUVRGB(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,int ColorOffset,short TSB,int TextureID);
Vao_t *VaoInitXYZUV(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,short TSB,int TextureID);
Vao_t *VaoInitXYZRGB(float *Data,int DataSize,int Stride,int VertexOffset,int ColorOffset);
Vao_t *VaoInitXYZ(float *Data,int DataSize,int Stride,int VertexOffset);
#endif //__VAO_H_
