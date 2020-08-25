/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
    VABWalker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VABWalker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VABWalker.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#include "VABWalker.h"

const double ADPCMFilter[5][2] =
{
    { 0.0, 0.0 },
    { 60.0 / 64.0,  0.0 },
    { 115.0 / 64.0, -52.0 / 64.0 },
    { 98.0 / 64.0, -55.0 / 64.0 },
    { 122.0 / 64.0, -60.0 / 64.0 }
};

typedef struct VagHeader_s {
    int  ID;
    int  Version;
    int  Reserved0;
    int  DataSize;
    int  SamplingFrequency;
    char Reserved1[12];
    char Name[16];
} VagHeader_t;

typedef struct Vag_s {
    VagHeader_t Header;
    short *Data;
    int    NumFrame;
} Vag_t;
typedef struct VabVagOffset_s {
    unsigned int   Offset;
    unsigned short Size;
//     unsigned short Size;
} VabVagOffset_t;

typedef struct VabToneAttribute_s {

    Byte  Priority;
    Byte  Mode;
    Byte  Volume;
    Byte  Pan;
    Byte  CenterNote;
    Byte  Shift;
    Byte  MinNote;
    Byte  MaxNote;
    Byte  VibrateDepth;
    Byte  VibrateDuration;
    Byte  PortamentoDepth;
    Byte  PortamentoDuration;
    Byte  PitchBendMin;
    Byte  PitchBendMax;
    Byte  Reserved0;
    Byte  Reserved1;
    unsigned short Adsr1;
    unsigned short Adsr2;
    short          ParentProgram;
    short          VagReference;
    short          Reserved[4];
} VabToneAttribute_t;

typedef struct VabProgramAttribute_s {
    Byte NumTones;
    Byte Volume;
    Byte Priority;
    Byte Mode;          
    Byte Pan;
    char Reserved0;
    short Attribute;           
    int Reserved1;
    int Reserved2;
} VabProgramAttribute_t;

typedef struct VabHeader_s {
//     int MagicID;
    char MagicID[4]; //Alway VABp or pBAV?
    unsigned int Version;
    unsigned int VABID;
    unsigned int VABSize;
    unsigned short Reserved0;
    unsigned short NumPrograms;
    unsigned short NumTones;
    unsigned short NumVags;
    Byte MasterVolume;
    Byte MasterPan;
    Byte UserDefined1;
    Byte UserDefined2;
    int Reserved1;
} VabHeader_t;

typedef struct VabFile_s {
    VabHeader_t Header;
    VabProgramAttribute_t ProgramAttributeTable[VAB_MAX_PROGRAM_NUMBER];
    VabToneAttribute_t *ToneAttributeTable;
    VabVagOffset_t VagOffsetTable[VAB_MAX_NUM_VAG_OFFSET];
    int BodyStartingPosition; // File position after the VAB header.
    Vag_t *Vag;
} VabFile_t;

typedef struct TafVabFileHeader_s {
    int NumVabs;
    int *VabOffset;
} TafVabFileHeader_t;

// Assuming the Tim Images have already been loaded.
typedef struct TafFile_s {
    TafVabFileHeader_t Header;
    VabFile_t *Vab;
} TafFile_t;


short (*BigShort) ( short s );
short (*LittleShort) ( short s );
int (*BigLong) ( int i );
int (*LittleLong) ( int i );
float (*BigFloat) ( float f );
float (*LittleFloat) ( float f );
int Endian;

void DPrintf(char *Fmt, ...)
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

bool IsBigEndian()
{
    return Endian == 0;
}

bool IsLittleEndian()
{
    return Endian == 1;
}


//Byte swapping.
short ShortSwap(short s)
{
    unsigned char b1,b2;

    b1 =  s & 255;
    b2 = (s << 8) & 255;

    return (b1 << 8) + b2;
}

