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
#include "MOHLevelViewer.h"

Vao_t *VaoInitXYZUVRGB(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,int ColorOffset,short TSB,int TextureID)
{
    Vao_t *Vao;
    
    Vao = malloc(sizeof(Vao_t));
    
    aglGenVertexArrays(1, &Vao->VaoID[0]);
    aglBindVertexArray(Vao->VaoID[0]);
        
    aglGenBuffers(1, Vao->VboID);
    aglBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    aglBufferData(GL_ARRAY_BUFFER, DataSize,Data, GL_STATIC_DRAW);
        
    aglVertexAttribPointer(0,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    aglEnableVertexAttribArray(0);
    aglVertexAttribPointer(1,2,GL_FLOAT,false,Stride,BUFFER_OFFSET(TextureOffset));
    aglEnableVertexAttribArray(1);
    aglVertexAttribPointer(2,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(ColorOffset));
    aglEnableVertexAttribArray(2);

    Vao->TSB = TSB;
    Vao->TextureID = TextureID;
    
    aglBindBuffer(GL_ARRAY_BUFFER,0);
    aglBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    aglBindVertexArray(0);
    
    return Vao;
}

Vao_t *VaoInitXYZUV(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,short TSB,int TextureID)
{
    Vao_t *Vao;
    
    Vao = malloc(sizeof(Vao_t));
    
    aglGenVertexArrays(1, &Vao->VaoID[0]);
    aglBindVertexArray(Vao->VaoID[0]);
        
    aglGenBuffers(1, Vao->VboID);
    aglBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    aglBufferData(GL_ARRAY_BUFFER, DataSize,Data, GL_STATIC_DRAW);
        
    aglVertexAttribPointer(0,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    aglEnableVertexAttribArray(0);
    aglVertexAttribPointer(1,2,GL_FLOAT,false,Stride,BUFFER_OFFSET(TextureOffset));
    aglEnableVertexAttribArray(1);

    Vao->TSB = TSB;
    Vao->TextureID = TextureID;
    
    aglBindBuffer(GL_ARRAY_BUFFER,0);
    aglBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    aglBindVertexArray(0);
    
    return Vao;
}

Vao_t *VaoInitXYZRGB(float *Data,int DataSize,int Stride,int VertexOffset,int ColorOffset)
{
    Vao_t *Vao;
    
    Vao = malloc(sizeof(Vao_t));
    
    aglGenVertexArrays(1, &Vao->VaoID[0]);
    aglBindVertexArray(Vao->VaoID[0]);
        
    aglGenBuffers(1, Vao->VboID);
    aglBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    aglBufferData(GL_ARRAY_BUFFER, DataSize,Data, GL_STATIC_DRAW);
        
    aglVertexAttribPointer(0,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    aglEnableVertexAttribArray(0);
    aglVertexAttribPointer(1,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(ColorOffset));
    aglEnableVertexAttribArray(1);

    Vao->TSB = -1;
    Vao->TextureID = -1;
    
    aglBindBuffer(GL_ARRAY_BUFFER,0);
    aglBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    aglBindVertexArray(0);
    
    return Vao;
}

Vao_t *VaoInitXYZ(float *Data,int DataSize,int Stride,int VertexOffset)
{
    Vao_t *Vao;
    
    Vao = malloc(sizeof(Vao_t));
    
    aglGenVertexArrays(1, &Vao->VaoID[0]);
    aglBindVertexArray(Vao->VaoID[0]);
        
    aglGenBuffers(1, Vao->VboID);
    aglBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    aglBufferData(GL_ARRAY_BUFFER, DataSize,Data, GL_STATIC_DRAW);
        
    aglVertexAttribPointer(0,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    aglEnableVertexAttribArray(0);

    Vao->TSB = -1;
    Vao->TextureID = -1;
    
    aglBindBuffer(GL_ARRAY_BUFFER,0);
    aglBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    aglBindVertexArray(0);
    
    return Vao;
}
