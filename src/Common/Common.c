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
#include "Common.h"
#include "Config.c"

char *AppName = NULL;

Byte HighNibble(Byte In)
{
    return (In >> 0x4) & 0xF;
}

Byte LowNibble(Byte In)
{
    return In & 0xF;
}

int SignExtend(int Temp)
{
    if ( Temp & 0x8000 ) {
        Temp |= 0xffff0000;
    }
    return Temp;
}

float Rand01()
{
    return (rand() / (float)(RAND_MAX));
}
int RandRangeI(int Min,int Max)
{
    return rand() % ( (Max + 1 - Min) + Min);
}
int vasprintf(char **Strp, const char *Fmt, va_list Ap)
{
    va_list APCopy;
    int Length;
    char *Buffer;
    int Result;

    va_copy(APCopy, Ap);
    Length = vsnprintf(NULL, 0, Fmt, APCopy);

    if (Length < 0) {
        return Length;
    }

    va_end(APCopy);
    Buffer = malloc(Length + 1);

    if (!Buffer) {
        return -1;
    }

    Result = vsnprintf(Buffer, Length + 1, Fmt, Ap);

    if (Result < 0) {
        free(Buffer);
    } else {
        *Strp = Buffer;
    }
    return Result;
}
/*
 sprintf version that takes an empty buffer and allocate the required space based on the format argument.
 It should be included in some linux version but it is not standard.
 */
int asprintf(char **Strp, const char *Fmt, ...)
{
    int Error;
    va_list Ap;

    va_start(Ap, Fmt);
    Error = vasprintf(Strp, Fmt, Ap);
    va_end(Ap);

    return Error;
}
char *StringCopy(const char *From)
{
    char *Dest;

    Dest = malloc(strlen(From) + 1);

    if ( !Dest ) {
        return NULL;
    }

    strcpy(Dest, From);

    return Dest;
}

char *StringAppend(const char *FirstString,const char *SecondString)
{
    char *Result;
    if( !FirstString || !SecondString ) {
        return NULL;
    }
    Result = malloc(strlen(FirstString) + strlen(SecondString) + 1);
    strcpy(Result, FirstString);
    strcat(Result, SecondString);
    return Result;
}

char *StringToUpper(const char *In)
{
    char *Result;
    int i;
    
    Result = StringCopy(In);
    for( i = 0; Result[i]; i++ ){
        if( Result[i] >= 'a' && Result[i] <= 'z' ){
            Result[i] -= ('a' - 'A');
        }
    }
    return Result;
}

int StringToInt(const char *String)
{
    char *EndPtr;    
    long Value;
    
    Value = strtol(String, &EndPtr, 10);
    
    if( errno == ERANGE && Value == LONG_MIN ) {
        DPrintf("StringToInt %s (%lu) invalid...underflow occurred\n",String,Value);
        return 0;
    } else if( errno == ERANGE && Value == LONG_MAX ) {
        DPrintf("StringToInt %s (%lu) invalid...overflow occurred\n",String,Value);
        return 0;
    }
    return Value;
}

char *GetBaseName(const char *Path)
{
    char *Out;
    Out = strrchr(Path, PATH_SEPARATOR);
    if (Out == NULL) {
        return strdup(Path);
    } else {
        //Skip remaining dir separator.
        Out++;
    }
    return strdup(Out);
}

void CreateDirIfNotExists(const char *DirName) {
    struct stat FileStat;

    if (stat(DirName, &FileStat) == -1) {
#ifdef _WIN32
        mkdir(DirName);
#else
        mkdir(DirName, 0700);
#endif
    }
}

