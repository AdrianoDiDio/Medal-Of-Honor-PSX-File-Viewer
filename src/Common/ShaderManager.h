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

#ifndef __SHADERMANAGER_H_
#define __SHADERMANAGER_H_


typedef struct Shader_s
{
    char   *Name;
    int     ProgramId;
    struct Shader_s *Next;
} Shader_t;

Shader_t    *ShaderCache(const char *ShaderName,const char *VertexShaderFile,const char *FragmentShaderFile);
Shader_t    *ShaderGet(const char *ShaderName);
void        ShaderManagerInit();
void        ShaderManagerFree();

#endif //__SHADERMANAGER_H_
