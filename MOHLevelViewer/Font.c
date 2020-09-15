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

Vao_t *TestVao;
/*
    FONT DESCRIPTION

Each taf file contains the data of the used font.
It starts at 224;0 and ends at 244;0.
This means that we have 2 columns where each column has a width of 10.
Each Font Sprite has an eight of 7.
24 row * 7 height = 168

224;0 234;0
224;7 234;7
*/

static const uint8_t ASCII_To_MOH_Table[128] = {
    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    0 ,  43 ,  46 ,  0 ,  0 ,  0 ,  0 ,  24 ,  0 ,  0 ,  0 ,  0 ,  0 , '-', '.',  0 ,
    30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  0 ,  0 ,  0 ,  0 ,  0 ,  42 ,
    0 ,  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  25,  26,  27,  28,  29,  0 ,  0 ,  0 ,  0 ,  0 ,
    0 , 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  25,  26,  27,  28,  29,  0 ,  0 ,  0 ,  0 ,  0 ,
};

int FontGetStride()
{
//          XY  UV
    return (2 + 2) * sizeof(float);
}
void FontLoadChar(Font_t *Font,int CharIndex,float RowX,float RowY)
{
    float x;
    float y;
    float w;
    float h;
    float u0;
    float v0;
    float TexWidth;
    float TexHeight;
    float *VertexData;
    int Stride;
    int DataSize;
    int VertexPointer;
    
    Stride = FontGetStride();
    //We need 6 vertices to describe a quad...
    DataSize = Stride * 6;
    
    u0 = ((float)RowX) / 256.f;
    v0 = ((float)RowY) / 256.f;
    TexWidth = ((float)MOH_FONT_CHAR_WIDTH) / 256.f;
    TexHeight = ((float)MOH_FONT_CHAR_HEIGHT) / 256.f;
    
    VertexData = malloc(FontGetStride() * 6/** sizeof(float)*/);
    VertexPointer = 0;
    
    x = 0;
    y = 0;
    w = MOH_FONT_CHAR_WIDTH;
    h = MOH_FONT_CHAR_HEIGHT;

    VertexData[VertexPointer] =  x;
    VertexData[VertexPointer+1] = y + h;
    VertexData[VertexPointer+2] = u0;
    VertexData[VertexPointer+3] = v0 + TexHeight;
    VertexPointer += 4;
            
    VertexData[VertexPointer] =  x;
    VertexData[VertexPointer+1] = y;
    VertexData[VertexPointer+2] = u0;
    VertexData[VertexPointer+3] = v0;
    VertexPointer += 4;
            
    VertexData[VertexPointer] =  x + w;
    VertexData[VertexPointer+1] = y+h;
    VertexData[VertexPointer+2] = u0 + TexWidth;
    VertexData[VertexPointer+3] = v0 + TexHeight;
    VertexPointer += 4;
            

    VertexData[VertexPointer] =  x + w;
    VertexData[VertexPointer+1] = y + h;
    VertexData[VertexPointer+2] = u0 + TexWidth;
    VertexData[VertexPointer+3] = v0 + TexHeight;
    VertexPointer += 4;
            
    VertexData[VertexPointer] =  x;
    VertexData[VertexPointer+1] = y;
    VertexData[VertexPointer+2] = u0;
    VertexData[VertexPointer+3] = v0;
    VertexPointer += 4;
            
    VertexData[VertexPointer] =  x + w;
    VertexData[VertexPointer+1] = y;
    VertexData[VertexPointer+2] = u0 + TexWidth;
    VertexData[VertexPointer+3] = v0;
    VertexPointer += 4;        
    Font->Characters[CharIndex] = VaoInitXYUV(VertexData,DataSize,Stride,0,2,-1,-1,true);
}

void FontDrawChar(char c,float x,float y,Color_t Color)
{
    GL_Shader_t *Shader;
    int OrthoMatrixID;
    int ColorID;
    vec3 temp;
    vec4 color;
    int CharIndex;
    
    
    color[0] = Color.r;
    color[1] = Color.g;
    color[2] = Color.b;
    color[3] = Color.a;
    
    Shader = Shader_Cache("FontShader","Shaders/FontVertexShader.glsl","Shaders/FontFragmentShader.glsl");
    glUseProgram(Shader->ProgramID);
    OrthoMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
    ColorID = glGetUniformLocation(Shader->ProgramID,"Color");
    glUniform4fv(ColorID,1,color);
    glm_mat4_identity(VidConf.ModelViewMatrix);
    temp[0] = x;
    temp[1] = y;
    temp[2] = 0;
    glm_translate(VidConf.ModelViewMatrix,temp);
    glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
    glUniformMatrix4fv(OrthoMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
    glBindTexture(GL_TEXTURE_2D, Level->VRam->Page4Bit[MOH_FONT_TEXTURE_VRAM_PAGE].TextureID);
    CharIndex = (int) c;
    glBindVertexArray(Level->Font->Characters[ASCII_To_MOH_Table[CharIndex]]->VaoID[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void FontDrawString(Level_t *Level,char *String,float x,float y,Color_t Color)
{

    float Spacing = 10.f;
    float currentX;
    
    currentX = x;
    
    while( *String ) {
        if( *String == ' ' ) {
            currentX += Spacing;
            String++;
            continue;
        }
        FontDrawChar(*String,currentX,y,Color);
        currentX += Spacing;
        String++;
    }
}

void FontLoad(Font_t *Font)
{
    int i;
    float RowPosition;
    float ColumnPosition;
    
    RowPosition = MOH_FONT_CHAR_STARTING_TEXTURE_X;
    ColumnPosition = MOH_FONT_CHAR_STARTING_TEXTURE_Y;
    for( i = 0; i < NUM_MOH_FONT_CHARS; i++ ) {
        if( i == NUM_MOH_FONT_CHAR_PER_COLUMN ) {
            RowPosition = MOH_FONT_CHAR_STARTING_TEXTURE_X + MOH_FONT_CHAR_WIDTH;
            ColumnPosition = 0;
        }
        DPrintf("Fetching char %i at %fx%f\n",i,RowPosition,ColumnPosition);
        FontLoadChar(Font,i,RowPosition,ColumnPosition);
        ColumnPosition += MOH_FONT_CHAR_HEIGHT;
    }
}
Font_t *FontInit()
{
    Font_t *Font;
    Font = malloc(sizeof(Font_t));
    FontLoad(Font);
    return Font;
//     FontLoadChar();
}
