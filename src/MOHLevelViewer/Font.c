// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2025 Adriano Di Dio.
    
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
    if( Font == NULL ) {
        return;
    }
    VAOFree(Font->VAO);
    free(Font);
}
int FontGetStride()
{
//          XY  UV
    return (2 + 2) * sizeof(float);
}

void FontDrawChar(Font_t *Font,const VRAM_t *VRAM,char c,float x,float y,int ColorId,Color4f_t Color)
{
    Shader_t *Shader;
    vec4 CharColor;
    int ShaderColorId;
    float Width;
    float Height;
    float TexWidth;
    float TexHeight;
    float u0;
    float v0;
    float CharTexelX;
    float CharTexelY;
    int CharIndex;
    float VertexData[6][4];

    CharColor[0] = Color.r;
    CharColor[1] = Color.g;
    CharColor[2] = Color.b;
    CharColor[3] = Color.a;

    if( ColorId == -1 ) {
        Shader = ShaderCache("FontShader","Shaders/FontVertexShader.glsl","Shaders/FontFragmentShader.glsl");
        if( !Shader ) {
        DPrintf("FontDrawChar:Invalid shader\n");
            return;
        }
        glUseProgram(Shader->ProgramId);
        ShaderColorId = glGetUniformLocation(Shader->ProgramId,"Color");
    } else {
        ShaderColorId = ColorId;
    }
    glUniform4fv(ShaderColorId,1,CharColor);
    
    Width = MOH_FONT_CHAR_WIDTH;
    Height = MOH_FONT_CHAR_HEIGHT;
    CharIndex = ASCII_To_MOH_Table[(int) c];
    CharTexelX = MOH_FONT_CHAR_STARTING_TEXTURE_X + ( (CharIndex / 24) * MOH_FONT_CHAR_WIDTH);
    CharTexelY = MOH_FONT_CHAR_STARTING_TEXTURE_Y + ( (CharIndex % 24) * MOH_FONT_CHAR_HEIGHT);
    u0 = ((float)CharTexelX + VRAMGetTexturePageX(MOH_FONT_TEXTURE_VRAM_PAGE)) / VRAM->Page.Width;
    //Color Mode 0 => 4 BPP texture
    v0 = ((float)CharTexelY + VRAMGetTexturePageY(MOH_FONT_TEXTURE_VRAM_PAGE,0)) / VRAM->Page.Height;
    TexWidth = ((float)MOH_FONT_CHAR_WIDTH) / VRAM->Page.Width;
    TexHeight = ((float)MOH_FONT_CHAR_HEIGHT) / VRAM->Page.Height;
            
    VertexData[0][0] = x;
    VertexData[0][1] = y + Height;
    VertexData[0][2] = u0;
    VertexData[0][3] = v0 + TexHeight;
            
    VertexData[1][0] = x;
    VertexData[1][1] = y;
    VertexData[1][2] = u0;
    VertexData[1][3] = v0;
            
    VertexData[2][0] =  x + Width;
    VertexData[2][1] = y+Height;
    VertexData[2][2] = u0 + TexWidth;
    VertexData[2][3] = v0 + TexHeight;
            

    VertexData[3][0] = x + Width;
    VertexData[3][1] = y + Height;
    VertexData[3][2] = u0 + TexWidth;
    VertexData[3][3] = v0 + TexHeight;
            
    VertexData[4][0] = x;
    VertexData[4][1] = y;
    VertexData[4][2] = u0;
    VertexData[4][3] = v0;
            
    VertexData[5][0] = x + Width;
    VertexData[5][1] = y;
    VertexData[5][2] = u0 + TexWidth;
    VertexData[5][3] = v0;
    
    VAOUpdate(Font->VAO,&VertexData,FontGetStride() * 6,0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    if( ColorId == -1 ) {
        glUseProgram(0);
    }
}

void FontDrawString(Font_t *Font,const VRAM_t *VRAM,const char *String,float x,float y,Color4f_t Color)
{

    float Spacing;
    float CurrentX;
    mat4 ProjectionMatrix;
    Shader_t *Shader;
    int OrthoMatrixId;
    int ColorId;
    
    Shader = ShaderCache("FontShader","Shaders/FontVertexShader.glsl","Shaders/FontFragmentShader.glsl");
    if( !Shader ) {
        DPrintf("FontDrawChar:Invalid shader\n");
        return;
    }
    CurrentX = x;
    Spacing = (float) MOH_FONT_CHAR_SPACING;
    glm_mat4_identity(ProjectionMatrix);
    glm_ortho(0,VidConfigWidth->IValue,VidConfigHeight->IValue,0,-1,1,ProjectionMatrix);
    glUseProgram(Shader->ProgramId);
    OrthoMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
    ColorId = glGetUniformLocation(Shader->ProgramId,"Color");
    glUniformMatrix4fv(OrthoMatrixId,1,false,&ProjectionMatrix[0][0]);
    glBindTexture(GL_TEXTURE_2D, VRAM->Page.TextureId);
    glBindVertexArray(Font->VAO->VAOId[0]);
    
    while( *String ) {
        if( *String == ' ' ) {
            CurrentX += Spacing;
            String++;
            continue;
        }
        FontDrawChar(Font,VRAM,*String,CurrentX,y,ColorId,Color);
        CurrentX += Spacing;
        String++;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void FontLoad(Font_t *Font,const VRAM_t *VRAM)
{
    if( !Font ) {
        return;
    }
    
    Font->VAO = VAOInitXYUV(NULL,FontGetStride() * 6,FontGetStride(),0,2,false);
}

Font_t *FontInit(const VRAM_t *VRAM)
{
    Font_t *Font;
    Font = malloc(sizeof(Font_t));
    if( !Font ) {
        DPrintf("FontInit:Failed to allocate memory for Font data\n");
        return NULL;
    }
    Font->VAO = NULL;
    FontLoad(Font,VRAM);
    return Font;
}
