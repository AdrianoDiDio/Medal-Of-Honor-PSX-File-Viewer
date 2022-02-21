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
#ifndef __VAO_H_
#define __VAO_H_ 

#define BUFFER_OFFSET(i) ((GLvoid*)(i * sizeof(GL_FLOAT)))

typedef struct Vao_s
{
    unsigned int VaoID[1];
    unsigned int VboID[1];
    unsigned int IboID[1];
    int          TextureID;
    short        TSB;
//     float       *Data;
    int          Count;
    struct Vao_s *Next;
} Vao_t;

// 3D
Vao_t *VaoInitXYZUVRGB(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,int ColorOffset,short TSB,int TextureID,int Count);
Vao_t *VaoInitXYZUV(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,short TSB,int TextureID,int Count);
Vao_t *VaoInitXYZRGB(float *Data,int DataSize,int Stride,int VertexOffset,int ColorOffset);
Vao_t *VaoInitXYZ(float *Data,int DataSize,int Stride,int VertexOffset);
// 3D Indexed
Vao_t *VaoInitXYZIBO(float *Data,int DataSize,int Stride,unsigned short *Index,int IndexSize,int VertexOffset);
// 2D
Vao_t *VaoInitXYUVRGB(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,int ColorOffset,short TSB,int TextureID,
                      bool StaticDraw);
Vao_t *VaoInitXYUV(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,short TSB,int TextureID,
                      bool StaticDraw);
Vao_t *VaoInitXYRGB(float *Data,int DataSize,int Stride,int VertexOffset,int ColorOffset,bool StaticDraw);

void VaoFree(Vao_t *Vao);
#endif //__VAO_H_
