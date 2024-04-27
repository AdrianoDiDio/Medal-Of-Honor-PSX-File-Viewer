// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    SSTViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SSTViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SSTViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/
#include "RSC.h"
#include "SSTViewer.h"

char *RSCGetErrorString(RSCError_t RSCError)
{
    switch(RSCError) {
        case RSC_INVALID_DATA:
            return "Invalid RSC Data";
        case RSC_FILE_NOT_FOUND:
            return "File was not found inside RSC file";
        default:
            return "Unknown Error";
    }
}

int RSCFree(RSC_t *RSC)
{
    int i;
    
    if( !RSC ) {
        printf("RSCOpen:Invalid RSC data\n");
        return RSC_INVALID_DATA; 
    }
    
    for( i = 0; i < (int) RSC->Header.NumEntry; i++ ) {
        free(RSC->EntryList[i].Data);
    }
    free(RSC->EntryList);
    free(RSC);
    return RSC_OK;
}
int RSCSearch(RSC_t *RSC,char *FileName)
{
    int i;
    for( i = 0; i < (int) RSC->Header.NumEntry; i++ ) {
//         DPrintf("Comparing %s\n",RSC->EntryList[i].Name);
        if( !strcmp(RSC->EntryList[i].Name,FileName) ) {
            DPrintf("Found Entry %s in list\n",FileName);
            return i;
        }
    }
    return RSC_FILE_NOT_FOUND;
}
int RSCOpen(RSC_t *RSC,char *FileName,RSCEntry_t *OutEntry) {
    int EntryIndex;
    
    if( !RSC ) {
        printf("RSCOpen:Invalid RSC data\n");
        return RSC_INVALID_DATA; 
    }
    
    if( !OutEntry ) {
        DPrintf("RSCOpen:Invalid Out Entry\n");
        return RSC_INVALID_DATA;
    }
    
    EntryIndex = RSCSearch(RSC,FileName);
    
    if( EntryIndex < 0 ) {
        DPrintf("An error has occurred:%s\n",RSCGetErrorString(EntryIndex));
        return RSC_FILE_NOT_FOUND;
    }
    
    *OutEntry = RSC->EntryList[EntryIndex];
    
    return RSC_OK;
}

RSC_t *RSCLoad(char *FileName)
{
    FILE *RSCFile;
    RSC_t *RSC;
    int EntryListSize;
    int PreviousFilePosition;
    int i;
    
    if( !FileName ) {
        DPrintf("RSCLoad:Invalid FileName\n");
        return NULL;
    }

    printf("Loading pack %s...\n",FileName);
    
    RSCFile = fopen(FileName,"rb");
    
    if( RSCFile == NULL ) {
        DPrintf("RSCLoad:Failed opening RSC file.\n");
        return NULL;
    }
    
    RSC = malloc(sizeof(RSC_t));
    
    DPrintf("Scanning elements...\n");
    fread(&RSC->Header, sizeof(RSC->Header), 1, RSCFile);
    
    printf("Dir Name: %s\n",RSC->Header.DirName);
    printf("Dir Contains %i entries\n",(int)RSC->Header.NumEntry);
    EntryListSize = (int)RSC->Header.NumEntry * sizeof(RSCEntry_t);
    RSC->EntryList = malloc(EntryListSize);
    memset(RSC->EntryList,0,EntryListSize);
    
    for( i = 0; i < (int) RSC->Header.NumEntry; i++ ) {
        fread(&RSC->EntryList[i].Name, sizeof(RSC->EntryList[i].Name), 1, RSCFile);
        fread(&RSC->EntryList[i].Length, sizeof(RSC->EntryList[i].Length), 1, RSCFile);
        fread(&RSC->EntryList[i].Offset, sizeof(RSC->EntryList[i].Offset), 1, RSCFile);
        RSC->EntryList[i].Data = malloc(RSC->EntryList[i].Length);
        PreviousFilePosition = ftell(RSCFile);
        fseek(RSCFile,RSC->EntryList[i].Offset,SEEK_SET);
        fread(RSC->EntryList[i].Data,RSC->EntryList[i].Length,1,RSCFile);
        fseek(RSCFile,PreviousFilePosition,SEEK_SET);
        printf("Reading entry %i....got %s with length %i and offset %i\n",i,RSC->EntryList[i].Name,RSC->EntryList[i].Length,
               (int)RSC->EntryList[i].Offset);
//         DumpChunk(PackFile,&Entries[i]);
    }
    fclose(RSCFile);
    return RSC;
}
