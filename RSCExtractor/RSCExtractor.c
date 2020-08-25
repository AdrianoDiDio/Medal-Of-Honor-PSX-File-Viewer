/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
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


#ifdef __linux__
#include <sys/stat.h>
#define PATHSEPARATOR '/'
#else
#include <windows.h>
#define PATHSEPARATOR '\\'
#endif

typedef struct RCS_s {
    char DirName[64];
    long long NumEntry;
} RCS_t;

typedef struct RCS_Entry_s {
    char FName[68];
    int  Length;
    long long Offset;
} RCS_Entry_t;

typedef enum {
    false,true
} bool;

void CreateDirIfNotExists(char *DirName) {
#ifdef __linux__
    struct stat FileStat;

    if (stat(DirName, &FileStat) == -1) {
        mkdir(DirName, 0700);
    }
#else
    CreateDirectory(DirName,NULL);
#endif
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
void DumpChunk(FILE *RCSFile,RCS_Entry_t *Entry) {
    FILE *OutFile;
    void *Chunk;
    int CurrentFilePosition;
    char *NormalizedFilePath;
    bool BadFile;
    
    if( RCSFile == NULL || Entry == NULL ) {
        BadFile = (RCSFile == NULL) ? true : false;
        printf("DumpChunk:Invalid %s\n", BadFile ? "File" : "Entry");
        return;
    }
    
    NormalizedFilePath = NormalizeSeparatorAndCreateDir(Entry->FName);
    OutFile = fopen(NormalizedFilePath,"wb");
    if( OutFile == NULL ) {
        printf("Couldn't open output file.\n");
        return;
    }
    CurrentFilePosition = ftell(RCSFile);
    Chunk = (void *) malloc(Entry->Length);
    fseek(RCSFile,Entry->Offset,SEEK_SET);
    fread(Chunk,Entry->Length,1,RCSFile);
    fwrite(Chunk,Entry->Length,1,OutFile);
    fseek(RCSFile,CurrentFilePosition,SEEK_SET);
    free(NormalizedFilePath);
    fflush(OutFile);
    fclose(OutFile);
    return;
}

int main(int argc,char **argv) {
    FILE *PackFile;
    RCS_t Pack;
    RCS_Entry_t *Entries;
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
    printf("Dir Contains %i entries %li\n",(int)Pack.NumEntry,sizeof(Pack.NumEntry));
    ArraySize = (int)Pack.NumEntry * sizeof(RCS_Entry_t);
    Entries = malloc(ArraySize);
    memset(Entries,0,ArraySize);
    
    for( i = 0; i < (int) Pack.NumEntry; i++ ) {
        fread(&Entries[i], sizeof(RCS_Entry_t), 1, PackFile);
        printf("Reading entry %i....got %s with length %i and offset %i\n",i,Entries[i].FName,Entries[i].Length,(int)Entries[i].Offset);
        DumpChunk(PackFile,&Entries[i]);
    }
    fclose(PackFile);
    free(Entries);
    return 0;
}
