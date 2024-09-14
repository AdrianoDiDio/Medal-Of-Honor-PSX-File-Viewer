// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
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
#include "RSC.h"

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
    RSC_t *Temp;
    int i;
    
    if( !RSC ) {
        printf("RSCFree:Invalid RSC data\n");
        return RSC_INVALID_DATA; 
    }
    while( RSC ) {
        for( i = 0; i < (int) RSC->Header.NumEntry; i++ ) {
            free(RSC->EntryList[i].Data);
        }
        free(RSC->EntryList);
        Temp = RSC;
        RSC = RSC->Next;
        free(Temp);
    }
    return RSC_OK;
}
int RSCSearch(RSC_t *RSC,const char *FileName)
{
    int i;
    for( i = 0; i < (int) RSC->Header.NumEntry; i++ ) {
        if( !strcmp(RSC->EntryList[i].Name,FileName) ) {
            DPrintf("RSCSearch:Found Entry %s in list\n",FileName);
            return i;
        }
    }
    return RSC_FILE_NOT_FOUND;
}
int RSCOpen(const RSC_t *RSC,const char *FileName,RSCEntry_t *OutEntry) 
{
    RSC_t *Iterator;
    int EntryIndex;
    
    if( !RSC ) {
        printf("RSCOpen:Invalid RSC data\n");
        return RSC_INVALID_DATA; 
    }
    
    if( !OutEntry ) {
        DPrintf("RSCOpen:Invalid Out Entry\n");
        return RSC_INVALID_DATA;
    }
    
    for( Iterator = RSC; Iterator; Iterator = Iterator->Next ) {
        EntryIndex = RSCSearch(Iterator,FileName);
        if( EntryIndex >= 0 ) {
            *OutEntry = Iterator->EntryList[EntryIndex];
            return RSC_OK;
        }
    }
    DPrintf("RSCOpen:An error has occurred:%s\n",RSCGetErrorString(EntryIndex));
    return RSC_FILE_NOT_FOUND;
}

char *RSCGetBaseName(const char *RSCPath)
{
    char *Out;
    Out = strrchr(RSCPath, '\\');
    if (Out == NULL) {
        return strdup(RSCPath);
    } else {
        //Skip remaining dir separator.
        Out++;
    }
    return strdup(Out);
}

void RSCAppend(RSC_t **RSCList,RSC_t *RSC)
{
    RSC_t *LastNode;
    if( !*RSCList ) {
        *RSCList = RSC;
    } else {
        LastNode = *RSCList;
        while( LastNode->Next ) {
            LastNode = LastNode->Next;
        }
        LastNode->Next = RSC;
    }
}

