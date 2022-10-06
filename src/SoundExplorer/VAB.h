/*
===========================================================================
    Copyright (C) 2018-2022 Adriano Di Dio.
    
    SoundExplorer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SoundExplorer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SoundExplorer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#ifndef __VAB_H_
#define __VAB_H_

#include "../Common/Common.h"
#include "../Common/Sound.h"

#define VAB_MAX_PROGRAM_NUMBER 128
#define VAB_MAX_TONE_NUMBER 16
#define VAB_MAX_NUM_VAG_OFFSET 256
#define VAB_MAX_NUM_VAG 256

typedef struct VAGHeader_s {
    int  ID;
    int  Version;
    int  Reserved0;
    int  DataSize;
    int  SamplingFrequency;
    char Reserved1[12];
    char Name[16];
} VAGHeader_t;

typedef struct VABVAGOffset_s {
    unsigned int   Offset;
    unsigned short Size;
} VABVAGOffset_t;

typedef struct VABToneAttribute_s {

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
    short          VAGReference;
    short          Reserved[4];
} VABToneAttribute_t;

typedef struct VABProgramAttribute_s {
    Byte NumTones;
    Byte Volume;
    Byte Priority;
    Byte Mode;          
    Byte Pan;
    char Reserved0;
    short Attribute;           
    int Reserved1;
    int Reserved2;
} VABProgramAttribute_t;

typedef struct VABHeader_s {
//     int MagicID;
    char MagicID[4]; //Alway VABp or pBAV?
    unsigned int Version;
    unsigned int VABID;
    unsigned int VABSize;
    unsigned short Reserved0;
    unsigned short NumPrograms;
    unsigned short NumTones;
    unsigned short NumVAG;
    Byte MasterVolume;
    Byte MasterPan;
    Byte UserDefined1;
    Byte UserDefined2;
    int Reserved1;
} VABHeader_t;

typedef struct VAB_s {
    VABHeader_t Header;
    VABProgramAttribute_t ProgramAttributeTable[VAB_MAX_PROGRAM_NUMBER];
    VABToneAttribute_t *ToneAttributeTable;
    VABVAGOffset_t VAGOffsetTable[VAB_MAX_NUM_VAG_OFFSET];
} VAB_t;

VBMusic_t   *VABLoadAllVAGs(FILE *VABFile,int VABNumber,int VABOffset);
void        VABFree(VAB_t *VAB);

#endif//__VAB_H_