char *SwitchExt(const char *In, const char *Ext)
{
    char *NewExt;
    int StrippedFileNameLength;
    char *Temp;
    
    if ( !In || !Ext ) {
        return NULL;
    }

    Temp = strrchr(In,'.');
    if( Temp ) {
        StrippedFileNameLength = Temp - In;
    } else {
        StrippedFileNameLength = strlen(In);
    }
    NewExt = malloc(StrippedFileNameLength + strlen(Ext) + 1);
    if( !NewExt ) {
        return NULL;
    }
    strncpy(NewExt,In,StrippedFileNameLength);
    NewExt[StrippedFileNameLength] = '\0';
    //Now append the extension to the string.
    strncat(NewExt, Ext, strlen(NewExt));
    return NewExt;
}
char *GetFileExtension(const char *FileName) {
    char *Dot;
    Dot = strrchr(FileName, '.');
    if(!Dot || Dot == FileName) { 
        return NULL;
    }
    return Dot + 1;
}
int GetFileLength(FILE *Fp)
{
    int Length;
    int CurrentPosition;

    if ( !Fp ) {
        return -1; //Must be a valid file
    }

    CurrentPosition = ftell(Fp);
    fseek(Fp, 0, SEEK_END);
    Length = ftell(Fp);
    fseek(Fp, CurrentPosition, SEEK_SET);

    return Length;
}

char *ReadTextFile(const char *File,int Length)
{
    FILE *Fp;
    int FileSize;
    char *Result;
    int Ret;
    
    Fp = fopen(File,"rb");
    
    if( !Fp ) {
        DPrintf("ReadTextFile:File %s was not found.\n",File);
        return NULL;
    }
    FileSize = Length != 0 ? Length : GetFileLength(Fp);
    Result = malloc(FileSize + 1);
    
    if( !Result ) {
        DPrintf("ReadTextFile:Failed to allocate buffer\n");
        fclose(Fp);
        return NULL;
    }
    Ret = fread(Result,1, FileSize,Fp);
    fclose(Fp);
    
    if( Ret != FileSize ) {
        DPrintf("Failed to read file %s\n",File);
        free(Result);
        return NULL;
    }
    Result[Ret] = '\0';
    return Result;
}

int GetCurrentFilePosition(FILE *Fp)
{
    return ftell(Fp);
}

void SkipFileSection(int SectionSize,FILE *InFile)
{
    if( !InFile ) {
        printf("SkipFileSection: Invalid file.\n");
        return;
    }
    int CurrentSection = ftell(InFile);
    fseek(InFile,SectionSize,SEEK_CUR);
    assert((ftell(InFile) - CurrentSection) == SectionSize);
}

char *AppGetConfigPath()
{
    if( !AppName ) {
        DPrintf("AppGetConfigPath:CommonInit was not called!\n");
        assert(1!=1);
    }
    return SDL_GetPrefPath(NULL,AppName);
}
int SysMilliseconds()
{
    return SDL_GetTicks64();
}

void DPrintf(const char *Fmt, ...)
{
    char Temp[1000];
    va_list arglist;

    va_start(arglist, Fmt);
    vsnprintf(Temp, sizeof( Temp ), Fmt, arglist);
#ifdef _DEBUG
    fputs(Temp, stdout);
#endif
    va_end(arglist);
}

void SysHideCursor()
{
    SDL_ShowCursor(false);
}

void SysShowCursor()
{
    SDL_ShowCursor(true);
}
void CommonRegisterSettings()
{
    ConfigRegister("VideoWidth","800",NULL);
    ConfigRegister("VideoHeight","600",NULL);
    ConfigRegister("VideoRefreshRate","60",NULL);
    ConfigRegister("VideoFullScreen","0",NULL);
    ConfigRegister("VideoVSync","-1","Enable or disable vsync.\nPossible values are:-1 enable adaptive VSync (where supported),\n"
                    "0 Disables it and 1 enables standard VSync.");
    
    ConfigRegister("GUIFont","Fonts/DroidSans.ttf","Sets the file to be used as the GUI font,if not valid the application will use the default one");
    ConfigRegister("GUIFontSize","14.f",NULL);
    ConfigRegister("GUIShowFPS","1",NULL);
    
    ConfigRegister("SoundVolume","128","Sets the sound volume, the value must be in range 0-128, values outside that range will be clamped.");

}
void CommonShutdown()
{
    if( AppName ) {
        free(AppName);
    }
}
void CommonInit(const char *ApplicationName)
{
    if( AppName ) {
        DPrintf("CommonInit:Already called!\n");
        return;
    }
    CommonRegisterSettings();
    AppName = StringCopy(ApplicationName != NULL ? ApplicationName : "UnknownApp");
}
