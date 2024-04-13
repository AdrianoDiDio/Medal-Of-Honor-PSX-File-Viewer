// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
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

#include "VAB.h"
void VABFree(VAB_t *VAB)
{
    if( !VAB ) {
        return;
    }
    if( VAB->ToneAttributeTable ) {
        free(VAB->ToneAttributeTable);
    }
    free(VAB);
}

VBMusic_t *VABReadVAGFileList(VAB_t *VAB,int VABNumber,FILE *InFile)
{
    VBMusic_t *List;
    VBMusic_t *Temp;
    char      *TempBuffer;
    float      SamplingFrequency;
    int VABBodyPosition;
    int i;
    if( !VAB || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VABReadVAGFileList: Invalid %s.\n",InvalidFile ? "file" : "VAB struct");
        return NULL;
    }
    List = NULL;
    VABBodyPosition = ftell(InFile);
    for( i = 0; i < VAB->Header.NumVAG; i++ ) {
        fseek(InFile,VABBodyPosition + VAB->VAGOffsetTable[i].Offset,SEEK_SET);
        asprintf(&TempBuffer,"%i_%i",VABNumber,i + 1);
        if( VABNumber >= 3 ) {
            SamplingFrequency = 11025.f;
        } else {
            SamplingFrequency = 22050.f;
        }
        Temp = SoundSystemLoadVBFile(InFile,VAB->VAGOffsetTable[i].Size,TempBuffer,SamplingFrequency);
        SoundSystemAddMusicToList(&List,Temp);
        free(TempBuffer);
    }
    return List;
}

void VABPrintVAGOffsetTable(VAB_t *VAB)
{
    int i;
    if( !VAB ) {
        DPrintf("VABPrintVAGOffsetTable:Invalid VAB\n");
        return;
    }
    DPrintf(" -- VAB VAG OFFSET TABLE -- \n");
    for( i = 0; i < VAB->Header.NumVAG; i++ ) {
        DPrintf("-- VAG %i -- \n",i);
        DPrintf("Offset %u\n",VAB->VAGOffsetTable[i].Offset);
        DPrintf("Size %u\n",VAB->VAGOffsetTable[i].Size);
    }
}

void VABReadOffsetTable(VAB_t *VAB,FILE *InFile)
{
    short StartingOffset;
    int i;
    
    if( !VAB || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VABReadOffsetTable: Invalid %s.\n",InvalidFile ? "file" : "VAB struct");
        return;
    }
    fread(&StartingOffset,sizeof(StartingOffset),1,InFile);
    for( i = 0; i < VAB_MAX_NUM_VAG_OFFSET - 1; i++ ) {
        fread(&VAB->VAGOffsetTable[i].Size,sizeof(VAB->VAGOffsetTable[i].Size),1,InFile);
        VAB->VAGOffsetTable[i].Size = VAB->VAGOffsetTable[i].Size << 3;
        if( i != 0 ) {
            VAB->VAGOffsetTable[i].Offset = VAB->VAGOffsetTable[i-1].Offset + VAB->VAGOffsetTable[i-1].Size;
        } else {
            VAB->VAGOffsetTable[i].Offset = StartingOffset;
        }
    }
    VABPrintVAGOffsetTable(VAB);
}

