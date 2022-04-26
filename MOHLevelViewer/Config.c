// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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

#include "Config.h"
#include "MOHLevelViewer.h"

Config_t *ConfigList;

void ConfigTokenizeSettings(char *String)
{
    char *Temp;
    char  ConfigBuffer[1024];
    char *ConfigLine[2];
    int   NumArgs;
    int i;
    
    Temp = String;
    NumArgs = 0;
    i = 0;
    ConfigLine[0] = ConfigLine[1] = NULL;

    while( 1 ) {
        
        if( NumArgs >= 2 ) {
            DPrintf("ConfigTokenizeSettings:Found a key with multiple values...discarding them\n");
            break;
        }
        //Skip any whitespace...
        while( *Temp && *Temp <= ' ' ) {
            Temp++;
        }

        if( !*Temp ) {
            ConfigBuffer[i++] = '\0';
            ConfigLine[NumArgs++] = StringCopy(ConfigBuffer);
            i = 0;
            break;
        }
        
        if( *Temp == '"' ) {
            Temp++;
            while ( *Temp && *Temp != '"' ) {
                ConfigBuffer[i++] = *Temp++;
            }
            if( *Temp != '"' ) { 
                DPrintf("ConfigTokenizeSettings:Malformed quote found expected \" found %c.\n",*Temp);
                break;
            }
            Temp++;
            continue;
        }
        //Found a string copy it...
        if( *Temp > ' ' ) {
            do {
                ConfigBuffer[i++] = *Temp++;
            } while( *Temp && *Temp != ' ');
            ConfigBuffer[i++] = 0;
            ConfigLine[NumArgs++] = StringCopy(ConfigBuffer);
            i = 0;
            continue;
        }
    }
    
    if( NumArgs <= 1 ) {
        DPrintf("ConfigTokenizeSettings:Found key %s but not value.\n",ConfigLine[0]);
    } else {
        DPrintf("Setting config %s to %s\n",ConfigLine[0],ConfigLine[1]);
    }
    if( ConfigLine[0] ) {
        free(ConfigLine[0]);
    }
    if( ConfigLine[1] ) {
        free(ConfigLine[1]);
    }
}

void ConfigReadSettings()
{
    char *PrefPath;
    char *PrefFile;
    char  ConfigLine[1024];
    int   ConfigLineIndex;
    char *ConfigBuffer;
    char *Temp;
    
    PrefPath = SysGetConfigPath();
    asprintf(&PrefFile,"%sConfig.cfg",PrefPath);
    ConfigBuffer = ReadTextFile(PrefFile,0);

    ConfigLineIndex = 0;
    Temp = ConfigBuffer;
    
    DPrintf("ConfigReadSettings:Loading %s\n",PrefFile);

    while( 1 ) {
        if( !*Temp ) {
            break;
        }
        //Skip comments
        if( Temp[0] == '/' ) {
            if( Temp[1] == '/' ) {
                //This line is an inline comment and can be skipped!
                do {
                    Temp++;
                } while( *Temp && *Temp != '\n' );
                Temp++;
                continue;
            } else if ( Temp[1] == '*' ) {
                //This line contains a multi-line comment and can be skipped.
                Temp++;
                do {
                    Temp++;
                } while( *Temp && !(Temp[0] == '*' && Temp[1] == '/') );
                Temp += 2;
            }
        }
        
        if( *Temp > ' ' ) {
            do {
                ConfigLine[ConfigLineIndex++] = *Temp++;
            } while( *Temp && *Temp != '\n');
            ConfigLine[ConfigLineIndex++] = '\0';
            ConfigLineIndex = 0;
            ConfigTokenizeSettings(ConfigLine);
        }
        Temp++;
    }
    free(ConfigBuffer);
    free(PrefFile);
    return;
}

void ConfigSaveSettings()
{
    FILE *ConfigFile;
    char *PrefPath;
    char *PrefFile;
    Config_t *Config;
    PrefPath = SysGetConfigPath();
    asprintf(&PrefFile,"%sConfig.cfg",PrefPath);
    
    ConfigFile = fopen(PrefFile,"w+");
    
    for(Config = ConfigList; Config; Config = Config->Next ){
        if( Config->Description ) {
            fprintf(ConfigFile,"//%s\n",Config->Description);
        }
        fprintf(ConfigFile,"%s \"%s\"\n",Config->Name,Config->Value);
    }
    
    free(PrefFile);
    fclose(ConfigFile);
}
Config_t *ConfigGet(char *Name)
{
    Config_t *Config;
    for(Config = ConfigList; Config; Config = Config->Next ){
        if( !strcmp(Config->Name,Name) ) {
            return Config;
        }
    }
    return NULL;
}
int ConfigSet(char *Name,char *Value)
{
    Config_t *Config;
    for(Config = ConfigList; Config; Config = Config->Next ){
        if( !strcmp(Config->Name,Name) ) {
            free(Config->Value);
            Config->Value = StringCopy(Value);
            ConfigSaveSettings();
            return 1;
        }
    }
    return 0;
}
int ConfigRegister(char *Name,char *Value,char *Description)
{
    Config_t *Config;
    
    if( ConfigGet(Name) != NULL ) {
        return 0;
    }
    
    Config = malloc(sizeof(Config_t));
    
    if( !Config ) {
        return -1;
    }
    
    Config->Name = StringCopy(Name);
    Config->Value = StringCopy(Value);
    if( Description ) {
        Config->Description = StringCopy(Description);
    } else {
        Config->Description = NULL;
    }
    Config->Next = ConfigList;
    ConfigList = Config;
    
    return 1;
}

void ConfigRegisterDefaultSettings()
{
    ConfigRegister("VideoWidth","800",NULL);
    ConfigRegister("VideoHeight","600",NULL);
    ConfigRegister("BasePath","","Sets the path from which the game will be loaded");
}

void ConfigInit()
{
    ConfigRegisterDefaultSettings();
    ConfigReadSettings();
//     ConfigSet("VideoWidth","444");
//     Config_t *VidWidth = ConfigGet("VideoWidth");
//     VidWidth->Value = "666";
//     ConfigSaveSettings();
}