short ShortNoSwap(short s)
{
    return s;
}
int LongSwap(int i)
{
    unsigned char b1,b2,b3,b4;

    b1 =  i & 255;
    b2 = (i >> 8) & 255;
    b3 = (i >> 16) & 255;
    b4 = (i >> 24) & 255;

    return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

int LongNoSwap(int i)
{
    return i;
}

float FloatSwap(float f)
{
    union{
        float f;
        unsigned char  b[4];
    }dat1,dat2;

    dat1.f = f;
    dat2.b[0] = dat1.b[3];
    dat2.b[1] = dat1.b[2];
    dat2.b[2] = dat1.b[1];
    dat2.b[3] = dat1.b[0];

    return dat2.f;
}

float FloatNoSwap(float f)
{
    return f;
}

void SetupEndianness()
{
    int i = 1;

    if( (*(char*)&i) == 0 ){
        DPrintf("Running on a big-endian system.\n");
        BigShort = ShortNoSwap;
        LittleShort = ShortSwap;
        BigLong = LongNoSwap;
        LittleLong = LongSwap;
        BigFloat = FloatSwap;
        LittleFloat = FloatNoSwap;
        Endian = 0;
    } else {
        DPrintf("Running on a little-endian system.\n");
        BigShort = ShortSwap;
        LittleShort = ShortNoSwap;
        BigLong = LongSwap;
        LittleLong = LongNoSwap;
        BigFloat = FloatSwap;
        LittleFloat = FloatNoSwap;
        Endian = 1;
    }
    return;
}


void CreateDirIfNotExists(char *DirName) {
    struct stat FileStat;

    if (stat(DirName, &FileStat) == -1) {
        mkdir(DirName, 0700);
    }
}

int GetFileLength(FILE *FPointer)
{
    int Length;
    int Curr;

    if ( !FPointer ) {
        return 0;
    }

    Curr = ftell(FPointer);
    fseek(FPointer, 0, SEEK_END);
    Length = ftell(FPointer);
    fseek(FPointer, Curr, SEEK_SET);

    return Length;
}

char *SwitchExt(const char *In, const char *Ext)
{
    char *NewExt;
    int i;
    
    if ( !In || !Ext ) {
        return NULL;
    }

    NewExt = malloc(strlen(In) + 1);

    for ( i = 0; In[i] != '.'; i++ ) {
        NewExt[i] = In[i];
    }
    NewExt[i] = '\0';

    //Now append the extension to the string.
    strncat(NewExt, Ext, strlen(NewExt));
    return NewExt;
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

short *ReadVagFile(FILE *VagFile,int VagWaveSize,int *NumFrames)
{
    Byte Header;
    Byte Shift;
    Byte Predictor;
    Byte Flag;
    int Size;
    Byte Data[14];
    double Sample[28];
//     int  SampleRate;
    int  NumFramePerBlock;
//     float Duration;
    int BaseIndex;
    short *OutBuffer;
    double State[2];
    int  i;
    int Temp;
    int NumWrittenBytes;
    int NumTotalSample;
    int j;
    assert(VagFile);
    
//     SampleRate = 22050;
    State[0] = 0;
    State[1] = 0;
    // In Every block we have 14 bytes of compressed data where each Byte contains 2 compressed sample.
    // Thats why NumFramePerBlock is 28 ( 14 * 2).
    NumFramePerBlock = 28;
    NumTotalSample = VagWaveSize / 16;
    *NumFrames = NumTotalSample * NumFramePerBlock;
    Size = *NumFrames * sizeof(short);
//     Duration = *NumFrames / (float) SampleRate;
    OutBuffer = malloc(Size);
    NumWrittenBytes = 0;
    for( j = 0; j < NumTotalSample; j++ ) {
//     while( 1 ) {
        fread(&Header,sizeof(Header),1,VagFile);
        Predictor = HighNibble(Header);
        Shift = LowNibble(Header);
        fread(&Flag,sizeof(Flag),1,VagFile);
        fread(&Data,sizeof(Data),1,VagFile);
//         if( Flag == 7 ) {
//             break;
//         }
        for( i = 0; i < 14; i++ ) {
            BaseIndex = i * 2;
            Temp = LowNibble(Data[i]) << 12;
            Temp = SignExtend(Temp);    
            Sample[BaseIndex] = (double) ( Temp >> Shift  );
            Temp = HighNibble(Data[i]) << 12;
            Temp = SignExtend(Temp);
            Sample[BaseIndex+1] = (double) ( Temp >> Shift  );
        }
        for( i = 0; i < 28; i++ ) {
            Sample[i] = Sample[i] + State[0] * ADPCMFilter[Predictor][0] + State[1] * ADPCMFilter[Predictor][1];
            State[1] = State[0];
            State[0] = Sample[i];
            OutBuffer[NumWrittenBytes++] = Quantize(Sample[i]);
        }
    }
    return OutBuffer;
}

void VabPrintVagOffsetTable(VabFile_t *Vab)
{
    int i;
    DPrintf(" -- VAB VAG OFFSET TABLE -- \n");
    for( i = 0; i < Vab->Header.NumVags; i++ ) {
        DPrintf("-- VAG %i -- \n",i);
        DPrintf("Offset %u\n",Vab->VagOffsetTable[i].Offset);
        DPrintf("Size %u\n",Vab->VagOffsetTable[i].Size);
    }
}
void VabPrintToneAttributeTable(VabFile_t *Vab)
{
    int i;
    DPrintf(" -- VAB TONE ATTRIBUTE TABLE -- \n");
    for( i = 0; i < VAB_MAX_TONE_NUMBER * Vab->Header.NumPrograms; i++ ) {
        DPrintf(" -- TONE %i --\n",i);
        DPrintf("Priority:%u\n",Vab->ToneAttributeTable[i].Priority);
        DPrintf("Mode:%u\n",Vab->ToneAttributeTable[i].Mode);
        DPrintf("Volume:%u\n",Vab->ToneAttributeTable[i].Volume);
        DPrintf("Pan:%u\n",Vab->ToneAttributeTable[i].Pan);
        DPrintf("Center Note:%u\n",Vab->ToneAttributeTable[i].CenterNote);
        DPrintf("Shift:%u\n",Vab->ToneAttributeTable[i].Shift);
        DPrintf("Min Note:%u\n",Vab->ToneAttributeTable[i].MinNote);
        DPrintf("Max Note:%u\n",Vab->ToneAttributeTable[i].MaxNote);
        DPrintf("Vibrate Depth:%u\n",Vab->ToneAttributeTable[i].VibrateDepth);
        DPrintf("Vibrate Duration:%u\n",Vab->ToneAttributeTable[i].VibrateDuration);
        DPrintf("Portamento Depth:%u\n",Vab->ToneAttributeTable[i].PortamentoDepth);
        DPrintf("Portamento Duration:%u\n",Vab->ToneAttributeTable[i].PortamentoDuration);
        DPrintf("Pitch Bend Min:%u\n",Vab->ToneAttributeTable[i].PitchBendMin);
        DPrintf("Pitch Bend Max:%u\n",Vab->ToneAttributeTable[i].PitchBendMax);
        DPrintf("ParentProgram:%i\n",Vab->ToneAttributeTable[i].ParentProgram);
        DPrintf("VagReference:%i\n",Vab->ToneAttributeTable[i].VagReference);
        DPrintf("Reserved0:%u\n",Vab->ToneAttributeTable[i].Reserved0);
        DPrintf("Reserved1:%u\n",Vab->ToneAttributeTable[i].Reserved1);
        DPrintf("Reserved[4]:%i %i %i %i\n",Vab->ToneAttributeTable[i].Reserved[0],Vab->ToneAttributeTable[i].Reserved[1],
            Vab->ToneAttributeTable[i].Reserved[2],Vab->ToneAttributeTable[i].Reserved[3]
        );
    }
}

void VAbPrintProgramAttributeTable(VabFile_t *Vab)
{
    int i;
    DPrintf(" -- VAB PROGRAM ATTRIBUTE TABLE -- \n");
    for( i = 0; i < VAB_MAX_PROGRAM_NUMBER; i++ ) {
        DPrintf(" -- PROGRAM %i --\n",i);
        DPrintf("Number of Tones:%u\n",Vab->ProgramAttributeTable[i].NumTones);
        DPrintf("Volume:%u\n",Vab->ProgramAttributeTable[i].Volume);
        DPrintf("Priority:%u\n",Vab->ProgramAttributeTable[i].Priority);
        DPrintf("Mode:%u\n",Vab->ProgramAttributeTable[i].Mode);
        DPrintf("Pan:%u\n",Vab->ProgramAttributeTable[i].Pan);
        DPrintf("Attribute:%i\n",Vab->ProgramAttributeTable[i].Attribute);
        DPrintf("Reserved0:%u\n",Vab->ProgramAttributeTable[i].Reserved0);
        DPrintf("Reserved1:%i\n",Vab->ProgramAttributeTable[i].Reserved1);
        DPrintf("Reserved2:%i\n",Vab->ProgramAttributeTable[i].Reserved2);
    }
}

void VabPrintHeader(VabFile_t *Vab)
{
    DPrintf(" -- VAB HEADER -- \n");
    DPrintf("MagicID:%c%c%c%c\n",Vab->Header.MagicID[3],Vab->Header.MagicID[2],Vab->Header.MagicID[1],Vab->Header.MagicID[0]);
    DPrintf("Version:%i\n",Vab->Header.Version);
    DPrintf("VABID:%i\n",Vab->Header.VABID);
    DPrintf("VABSize:%i\n",Vab->Header.VABSize);
    DPrintf("Reserved:%u\n",Vab->Header.Reserved0);
    DPrintf("NumPrograms:%u\n",Vab->Header.NumPrograms);
    DPrintf("NumTones:%u\n",Vab->Header.NumTones);
    DPrintf("NumVags:%u\n",Vab->Header.NumVags);
    DPrintf("MasterVolume:%i\n",Vab->Header.MasterVolume);
    DPrintf("MasterPan:%i\n",Vab->Header.MasterPan);
    DPrintf("UserDefined1:%i\n",Vab->Header.UserDefined1);
    DPrintf("UserDefined2:%i\n",Vab->Header.UserDefined2);
    DPrintf("Reserved1:%i\n",Vab->Header.Reserved1);
}

void VabPrintVagHeader(Vag_t *Vag)
{
    int i;
    DPrintf(" -- VAG HEADER --\n");
    DPrintf("ID:%i\n",Vag->Header.ID);
    DPrintf("Version:%i\n",Vag->Header.Version);
    DPrintf("Reserved:%i\n",Vag->Header.Reserved0);
    DPrintf("Data Size:%i\n",Vag->Header.DataSize);
    DPrintf("Sampling Frequency:%i\n",BigShort(Vag->Header.SamplingFrequency));
    DPrintf("Name:%s\n",Vag->Header.Name);
    for( i = 0; i < 12; i++ ) {
        DPrintf("Reserved1[%i]:%i\n",i,Vag->Header.Reserved1[i]);
    }
}

void VabReadVagFile(Vag_t *Vag,int VagPosition,int VagSize,FILE *InFile)
{
    int VagWaveSize;
    if( !Vag || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VabReadVagFile: Invalid %s.\n",InvalidFile ? "file" : "vab struct");
        return;
    }
    DPrintf("VabReadVagFile:Reading Vag file at %i\n",VagPosition);
//     assert(sizeof(Vag->Header) == 48);
    fseek(InFile,VagPosition,SEEK_SET);
//     fread(&Vag->Header,sizeof(Vag->Header),1,InFile);
//     VabPrintVagHeader(Vag);
    DPrintf("Reading Vag Wave Data at %li\n",ftell(InFile));
    VagWaveSize = VagSize/* - sizeof(Vag->Header)*/;
    Vag->Data = ReadVagFile(InFile,VagWaveSize,&Vag->NumFrame);
}

void VabReadVagFileList(VabFile_t *Vab,FILE *InFile)
{
    int i;
    
    if( !Vab || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VabReadVagFileList: Invalid %s.\n",InvalidFile ? "file" : "vab struct");
        return;
    }
    Vab->Vag = malloc(sizeof(Vag_t) * Vab->Header.NumVags);
    for( i = 0; i < Vab->Header.NumVags; i++ ) {
        if( i == 27 || i == 0 ) {
            DPrintf("Looking at vag %i\n",i);
        }
        DPrintf("Loading Vag %i at %i Size %i\n",i,Vab->VagOffsetTable[i].Offset,Vab->VagOffsetTable[i].Size);
        VabReadVagFile(&Vab->Vag[i],Vab->BodyStartingPosition + Vab->VagOffsetTable[i].Offset,Vab->VagOffsetTable[i].Size,InFile);
    }
}

void VabReadOffsetTable(VabFile_t *Vab,FILE *InFile)
{
    short StartingOffset;
    int i;
    
    if( !Vab || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VabReadOffsetTable: Invalid %s.\n",InvalidFile ? "file" : "vab struct");
        return;
    }
    fread(&StartingOffset,sizeof(StartingOffset),1,InFile);
    for( i = 0; i < VAB_MAX_NUM_VAG_OFFSET - 1; i++ ) {
        fread(&Vab->VagOffsetTable[i].Size,sizeof(Vab->VagOffsetTable[i].Size),1,InFile);
//         if( i == 40 ) {
            DPrintf("Vag%i has real offset(Size):%u\n",i,Vab->VagOffsetTable[i].Size);
//         }
        Vab->VagOffsetTable[i].Size = Vab->VagOffsetTable[i].Size << 3;
        if( i != 0 ) {
            Vab->VagOffsetTable[i].Offset = Vab->VagOffsetTable[i-1].Offset + Vab->VagOffsetTable[i-1].Size;
        } else {
            Vab->VagOffsetTable[i].Offset = StartingOffset;
        }
    }
    VabPrintVagOffsetTable(Vab);
}
void VabReadToneAttributeTable(VabFile_t *Vab,FILE *InFile)
{
    int Result;
    if( !Vab || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VabReadToneAttributeTable: Invalid %s.\n",InvalidFile ? "file" : "vab struct");
        return;
    }
    if( Vab->Header.NumPrograms == 0 ) {
        DPrintf("VabReadToneAttributeTable:No tones specified in header.\n");
        return;
    }
    Vab->ToneAttributeTable = malloc(VAB_MAX_TONE_NUMBER * Vab->Header.NumPrograms * sizeof(VabToneAttribute_t));
    Result = fread(&Vab->ToneAttributeTable,sizeof(Vab->ToneAttributeTable),1,InFile);
    if( Result != 1 ) {
        DPrintf("VabReadToneAttributeTable:Error when reading tone attribute table.\n");
        return;
    }
//     VABPrintToneAttributeTable(Vab);
}

void VabReadProgramAttributeTable(VabFile_t *Vab,FILE *InFile)
{
    int Result;
    if( !Vab || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VabReadProgramAttributeTable: Invalid %s.\n",InvalidFile ? "file" : "vab struct");
        return;
    }
    Result = fread(&Vab->ProgramAttributeTable,sizeof(Vab->ProgramAttributeTable),1,InFile);
    if( Result != 1 ) {
        DPrintf("VabReadProgramAttributeTable:Error when reading program attribute table.\n");
        return;
    }
//     VABPrintProgramAttributeTable(Vab);
}

void VabReadHeader(VabFile_t *Vab,FILE *InFile)
{
    int Result;
    if( !Vab || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VabReadHeader: Invalid %s.\n",InvalidFile ? "file" : "vab struct");
        return;
    }
    Result = fread(&Vab->Header,sizeof(Vab->Header),1,InFile);
    if( Result != 1 ) {
        DPrintf("VabReadHeader:Error when reading header.\n");
        return;
    }
    VabPrintHeader(Vab);
}

void TafPrintHeader(TafFile_t *Taf)
{
    int i;
    if( !Taf ) {
        return;
    }
    DPrintf("TafPrintHeader:Taf has %i Vab file\n",Taf->Header.NumVabs);
    for( i = 0; i < Taf->Header.NumVabs; i++ ) {
        DPrintf("TafPrintHeader:Taf %i at %i\n",i,Taf->Header.VabOffset[i]);
    }
}

void TafReadHeader(TafFile_t *Taf,FILE *InFile)
{
    int Result;
    int i;
    
    if( !Taf || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VabReadHeader: Invalid %s.\n",InvalidFile ? "file" : "vab struct");
        return;
    }
    
    Result = fread(&Taf->Header.NumVabs,sizeof(Taf->Header.NumVabs),1,InFile);
    
    if( Result != 1 ) {
        DPrintf("TafReadHeader:Error when reading header.\n");
        return;
    }
    Taf->Header.VabOffset = malloc(Taf->Header.NumVabs * sizeof(int));
    for( i = 0; i < Taf->Header.NumVabs; i++ ) {
        fread(&Taf->Header.VabOffset[i],sizeof(Taf->Header.VabOffset[i]),1,InFile);
    }
    TafPrintHeader(Taf);
}

VabFile_t *VabLoad(FILE *VabFile)
{
    VabFile_t *Result;

    if( !VabFile ) {
        DPrintf("VabLoad:Invalid file!\n");
        return NULL;
    }
    Result = malloc(sizeof(VabFile_t));
    VabReadHeader(Result,VabFile);
    VabReadProgramAttributeTable(Result,VabFile);
    VabReadToneAttributeTable(Result,VabFile);
    VabReadOffsetTable(Result,VabFile);
    Result->BodyStartingPosition = ftell(VabFile);
    VabReadVagFileList(Result,VabFile);
    return Result;
}

TafFile_t *TafLoad(FILE *TafFile)
{
    TafFile_t *Taf;
    int i;

    if( !TafFile ) {
        DPrintf("TafLoad:Invalid file!\n");
        return NULL;
    }
    Taf = malloc(sizeof(TafFile_t));
    TafReadHeader(Taf,TafFile);
    Taf->Vab = malloc(sizeof(VabFile_t) * Taf->Header.NumVabs);
    for( i = 0; i < Taf->Header.NumVabs; i++ ) {
        fseek(TafFile,Taf->Header.VabOffset[i],SEEK_SET);
        VabReadHeader(&Taf->Vab[i],TafFile);
        VabReadProgramAttributeTable(&Taf->Vab[i],TafFile);
        VabReadToneAttributeTable(&Taf->Vab[i],TafFile);
        VabReadOffsetTable(&Taf->Vab[i],TafFile);
        Taf->Vab[i].BodyStartingPosition = ftell(TafFile);
        VabReadVagFileList(&Taf->Vab[i],TafFile);
    }
//     VabReadHeader(Result,VabFile);
//     VabReadProgramAttributeTable(Result,VabFile);
//     VabReadToneAttributeTable(Result,VabFile);
//     VabReadOffsetTable(Result,VabFile);
//     Result->BodyStartingPosition = ftell(VabFile);
//     VabReadVagFileList(Result,VabFile);
    return Taf;
}

void VabDumpVag(Vag_t Vag,int VabNumber,int VagNumber,char *OutDir)
{
    SF_INFO SoundInfo;
    SNDFILE *SndFile;
    int   SampleRate;    
    char Buffer[256];
    
    SampleRate = 22050;

    CreateDirIfNotExists(OutDir);
        
    SoundInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    SoundInfo.channels = 1;
    SoundInfo.samplerate = SampleRate;

    sprintf(Buffer,"%s/%i_%i.wav",OutDir,VabNumber,VagNumber);
    SndFile = sf_open(Buffer, SFM_WRITE, &SoundInfo);
   
    if (SndFile == NULL) {
        DPrintf("Error opening sound file '%s'\n", Buffer);
        return;
    }
    DPrintf("VabDumpVag:NumFrame is %i\n",Vag.NumFrame);
    sf_writef_short(SndFile, Vag.Data, Vag.NumFrame);
    sf_set_string (SndFile, SF_STR_TITLE, "WavCreator") ;
    sf_write_sync(SndFile);
    sf_close(SndFile);
}
void VabDump(VabFile_t *Vab,char *OutDir)
{
    int i;
    for( i = 0; i < Vab->Header.NumVags; i++ ) {
        VabDumpVag(Vab->Vag[i],0,i,OutDir);
    }
}

void TafDump(TafFile_t *Taf,char *OutDir)
{
    int i;
    int j;
    for( i = 0; i < Taf->Header.NumVabs; i++ ) {
        for( j = 0; j < Taf->Vab[i].Header.NumVags; j++ ) {
            VabDumpVag(Taf->Vab[i].Vag[j],i,j,OutDir);
        }
    }
}

int main(int argc,char **argv)
{
    FILE *InFile;
    VabFile_t  *Vab;
    TafFile_t *Taf;
    Vag_t Vag;
    bool IsTaf;
    bool IsVag;
//     VABFile_t  *Iterator;
//     VABFile_t  *Temp;
//     int NumVabs;
//     int i;
//     char *DirName;
//     char FileName[256];
    
    if( argc != 5 ) {
        DPrintf("%s <Input.vab> <IsTaf> <Output Directory> <IsVag>.\n",argv[0]);
        return -1;
    }

    DPrintf("Loading file %s...\n",argv[1]);
    InFile = fopen(argv[1],"rb");
    
    if( !InFile ) {
        DPrintf("Error opening vab file %s...\n",argv[1]);
        return -1;
    }
    
    SetupEndianness();
    
    IsTaf = atoi(argv[2]);
    IsVag = atoi(argv[4]);
    
    if( IsTaf ) {
        Taf = TafLoad(InFile);
        TafDump(Taf,argv[3]);
    } else if( IsVag ) {
        VabReadVagFile(&Vag,0,GetFileLength(InFile),InFile);
        VabDumpVag(Vag,0,0,argv[3]);
    } else {
        Vab = VabLoad(InFile);
        VabDump(Vab,argv[3]);
        free(Vab);
    }

//     DPrintf("Found %i vab in taf file.\n",NumVabs);

    return 0;
}
