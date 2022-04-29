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

#ifndef __SHADERMANAGER_H_
#define __SHADERMANAGER_H_


typedef struct Shader_s
{
    char   *Name;
    int     ProgramId;
    struct Shader_s *Next;
} Shader_t;

extern Shader_t *ShaderList;
extern int NumShaders;

Shader_t *ShaderCache(char *ShaderName,char *VertexShaderFile,char *FragmentShaderFile);
Shader_t *ShaderGet(const char *ShaderName);
void         ShaderManagerInit();
void         ShaderManagerFree();

#endif //__SHADERMANAGER_H_
