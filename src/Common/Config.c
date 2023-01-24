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

#include "Config.h"
#include "Common.h"

Config_t *ConfigList;

void ConfigFree()
{
    Config_t *Temp;
    
    while( ConfigList ) {
        free(ConfigList->Name);
        free(ConfigList->Value);
        if( ConfigList->Description ) {
            free(ConfigList->Description);
        }
        Temp = ConfigList;
        ConfigList = ConfigList->Next;
        free(Temp);
    }
}
void ConfigTokenizeSettings(const char *String)
{
    const char *Temp;
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
            //NOTE(Adriano):Make sure to finalize the string only when we have something in the buffer.
            if( i > 0 ) {
                ConfigBuffer[i++] = '\0';
                ConfigLine[NumArgs++] = StringCopy(ConfigBuffer);
                i = 0;
            }
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
        ConfigSet(ConfigLine[0],ConfigLine[1]);
    }
    if( ConfigLine[0] ) {
        free(ConfigLine[0]);
    }
    if( ConfigLine[1] ) {
        free(ConfigLine[1]);
    }
}

void ConfigLoadSettings()
{
    char *PrefPath;
    char *PrefFile;
    char  ConfigLine[1024];
    int   ConfigLineIndex;
    char *ConfigBuffer;
    char *Temp;
    
    PrefPath = AppGetConfigPath();
    asprintf(&PrefFile,"%sConfig.cfg",PrefPath);
    ConfigBuffer = ReadTextFile(PrefFile,0);
    
    //Settings didn't exists save the default ones.
    if( !ConfigBuffer ) {
        free(PrefFile);
        free(PrefPath);
        ConfigSaveSettings();
        return;
    }

    ConfigLineIndex = 0;
    Temp = ConfigBuffer;
    
    DPrintf("ConfigLoadSettings:Loading %s\n",PrefFile);

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
                if( !*Temp ) {
                    break;
                }
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
    free(PrefPath);
    return;
}

void ConfigSaveSettings()
{
    FILE *ConfigFile;
    char *PrefPath;
    char *PrefFile;
    Config_t *Config;

    PrefPath = AppGetConfigPath();
    asprintf(&PrefFile,"%sConfig.cfg",PrefPath);
    
    ConfigFile = fopen(PrefFile,"w+");
    
    fprintf(ConfigFile,"/*\n\t\t\t%s\n*/\n",CONFIG_FILE_HEADER);
    for(Config = ConfigList; Config; Config = Config->Next ){
        if( Config->Description ) {
            fprintf(ConfigFile,"/*\n%s:\n%s\n*/\n",Config->Name,Config->Description);
        }
        fprintf(ConfigFile,"%s \"%s\"\n",Config->Name,Config->Value);
    }
    
    free(PrefFile);
    fclose(ConfigFile);
    free(PrefPath);
}
Config_t *ConfigGet(const char *Name)
{
    Config_t *Config;
    for(Config = ConfigList; Config; Config = Config->Next ){
        if( !strcmp(Config->Name,Name) ) {
            return Config;
        }
    }
    return NULL;
}

void ConfigUpdateValue(Config_t *Config,const char *Value)
{
    if( !Config ) {
        DPrintf("ConfigUpdateValue:Invalid config.\n");
        return;
    }
    if( Config->Value ) {
        free(Config->Value);
    }
    Config->Value = StringCopy(Value);
    Config->IValue = StringToInt(Value);
    Config->FValue = atof(Value);
}
/*
 Sets the Value of a config by Name.
 If the config was found, its value is updated and persisted inside
 the default config file.
 Returns 1 if operation succeeded 0 otherwise.
 */
int ConfigSet(const char *Name,const char *Value)
{
    Config_t *Config;
    if( !Name ) {
        DPrintf("ConfigSet:Invalid name\n");
        return 0;
    }
    if( !Value ) {
        DPrintf("ConfigSet:Invalid value\n");
        return 0;
    }
    for(Config = ConfigList; Config; Config = Config->Next ){
        if( !strcmp(Config->Name,Name) ) {
            ConfigUpdateValue(Config,Value);
            ConfigSaveSettings();
            return 1;
        }
    }
    DPrintf("ConfigSet:No config named \"%s\" was found in the list.\n",Name);
    return 0;
}

int ConfigSetNumber(const char *Name,float Value)
{
    char SmallBuf[64];
    int Truncated;
    
    Truncated = (int) Value;
    //If truncating the variable results in the same variable
    //then it is probably just an int.
    if( Value == Truncated ) {
        sprintf(SmallBuf,"%i",Truncated);
    } else {
        sprintf(SmallBuf,"%f",Value);
    }
    return ConfigSet(Name,SmallBuf);
}

/*
 Register a new configuration key using the given Name,Value and Description.
 If the config has already been registered the function will return 0,
 otherwise a new config entry is created and added to the list and the return value will be 1.
 If the config cannot be created due to memory allocation errors then it will return -1.
 */
int ConfigRegister(const char *Name,const char *Value,const char *Description)
{
    Config_t *Config;
    
    if( ( Config = ConfigGet(Name) ) != NULL ) {
        return 0;
    }
    
    Config = malloc(sizeof(Config_t));
    
    if( !Config ) {
        return -1;
    }
    
    Config->Name = StringCopy(Name);
    Config->Value = NULL;
    ConfigUpdateValue(Config,Value);
    if( Description ) {
        Config->Description = StringCopy(Description);
    } else {
        Config->Description = NULL;
    }
    Config->Next = ConfigList;
    ConfigList = Config;
    
    return 1;
}

void ConfigDumpSettings()
{
    Config_t *Config;
    
    for(Config = ConfigList; Config; Config = Config->Next ){
        DPrintf("Config:%s Value:%s %i %f\n",Config->Name,Config->Value,Config->IValue,Config->FValue);
    }
    
}
void ConfigInit()
{
    ConfigLoadSettings();
}
