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
#include "Sound.h"

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
        free(MusicList->OriginalData);
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
    free(SoundSystem);
}

void SoundSystemPause(SoundSystem_t *SoundSystem)
{
    SDL_PauseAudioDevice(SoundSystem->Device, 1);
}
void SoundSystemPlay(SoundSystem_t *SoundSystem)
{
    SDL_PauseAudioDevice(SoundSystem->Device, 0);
}
int SoundSystemIsPaused(SoundSystem_t *SoundSystem)
{
    return SDL_GetAudioDeviceStatus(SoundSystem->Device) == SDL_AUDIO_PAUSED;
}
void SoundSystemLockDevice(SoundSystem_t *SoundSystem)
{
    SDL_LockAudioDevice(SoundSystem->Device);
}
void SoundSystemUnlockDevice(SoundSystem_t *SoundSystem)
{
    SDL_UnlockAudioDevice(SoundSystem->Device);
}
float SoundSystemQuantize(double Sample)
{
    int OutSample;
    OutSample = (int ) (Sample + 0.5);
    if ( OutSample < -32768 ) {
        return -1.f;
    }
    if ( OutSample > 32767 ) {
        return 1.f;
    }
    
    return (float) Sample / 32768.f;
}
int SoundSystemGetByteRate(int Frequency,int NumChannels,int BitDepth)
{
    return (Frequency * NumChannels * (BitDepth / 8));
}
int SoundSystemCalculateSoundDuration(int Size,int Frequency,int NumChannels,int BitDepth)
{
    return Size / SoundSystemGetByteRate(Frequency,NumChannels,BitDepth);
}
int SoundSystemGetSoundDuration(VBMusic_t *Music,int *Minutes,int *Seconds)
{
    if( !Music ) {
        return 0;
    }
    *Minutes = Music->Duration / 60;
    *Seconds = Music->Duration % 60;
    return Music->Duration;
}

int SoundSystemGetCurrentSoundTime(VBMusic_t *Music,int *Minutes,int *Seconds)
{
    int CurrentLength;
    if( !Music ) {
        return 0;
    }
    CurrentLength = SoundSystemCalculateSoundDuration(Music->DataPointer,SOUND_SYSTEM_FREQUENCY,SOUND_SYSTEM_NUM_CHANNELS,32);
    *Minutes = CurrentLength / 60;
    *Seconds = CurrentLength % 60;
    return CurrentLength;
}
/*
 * Convert ADPCM data to PCM.
 * Length can be -1 to indicate that the length is the whole file size or a
 * number of bytes containing the ADPCM data.
 */
float *SoundSystemConvertADPCMToPCM(FILE *VBFile,int Length,int *NumFrames)
{
    float *Result;
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
    int LocalNumFrames;
    int Size;
    int  i;
    int  j;
    int  n;
    
    if( !VBFile ) {
        DPrintf("SoundSystemConvertADPCMToPCM:Invalid file\n");
        return NULL;
    }

    VBFileSize = Length == -1 ? GetFileLength(VBFile) : Length;
    DPrintf("SoundSystemConvertADPCMToPCM:Length %i\n",VBFileSize);
    State[0] = 0;
    State[1] = 0;
    // In Every block we have 14 bytes of compressed data where each Byte contains 2 compressed sample.
    // Thats why NumFramePerBlock is 28 ( 14 * 2).
    NumFramePerBlock = 28;
    NumTotalSample = VBFileSize / 16;
    LocalNumFrames = NumTotalSample * NumFramePerBlock;
    //Size depends from the number of channels (in this case is 2).
    Size = LocalNumFrames * 2 * sizeof(float);
    float samples = NumTotalSample / 2 / ( 16 / 8 );
    
    if( NumFrames ) {
        *NumFrames = LocalNumFrames;
    }
    Result = malloc(Size);
    if( !Result ) {
        DPrintf("SoundSystemConvertADPCMToPCM:Couldn't allocate memory for PCM data\n");
        return NULL;
    }
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
                //Duplicate the audio for both channels...
                Result[NumWrittenBytes++] = SoundSystemQuantize(Sample[BaseIndex]);
                Result[NumWrittenBytes++] = SoundSystemQuantize(Sample[BaseIndex]);
            }
        }
    }
    return Result;
}
/*
 * Given an input buffer containing raw PCM data sampled using the given frequency, resample
 * it to the system frequency, returning a new buffer with the new data at the desired frequency.
 */
