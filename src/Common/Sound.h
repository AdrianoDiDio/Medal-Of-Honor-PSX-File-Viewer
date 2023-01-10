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
#ifndef __SOUND_H_
#define __SOUND_H_

#include "Common.h"
#include "Config.h"
#include "samplerate.h"

#define SOUND_SYSTEM_FREQUENCY 44100
#define SOUND_SYSTEM_BUFFER_FORMAT AUDIO_F32
#define SOUND_SYSTEM_NUM_SAMPLES 512
#define SOUND_SYSTEM_NUM_CHANNELS 2

typedef struct WAVHeader_s {
    char    RIFFHeader[4];
    int     WAVSize;
    char    WAVEHeader[4];
    char    FormatHeader[4];
    int     FormatSize;
    short   AudioFormat;
    short   NumChannels;
    int     SampleRate;
    int     ByteRate;
    short   BlockAlign;
    short   BitsPerSample;
    
    char    DataHeader[4];
    int     DataSize;
} WAVHeader_t;
//VAB Body Music files.
typedef struct VBMusic_s {
    char    *Name;
    Byte    *Data;
    float   *OriginalData;
    int     Size;
    int     DataLeft;
    int     NumFrames;
    int     Duration;
    int     DataPointer;
    float   Frequency;
    int     OriginalNumFrames;
    
    struct VBMusic_s *Next;
} VBMusic_t;

typedef struct SoundSystem_s {
    SDL_AudioDeviceID Device;
} SoundSystem_t;

// typedef struct LevelManager_s LevelManager_t;
SoundSystem_t   *SoundSystemInit(SDL_AudioCallback Callback,void *UserData);
void            SoundSystemLockDevice(SoundSystem_t *SoundSystem);
void            SoundSystemUnlockDevice(SoundSystem_t *SoundSystem);
void            SoundSystemPlay(SoundSystem_t *SoundSystem);
void            SoundSystemPause(SoundSystem_t *SoundSystem);
int             SoundSystemIsPaused(SoundSystem_t *SoundSystem);
int             SoundSystemResampleMusic(VBMusic_t *Music,float DesiredFrequency);
VBMusic_t       *SoundSystemLoadVBMusic(const char *VBFileName,int Length);
VBMusic_t       *SoundSystemLoadVBFile(FILE *VBFile,int Length,const char *VBFileName,float InFrequency);
int             SoundSystemGetSoundDuration(VBMusic_t *Music,int *Minutes,int *Seconds);
int             SoundSystemGetCurrentSoundTime(VBMusic_t *Music,int *Minutes,int *Seconds);
void            SoundSystemAddMusicToList(VBMusic_t **MusicList,VBMusic_t *Music);
void            SoundSystemDumpMusicToWav(VBMusic_t *Music,const char *EngineName,const char *OutDirectory);
void            SoundSystemClearMusicList(VBMusic_t *MusicList);
void            SoundSystemCleanUp(SoundSystem_t *SoundSystem);

extern Config_t *SoundVolume;
#endif//__SOUND_H_
