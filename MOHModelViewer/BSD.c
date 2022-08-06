// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2022 Adriano Di Dio.
    
    MOHModelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MOHModelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MOHModelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/
#include "BSD.h"
#include "MOHModelViewer.h" 

void BSDFree(BSD_t *BSD)
{
    if( !BSD ) {
        return;
    }
    free(BSD);
}
BSD_t *BSDLoad(const char *FName)
{
    FILE *BSDFile;
    BSD_t *BSD;
    
    BSDFile = fopen(FName,"rb");
    BSD = NULL;
    
    if( BSDFile == NULL ) {
        DPrintf("Failed opening BSD File %s.\n",FName);
        goto Failure;
    }
    BSD = malloc(sizeof(BSD_t));
    
    return BSD;
Failure:
    BSDFree(BSD);
    if( BSDFile ) {
        fclose(BSDFile);
    }
    return NULL;
}
