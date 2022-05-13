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
#include "Sound.h"
#include "MOHLevelViewer.h"

const double ADPCMFilterGainPositive[5] = {
    0.0, 
    0.9375, 
    1.796875, 
    1.53125, 
    1.90625
};
const double ADPCMFilterGainNegative[5] = {
    0.0, 
    0.0, 
    -0.8125, 
    -0.859375, 
    -0.9375
};

Config_t *SoundVolume;
void SoundSystemClearMusicList(VBMusic_t *MusicList)
{
    VBMusic_t *Temp;
    while( MusicList ) {
        free(MusicList->Data);
        free(MusicList->Name);
        Temp = MusicList;
        MusicList = MusicList->Next;
        free(Temp);
    }
}
void SoundSystemCleanUp(SoundSystem_t *SoundSystem)
{
    SDL_CloseAudioDevice(SoundSystem->Device);
    SoundSystemClearMusicList(SoundSystem->MusicList);
    SoundSystemClearMusicList(SoundSystem->AmbientMusicList);
    free(SoundSystem);
}
void SoundSystemOnAudioUpdate(void *UserData,Byte *Stream,int Length)
{
    SoundSystem_t *SoundSystem;
    VBMusic_t *CurrentMusic;
    VBMusic_t **CurrentMusicAddress;
    int ChunkLength;
    
    SoundSystem = (SoundSystem_t *) UserData;
    CurrentMusic = SoundSystem->CurrentMusic;
    CurrentMusicAddress = &SoundSystem->CurrentMusic;

    if( CurrentMusic->DataPointer >= CurrentMusic->Size ) {
        CurrentMusic->DataPointer = 0;
        if( CurrentMusic->Next ) {
            *CurrentMusicAddress = (*CurrentMusicAddress)->Next;
        } else {
            if( SoundSystem->IsAmbient ) {
                *CurrentMusicAddress = SoundSystem->AmbientMusicList;
            } else {
                *CurrentMusicAddress = SoundSystem->MusicList;
            }
        }
    }
    for (int i = 0; i < Length; i++) {
        Stream[i] = 0;
    }

    ChunkLength = (CurrentMusic->Size - CurrentMusic->DataPointer);
    if( ChunkLength > Length ) {
        ChunkLength = Length;
    }
    if( SoundVolume->IValue > 128 || SoundVolume->IValue < 0 ) {
        ConfigSetNumber("SoundVolume",128);
    }
    SDL_MixAudioFormat(Stream, &CurrentMusic->Data[CurrentMusic->DataPointer], AUDIO_S16, ChunkLength, SoundVolume->IValue);
    CurrentMusic->DataPointer += ChunkLength;
}

