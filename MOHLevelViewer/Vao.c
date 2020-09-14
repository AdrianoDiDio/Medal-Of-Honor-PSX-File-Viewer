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
    
    glGenVertexArrays(1, &Vao->VaoID[0]);
    glBindVertexArray(Vao->VaoID[0]);
        
    glGenBuffers(1, Vao->VboID);
    glBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    glBufferData(GL_ARRAY_BUFFER, DataSize,Data, GL_STATIC_DRAW);
        
    glVertexAttribPointer(0,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,false,Stride,BUFFER_OFFSET(TextureOffset));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(ColorOffset));
    glEnableVertexAttribArray(2);

    Vao->TSB = TSB;
    Vao->TextureID = TextureID;
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    return Vao;
}

Vao_t *VaoInitXYUVRGB(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,int ColorOffset,short TSB,int TextureID,
    bool StaticDraw)
{
    Vao_t *Vao;
    
    Vao = malloc(sizeof(Vao_t));
    
    glGenVertexArrays(1, &Vao->VaoID[0]);
    glBindVertexArray(Vao->VaoID[0]);
        
    glGenBuffers(1, Vao->VboID);
    glBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    glBufferData(GL_ARRAY_BUFFER, DataSize,Data, StaticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        
    glVertexAttribPointer(0,2,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,false,Stride,BUFFER_OFFSET(TextureOffset));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(ColorOffset));
    glEnableVertexAttribArray(2);

    Vao->TSB = TSB;
    Vao->TextureID = TextureID;
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    return Vao;
}

Vao_t *VaoInitXYRGB(float *Data,int DataSize,int Stride,int VertexOffset,int ColorOffset,bool StaticDraw)
{
    Vao_t *Vao;
    
    Vao = malloc(sizeof(Vao_t));
    
    glGenVertexArrays(1, &Vao->VaoID[0]);
    glBindVertexArray(Vao->VaoID[0]);
        
    glGenBuffers(1, Vao->VboID);
    glBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    glBufferData(GL_ARRAY_BUFFER, DataSize,Data, StaticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        
    glVertexAttribPointer(0,2,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(ColorOffset));
    glEnableVertexAttribArray(1);

    Vao->TSB = -1;
    Vao->TextureID = -1;
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    return Vao;
}

Vao_t *VaoInitXYUV(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,short TSB,int TextureID,bool StaticDraw)
{
    Vao_t *Vao;
    
    Vao = malloc(sizeof(Vao_t));
    
    glGenVertexArrays(1, &Vao->VaoID[0]);
    glBindVertexArray(Vao->VaoID[0]);
        
    glGenBuffers(1, Vao->VboID);
    glBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    glBufferData(GL_ARRAY_BUFFER, DataSize,Data, StaticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        
    glVertexAttribPointer(0,2,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,false,Stride,BUFFER_OFFSET(TextureOffset));
    glEnableVertexAttribArray(1);

    Vao->TSB = TSB;
    Vao->TextureID = TextureID;
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    return Vao;
}

Vao_t *VaoInitXYZUV(float *Data,int DataSize,int Stride,int VertexOffset,int TextureOffset,short TSB,int TextureID)
{
    Vao_t *Vao;
    
    Vao = malloc(sizeof(Vao_t));
    
    glGenVertexArrays(1, &Vao->VaoID[0]);
    glBindVertexArray(Vao->VaoID[0]);
        
    glGenBuffers(1, Vao->VboID);
    glBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    glBufferData(GL_ARRAY_BUFFER, DataSize,Data, GL_STATIC_DRAW);
        
    glVertexAttribPointer(0,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,false,Stride,BUFFER_OFFSET(TextureOffset));
    glEnableVertexAttribArray(1);

    Vao->TSB = TSB;
    Vao->TextureID = TextureID;
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    return Vao;
}

Vao_t *VaoInitXYZRGB(float *Data,int DataSize,int Stride,int VertexOffset,int ColorOffset)
{
    Vao_t *Vao;
    
    Vao = malloc(sizeof(Vao_t));
    
    glGenVertexArrays(1, &Vao->VaoID[0]);
    glBindVertexArray(Vao->VaoID[0]);
        
    glGenBuffers(1, Vao->VboID);
    glBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    glBufferData(GL_ARRAY_BUFFER, DataSize,Data, GL_STATIC_DRAW);
        
    glVertexAttribPointer(0,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(ColorOffset));
    glEnableVertexAttribArray(1);

    Vao->TSB = -1;
    Vao->TextureID = -1;
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    return Vao;
}

Vao_t *VaoInitXYZ(float *Data,int DataSize,int Stride,int VertexOffset)
{
    Vao_t *Vao;
    
    Vao = malloc(sizeof(Vao_t));
    
    glGenVertexArrays(1, &Vao->VaoID[0]);
    glBindVertexArray(Vao->VaoID[0]);
        
    glGenBuffers(1, Vao->VboID);
    glBindBuffer(GL_ARRAY_BUFFER, Vao->VboID[0]);
            
    glBufferData(GL_ARRAY_BUFFER, DataSize,Data, GL_STATIC_DRAW);
        
    glVertexAttribPointer(0,3,GL_FLOAT,false,Stride,BUFFER_OFFSET(VertexOffset));
    glEnableVertexAttribArray(0);

    Vao->TSB = -1;
    Vao->TextureID = -1;
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    return Vao;
}
