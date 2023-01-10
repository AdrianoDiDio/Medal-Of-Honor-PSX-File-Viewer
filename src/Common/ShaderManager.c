// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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
#include "ShaderManager.h"
#include "Common.h"

Shader_t *ShaderList;
int NumShaders;

void ShaderManagerFree()
{
    Shader_t *Temp;
    
    while( ShaderList ) {
        free(ShaderList->Name);
        glDeleteProgram(ShaderList->ProgramId);
        Temp = ShaderList;
        ShaderList = ShaderList->Next;
        free(Temp);
    }
}
char *ShaderRead(const char *ShaderPath)
{
    char *Result;
    Result = ReadTextFile(ShaderPath,0);
    if( Result == NULL ) {
        DPrintf("ShaderRead:File error.\n");
        return NULL;
    }
    return Result;
}

Shader_t *ShaderGet(const char *ShaderName)
{
    Shader_t *Temp;

    for( Temp = ShaderList; Temp ; Temp = Temp->Next ) {
        if( !strcmp(Temp->Name,ShaderName) ) {
            return Temp;
        }
    }
    return NULL;
}


Shader_t *ShaderCache(const char *ShaderName,const char *VertexShaderFile,const char *FragmentShaderFile)
{
    Shader_t *Result;
    char *ShaderInfoLog;
    int VertexShaderId;
    int FragmentShaderId;
    int ProgramId;
    int InfoLogLength;
    int ShaderTaskResult;
    char *ShaderSource;
    
    Result = NULL;
    ShaderInfoLog = NULL;
    ShaderSource = NULL;
    
    if( !ShaderName ) {
        DPrintf("ShaderCache:Invalid name\n");
        goto Failure;
    }
    
    if( !VertexShaderFile ) {
        DPrintf("ShaderCache:Invalid Vertex Shader\n");
        goto Failure;
    }
    
    if( !FragmentShaderFile ) {
        DPrintf("ShaderCache:Invalid Fragment Shader\n");
        goto Failure;
    }
    
    if( (Result = ShaderGet(ShaderName)) != NULL ) {
        return Result;
    }
    
    DPrintf("ShaderCache:Caching shader %s\n",ShaderName);
    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    
    DPrintf("Compiling Vertex Shader: %s\n", VertexShaderFile);
    ShaderSource = ShaderRead(VertexShaderFile);
    if( !ShaderSource ) {
        printf("Failed to open vertex shader %s\n",VertexShaderFile);
        goto Failure;
    }
    glShaderSource(VertexShaderId, 1, (const GLchar**) &ShaderSource, NULL);
    glCompileShader(VertexShaderId);

    glGetShaderiv(VertexShaderId, GL_COMPILE_STATUS, &ShaderTaskResult);
    glGetShaderiv(VertexShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    
    if ( InfoLogLength > 0 && ShaderTaskResult == 0 ){
        ShaderInfoLog = malloc(InfoLogLength + 1);
        if( ShaderInfoLog ) {
            glGetShaderInfoLog(VertexShaderId, InfoLogLength, NULL, ShaderInfoLog);
            ShaderInfoLog[InfoLogLength] = '\0';
            DPrintf("Compile Error:%s\n", ShaderInfoLog);
            free(ShaderInfoLog);
            goto Failure;
        }
    }
    free(ShaderSource);
    DPrintf("Compiling Fragment Shader: %s\n", FragmentShaderFile);
    ShaderSource = ShaderRead(FragmentShaderFile);
    if( !ShaderSource ) {
        printf("Failed to open fragment shader %s\n",FragmentShaderFile);
        goto Failure;
    }
    glShaderSource(FragmentShaderId, 1, (const GLchar**) &ShaderSource, NULL);
    glCompileShader(FragmentShaderId);

    glGetShaderiv(FragmentShaderId, GL_COMPILE_STATUS, &ShaderTaskResult);
    glGetShaderiv(FragmentShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    
    if ( InfoLogLength > 0 && ShaderTaskResult == 0){
        ShaderInfoLog = malloc(InfoLogLength + 1);
        if( ShaderInfoLog ) {
            glGetShaderInfoLog(FragmentShaderId, InfoLogLength, NULL, ShaderInfoLog);
            ShaderInfoLog[InfoLogLength] = '\0';
            DPrintf("Compile Error:%s\n", ShaderInfoLog);
            free(ShaderInfoLog);
            goto Failure;
        }
    }
    
    DPrintf("Linking...\n");
    
    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);

    glGetProgramiv(ProgramId, GL_LINK_STATUS, &ShaderTaskResult);
    glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( InfoLogLength > 0  && ShaderTaskResult == 0){
        ShaderInfoLog = malloc(InfoLogLength + 1);
        if( ShaderInfoLog ) {
            glGetProgramInfoLog(ProgramId, InfoLogLength, NULL, ShaderInfoLog);
            ShaderInfoLog[InfoLogLength] = '\0';
            DPrintf("Linking Error:%s\n", ShaderInfoLog);
            free(ShaderInfoLog);
            goto Failure;
        }
    }
    Result = malloc(sizeof(Shader_t));
    
    if( !Result ) {
        DPrintf("ShaderCache:Failed to allocate struct\n");
        goto Failure;
    }
    Result->Name = StringCopy(ShaderName);
    Result->ProgramId = ProgramId;
    
    Result->Next = ShaderList;
    ShaderList= Result;
    
    NumShaders++;
    free(ShaderSource);
    glDetachShader(ProgramId, VertexShaderId);
    glDetachShader(ProgramId, FragmentShaderId);
    glDeleteShader(VertexShaderId);
    glDeleteShader(FragmentShaderId);
    return Result;
Failure:
    if( ShaderSource ) {
        free(ShaderSource);
    }
    if( Result ) {
        free(Result);
    }
    return NULL;
}

void ShaderManagerInit()
{
    ShaderList = NULL;
    NumShaders = 0;
}