void SoundSystemPause(SoundSystem_t *SoundSystem)
{
    SDL_PauseAudioDevice(SoundSystem->Device, 1);
}
void SoundSystemPlay(SoundSystem_t *SoundSystem)
{
    SDL_PauseAudioDevice(SoundSystem->Device, 0);
}
void SoundSystemPlayMusic(SoundSystem_t *SoundSystem,int IsAmbient)
{
    if( !SoundSystem->MusicList ) {
        return;
    }
    SoundSystemPause(SoundSystem);
    SoundSystem->CurrentMusic = IsAmbient ? SoundSystem->AmbientMusicList : SoundSystem->MusicList;
    SoundSystem->IsAmbient = IsAmbient;
    SoundSystemPlay(SoundSystem);
}
void SoundSystemStopMusic(SoundSystem_t *SoundSystem)
{
    SoundSystemPause(SoundSystem);
    SoundSystem->CurrentMusic = NULL;
    SoundSystem->IsAmbient = 0;
}
void SoundSystemLockDevice(SoundSystem_t *SoundSystem)
{
    SDL_LockAudioDevice(SoundSystem->Device);
}
void SoundSystemUnlockDevice(SoundSystem_t *SoundSystem)
{
    SDL_UnlockAudioDevice(SoundSystem->Device);
}
short SoundSystemQuantize(double Sample)
{
    int OutSample;
    OutSample = (int ) (Sample + 0.5);
    if ( OutSample < -32768 ) {
        return -32768;
    }
    if ( OutSample > 32767 ) {
        return 32767;
    }
    
    return (short) OutSample;
}
int SoundSystemCalculateSoundDuration(int Size,int Frequency,int NumChannels,int BitDepth)
{
    return Size / (Frequency * NumChannels * (BitDepth / 8));
}
int SoundSystemGetSoundDuration(SoundSystem_t *SoundSystem,int *Minutes,int *Seconds)
{
    if( !SoundSystem->CurrentMusic ) {
        return 0;
    }
    *Minutes = SoundSystem->CurrentMusic->Duration / 60;
    *Seconds = SoundSystem->CurrentMusic->Duration % 60;
    return SoundSystem->CurrentMusic->Duration;
}
int SoundSystemGetCurrentSoundTime(SoundSystem_t *SoundSystem,int *Minutes,int *Seconds)
{
    int CurrentLength;
    if( !SoundSystem->CurrentMusic ) {
        return 0;
    }
    CurrentLength = SoundSystemCalculateSoundDuration(SoundSystem->CurrentMusic->DataPointer,44100,2,16);
    *Minutes = CurrentLength / 60;
    *Seconds = CurrentLength % 60;
    return CurrentLength;
}
short *SoundSystemConvertADPCMToPCM(FILE *VBFile,int *NumFrames)
{
//     VBMusic_t *VBMusic;
    short *Result;
    int VBFileSize;
    Byte Header;
    Byte Shift;
    Byte Predictor;
    Byte Flag;
    Byte Data[14];
    double Sample[28];
    int  NumFramePerBlock;
    int BaseIndex;
    double State[2];
    int Temp;
    int NumWrittenBytes;
    int NumTotalSample;
    int Size;
    int  i;
    int  j;
    int  n;
    
    if( !VBFile ) {
        DPrintf("SoundSystemLoadVBFile:Invalid file\n");
        return NULL;
    }

    VBFileSize = GetFileLength(VBFile);
    
    State[0] = 0;
    State[1] = 0;
    // In Every block we have 14 bytes of compressed data where each Byte contains 2 compressed sample.
    // Thats why NumFramePerBlock is 28 ( 14 * 2).
    NumFramePerBlock = 28;
    NumTotalSample = VBFileSize / 16;
    *NumFrames = NumTotalSample * NumFramePerBlock;
    //Size depends from the number of channels (in this case is 1).
    Size = *NumFrames * 1 * sizeof(short);
    Result = malloc(Size);
    NumWrittenBytes = 0;
    for( i = 0; i < NumTotalSample; i++ ) {
        fread(&Header,sizeof(Header),1,VBFile);
        Predictor = HighNibble(Header);
        Shift = LowNibble(Header);
        fread(&Flag,sizeof(Flag),1,VBFile);
        fread(&Data,sizeof(Data),1,VBFile);

        //Decompress the sample contained in the next 14-bytes.
        //Each sample contains two value stored as two's complement.
        for( j = 0; j < 14; j++) {
            for( n = 0; n < 2; n++ ) {
                BaseIndex = (j * 2) + n;
                //Sample is contained inside 8-bit...
                //In the first iteration we grab it from the first 4-bit
                //while on the second one we grab the other half.
                if( n & 1 ) {
                    Temp = HighNibble(Data[j]) << 12;
                } else {
                    Temp = LowNibble(Data[j]) << 12;
                }
                Temp = SignExtend(Temp);    
                Sample[BaseIndex] = (double) ( Temp >> Shift  );
                Sample[BaseIndex] += (State[0] * ADPCMFilterGainPositive[Predictor] + State[1] * ADPCMFilterGainNegative[Predictor] );
                State[1] = State[0];
                State[0] = Sample[BaseIndex];
                Result[NumWrittenBytes++] = SoundSystemQuantize(Sample[BaseIndex]);
            }
        }
    }
    return Result;
}
void SoundSystemDumpMusic(VBMusic_t *Music,char *EngineName,char *OutDirectory)
{
    WAVHeader_t WAVHeader;
    FILE *WAVFile;
    char *VBFileName;
    char *WAVFileName;
    
    if( !Music ) {
        DPrintf("SoundSystemDumpMusic:Invalid Music data\n");
        return;
    }
    DPrintf("Switching %s\n",Music->Name);
    VBFileName = SwitchExt(Music->Name, ".wav");
    DPrintf("Switched %s\n",VBFileName);
    asprintf(&WAVFileName,"%s%c%s-%s",OutDirectory,PATH_SEPARATOR,EngineName,VBFileName);
    WAVFile = fopen(WAVFileName,"wb");
    if( !WAVFile ) {
        return;
    }
    
    WAVHeader.RIFFHeader[0] = 'R';
    WAVHeader.RIFFHeader[1] = 'I';
    WAVHeader.RIFFHeader[2] = 'F';
    WAVHeader.RIFFHeader[3] = 'F';
    
    WAVHeader.WAVSize = 4 + (8 + 16) + (8 + Music->Size);
    
    WAVHeader.WAVEHeader[0] = 'W';
    WAVHeader.WAVEHeader[1] = 'A';
    WAVHeader.WAVEHeader[2] = 'V';
    WAVHeader.WAVEHeader[3] = 'E';
    
    WAVHeader.FormatHeader[0] = 'f';
    WAVHeader.FormatHeader[1] = 'm';
    WAVHeader.FormatHeader[2] = 't';
    WAVHeader.FormatHeader[3] = ' ';
    
    WAVHeader.FormatSize = 16;
    WAVHeader.AudioFormat = 1;
    WAVHeader.NumChannels = 2;
    WAVHeader.SampleRate = 44100;
    WAVHeader.ByteRate = (44100 * 2 * (16 / 8));
    WAVHeader.BlockAlign = (2 * (16/8));
    WAVHeader.BitsPerSample = 16;
    
    WAVHeader.DataHeader[0] = 'd';
    WAVHeader.DataHeader[1] = 'a';
    WAVHeader.DataHeader[2] = 't';
    WAVHeader.DataHeader[3] = 'a';
    WAVHeader.DataSize = Music->Size;
    fwrite(&WAVHeader,sizeof(WAVHeader_t), 1,  WAVFile);
    fwrite(Music->Data,Music->Size,1,WAVFile);
    free(VBFileName);
    free(WAVFileName);
    fclose(WAVFile);
}
int SoundSystemDumpMusicToWav(SoundSystem_t *SoundSystem,char *EngineName,char *OutDirectory)
{
    VBMusic_t *Iterator;
    if( !SoundSystem->MusicList ) {
        return 0;
    }
    
    for( Iterator = SoundSystem->MusicList; Iterator; Iterator = Iterator->Next ) {
        SoundSystemDumpMusic(Iterator,EngineName,OutDirectory);
    }
    for( Iterator = SoundSystem->AmbientMusicList; Iterator; Iterator = Iterator->Next ) {
        SoundSystemDumpMusic(Iterator,EngineName,OutDirectory);
    }
    return 1;
    
}
/*
 * NOTE(Adriano):
 * VB files are used to store level music and contain raw ADPCM data that gets sent
 * to the SPU of the PSX.
 * They use a sample frequency of 22050Hz and contains only one channel (Mono).
 * In order to load it we need to first convert the ADPCM data to standard PCM and then
 * feed it to SDL (doing an internal conversion to change the frequency and the number of channels) in
 * order to be able to play it.
 */