void VABPrintToneAttributeTable(VAB_t *VAB)
{
    int i;
    if( !VAB ) {
        DPrintf("VABPrintToneAttributeTable:Invalid VAB\n");
        return;
    }
    DPrintf(" -- VAB TONE ATTRIBUTE TABLE -- \n");
    for( i = 0; i < VAB_MAX_TONE_NUMBER * VAB->Header.NumPrograms; i++ ) {
        DPrintf(" -- TONE %i --\n",i);
        DPrintf("Priority:%u\n",VAB->ToneAttributeTable[i].Priority);
        DPrintf("Mode:%u\n",VAB->ToneAttributeTable[i].Mode);
        DPrintf("Volume:%u\n",VAB->ToneAttributeTable[i].Volume);
        DPrintf("Pan:%u\n",VAB->ToneAttributeTable[i].Pan);
        DPrintf("Center Note:%u\n",VAB->ToneAttributeTable[i].CenterNote);
        DPrintf("Shift:%u\n",VAB->ToneAttributeTable[i].Shift);
        DPrintf("Min Note:%u\n",VAB->ToneAttributeTable[i].MinNote);
        DPrintf("Max Note:%u\n",VAB->ToneAttributeTable[i].MaxNote);
        DPrintf("Vibrate Depth:%u\n",VAB->ToneAttributeTable[i].VibrateDepth);
        DPrintf("Vibrate Duration:%u\n",VAB->ToneAttributeTable[i].VibrateDuration);
        DPrintf("Portamento Depth:%u\n",VAB->ToneAttributeTable[i].PortamentoDepth);
        DPrintf("Portamento Duration:%u\n",VAB->ToneAttributeTable[i].PortamentoDuration);
        DPrintf("Pitch Bend Min:%u\n",VAB->ToneAttributeTable[i].PitchBendMin);
        DPrintf("Pitch Bend Max:%u\n",VAB->ToneAttributeTable[i].PitchBendMax);
        DPrintf("ParentProgram:%i\n",VAB->ToneAttributeTable[i].ParentProgram);
        DPrintf("VAGReference:%i\n",VAB->ToneAttributeTable[i].VAGReference);
        DPrintf("Reserved0:%u\n",VAB->ToneAttributeTable[i].Reserved0);
        DPrintf("Reserved1:%u\n",VAB->ToneAttributeTable[i].Reserved1);
        DPrintf("Reserved[4]:%i %i %i %i\n",VAB->ToneAttributeTable[i].Reserved[0],VAB->ToneAttributeTable[i].Reserved[1],
            VAB->ToneAttributeTable[i].Reserved[2],VAB->ToneAttributeTable[i].Reserved[3]
        );
    }
}

void VABReadToneAttributeTable(VAB_t *VAB,FILE *InFile)
{
    int Size;
    int Result;
    if( !VAB || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VABReadToneAttributeTable: Invalid %s.\n",InvalidFile ? "file" : "VAB struct");
        return;
    }
    if( VAB->Header.NumPrograms == 0 ) {
        DPrintf("VABReadToneAttributeTable:No tones specified in header.\n");
        return;
    }
    Size = VAB_MAX_TONE_NUMBER * VAB->Header.NumPrograms * sizeof(VABToneAttribute_t);
    VAB->ToneAttributeTable = malloc(Size);
    Result = fread(VAB->ToneAttributeTable,Size,1,InFile);
    if( Result != 1 ) {
        DPrintf("VABReadToneAttributeTable:Error when reading tone attribute table.\n");
        return;
    }
    VABPrintToneAttributeTable(VAB);
}

void VABPrintProgramAttributeTable(VAB_t *VAB)
{
    int i;
    if( !VAB ) {
        DPrintf("VABPrintProgramAttributeTable:Invalid VAB\n");
        return;
    }
    DPrintf(" -- VAB PROGRAM ATTRIBUTE TABLE -- \n");
    for( i = 0; i < VAB_MAX_PROGRAM_NUMBER; i++ ) {
        DPrintf(" -- PROGRAM %i --\n",i);
        DPrintf("Number of Tones:%u\n",VAB->ProgramAttributeTable[i].NumTones);
        DPrintf("Volume:%u\n",VAB->ProgramAttributeTable[i].Volume);
        DPrintf("Priority:%u\n",VAB->ProgramAttributeTable[i].Priority);
        DPrintf("Mode:%u\n",VAB->ProgramAttributeTable[i].Mode);
        DPrintf("Pan:%u\n",VAB->ProgramAttributeTable[i].Pan);
        DPrintf("Attribute:%i\n",VAB->ProgramAttributeTable[i].Attribute);
        DPrintf("Reserved0:%u\n",VAB->ProgramAttributeTable[i].Reserved0);
        DPrintf("Reserved1:%i\n",VAB->ProgramAttributeTable[i].Reserved1);
        DPrintf("Reserved2:%i\n",VAB->ProgramAttributeTable[i].Reserved2);
    }
}