int RSCGetDirectoryFileCount(const RSC_t *RSC,const char *Directory)
{
    const RSC_t *Iterator;
    int NumFiles;
    int i;
    if( !RSC ) {
        printf("RSCGetDirectoryFileCount:Invalid RSC data\n");
        return RSC_INVALID_DATA; 
    }
    
    if( !Directory ) {
        DPrintf("RSCGetDirectoryFileCount:Invalid Directory\n");
        return RSC_INVALID_DATA;
    }
    NumFiles = 0;
    //NOTE(Adriano):This should not be a problem since every RSC file has his own directories that are not shared
    for( Iterator = RSC; Iterator; Iterator = Iterator->Next ) {
        for(i = 0; i < Iterator->Header.NumEntry; i++ ) {
            if( StringStartsWith(Iterator->EntryList[i].Name,Directory) ) {
                NumFiles++;
            }
        }
    }
    return NumFiles;
}
RSCEntry_t *RSCGetDirectoryEntries(const RSC_t *RSC,const char *Directory,int *NumEntry)
{
    const RSC_t *Iterator;
    RSCEntry_t *EntryList;
    int i;
    int NumEntries;
    int CurrentEntry;
    if( !RSC ) {
        printf("RSCGetDirectoryEntries:Invalid RSC data\n");
        return NULL; 
    }
    
    if( !Directory ) {
        DPrintf("RSCGetDirectoryEntries:Invalid Directory\n");
        return NULL;
    }
    NumEntries = RSCGetDirectoryFileCount(RSC,Directory);
    if( NumEntries < 0 ) {
        DPrintf("RSCGetDirectoryEntries:Error %s\n",RSCGetErrorString(NumEntries));
        return NULL;
    }
    if( NumEntries == 0 ) {
        DPrintf("RSCGetDirectoryEntries:No entries found for directory %s\n",Directory);
        return NULL;
    }
    CurrentEntry = 0;
    EntryList = malloc(NumEntries * sizeof(RSCEntry_t));
    if( !EntryList ) {
        DPrintf("RSCGetDirectoryEntries:Failed to allocate memory for entry list\n");
        return NULL;

    }
    //NOTE(Adriano):This should not be a problem since every RSC file has his own directories that are not shared
    for( Iterator = RSC; Iterator; Iterator = Iterator->Next ) {
        for(i = 0; i < Iterator->Header.NumEntry; i++ ) {
            if( CurrentEntry > NumEntries ) {
                DPrintf("RSCGetDirectoryEntries:Buffer overrun...expected %i entries but found %i\n",NumEntries,CurrentEntry);
                free(EntryList);
                return NULL;
            }
            if( StringStartsWith(Iterator->EntryList[i].Name,Directory) ) {
                EntryList[CurrentEntry] = Iterator->EntryList[i];
                CurrentEntry++;
            }
        }
    }
    if( NumEntry ) {
        *NumEntry = CurrentEntry;
    }
    return EntryList;
}
RSC_t *RSCLoad(const char *FileName)
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

    DPrintf("Loading pack %s...\n",FileName);
    
    RSCFile = fopen(FileName,"rb");
    
    if( RSCFile == NULL ) {
        DPrintf("RSCLoad:Failed opening RSC file.\n");
        return NULL;
    }
    
    RSC = malloc(sizeof(RSC_t));
    if( !RSC ) {
        DPrintf("RSCLoad:Failed allocate memory for RSC file.\n");
        fclose(RSCFile);
        return NULL;
    }
    RSC->Next = NULL;
    
    DPrintf("Scanning elements...\n");
    fread(&RSC->Header, sizeof(RSC->Header), 1, RSCFile);
    
    DPrintf("Dir Name: %s\n",RSC->Header.DirName);
    DPrintf("Dir Contains %i entries\n",RSC->Header.NumEntry);
    DPrintf("Dir Unknown:%i\n",RSC->Header.Unknown);
    EntryListSize = RSC->Header.NumEntry * sizeof(RSCEntry_t);
    RSC->EntryList = malloc(EntryListSize);
    
    if( !RSC->EntryList ) {
        fclose(RSCFile);
        RSCFree(RSC);
        return NULL;
    }
    memset(RSC->EntryList,0,EntryListSize);
    
    for( i = 0; i < RSC->Header.NumEntry; i++ ) {
        fread(&RSC->EntryList[i].Name, sizeof(RSC->EntryList[i].Name), 1, RSCFile);
        fread(&RSC->EntryList[i].Index, sizeof(RSC->EntryList[i].Index), 1, RSCFile);
        fread(&RSC->EntryList[i].Length, sizeof(RSC->EntryList[i].Length), 1, RSCFile);
        fread(&RSC->EntryList[i].Offset, sizeof(RSC->EntryList[i].Offset), 1, RSCFile);
        fread(&RSC->EntryList[i].Pad, sizeof(RSC->EntryList[i].Pad), 1, RSCFile);
        RSC->EntryList[i].Data = malloc(RSC->EntryList[i].Length + 1);
        PreviousFilePosition = ftell(RSCFile);
        fseek(RSCFile,RSC->EntryList[i].Offset,SEEK_SET);
        fread(RSC->EntryList[i].Data,RSC->EntryList[i].Length,1,RSCFile);
        RSC->EntryList[i].Data[RSC->EntryList[i].Length] = 0;
        fseek(RSCFile,PreviousFilePosition,SEEK_SET);
        DPrintf("Reading entry %i....got %s with length %i, index %i, pad %i and offset %i\n",i,RSC->EntryList[i].Name,RSC->EntryList[i].Length,
               RSC->EntryList[i].Index,RSC->EntryList[i].Pad,RSC->EntryList[i].Offset);
    }
    fclose(RSCFile);
    return RSC;
}
