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

char *Shader_Read(char *ShaderPath)
{
    char *Result;
    Result = ReadTextFile(ShaderPath,0);
    if( Result == NULL ) {
        DPrintf("Shader_Read:File error.\n");
        return NULL;
    }
    return Result;
}

GL_Shader_t *Shader_Get(const char *ShaderName)
{
    GL_Shader_t *Temp;

    for( Temp = ShaderList; Temp ; Temp = Temp->Next ) {
        if( !strcmp(Temp->Name,ShaderName) ) {
            return Temp;
        }
    }
    return NULL;
}


GL_Shader_t *Shader_Cache(char *ShaderName,char *VertexShaderFile,char *FragmentShaderFile)
{
    GL_Shader_t *Result;
    char *ShaderInfoLog;
    int VertexShaderID;
    int FragmentShaderID;
    int ProgramID;
    int InfoLogLength;
    int CompileResult;
    char *ShaderSource;
    
    if( (Result = Shader_Get(ShaderName)) != NULL ) {
        return Result;
    }
    
    Result = malloc(sizeof(GL_Shader_t));
    
    VertexShaderID = aglCreateShader(GL_VERTEX_SHADER);
    FragmentShaderID = aglCreateShader(GL_FRAGMENT_SHADER);
    
    DPrintf("Compiling Vertex Shader: %s\n", VertexShaderFile);
    ShaderSource = Shader_Read(VertexShaderFile);
    aglShaderSource(VertexShaderID, 1, (const GLchar**) &ShaderSource, NULL);
    aglCompileShader(VertexShaderID);

    aglGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &CompileResult);
    aglGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    
    if ( InfoLogLength > 0 ){
        ShaderInfoLog = malloc(InfoLogLength + 1);
        aglGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, ShaderInfoLog);
        ShaderInfoLog[InfoLogLength] = '\0';
        DPrintf("Compile Error:%s\n", ShaderInfoLog);
    }
    
    DPrintf("Compiling Fragment Shader: %s\n", FragmentShaderFile);
    ShaderSource = Shader_Read(FragmentShaderFile);
    aglShaderSource(FragmentShaderID, 1, (const GLchar**) &ShaderSource, NULL);
    aglCompileShader(FragmentShaderID);

    aglGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &CompileResult);
    aglGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    
    if ( InfoLogLength > 0 ){
        ShaderInfoLog = malloc(InfoLogLength + 1);
        aglGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, ShaderInfoLog);
        ShaderInfoLog[InfoLogLength] = '\0';
        DPrintf("Compile Error:%s\n", ShaderInfoLog);
    }
    
    DPrintf("Linking...\n");
    
    ProgramID = aglCreateProgram();
    aglAttachShader(ProgramID, VertexShaderID);
    aglAttachShader(ProgramID, FragmentShaderID);
    aglLinkProgram(ProgramID);

    aglGetProgramiv(ProgramID, GL_LINK_STATUS, &CompileResult);
    aglGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( InfoLogLength > 0 ){
        ShaderInfoLog = malloc(InfoLogLength + 1);
        aglGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ShaderInfoLog);
        ShaderInfoLog[InfoLogLength] = '\0';
        DPrintf("Linking Error:%s\n", ShaderInfoLog);
    }
    
    Result->Name = String_Copy(ShaderName);
    Result->ProgramID = ProgramID;
    
    Result->Next = ShaderList;
    ShaderList= Result;
    
    NumShaders++;
    aglDetachShader(ProgramID, VertexShaderID);
    aglDetachShader(ProgramID, FragmentShaderID);
    aglDeleteShader(VertexShaderID);
    aglDeleteShader(FragmentShaderID);
    return Result;

}

void ShaderManager_Init()
{
    ShaderList = NULL;
    NumShaders = 0;
}