float *SoundSystemResample(float *InBuffer,int NumFrames,float Frequency,int *OutFrames)
{
    float *OutBuffer;
    float SamplingRatio;
    int NumChannels;
    SRC_DATA ConverterSrcData;
    
    SamplingRatio = (float) SOUND_SYSTEM_FREQUENCY / Frequency;
    NumChannels = 2;
    
    OutBuffer = malloc( NumFrames * SamplingRatio * NumChannels * sizeof(float));
    ConverterSrcData.data_in = InBuffer;
    ConverterSrcData.input_frames = NumFrames;
    ConverterSrcData.data_out = OutBuffer;
    ConverterSrcData.output_frames = NumFrames * SamplingRatio;
    ConverterSrcData.src_ratio = SamplingRatio;
    int ResamplingStatus = src_simple(&ConverterSrcData, SRC_LINEAR, NumChannels);
    if( ResamplingStatus != 0) {
        DPrintf("SoundSystemResample:Failed to resample audio %s.\n",src_strerror(ResamplingStatus));
        return NULL;
    }
    if( OutFrames ) {
        *OutFrames = ConverterSrcData.output_frames;
    }
    return OutBuffer;
}
int SoundSystemResampleMusic(VBMusic_t *Music,float DesiredFrequency)
{
    float *ConvertedData;
    int    ConvertedOutFrames;
    if( !Music ) {
        return 0;
    }
    if( Music->Frequency == DesiredFrequency ) {
        return 0;
    }
    ConvertedData = SoundSystemResample(Music->OriginalData,Music->OriginalNumFrames,DesiredFrequency,&ConvertedOutFrames);
    if( !ConvertedData ) {
        return 0;
    }
    free(Music->Data);
    Music->Size = ConvertedOutFrames * 2 * sizeof(float);
    Music->NumFrames = ConvertedOutFrames;
    Music->Duration = SoundSystemCalculateSoundDuration(Music->Size,SOUND_SYSTEM_FREQUENCY,SOUND_SYSTEM_NUM_CHANNELS,32);
    Music->DataPointer = 0;
    Music->Data = (Byte *) ConvertedData;
    Music->Frequency = DesiredFrequency;
    return 1;
}
void SoundSystemDumpMusicToWav(VBMusic_t *Music,const char *EngineName,const char *OutDirectory)
{
    WAVHeader_t WAVHeader;
    FILE *WAVFile;
    char *VBFileName;
    char *WAVFileName;
    const char *CurrentExt;
    
    if( !Music ) {
        DPrintf("SoundSystemDumpMusicToWav:Invalid Music data\n");
        return;
    }
    DPrintf("Switching %s\n",Music->Name);
    CurrentExt = GetFileExtension(Music->Name);
    if( CurrentExt != NULL ) {
        VBFileName = SwitchExt(Music->Name, ".wav");
    } else {
        VBFileName = StringAppend(Music->Name,".wav");
    }
    DPrintf("Switched %s\n",VBFileName);
    if( EngineName ) {
        asprintf(&WAVFileName,"%s%c%s-%s",OutDirectory,PATH_SEPARATOR,EngineName,VBFileName);
    } else {
        asprintf(&WAVFileName,"%s%c%s",OutDirectory,PATH_SEPARATOR,VBFileName);
    }
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
    WAVHeader.AudioFormat = 3; // 32 bits floating point sample.
    WAVHeader.NumChannels = 2;
    WAVHeader.SampleRate = 44100;
    WAVHeader.ByteRate = SoundSystemGetByteRate(44100,2,32);
    WAVHeader.BlockAlign = (2 * (32/8));
    WAVHeader.BitsPerSample = 32;
    
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

/*
 * NOTE(Adriano):
 * VB files are used to store level music and effects.
 * They contains raw ADPCM data that gets sent to the SPU of the PSX.
 * They use a sample frequency of 22050Hz for all the effects and music, while the voices are sampled at a frequency
 * of 11025Hz, and contains only one channel (Mono).
 * In order to load it we need to first convert the ADPCM data to standard PCM and then
 * feed it to SDL (doing an internal conversion to change the frequency) in order to be able to play it.
 */
VBMusic_t *SoundSystemLoadVBFile(FILE *VBFile,int Length,const char *VBFileName,float InFrequency)
{
    VBMusic_t *Music;
    float  *PCMData;
    float  *ConvertedData;
    int    NumFrames;
    float SamplingRatio;
    int   ResamplingStatus;
    int   NumChannels;
    int OutFrames;
    SRC_DATA ConverterSrcData;
    
    if( !VBFile ) {
        return NULL;
    }
    
    PCMData = SoundSystemConvertADPCMToPCM(VBFile,Length,&NumFrames);
    if( !PCMData ) {
        DPrintf("SoundSystemLoadVBFile:Failed to convert ADPCM to PCM\n");
        return NULL;
    }
    NumChannels = 2;
    ConvertedData = SoundSystemResample(PCMData,NumFrames,InFrequency,&OutFrames);

    if( !ConvertedData) {
        free(PCMData);
        return NULL;
    }
    Music = malloc(sizeof(VBMusic_t));
    if( !Music ) {
        DPrintf("SoundSystemLoadVBFile:Failed to allocate memory for music data\n");
        free(PCMData);
        return NULL;
    }
    Music->Name = GetBaseName(VBFileName);
    Music->Size = OutFrames * NumChannels * sizeof(float);
    Music->NumFrames = OutFrames;
    Music->Duration = SoundSystemCalculateSoundDuration(Music->Size,SOUND_SYSTEM_FREQUENCY,SOUND_SYSTEM_NUM_CHANNELS,32);
    Music->DataPointer = 0;
    Music->OriginalData = PCMData;
    Music->Data = (Byte *) ConvertedData;
    Music->OriginalNumFrames = NumFrames;
    Music->Next = NULL;
    Music->Frequency = InFrequency;
    return Music;
}

VBMusic_t *SoundSystemLoadVBMusic(const char *VBFileName,int Length)
{
    FILE *VBFile;
    VBMusic_t *Result;
    if( !VBFileName ) {
        DPrintf("SoundSystemLoadVBMusic:Invalid file\n");
        return NULL;
    }
    VBFile = fopen(VBFileName,"rb");
    if( !VBFile ) {
        return NULL;
    }
    //NOTE(Adriano):Music is always guaranteed to have a sampling frequency of 22050Hz.
    Result = SoundSystemLoadVBFile(VBFile,Length,VBFileName,22050.f);
    fclose(VBFile);
    return Result;
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

SoundSystem_t *SoundSystemInit(SDL_AudioCallback Callback,void *UserData)
{
    SoundSystem_t *SoundSystem;
    SDL_AudioSpec DesiredAudioSpec;
    SDL_AudioSpec ObtainedAudioSpec;
    
    SoundSystem = malloc(sizeof(SoundSystem_t));

    if( !SoundSystem ) {
        DPrintf("SoundSystemInit:Failed to initialize sound system.\n");
        return NULL;
    }

    SoundVolume = ConfigGet("SoundVolume");
    
    SDL_zero(DesiredAudioSpec);
    DesiredAudioSpec.freq = SOUND_SYSTEM_FREQUENCY;
    DesiredAudioSpec.format = SOUND_SYSTEM_BUFFER_FORMAT;
    DesiredAudioSpec.channels = SOUND_SYSTEM_NUM_CHANNELS;
    DesiredAudioSpec.samples = SOUND_SYSTEM_NUM_SAMPLES;
    DesiredAudioSpec.callback = Callback;
    DesiredAudioSpec.userdata = UserData;
    
    SoundSystem->Device = SDL_OpenAudioDevice(NULL,0,&DesiredAudioSpec, &ObtainedAudioSpec,0);
    
    if( ObtainedAudioSpec.freq != DesiredAudioSpec.freq || ObtainedAudioSpec.channels != DesiredAudioSpec.channels || 
        ObtainedAudioSpec.samples != DesiredAudioSpec.samples
    ) {
        printf("SoundSystemInit:Wanted a frequency of %i with %i samples and %i channels...\n",DesiredAudioSpec.freq,DesiredAudioSpec.samples,
                DesiredAudioSpec.channels);
        printf("...Obtained a frequency of %i with %i samples and %i channels.\n",ObtainedAudioSpec.freq,ObtainedAudioSpec.samples,
                ObtainedAudioSpec.channels);
        SDL_CloseAudioDevice(SoundSystem->Device);
        free(SoundSystem);
        return NULL;
    }
    return SoundSystem;
}