void VABReadProgramAttributeTable(VAB_t *VAB,FILE *InFile)
{
    int Result;
    if( !VAB || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VABReadProgramAttributeTable: Invalid %s.\n",InvalidFile ? "file" : "VAB struct");
        return;
    }
    Result = fread(&VAB->ProgramAttributeTable,sizeof(VAB->ProgramAttributeTable),1,InFile);
    if( Result != 1 ) {
        DPrintf("VABReadProgramAttributeTable:Error when reading program attribute table.\n");
        return;
    }
    VABPrintProgramAttributeTable(VAB);
}

void VABPrintHeader(VAB_t *VAB)
{
    if( !VAB ) {
        DPrintf("VABPrintHeader:Invalid VAB\n");
        return;
    }
    DPrintf(" -- VAB HEADER -- \n");
    DPrintf("MagicID:%c%c%c%c\n",VAB->Header.MagicID[3],VAB->Header.MagicID[2],VAB->Header.MagicID[1],VAB->Header.MagicID[0]);
    DPrintf("Version:%i\n",VAB->Header.Version);
    DPrintf("VABID:%i\n",VAB->Header.VABID);
    DPrintf("VABSize:%i\n",VAB->Header.VABSize);
    DPrintf("Reserved:%u\n",VAB->Header.Reserved0);
    DPrintf("NumPrograms:%u\n",VAB->Header.NumPrograms);
    DPrintf("NumTones:%u\n",VAB->Header.NumTones);
    DPrintf("NumVAGs:%u\n",VAB->Header.NumVAG);
    DPrintf("MasterVolume:%i\n",VAB->Header.MasterVolume);
    DPrintf("MasterPan:%i\n",VAB->Header.MasterPan);
    DPrintf("UserDefined1:%i\n",VAB->Header.UserDefined1);
    DPrintf("UserDefined2:%i\n",VAB->Header.UserDefined2);
    DPrintf("Reserved1:%i\n",VAB->Header.Reserved1);
}

void VABReadHeader(VAB_t *VAB,FILE *InFile)
{
    int Result;
    if( !VAB || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("VABReadHeader: Invalid %s.\n",InvalidFile ? "file" : "VAB struct");
        return;
    }
    DPrintf("Reading header at %li\n",ftell(InFile));
    Result = fread(&VAB->Header,sizeof(VAB->Header),1,InFile);
    if( Result != 1 ) {
        DPrintf("VABReadHeader:Error when reading header.\n");
        return;
    }
    VABPrintHeader(VAB);
}

VBMusic_t *VABLoadAllVAGs(FILE *VABFile,int VABNumber,int VABOffset)
{
    VAB_t *VAB;
    VBMusic_t *VAGList;
    int PrevPosition;
    if( !VABFile ) {
        DPrintf("VABLoadFile:Invalid File\n");
        return NULL;
    }
    if( VABOffset != -1 ) {
        PrevPosition = ftell(VABFile);
        fseek(VABFile,VABOffset,SEEK_SET);
    }
    VAB = malloc(sizeof(VAB_t));
    if( !VAB ) {
        DPrintf("VABLoadFile:Failed to allocate memory for VAB struct\n");
        return NULL;
    }
    VAB->ToneAttributeTable = NULL;
    VABReadHeader(VAB,VABFile);
    VABReadProgramAttributeTable(VAB,VABFile);
    VABReadToneAttributeTable(VAB,VABFile);
    VABReadOffsetTable(VAB,VABFile);
    VAGList = VABReadVAGFileList(VAB,VABNumber,VABFile);

    if( VABOffset != -1 ) {
        fseek(VABFile,PrevPosition,SEEK_SET);
    }
    VABFree(VAB);
    return VAGList;
}
