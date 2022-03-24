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
#include "MOHLevelViewer.h"

void ShaderManagerFree()
{
    Shader_t *Temp;
    
    while( ShaderList ) {
        Temp = ShaderList;
        ShaderList = ShaderList->Next;
        free(Temp);
    }
}
char *ShaderRead(char *ShaderPath)
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


Shader_t *ShaderCache(char *ShaderName,char *VertexShaderFile,char *FragmentShaderFile)
{
    Shader_t *Result;
    char *ShaderInfoLog;
    int VertexShaderID;
    int FragmentShaderID;
    int ProgramID;
    int InfoLogLength;
    int ShaderTaskResult;
    char *ShaderSource;
    
    if( (Result = ShaderGet(ShaderName)) != NULL ) {
        return Result;
    }
    
    Result = malloc(sizeof(Shader_t));
    
    VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    DPrintf("Compiling Vertex Shader: %s\n", VertexShaderFile);
    ShaderSource = ShaderRead(VertexShaderFile);
    glShaderSource(VertexShaderID, 1, (const GLchar**) &ShaderSource, NULL);
    glCompileShader(VertexShaderID);

    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &ShaderTaskResult);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    
    if ( InfoLogLength > 0 && ShaderTaskResult == 0 ){
        ShaderInfoLog = malloc(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, ShaderInfoLog);
        ShaderInfoLog[InfoLogLength] = '\0';
        DPrintf("Compile Error:%s\n", ShaderInfoLog);
    }
    
    DPrintf("Compiling Fragment Shader: %s\n", FragmentShaderFile);
    ShaderSource = ShaderRead(FragmentShaderFile);
    glShaderSource(FragmentShaderID, 1, (const GLchar**) &ShaderSource, NULL);
    glCompileShader(FragmentShaderID);

    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &ShaderTaskResult);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    
    if ( InfoLogLength > 0 && ShaderTaskResult == 0){
        ShaderInfoLog = malloc(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, ShaderInfoLog);
        ShaderInfoLog[InfoLogLength] = '\0';
        DPrintf("Compile Error:%s\n", ShaderInfoLog);
    }
    
    DPrintf("Linking...\n");
    
    ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &ShaderTaskResult);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( InfoLogLength > 0  && ShaderTaskResult == 0){
        ShaderInfoLog = malloc(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ShaderInfoLog);
        ShaderInfoLog[InfoLogLength] = '\0';
        DPrintf("Linking Error:%s\n", ShaderInfoLog);
    }
    
    Result->Name = StringCopy(ShaderName);
    Result->ProgramID = ProgramID;
    
    Result->Next = ShaderList;
    ShaderList= Result;
    
    NumShaders++;
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    return Result;

}

void ShaderManagerInit()
{
    ShaderList = NULL;
    NumShaders = 0;
}
