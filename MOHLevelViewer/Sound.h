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
#ifndef __SOUND_H_
#define __SOUND_H_

#include "Common.h"
#include "Config.h"

#define SOUND_SYSTEM_FREQUENCY 44100
#define SOUND_SYSTEM_BUFFER_FORMAT AUDIO_S16
#define SOUND_SYSTEM_NUM_SAMPLES 512
#define SOUND_SYSTEM_NUM_CHANNELS 2

//VAB Body Music files.
typedef struct VBMusic_s {
    Byte *Data;
    int    Size;
    int    DataLeft;
    int    NumFrames;
    int    Duration;
    int    DataPointer;
    
    struct VBMusic_s *Next;
} VBMusic_t;
typedef struct SoundSystem_s {
    SDL_AudioDeviceID Device;
    VBMusic_t *MusicList;
    VBMusic_t *CurrentMusic;

} SoundSystem_t;

// typedef struct LevelManager_s LevelManager_t;
SoundSystem_t *SoundSystemInit();
void SoundSystemLoadLevelMusic(SoundSystem_t *SoundSystem,char *MissionPath,int MissionNumber,int LevelNumber,int GameEngine,int AmbientOnly);
int SoundSystemGetSoundDuration(SoundSystem_t *SoundSystem,int *Minutes,int *Seconds);
int SoundSystemGetCurrentSoundTime(SoundSystem_t *SoundSystem,int *Minutes,int *Seconds);
void SoundSystemCleanUp(SoundSystem_t *SoundSystem);

extern Config_t *SoundVolume;
#endif//__SOUND_H_
