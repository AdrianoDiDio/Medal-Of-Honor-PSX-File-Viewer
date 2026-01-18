// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2026 Adriano Di Dio.
    
    RSCExtractor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RSCExtractor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RSCExtractor.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

#ifdef __linux__
#define PATHSEPARATOR '/'
#else
#include <windows.h>
#define PATHSEPARATOR '\\'
#endif

typedef struct RSCHeader_s {
    char DirName[64];
    int  NumEntry;
    int  Unknown;
} RSC_t;
typedef struct RSCEntry_s {
    char    Name[64];
    int     Index;
    int     Length;
    int     Offset;
    int     Pad;
} RSCEntry_t;

void CreateDirIfNotExists(char *DirName) {
    struct stat FileStat;

    if (stat(DirName, &FileStat) == -1) {
#ifdef _WIN32
        mkdir(DirName);
#else
        mkdir(DirName, 0700);
#endif
    }
}

char *NormalizeSeparatorAndCreateDir(char *FName) {
    char   *Result;
    char   *Out;
    char   *Iterator;
    int     i;
    char    Dir[256];
    
    Result = (char *) malloc(strlen(FName) + 1);

    Iterator = FName;
    Out = Result;
    i = 0;
    
    while( *Iterator ) {
        if( *Iterator == '\\' ) {
            Dir[i] = '\0';
            CreateDirIfNotExists(Dir);
#ifdef __linux__
            Dir[i++] = *Out++ = '/';
            Iterator++;
            continue;
#endif
        }
        Dir[i++] = *Out++ = *Iterator++;
    }
    *Out = '\0';
    return Result;
}
void DumpChunk(FILE *RSCFile,RSCEntry_t *Entry) {
    FILE *OutFile;
    void *Chunk;
    int CurrentFilePosition;
    char *NormalizedFilePath;
    bool BadFile;
    
    if( RSCFile == NULL || Entry == NULL ) {
        BadFile = (RSCFile == NULL) ? true : false;
        printf("DumpChunk:Invalid %s\n", BadFile ? "File" : "Entry");
        return;
    }
    
    NormalizedFilePath = NormalizeSeparatorAndCreateDir(Entry->Name);
    OutFile = fopen(NormalizedFilePath,"wb");
    if( OutFile == NULL ) {
        printf("Couldn't open output file.\n");
        return;
    }
    CurrentFilePosition = ftell(RSCFile);
    Chunk = (void *) malloc(Entry->Length);
    fseek(RSCFile,Entry->Offset,SEEK_SET);
    fread(Chunk,Entry->Length,1,RSCFile);
    fwrite(Chunk,Entry->Length,1,OutFile);
    fseek(RSCFile,CurrentFilePosition,SEEK_SET);
    free(NormalizedFilePath);
    free(Chunk);
    fflush(OutFile);
    fclose(OutFile);
    return;
}

int main(int argc,char **argv) {
    FILE *PackFile;
    RSC_t Pack;
    RSCEntry_t *Entries;
    int i;
    int ArraySize;
    
    if( argc == 1 ) {
        printf("%s <Input.rsc> will extract the content of Input.rsc"
         " in the current folder creating all the necessary directories.\n",argv[0]);
        return -1;
    }

    printf("Loading pack %s...\n",argv[1]);
    
    PackFile = fopen(argv[1],"rb");
    
    if( PackFile == NULL ) {
        printf("Failed opening RSC pack.\n");
        return -1;
    }
    
    printf("Scanning elements...\n");
    fread(&Pack, sizeof(Pack), 1, PackFile);
    
    printf("Dir Name: %s\n",Pack.DirName);
    printf("Dir Contains %i entries %li\n",Pack.NumEntry,sizeof(Pack.NumEntry));
    printf("Dir unknown:%i\n",Pack.Unknown);
    ArraySize = Pack.NumEntry * sizeof(RSCEntry_t);
    Entries = malloc(ArraySize);
    memset(Entries,0,ArraySize);
    
    for( i = 0; i < Pack.NumEntry; i++ ) {
        fread(&Entries[i], sizeof(RSCEntry_t), 1, PackFile);
        printf("Reading entry %i....got %s with length %i, index %i, pad %i and offset %i\n",i,Entries[i].Name,Entries[i].Length,
               Entries[i].Index,Entries[i].Pad,Entries[i].Offset);
        DumpChunk(PackFile,&Entries[i]);
    }
    fclose(PackFile);
    free(Entries);
    return 0;
}
