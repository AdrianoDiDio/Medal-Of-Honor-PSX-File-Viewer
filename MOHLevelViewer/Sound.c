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
void SoundSystemClearMusicList(SoundSystem_t *SoundSystem)
{
    VBMusic_t *Temp;
    while( SoundSystem->MusicList ) {
        free(SoundSystem->MusicList->Data);
        Temp = SoundSystem->MusicList;
        SoundSystem->MusicList = SoundSystem->MusicList->Next;
        free(Temp);
    }
}
void SoundSystemCleanUp(SoundSystem_t *SoundSystem)
{
    SDL_CloseAudioDevice(SoundSystem->Device);
    SoundSystemClearMusicList(SoundSystem);
    free(SoundSystem);
}
void SoundSystemOnAudioUpdate(void *UserData,Byte *Stream,int Length)
{
    SoundSystem_t *SoundSystem;
    SoundSystem = (SoundSystem_t *) UserData;
    VBMusic_t *CurrentMusic;
    
    CurrentMusic = SoundSystem->CurrentMusic;
    
    if( CurrentMusic->DataPointer > CurrentMusic->Size ) {
        DPrintf("AudioCallback:Restarting it\n");
        CurrentMusic->DataPointer = 0;
    }
    for (int i = 0; i < Length; i++) {
        Stream[i] = 0;
    }
    SDL_MixAudioFormat(Stream, (Byte *) (CurrentMusic->Data + CurrentMusic->DataPointer), AUDIO_S16, Length, 128);
    CurrentMusic->DataPointer += (Length/2);
}

void SoundSystemPause(SoundSystem_t *SoundSystem)
{
    SDL_PauseAudioDevice(SoundSystem->Device, 1);
}
void SoundSystemPlay(SoundSystem_t *SoundSystem)
{
    SDL_PauseAudioDevice(SoundSystem->Device, 0);
}

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
short Quantize(double Sample)
{
    int a;
    a = (int ) (Sample + 0.5);
    if ( a < -32768 ) {
        return -32768;
    }
    if ( a > 32767 ) {
        return 32767;
    }
    
    return (short) a;
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
    int j;
    
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
//     VBMusic->Duration = VBMusic->Size / (22050 * 1 * 2);
//     Duration = *NumFrames / (float) SampleRate;
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
        for( j = 0; j < 14; j++ ) {
            BaseIndex = j * 2;
            Temp = LowNibble(Data[j]) << 12;
            Temp = SignExtend(Temp);    
            Sample[BaseIndex] = (double) ( Temp >> Shift  );
            Temp = HighNibble(Data[j]) << 12;
            Temp = SignExtend(Temp);
            Sample[BaseIndex+1] = (double) ( Temp >> Shift  );
        }
        //we have decompressed our samples now try to decode it.
        for( j = 0; j < 28; j++ ) {
            Sample[j] += (State[0] * ADPCMFilterGainPositive[Predictor] + State[1] * ADPCMFilterGainNegative[Predictor] );
            State[1] = State[0];
            State[0] = Sample[j];
            Result[NumWrittenBytes++] = Quantize(Sample[j]);

        }
    }
    return Result;
//     SoundSystemConvertADPCMToPCM(
}

VBMusic_t *SoundSystemLoadVBFile(FILE *VBFile)
{
    VBMusic_t *Music;
    SDL_AudioStream *AudioStream;
    short *PCMData;
    int    NumFrames;
    if( !VBFile ) {
        DPrintf("SoundSystemLoadVBFile:Invalid file\n");
        return NULL;
    }
    PCMData = SoundSystemConvertADPCMToPCM(VBFile,&NumFrames);
    if( !PCMData ) {
        DPrintf("SoundSystemLoadVBFile:Failed to convert ADPCM to PCM\n");
        return NULL;
    }
    Music = malloc(sizeof(VBMusic_t));
    if( !Music ) {
        DPrintf("SoundSystemLoadVBFile:Failed to allocate memory for music data\n");
        free(PCMData);
        return NULL;
    }
    //Perform a fast conversion to a more appropriate format...
    AudioStream = SDL_NewAudioStream(AUDIO_S16, 1, 22050, AUDIO_S16, 2, 44100);
    SDL_AudioStreamPut(AudioStream, PCMData, NumFrames * sizeof (Sint16));
    SDL_AudioStreamFlush(AudioStream);
    Music->Size = SDL_AudioStreamAvailable(AudioStream);
    Music->Duration = Music->Size / (44100 * 2 * 2);
    Music->DataPointer = 0;
    Music->Data = malloc(Music->Size);
    SDL_AudioStreamGet(AudioStream,Music->Data,Music->Size);
    SDL_FreeAudioStream(AudioStream);
    free(PCMData);
    return Music;
}
void SoundSystemLoadLevelMusic(SoundSystem_t *SoundSystem,char *MissionPath,int MissionNumber,int LevelNumber,int GameEngine,int AmbientOnly)
{
    VBMusic_t *Music;
    FILE *VBFile;
    char *Buffer;
    int NumLoadedVB;
    
    SoundSystemPause(SoundSystem);
    SoundSystemClearMusicList(SoundSystem);
    NumLoadedVB = 1;
    
    if( GameEngine == MOH_GAME_STANDARD ) {
        if( AmbientOnly ) {
            asprintf(&Buffer,"%s/M%i_%iA.VB",MissionPath,MissionNumber,LevelNumber);
        } else {
            asprintf(&Buffer,"%s/M%i_%i.VB",MissionPath,MissionNumber,LevelNumber);
        }
        VBFile = fopen(Buffer,"rb");
        if( !VBFile ) {
            free(Buffer);
            return;
        }
        Music = SoundSystemLoadVBFile(VBFile);
        DPrintf("Duration:%i:%i\n",Music->Duration/60,Music->Duration % 60);
        Music->Next = SoundSystem->MusicList;
        SoundSystem->MusicList = Music;
        SoundSystem->CurrentMusic = Music;
        fclose(VBFile);
        NumLoadedVB++;
        free(Buffer);
    } else {
        while( 1 ) {
            if( AmbientOnly ) {
                asprintf(&Buffer,"%s/M%i_%i_%iA.VB",MissionPath,MissionNumber,LevelNumber,NumLoadedVB);
            } else {
                asprintf(&Buffer,"%s/M%i_%i_%i.VB",MissionPath,MissionNumber,LevelNumber,NumLoadedVB);
            }
         
            VBFile = fopen(Buffer,"rb");
            if( !VBFile ) {
                free(Buffer);
                break;
            }
            Music = SoundSystemLoadVBFile(VBFile);
    //         DPrintf("Duration:%i\n",Music->Duration);
            Music->Next = SoundSystem->MusicList;
            SoundSystem->MusicList = Music;
            fclose(VBFile);
            NumLoadedVB++;
            free(Buffer);
        }
    }
    SoundSystemPlay(SoundSystem);
}
SoundSystem_t *SoundSystemInit()
{
    SoundSystem_t *SoundSystem;
    SDL_AudioSpec DesiredAudioSpec;
    SDL_AudioSpec ObtainedAudioSpec;
    
    SoundSystem = malloc(sizeof(SoundSystem_t));
    SoundSystem->MusicList = NULL;
    SoundSystem->CurrentMusic = NULL;
    
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