VBMusic_t *SoundSystemLoadVBFile(char *VBFileName)
{
    VBMusic_t *Music;
    FILE *VBFile;
    SDL_AudioStream *AudioStream;
    short *PCMData;
    int    NumFrames;
    if( !VBFileName ) {
        DPrintf("SoundSystemLoadVBFile:Invalid file\n");
        return NULL;
    }
    VBFile = fopen(VBFileName,"rb");
    if( !VBFile ) {
        return NULL;
    }
    PCMData = SoundSystemConvertADPCMToPCM(VBFile,&NumFrames);
    if( !PCMData ) {
        DPrintf("SoundSystemLoadVBFile:Failed to convert ADPCM to PCM\n");
        fclose(VBFile);
        return NULL;
    }
    Music = malloc(sizeof(VBMusic_t));
    if( !Music ) {
        DPrintf("SoundSystemLoadVBFile:Failed to allocate memory for music data\n");
        free(PCMData);
        fclose(VBFile);
        return NULL;
    }
    Music->Name = StringCopy(GetBaseName(VBFileName));
    //Perform a fast conversion to a more appropriate format...
    AudioStream = SDL_NewAudioStream(AUDIO_S16, 1, 22050, AUDIO_S16, 2, 44100);
    SDL_AudioStreamPut(AudioStream, PCMData, NumFrames * sizeof (Sint16));
    SDL_AudioStreamFlush(AudioStream);
    Music->Size = SDL_AudioStreamAvailable(AudioStream);
    Music->Duration = SoundSystemCalculateSoundDuration(Music->Size,44100,2,16);
    Music->DataPointer = 0;
    Music->Data = malloc(Music->Size);
    Music->Next = NULL;
    SDL_AudioStreamGet(AudioStream,Music->Data,Music->Size);
    SDL_FreeAudioStream(AudioStream);
    free(PCMData);
    fclose(VBFile);
    return Music;
}
void SoundSystemAddMusicToList(VBMusic_t **MusicList,VBMusic_t *Music)
{
    VBMusic_t *LastNode;
    if( !*MusicList ) {
        *MusicList = Music;
    } else {
        LastNode = *MusicList;
        while( LastNode->Next ) {
            LastNode = LastNode->Next;
        }
        LastNode->Next = Music;
    }
}
void SoundSystemLoadLevelMusic(SoundSystem_t *SoundSystem,char *MissionPath,int MissionNumber,int LevelNumber,int GameEngine)
{
    VBMusic_t *Music;
    char *Buffer;
    int NumLoadedVB;
    
    SoundSystemPause(SoundSystem);
    SoundSystemClearMusicList(SoundSystem->MusicList);
    SoundSystemClearMusicList(SoundSystem->AmbientMusicList);
    SoundSystem->MusicList = NULL;
    SoundSystem->AmbientMusicList = NULL;
    SoundSystem->CurrentMusic = NULL;
    SoundSystem->IsAmbient = 0;
    NumLoadedVB = 1;
    
    if( GameEngine == MOH_GAME_STANDARD ) {
        asprintf(&Buffer,"%s%cM%i_%i.VB",MissionPath,PATH_SEPARATOR,MissionNumber,LevelNumber);
        Music = SoundSystemLoadVBFile(Buffer);
        if( !Music ) {
            DPrintf("SoundSystemLoadLevelMusic:Failed to open %s\n",Buffer);
            free(Buffer);
            return;
        }
        SoundSystemAddMusicToList(&SoundSystem->MusicList,Music);
        free(Buffer);
        asprintf(&Buffer,"%s%cM%i_%iA.VB",MissionPath,PATH_SEPARATOR,MissionNumber,LevelNumber);
        Music = SoundSystemLoadVBFile(Buffer);
        if( !Music ) {
            DPrintf("SoundSystemLoadLevelMusic:Failed to open %s\n",Buffer);
            free(Buffer);
            return;
        }
        SoundSystemAddMusicToList(&SoundSystem->AmbientMusicList,Music);
        NumLoadedVB++;
        free(Buffer);
    } else {
        /*
         * NOTE(Adriano):
         * MOH:Underground uses multiple track files per level that are swapped dynamically based on the 
         * collision between the camera and a specific node (like the TSP trigger).
         * What we do here is just append all the available VB files into a circular list that gets played
         * in a loop inside the audio callback.
         * Since we don't know how many VB files are required per level, we just try to load them in a loop that
         * stops only when the file cannot be opened (meaning that we have loaded all the available files).
         */
        while( 1 ) {

            asprintf(&Buffer,"%s%cM%i_%i_%i.VB",MissionPath,PATH_SEPARATOR,MissionNumber,LevelNumber,NumLoadedVB);
            Music = SoundSystemLoadVBFile(Buffer);
            if( !Music ) {
                free(Buffer);
                break;
            }
            SoundSystemAddMusicToList(&SoundSystem->MusicList,Music);
            free(Buffer);
            asprintf(&Buffer,"%s%cM%i_%i_%iA.VB",MissionPath,PATH_SEPARATOR,MissionNumber,LevelNumber,NumLoadedVB);
            Music = SoundSystemLoadVBFile(Buffer);
            if( !Music ) {
                free(Buffer);
                break;
            }
            SoundSystemAddMusicToList(&SoundSystem->AmbientMusicList,Music);
            NumLoadedVB++;
            free(Buffer);
        }
        DPrintf("Loaded %i files\n",NumLoadedVB);
    }
    return;
}
SoundSystem_t *SoundSystemInit()
{
    SoundSystem_t *SoundSystem;
    SDL_AudioSpec DesiredAudioSpec;
    SDL_AudioSpec ObtainedAudioSpec;
    
    SoundSystem = malloc(sizeof(SoundSystem_t));
    SoundSystem->MusicList = NULL;
    SoundSystem->AmbientMusicList = NULL;
    SoundSystem->CurrentMusic = NULL;
    
    SoundVolume = ConfigGet("SoundVolume");
    
    if( !SoundSystem ) {
        DPrintf("SoundSystemInit:Failed to initialize sound system.\n");
        return NULL;
    }
    
    SDL_zero(DesiredAudioSpec);
    DesiredAudioSpec.freq = SOUND_SYSTEM_FREQUENCY;
    DesiredAudioSpec.format = SOUND_SYSTEM_BUFFER_FORMAT;
    DesiredAudioSpec.channels = SOUND_SYSTEM_NUM_CHANNELS;
    DesiredAudioSpec.samples = SOUND_SYSTEM_NUM_SAMPLES;
    DesiredAudioSpec.callback = SoundSystemOnAudioUpdate;
    DesiredAudioSpec.userdata = SoundSystem;
    
    SoundSystem->Device = SDL_OpenAudioDevice(NULL,0,&DesiredAudioSpec, &ObtainedAudioSpec,0);
    
    if( ObtainedAudioSpec.freq != DesiredAudioSpec.freq || ObtainedAudioSpec.channels != DesiredAudioSpec.channels || 
        ObtainedAudioSpec.samples != DesiredAudioSpec.samples
    ) {
        DPrintf("SoundSystemInit:Wanted a frequency of %i with %i samples and %i channels...\n",DesiredAudioSpec.freq,DesiredAudioSpec.samples,
                DesiredAudioSpec.channels);
        DPrintf("...Obtained a frequency of %i with %i samples and %i channels.\n",ObtainedAudioSpec.freq,ObtainedAudioSpec.samples,
                ObtainedAudioSpec.channels);
        SDL_CloseAudioDevice(SoundSystem->Device);
        assert(1!=1);
        free(SoundSystem);
        return NULL;
    }
    return SoundSystem;
}
