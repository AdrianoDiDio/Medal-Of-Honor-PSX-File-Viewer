// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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
 
#include "Font.h"
#include "../Common/ShaderManager.h"
#include "MOHLevelViewer.h"

/*
    FONT DESCRIPTION

Each taf file contains the data of the used font.
Font data is contained inside page 15 of the VRAM.
It starts at 224;0 and ends at 244;0.
This means that we have 2 columns where each column has a width of 10.
Each Font Sprite has an height of 7.
24 row * 7 height = 168

224;0 234;0
224;7 234;7

NOTE that the font only contains a limited set of characters and we need to
map them from ASCII to the one available in the set.
*/

static const Byte ASCII_To_MOH_Table[128] = {
    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    0 ,  43 ,  46 ,  0 ,  0 ,  0 ,  0 ,  24 ,  0 ,  0 ,  0 ,  0 ,  0 , '-', '.',  0 ,
    30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  0 ,  0 ,  0 ,  0 ,  0 ,  42 ,
    0 ,  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  25,  26,  27,  28,  29,  0 ,  0 ,  0 ,  0 ,  0 ,
    0 , 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  25,  26,  27,  28,  29,  0 ,  0 ,  0 ,  0 ,  0 ,
};

void FontFree(Font_t *Font)
{
    int i;
    if( Font == NULL ) {
        return;
    }
    for( i = 0; i < NUM_MOH_FONT_CHARS; i++ ) {
        free(Font->Characters[i]);
    }
    free(Font);
}
int FontGetStride()
{
//          XY  UV
    return (2 + 2) * sizeof(float);
}
void FontLoadChar(Font_t *Font,const VRAM_t *VRAM,int CharIndex,float RowX,float RowY)
{
    float x;
    float y;
    float w;
    float h;
    float u0;
    float v0;
    float TexWidth;
    float TexHeight;
    float ImageWidth;
    float ImageHeight;
    float *VertexData;
    int Stride;
    int DataSize;
    int VertexPointer;
    
    Stride = FontGetStride();
    //We need 6 vertices to describe a quad...
    DataSize = Stride * 6;
    
    ImageWidth = VRAM->Page.Width;
    ImageHeight = VRAM->Page.Height;
    
    u0 = ((float)RowX + VRAMGetTexturePageX(MOH_FONT_TEXTURE_VRAM_PAGE)) / ImageWidth;
    //Color Mode 0 => 4 BPP texture
    v0 = ((float)RowY + VRAMGetTexturePageY(MOH_FONT_TEXTURE_VRAM_PAGE,0)) / ImageHeight;
    TexWidth = ((float)MOH_FONT_CHAR_WIdTH) / ImageWidth;
    TexHeight = ((float)MOH_FONT_CHAR_HEIGHT) / ImageHeight;
    
    VertexData = malloc(FontGetStride() * 6/** sizeof(float)*/);
    
    if( !VertexData ) {
        DPrintf("FontLoadChar:Couldn't allocate memory for vertex data.\n");
        return;
    }
    VertexPointer = 0;
    
    x = 0;
    y = 0;
    w = MOH_FONT_CHAR_WIdTH;
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
    Font->Characters[CharIndex] = VAOInitXYUV(VertexData,DataSize,Stride,0,2,true);
    free(VertexData);
}

void FontDrawChar(Font_t *Font,mat4 ProjectionMatrix,char c,float x,float y,Color4f_t Color)
{
    Shader_t *Shader;
    int OrthoMatrixId;
    int ColorId;
    vec3 temp;
    vec4 color;
    int CharIndex;
    mat4 ModelViewMatrix;
    mat4 MVPMatrix;
    
    
    color[0] = Color.r;
    color[1] = Color.g;
    color[2] = Color.b;
    color[3] = Color.a;
    
    Shader = ShaderCache("FontShader","Shaders/FontVertexShader.glsl","Shaders/FontFragmentShader.glsl");
    if( !Shader ) {
        DPrintf("FontDrawChar:Invalid shader\n");
        return;
    }
    glUseProgram(Shader->ProgramId);
    OrthoMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
    ColorId = glGetUniformLocation(Shader->ProgramId,"Color");
    glUniform4fv(ColorId,1,color);
    glm_mat4_identity(ModelViewMatrix);
    temp[0] = x;
    temp[1] = y;
    temp[2] = 0;
    glm_translate(ModelViewMatrix,temp);
    glm_mat4_mul(ProjectionMatrix,ModelViewMatrix,MVPMatrix);
    glUniformMatrix4fv(OrthoMatrixId,1,false,&MVPMatrix[0][0]);
    CharIndex = (int) c;
    glBindVertexArray(Font->Characters[ASCII_To_MOH_Table[CharIndex]]->VAOId[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void FontDrawString(Font_t *Font,const VRAM_t *VRAM,const char *String,float x,float y,Color4f_t Color)
{

    float Spacing = 10.f;
    float currentX;
    mat4 ProjectionMatrix;
    
    currentX = x;
    
    glm_mat4_identity(ProjectionMatrix);
    glm_ortho(0,VidConfigWidth->IValue,VidConfigHeight->IValue,0,-1,1,ProjectionMatrix);
    glBindTexture(GL_TEXTURE_2D, VRAM->Page.TextureId);
    while( *String ) {
        if( *String == ' ' ) {
            currentX += Spacing;
            String++;
            continue;
        }
        FontDrawChar(Font,ProjectionMatrix,*String,currentX,y,Color);
        currentX += Spacing;
        String++;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FontLoad(Font_t *Font,const VRAM_t *VRAM)
{
    int i;
    float RowPosition;
    float ColumnPosition;
    
    RowPosition = MOH_FONT_CHAR_STARTING_TEXTURE_X;
    ColumnPosition = MOH_FONT_CHAR_STARTING_TEXTURE_Y;
    for( i = 0; i < NUM_MOH_FONT_CHARS; i++ ) {
        if( i == NUM_MOH_FONT_CHAR_PER_COLUMN ) {
            RowPosition = MOH_FONT_CHAR_STARTING_TEXTURE_X + MOH_FONT_CHAR_WIdTH;
            ColumnPosition = 0;
        }
        DPrintf("Fetching char %i at %fx%f\n",i,RowPosition,ColumnPosition);
        FontLoadChar(Font,VRAM,i,RowPosition,ColumnPosition);
        ColumnPosition += MOH_FONT_CHAR_HEIGHT;
    }
}

Font_t *FontInit(const VRAM_t *VRAM)
{
    Font_t *Font;
    Font = malloc(sizeof(Font_t));
    if( !Font ) {
        DPrintf("FontInit:Failed to allocate memory for Font data\n");
        return NULL;
    }
    FontLoad(Font,VRAM);
    return Font;
}
