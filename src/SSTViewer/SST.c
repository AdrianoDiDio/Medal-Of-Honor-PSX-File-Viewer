// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    SSTViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SSTViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SSTViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/
#include "SST.h"
#include "../Common/RSC.h"
#include "../Common/TIM.h"
#include "SSTViewer.h" 

const SSTRSCMap_t MOHSSTRSCMap[] = {
    {
        "m_brief",
        28,
        (char*[]) {
            "SCR2/MBRIEF/M1/1.RSC",
            "SCR2/MBRIEF/M1/2.RSC",
            "SCR2/MBRIEF/M1/3.RSC",
            "SCR2/MBRIEF/M2/1.RSC",
            "SCR2/MBRIEF/M2/2.RSC",
            "SCR2/MBRIEF/M2/3.RSC",
            "SCR2/MBRIEF/M2/4.RSC",
            "SCR2/MBRIEF/M3/1.RSC",
            "SCR2/MBRIEF/M3/2.RSC",
            "SCR2/MBRIEF/M3/3.RSC",
            "SCR2/MBRIEF/M3/4.RSC",
            "SCR2/MBRIEF/M4/1.RSC",
            "SCR2/MBRIEF/M4/2.RSC",
            "SCR2/MBRIEF/M4/3.RSC",
            "SCR2/MBRIEF/M5/1.RSC",
            "SCR2/MBRIEF/M5/2.RSC",
            "SCR2/MBRIEF/M5/3.RSC",
            "SCR2/MBRIEF/M5/4.RSC",
            "SCR2/MBRIEF/M6/1.RSC",
            "SCR2/MBRIEF/M6/2.RSC",
            "SCR2/MBRIEF/M6/3.RSC",
            "SCR2/MBRIEF/M7/1.RSC",
            "SCR2/MBRIEF/M7/2.RSC",
            "SCR2/MBRIEF/M7/3.RSC",
            "SCR2/MBRIEF/M8/1.RSC",
            "SCR2/MBRIEF/M8/2.RSC",
            "SCR2/MBRIEF/M8/3.RSC",
            "SCR2/MBRIEF/M8/4.RSC",
        }
    },
    {
        "m_calib",
        1,
        (char*[]) {
            "SCR1/MCALIB.RSC"
        }
    },
    {
        "m_config",
        1,
        (char*[]) {
            "SCR1/MCONFIG.RSC"
        }
    },
    {
        "m_ctrl",
        1,
        (char*[]) {
        "SCR1/MCTRL.RSC"
        }
    },
    {
        "m_dev",
        1,
        (char*[]) {
        "SCR1/MDEV.RSC"
        }
    },
    {
        "m_gallery",
        1,
        (char*[]) {
            "SCR1/MGAL.RSC"
        }
    },
    {
        "m_legal",
        1,
        (char*[]) {
            "SCR1/MLEGAL.RSC"
        }
    },
    {
        "m_load",
        1,
        (char*[]) {
            "SCR1/MLOAD.RSC"
        }
    },
    {
        "m_mission",
                1,
        (char*[]) {
        "SCR1/MMISSION.RSC"
        }
    },
    {
        "m_option",
        1,
        (char*[]) {
            "SCR1/MOPTION.RSC"
        }
    },
    {
        "m_play",
        1,
        (char*[]) {
            "SCR1/MPLAY.RSC"
        }
    },
    {
        "m_save",
        1,
        (char*[]) {
            "SCR1/MSAVE.RSC"
        }
    },
    {
        "m_slide",
        80,
        (char*[]) {
            "SCR3/MSLIDE/M1/1.RSC",
            "SCR3/MSLIDE/M1/2.RSC",
            "SCR3/MSLIDE/M1/3.RSC",
            "SCR3/MSLIDE/M1/4.RSC",
            "SCR3/MSLIDE/M1/5.RSC",
            "SCR3/MSLIDE/M1/6.RSC",
            "SCR3/MSLIDE/M1/7.RSC",
            "SCR3/MSLIDE/M1/8.RSC",
            "SCR3/MSLIDE/M1/9.RSC",
            "SCR3/MSLIDE/M1/10.RSC",
            "SCR3/MSLIDE/M2/1.RSC",
            "SCR3/MSLIDE/M2/2.RSC",
            "SCR3/MSLIDE/M2/3.RSC",
            "SCR3/MSLIDE/M2/4.RSC",
            "SCR3/MSLIDE/M2/5.RSC",
            "SCR3/MSLIDE/M2/6.RSC",
            "SCR3/MSLIDE/M2/7.RSC",
            "SCR3/MSLIDE/M2/8.RSC",
            "SCR3/MSLIDE/M2/9.RSC",
            "SCR3/MSLIDE/M2/10.RSC",
            "SCR3/MSLIDE/M3/1.RSC",
            "SCR3/MSLIDE/M3/2.RSC",
            "SCR3/MSLIDE/M3/3.RSC",
            "SCR3/MSLIDE/M3/4.RSC",
            "SCR3/MSLIDE/M3/5.RSC",
            "SCR3/MSLIDE/M3/6.RSC",
            "SCR3/MSLIDE/M3/7.RSC",
            "SCR3/MSLIDE/M3/8.RSC",
            "SCR3/MSLIDE/M3/9.RSC",
            "SCR3/MSLIDE/M3/10.RSC",
            "SCR3/MSLIDE/M4/1.RSC",
            "SCR3/MSLIDE/M4/2.RSC",
            "SCR3/MSLIDE/M4/3.RSC",
            "SCR3/MSLIDE/M4/4.RSC",
            "SCR3/MSLIDE/M4/5.RSC",
            "SCR3/MSLIDE/M4/6.RSC",
            "SCR3/MSLIDE/M4/7.RSC",
            "SCR3/MSLIDE/M4/8.RSC",
            "SCR3/MSLIDE/M4/9.RSC",
            "SCR3/MSLIDE/M4/10.RSC",
            "SCR3/MSLIDE/M5/1.RSC",
            "SCR3/MSLIDE/M5/2.RSC",
            "SCR3/MSLIDE/M5/3.RSC",
            "SCR3/MSLIDE/M5/4.RSC",
            "SCR3/MSLIDE/M5/5.RSC",
            "SCR3/MSLIDE/M5/6.RSC",
            "SCR3/MSLIDE/M5/7.RSC",
            "SCR3/MSLIDE/M5/8.RSC",
            "SCR3/MSLIDE/M5/9.RSC",
            "SCR3/MSLIDE/M5/10.RSC",
            "SCR3/MSLIDE/M6/1.RSC",
            "SCR3/MSLIDE/M6/2.RSC",
            "SCR3/MSLIDE/M6/3.RSC",
            "SCR3/MSLIDE/M6/4.RSC",
            "SCR3/MSLIDE/M6/5.RSC",
            "SCR3/MSLIDE/M6/6.RSC",
            "SCR3/MSLIDE/M6/7.RSC",
            "SCR3/MSLIDE/M6/8.RSC",
            "SCR3/MSLIDE/M6/9.RSC",
            "SCR3/MSLIDE/M6/10.RSC",
            "SCR3/MSLIDE/M7/1.RSC",
            "SCR3/MSLIDE/M7/2.RSC",
            "SCR3/MSLIDE/M7/3.RSC",
            "SCR3/MSLIDE/M7/4.RSC",
            "SCR3/MSLIDE/M7/5.RSC",
            "SCR3/MSLIDE/M7/6.RSC",
            "SCR3/MSLIDE/M7/7.RSC",
            "SCR3/MSLIDE/M7/8.RSC",
            "SCR3/MSLIDE/M7/9.RSC",
            "SCR3/MSLIDE/M7/10.RSC",
            "SCR3/MSLIDE/M8/1.RSC",
            "SCR3/MSLIDE/M8/2.RSC",
            "SCR3/MSLIDE/M8/3.RSC",
            "SCR3/MSLIDE/M8/4.RSC",
            "SCR3/MSLIDE/M8/5.RSC",
            "SCR3/MSLIDE/M8/6.RSC",
            "SCR3/MSLIDE/M8/7.RSC",
            "SCR3/MSLIDE/M8/8.RSC",
            "SCR3/MSLIDE/M8/9.RSC",
            "SCR3/MSLIDE/M8/10.RSC",
        }
    },
    {
        "m_title",
        2,
        (char*[]) {
            "SCR1/MTITLE1.RSC",
            "SCR1/MTITLE2.RSC"
        }
    },
    {
        "m_war",
        1,
        (char*[]) {
            "SCR1/MWAR.RSC"
        }
    },
    {
        "m_audio",
        1,
        (char*[]) {
            "SCR2/MAUDIO.RSC"
        }
    },
    {
        "m_commend",
        1,
        (char*[]) {
            "SCR2/MCOMMEND.RSC"
        }
    },
    {
        "m_dev2",
        1,
        (char*[]) {
            "SCR2/MDEV.RSC"
        }
    },
    {
        "m_enigma",
        1,
        (char*[]) {
            "SCR2/MENIGMA.RSC"
        }
    },
    {
        "m_eval",
        1,
        (char*[]) {
            "SCR2/MEVAL.RSC"
        }
    },
    {
        "m_medals",
        1,
        (char*[]) {
            "SCR2/MMEDALS.RSC"
        }
    },
    {
        "m_multopt",
        1,
        (char*[]) {
            "SCR2/MMULTOPT.RSC"
        }
    },
    {
        "m_pmeval",                
        1,
        (char*[]) {
            "SCR2/MPMEVAL.RSC"
        }
    },
    {
        "m_pmeval2",
        1,
        (char*[]) {
            "SCR2/MPMEVAL2.RSC"
        }
    },
    {
        "m_pname",
        1,
        (char*[]) {
            "SCR2/MPNAME.RSC"
        }
    },
    {
        "m_postmis",
        1,
        (char*[]) {
            "SCR2/MPOSTMIS.RSC"
        }
    },
    {
        "m_secret",
        1,
        (char*[]) {
            "SCR2/MSECRET.RSC"
        }
    },
    {
        "m_uniform",
        1,
        (char*[]) {
            "SCR2/MUNIF.RSC"
        }
    },
    {
        "m_video",
        1,
        (char*[]) {
            "SCR2/MVIDEO.RSC"
        }
    }
};
int NumMOHSSTRSCMapEntry = sizeof(MOHSSTRSCMap) / sizeof(MOHSSTRSCMap[0]);

const SSTRSCMap_t MOHUndergroundSSTRSCMap[] = {
    {
        "m_brief",
        28,
        (char*[]) {
            "SCR2/MBRIEF/M1/1.RSC",
            "SCR2/MBRIEF/M1/2.RSC",
            "SCR2/MBRIEF/M1/3.RSC",
            "SCR2/MBRIEF/M2/1.RSC",
            "SCR2/MBRIEF/M2/2.RSC",
            "SCR2/MBRIEF/M2/3.RSC",
            "SCR2/MBRIEF/M2/4.RSC",
            "SCR2/MBRIEF/M3/1.RSC",
            "SCR2/MBRIEF/M3/2.RSC",
            "SCR2/MBRIEF/M3/3.RSC",
            "SCR2/MBRIEF/M3/4.RSC",
            "SCR2/MBRIEF/M4/1.RSC",
            "SCR2/MBRIEF/M4/2.RSC",
            "SCR2/MBRIEF/M4/3.RSC",
            "SCR2/MBRIEF/M5/1.RSC",
            "SCR2/MBRIEF/M5/2.RSC",
            "SCR2/MBRIEF/M5/3.RSC",
            "SCR2/MBRIEF/M5/4.RSC",
            "SCR2/MBRIEF/M6/1.RSC",
            "SCR2/MBRIEF/M6/2.RSC",
            "SCR2/MBRIEF/M6/3.RSC",
            "SCR2/MBRIEF/M7/1.RSC",
            "SCR2/MBRIEF/M7/2.RSC",
            "SCR2/MBRIEF/M7/3.RSC",
            "SCR2/MBRIEF/M7/4.RSC",
            "SCR2/MBRIEF/M8/1.RSC",
            "SCR2/MBRIEF/M8/2.RSC",
            "SCR2/MBRIEF/M8/3.RSC",
        }
    },
    {
        "m_calib",
        1,
        (char*[]) {
            "SCR1/MCALIB.RSC"
        }
    },
    {
        "m_config",
        1,
        (char*[]) {
            "SCR1/MCONFIG.RSC"
        }
    },
    {
        "m_ctrl",
        1,
        (char*[]) {
        "SCR1/MCTRL.RSC"
        }
    },
    {
        "m_dev",
        1,
        (char*[]) {
        "SCR1/MDEV.RSC"
        }
    },
    {
        "m_diff",
        1,
        (char*[]) {
        "SCR1/MDIFF.RSC"
        }
    },
    {
        "m_gallery",
        1,
        (char*[]) {
            "SCR1/MGAL.RSC"
        }
    },
    {
        "m_legal",
        1,
        (char*[]) {
            "SCR1/MLEGAL.RSC"
        }
    },
    {
        "m_load",
        1,
        (char*[]) {
            "SCR1/MLOAD.RSC"
        }
    },
    {
        "m_mission",
                1,
        (char*[]) {
        "SCR1/MMISSION.RSC"
        }
    },
    {
        "m_option",
        1,
        (char*[]) {
            "SCR1/MOPTION.RSC"
        }
    },
    {
        "m_play",
        1,
        (char*[]) {
            "SCR1/MPLAY.RSC"
        }
    },
    {
        "m_save",
        1,
        (char*[]) {
            "SCR1/MSAVE.RSC"
        }
    },
    {
        "m_slide",
        70,
        (char*[]) {
            "SCR3/MSLIDE/M1/F01.RSC",
            "SCR3/MSLIDE/M1/F02.RSC",
            "SCR3/MSLIDE/M1/F03.RSC",
            "SCR3/MSLIDE/M1/F04.RSC",
            "SCR3/MSLIDE/M1/F05.RSC",
            "SCR3/MSLIDE/M1/F06.RSC",
            "SCR3/MSLIDE/M1/F07.RSC",
            "SCR3/MSLIDE/M1/F08.RSC",
            "SCR3/MSLIDE/M1/F09.RSC",
            "SCR3/MSLIDE/M1/F10.RSC",
            "SCR3/MSLIDE/M2/F01.RSC",
            "SCR3/MSLIDE/M2/F02.RSC",
            "SCR3/MSLIDE/M2/F03.RSC",
            "SCR3/MSLIDE/M2/F04.RSC",
            "SCR3/MSLIDE/M2/F05.RSC",
            "SCR3/MSLIDE/M2/F06.RSC",
            "SCR3/MSLIDE/M2/F07.RSC",
            "SCR3/MSLIDE/M2/F08.RSC",
            "SCR3/MSLIDE/M2/F09.RSC",
            "SCR3/MSLIDE/M2/F10.RSC",
            "SCR3/MSLIDE/M3/F01.RSC",
            "SCR3/MSLIDE/M3/F02.RSC",
            "SCR3/MSLIDE/M3/F03.RSC",
            "SCR3/MSLIDE/M3/F04.RSC",
            "SCR3/MSLIDE/M3/F05.RSC",
            "SCR3/MSLIDE/M3/F06.RSC",
            "SCR3/MSLIDE/M3/F07.RSC",
            "SCR3/MSLIDE/M3/F08.RSC",
            "SCR3/MSLIDE/M3/F09.RSC",
            "SCR3/MSLIDE/M3/F10.RSC",
            "SCR3/MSLIDE/M4/F01.RSC",
            "SCR3/MSLIDE/M4/F02.RSC",
            "SCR3/MSLIDE/M4/F03.RSC",
            "SCR3/MSLIDE/M4/F04.RSC",
            "SCR3/MSLIDE/M4/F05.RSC",
            "SCR3/MSLIDE/M4/F06.RSC",
            "SCR3/MSLIDE/M4/F07.RSC",
            "SCR3/MSLIDE/M4/F08.RSC",
            "SCR3/MSLIDE/M4/F09.RSC",
            "SCR3/MSLIDE/M4/F10.RSC",
            "SCR3/MSLIDE/M5/F01.RSC",
            "SCR3/MSLIDE/M5/F02.RSC",
            "SCR3/MSLIDE/M5/F03.RSC",
            "SCR3/MSLIDE/M5/F04.RSC",
            "SCR3/MSLIDE/M5/F05.RSC",
            "SCR3/MSLIDE/M5/F06.RSC",
            "SCR3/MSLIDE/M5/F07.RSC",
            "SCR3/MSLIDE/M5/F08.RSC",
            "SCR3/MSLIDE/M5/F09.RSC",
            "SCR3/MSLIDE/M5/F10.RSC",
            "SCR3/MSLIDE/M6/F01.RSC",
            "SCR3/MSLIDE/M6/F02.RSC",
            "SCR3/MSLIDE/M6/F03.RSC",
            "SCR3/MSLIDE/M6/F04.RSC",
            "SCR3/MSLIDE/M6/F05.RSC",
            "SCR3/MSLIDE/M6/F06.RSC",
            "SCR3/MSLIDE/M6/F07.RSC",
            "SCR3/MSLIDE/M6/F08.RSC",
            "SCR3/MSLIDE/M6/F09.RSC",
            "SCR3/MSLIDE/M6/F10.RSC",
            "SCR3/MSLIDE/M7/F01.RSC",
            "SCR3/MSLIDE/M7/F02.RSC",
            "SCR3/MSLIDE/M7/F03.RSC",
            "SCR3/MSLIDE/M7/F04.RSC",
            "SCR3/MSLIDE/M7/F05.RSC",
            "SCR3/MSLIDE/M7/F06.RSC",
            "SCR3/MSLIDE/M7/F07.RSC",
            "SCR3/MSLIDE/M7/F08.RSC",
            "SCR3/MSLIDE/M7/F09.RSC",
            "SCR3/MSLIDE/M7/F10.RSC",
        }
    },
    {
        "m_title",
        2,
        (char *[]) {
            "SCR1/MTITLE1.RSC",
            "SCR1/MTITLE2.RSC"
        }
    },
    {
        "m_war",
        1,
        (char*[]) {
            "SCR1/MWAR.RSC"
        }
    },
    {
        "m_audio",
        1,
        (char*[]) {
            "SCR2/MAUDIO.RSC"
        }
    },
    {
        "m_commend",
        1,
        (char*[]) {
            "SCR2/MCOMMEND.RSC"
        }
    },
    {
        "m_dev2",
        1,
        (char*[]) {
            "SCR2/MDEV.RSC"
        }
    },
    {
        "m_enigma",
        1,
        (char*[]) {
            "SCR2/MENIGMA.RSC"
        }
    },
    {
        "m_eval",
        1,
        (char*[]) {
            "SCR2/MEVAL.RSC"
        }
    },
    {
        "m_medals",
        1,
        (char*[]) {
            "SCR2/MMEDALS.RSC"
        }
    },
    {
        "m_multopt",
        1,
        (char*[]) {
            "SCR2/MMULTOPT.RSC"
        }
    },
    {
        "m_pmeval",                
        1,
        (char*[]) {
            "SCR2/MPMEVAL.RSC"
        }
    },
    {
        "m_pmeval2",
        1,
        (char*[]) {
            "SCR2/MPMEVAL2.RSC"
        }
    },
    {
        "m_pname",
        1,
        (char*[]) {
            "SCR2/MPNAME.RSC"
        }
    },
    {
        "m_postmis",
        1,
        (char*[]) {
            "SCR2/MPOSTMIS.RSC"
        }
    },
    {
        "m_secret",
        1,
        (char*[]) {
            "SCR2/MSECRET.RSC"
        }
    },
    {
        "m_uniform",
        1,
        (char*[]) {
            "SCR2/MUNIF.RSC"
        }
    },
    {
        "m_video",
        1,
        (char*[]) {
            "SCR2/MVIDEO.RSC"
        }
    }
};
int NumMOHUndergroundSSTRSCMapEntry = sizeof(MOHUndergroundSSTRSCMap) / sizeof(MOHUndergroundSSTRSCMap[0]);
/*
 Every SST File starts with a type 1 section containing the name of the script.
 Basic Idea to begin loading it is to read and link section 2 and 3.
 
 typedef struct SSTCallback_s {
    char  SrcEvent[28];
    char  DestEvent[28];
    int   Unknown;
 }
 
 typedef struct SSTLabelData_s {
    
    SSTLabelData_s *Next;
 }
 typedef struct SSTLabel {
    SSTCallback_t *Callback;
    SSTLabelData_t *Data;
 }
 
 */
SSTGFX_t Models[5];
VAO_t  *LabelsVao[512];

int NumModels;

void SSTFreeLabelList(SSTLabel_t *LabelList)
{
    SSTLabel_t *Temp;
    
    while( LabelList ) {
        Temp = LabelList;
        LabelList = LabelList->Next;
        free(Temp);
    }
}
void SSTFreeCallbackList(SSTCallback_t *CallbackList)
{
    SSTCallback_t *Temp;
    
    while( CallbackList ) {
        Temp = CallbackList;
        CallbackList = CallbackList->Next;
        free(Temp);
    }
}
void SSTFree(SST_t *SST)
{
    SSTClass_t *Temp;
    
    while( SST->ClassList ) {
        Temp = SST->ClassList;
        if( Temp->LabelList ) {
            SSTFreeLabelList(Temp->LabelList);
        }
        if( Temp->CallbackList ) {
            SSTFreeCallbackList(Temp->CallbackList);
        }
        if( Temp->VideoInfo ) {
            free(Temp->VideoInfo);
        }
        SST->ClassList = SST->ClassList->Next;
        free(Temp);
    }
    free(SST);
}
int SSTCompare( const void *a, const void *b)
{
    SSTLabel_t *LabelA = (SSTLabel_t*) a;
    SSTLabel_t *LabelB = (SSTLabel_t*) b;
    int int_a = LabelA->Depth;
    int int_b = LabelB->Depth;

    if ( int_a == int_b ) return 0;
    else if ( int_a < int_b ) return 1;
    else return -1;
}


void SSTPrepareLabelVAO(SSTLabel_t *Label,VRAM_t* VRAM,int Index)
{
    float x0,y0;
    float u0,v0;
    float x1,y1;
    float u1,v1;
    float x2,y2;
    float u2,v2;
    float x3,y3;
    float u3,v3;
    float TextureWidth = 256.f;
    float TextureHeight = 256.f;
    float BaseTextureX;
    float BaseTextureY;
    float ColorModeOffset;
    float *VertexData;
    int Stride;
    int DataSize;
    int VertexPointer;
    int TextureID;
    //        XYZ  UV
    Stride = (3 + 2) * sizeof(float);
    //We need 6 vertices to describe a quad...
    DataSize = Stride * 6;
    
    VertexData = malloc(Stride * 6/** sizeof(float)*/);
    VertexPointer = 0;
        
//     if( Label->ImageInfo.ColorMode == BPP_4 ) {
//         ColorModeOffset = 4;
// //         TextureID = VRam->Page4Bit[Label->ImageInfo.TexturePage].TextureID;
//     } else {
//         ColorModeOffset = 2;
// //         TextureID = VRam->Page8Bit[Label->ImageInfo.TexturePage].TextureID;
//     }
        
    if( Label->ImageInfo.FrameBufferY >= 256 ) {
        BaseTextureX = (Label->ImageInfo.FrameBufferX - ((Label->ImageInfo.TexturePage - 16) * 64)) * ColorModeOffset;
        BaseTextureY = Label->ImageInfo.FrameBufferY - 256;
    } else {
        BaseTextureX = (Label->ImageInfo.FrameBufferX - (Label->ImageInfo.TexturePage * 64)) * ColorModeOffset;
        BaseTextureY = Label->ImageInfo.FrameBufferY;
    }

    if( Label->Unknown2 == 0 ) {
        u0 = BaseTextureX / TextureWidth;
        v0 = BaseTextureY / TextureHeight;
        u1 = BaseTextureX / TextureWidth;
        v1 = (BaseTextureY + Label->ImageInfo.Height) / TextureHeight;
        u2 = (BaseTextureX + Label->ImageInfo.Width ) / TextureWidth;
        v2 = (BaseTextureY + Label->ImageInfo.Height) / TextureHeight;
        u3 = (BaseTextureX + Label->ImageInfo.Width) / TextureWidth;
        v3 =  BaseTextureY / TextureHeight;
        //NOTE(Adriano):Rotate the label if necessary...
        if( Label->Unknown3 ) {
            x0 = Label->x;
            y0 = Label->y;
            x1 = x0;
            y1 = Label->y + Label->Height;
            x2 = Label->x  + Label->Width;
            y2 = y1;
            x3 = x2;
            y3 = y0;
        } else {
            x0 = Label->x;
            y0 = Label->y;
            x1 = x0;
            y1 = Label->y + Label->ImageInfo.Height;
            x2 = Label->x  + Label->ImageInfo.Width;
            y2 = y1;
            x3 = x2;
            y3 = y0;
        }

    } else {
        u0 = BaseTextureX / TextureWidth;
        v0 = (BaseTextureY + Label->ImageInfo.Height) / TextureHeight;
        u1 = (BaseTextureX + Label->ImageInfo.Width) / TextureWidth;
        v1 = (BaseTextureY + Label->ImageInfo.Height) / TextureHeight;
        u2 = (BaseTextureX + Label->ImageInfo.Width) / TextureWidth;
        v2 = BaseTextureY / TextureHeight;
        u3 = BaseTextureX / TextureWidth;
        v3 = BaseTextureY / TextureHeight;
        //NOTE(Adriano):Rotate the label if necessary...
        if( Label->Unknown3 ) {
            x0 = Label->x;
            y0 = Label->y;
            x1 = x0;
            y1 = Label->y + Label->Height;
            x2 = Label->x  + Label->Width;
            y2 = y1;
            x3 = x2;
            y3 = y0;
        } else {
            x0 = Label->x;
            y0 = Label->y;
            x1 = x0;
            y1 = Label->y + Label->ImageInfo.Width;
            x2 = Label->x  + Label->ImageInfo.Height;
            y2 = y1;
            x3 = x2;
            y3 = y0;
        }
    }
    

    VertexData[VertexPointer] =  x1;
    VertexData[VertexPointer+1] = y1;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u1;
    VertexData[VertexPointer+4] = v1;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x0;
    VertexData[VertexPointer+1] = y0;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u0;
    VertexData[VertexPointer+4] = v0;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x2;
    VertexData[VertexPointer+1] = y2;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u2;
    VertexData[VertexPointer+4] = v2;
    VertexPointer += 5;
            

    VertexData[VertexPointer] =  x2;
    VertexData[VertexPointer+1] = y2;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u2;
    VertexData[VertexPointer+4] = v2;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x0;
    VertexData[VertexPointer+1] = y0;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u0;
    VertexData[VertexPointer+4] = v0;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x3;
    VertexData[VertexPointer+1] = y3;
    VertexData[VertexPointer+2] = Label->Depth;
    VertexData[VertexPointer+3] = u3;
    VertexData[VertexPointer+4] = v3;
    VertexPointer += 5;

//     LabelsVao[Index] = VaoInitXYZUV(VertexData,DataSize,Stride,0,3,-1,TextureID);
    free(VertexData);
}
void SSTModelRender(VRAM_t *VRAM)
{
    int i;
    for( i = 0; i < NumModels; i++ ) {
        GFXRender(Models[i].Model,VRAM);
    }
}
void SSTRender(VRAM_t *VRAM)
{
//     GL_Shader_t *Shader;
//     float PsxScreenWidth = 512.f;
//     float PsxScreenHeight = 256.f;
//     int OrthoMatrixID;
//     int i;
//     
//     Shader = Shader_Cache("SSTShader","Shaders/SSTVertexShader.glsl","Shaders/SSTFragmentShader.glsl");
//     
//     for( i = 0; i < NumLabels; i++ ) {
//         glUseProgram(Shader->ProgramID);
//         OrthoMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
// 
//         glm_mat4_identity(VidConf.ModelViewMatrix);
//         vec3 v;
//         v[0] = (VidConf.Width / PsxScreenWidth);
//         v[1] = (VidConf.Height / PsxScreenHeight);
//         v[3] = 0;
//         glm_scale(VidConf.ModelViewMatrix,v);
//         glm_mat4_mul(VidConf.PMatrixM4,VidConf.ModelViewMatrix,VidConf.MVPMatrix);
//         glUniformMatrix4fv(OrthoMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
//         glBindTexture(GL_TEXTURE_2D, LabelsVao[i]->TextureID);
//         glBindVertexArray(LabelsVao[i]->VaoID[0]);
//         glDrawArrays(GL_TRIANGLES, 0, 6);
//         glBindVertexArray(0);
//     }
//     SSTModelRender(VRam);
}

void SSTLateInit(VRAM_t* VRAM)
{
    int i;
    /*for( i = 0; i < NumLabels; i++ ) {
        SSTPrepareLabelVAO(&Label[i],VRAM,i);
    }*/
    for( i = 0; i < NumModels; i++ ) {
        GFXPrepareVAO(Models[i].Model);
    }
}

const char **SSTGetRSCPathListFromClassName(SSTClass_t *Class, int GameEngine, int *NumRSCFile)
{
    SSTRSCMap_t *Map;
    int NumEntry;
    int i;
    
    if( !Class ) {
        DPrintf("SSTGetRSCPathFromClassName: Invalid class\n");
        return NULL;
    }
    
    if( GameEngine == MOH_GAME_STANDARD ) {
        Map = &MOHSSTRSCMap;
        NumEntry = NumMOHSSTRSCMapEntry;
    } else {
        Map = &MOHUndergroundSSTRSCMap;
        NumEntry = NumMOHUndergroundSSTRSCMapEntry;
    }
    
    for( i = 0; i < NumEntry; i++ ) {
        if( !strcmp(Class->Name,Map[i].ClassName) ) {
            if( NumRSCFile ) {
                *NumRSCFile = Map[i].NumRSC;
            }
            return Map[i].RSCList;
        }
    }
    return NULL;
}
void SSTLoadLabel(SST_t *SST, SSTClass_t *Class,RSC_t *RSC,Byte **SSTBuffer,int GameEngine)
{
    SSTLabel_t *Label;
    TIMImage_t *Image;
    RSCEntry_t Entry;
    char **RSCPathList;
    int NumRSCPath;
    int Ret;
    
    if( !SST ) {
        DPrintf("SSTLoadLabel: Invalid SST\n");
        return;
    }
    if( !Class ) {
        DPrintf("SSTLoadLabel: Invalid class\n");
        return;
    }
//     if( !RSC ) {
//         DPrintf("SSTLoadLabel: Invalid RSC\n");
//         return;
//     }
    if( !SSTBuffer ) {
        DPrintf("SSTLoadLabel: Invalid Buffer\n");
        return;
    }
    Label = malloc(sizeof(SSTLabel_t));
    if( !Label ) {
        DPrintf("SSTLoadLabel: Failed to load label for class %s\n",Class->Name);
        return;
    }
    Label->Next = NULL;
    memcpy(&Label->TextureFile,*SSTBuffer,sizeof(Label->TextureFile));
    *SSTBuffer += sizeof(Label->TextureFile);
    Label->Unknown = **(int **) SSTBuffer;
    *SSTBuffer += 4;
    Label->x = **(short **) SSTBuffer;
    *SSTBuffer += 2;
    Label->Pad1 = **(short **) SSTBuffer;
    *SSTBuffer += 2;
    Label->y = **(short **) SSTBuffer;
    *SSTBuffer += 2;
    Label->Pad2 = **(short **) SSTBuffer;
    *SSTBuffer += 2;
    Label->Width = **(short **) SSTBuffer;
    *SSTBuffer += 2;
    Label->Pad3 = **(short **) SSTBuffer;
    *SSTBuffer += 2;
    Label->Height = **(short **) SSTBuffer;
    *SSTBuffer += 2;
    Label->Pad4 = **(short **) SSTBuffer;
    *SSTBuffer += 2;
    Label->Unknown2 = **(Byte **) SSTBuffer;
    *SSTBuffer += 1;
    Label->Unknown3 = **(Byte **) SSTBuffer;
    *SSTBuffer += 1;
    Label->Unknown4 = **(Byte **) SSTBuffer;
    *SSTBuffer += 1;
    Label->Unknown5 = **(Byte **) SSTBuffer;
    *SSTBuffer += 1;
    Label->Depth = **(int **) SSTBuffer;
    *SSTBuffer += 4;
    memcpy(&Label->Unknown6,*SSTBuffer,sizeof(Label->Unknown6));
    *SSTBuffer += sizeof(Label->Unknown6);
    DPrintf("SSTLoadLabel:Label Texture:%s Unknown1:%i X:%i Y:%i Width:%i Height:%i Depth:%i %i %i %i %i\n",Label->TextureFile,
            Label->Unknown,Label->x,
            Label->y,Label->Width,Label->Height,Label->Depth,Label->Unknown2,
            Label->Unknown3,Label->Unknown4,Label->Unknown5);
    DPrintf("SSTLoadLabel: Unk6 is");
    for( int i = 0; i < 12 ; i++ ) {
        DPrintf(" %i ",Label->Unknown6[i]);
    }
    DPrintf("\n");
    RSCPathList = SSTGetRSCPathListFromClassName(Class, GameEngine, &NumRSCPath);
    if( RSCPathList != NULL ) {
        for (int i = 0; i < NumRSCPath; i++ ) {
            DPrintf("SSTLoadLabel: Assets should be loaded from %s\n",RSCPathList[i] );
        }
    } else {
        DPrintf("SSTLoadLabel: Missing map for class %s\n",Class->Name);
        assert(1!=1);
    }
    //Link it in!
    //TODO(Adriano): This probably needs to be sorted according to the depth of the label otherwise they
    //will overlap during rendering
    Label->Next = Class->LabelList;
    Class->LabelList = Label;
    //TODO(Adriano): Figure out what RSC file we need before loading the texture
    if( /*strcmp(Label->TextureFile,"NULL") != 0*/ 0 ) {
        if( Label->y > 512 ) {
            //Clamp to Height
            Label->y = 0;
        }
        Ret = RSCOpen(RSC,Label->TextureFile,&Entry);
        if( Ret > 0 ) {
            DPrintf("SSTLoadLabel:Texture is %s\n",Entry.Name);
            Image = TIMLoadAllImagesFromBuffer(Entry.Data);
            Label->ImageInfo.TexturePage = Image->TexturePage;
            Label->ImageInfo.FrameBufferX = Image->FrameBufferX;
            Label->ImageInfo.FrameBufferY = Image->FrameBufferY;
            Label->ImageInfo.Width = Image->Width;
            Label->ImageInfo.Height = Image->Height;
            Label->ImageInfo.ColorMode = Image->Header.BPP;
            Image->Next = SST->ImageList;
            SST->ImageList = Image;
        }
    }
    return;
}
void SSTLoadCallback(SST_t *SST, SSTClass_t *Class,Byte **SSTBuffer)
{
    SSTCallback_t *Callback;
    TIMImage_t *Image;
    RSCEntry_t Entry;
    int Ret;
    
    if( !SST ) {
        DPrintf("SSTLoadCallback: Invalid SST\n");
        return;
    }
    if( !Class ) {
        DPrintf("SSTLoadCallback: Invalid class\n");
        return;
    }
    if( !SSTBuffer ) {
        DPrintf("SSTLoadCallback: Invalid Buffer\n");
        return;
    }
    Callback = malloc(sizeof(SSTCallback_t));
    if( !Callback ) {
        DPrintf("SSTLoadCallback: Failed to load callback for class %s\n",Class->Name);
        return;
    }

    Callback->Next = NULL;
    memcpy(&Callback->SrcEvent,*SSTBuffer,sizeof(Callback->SrcEvent));
    *SSTBuffer += sizeof(Callback->SrcEvent);
    memcpy(&Callback->DestEvent,*SSTBuffer,sizeof(Callback->DestEvent));
    *SSTBuffer += sizeof(Callback->DestEvent);
    Callback->Unknown = **(int **) SSTBuffer;
    *SSTBuffer += 4;
    DPrintf("SSTLoadCallback: SrcEvent:%s DestEvent:%s Unknown: %i\n",Callback->SrcEvent,Callback->DestEvent,Callback->Unknown);
    //Link it in!
    Callback->Next = Class->CallbackList;
    Class->CallbackList = Callback;
    return;
}
SST_t *SSTLoad(Byte *SSTBuffer,int GameEngine)
{
    SST_t *SST;
    SSTClass_t *CurrrentClass;
    SSTCallback_t *SSTCallback;
    SSTVideoInfo_t *SSTVideoInfo;
    SSTLabel_t *SSTLabel;
    SSTLabel_t *TempLabel;
    RSC_t *RSCData;
    RSC_t *RSCData2;
    RSCEntry_t Entry;
    char Name[28];
    int Size;
    int Token;
    int Ret;
    int StoreLabel;
    
    if( !SSTBuffer ) {
        DPrintf("SSTLoad:Invalid data.\n");
        return NULL;
    }
    DPrintf("Loading it\n");
    
    SST = malloc(sizeof(SST_t));
    
    if( !SST ) {
        DPrintf("SSTLoad:Failed to allocate memory for struct\n");
        return NULL;
    }
    NumModels = 0;
    SST->Next = NULL;
    SST->ImageList = NULL;
    SST->ClassList = NULL;
    RSCData = RSCLoad("SSTScripts/mdev.rsc");
    RSCData2 = RSCLoad("SSTScripts/mdev2.rsc");

    StoreLabel = 0;

    while( 1 ) {
        if( !*SSTBuffer ) {
            DPrintf("SSTLoad:EOF reached or an error occurred...\n");
            break;
        }
        Token = *(int *) SSTBuffer;
        SSTBuffer += 4;
        DPrintf("SSTLoad:Got token %i\n",Token);
        switch( Token ) {
            case 1:
                CurrrentClass = malloc(sizeof(SSTClass_t));
                CurrrentClass->LabelList = NULL;
                CurrrentClass->CallbackList = NULL;
                CurrrentClass->VideoInfo = NULL;
                CurrrentClass->Next = NULL;
                memcpy(&CurrrentClass->Name,SSTBuffer,sizeof(CurrrentClass->Name));
                SSTBuffer += sizeof(CurrrentClass->Name);
                //Link it in!
                CurrrentClass->Next = SST->ClassList;
                SST->ClassList = CurrrentClass;
                DPrintf("SSTLoad:Class Name is %s\n",CurrrentClass->Name);
                break;
            case 2:
                SSTLoadCallback(SST,CurrrentClass,&SSTBuffer);
                //StoreLabel = 0;
                break;
            case 3:
                SSTLoadLabel(SST,CurrrentClass,RSCData,&SSTBuffer,GameEngine);
                break;
            case 5:
                DPrintf("BackDrop declaration started.\n");
                //StoreLabel = 1;
                break;
            case 7:
                DPrintf("STR file declaration\n");
                CurrrentClass->VideoInfo = malloc(sizeof(SSTVideoInfo_t));
                memcpy(CurrrentClass->VideoInfo,SSTBuffer,sizeof(SSTVideoInfo_t));
                SSTBuffer += sizeof(SSTVideoInfo_t);
                DPrintf("SSTLoad:Callback STR file:%s Unknown:%i Unknown2: %i\n",CurrrentClass->VideoInfo->STRFile,
                        CurrrentClass->VideoInfo->Unknown,CurrrentClass->VideoInfo->Unknown2);
                break;
            case 8:
                SSTBuffer += 276;
                break;
            case 9:
                memcpy(&Size,SSTBuffer,sizeof(Size));
                SSTBuffer += sizeof(Size);
                SSTBuffer += (4*Size) + 4;
                break;
            case 10:
                //GFX Model
                //TODO(Adriano): As for the labels we need to understand what RSC file
                //we need to know where to load data
                memcpy(&Name,SSTBuffer,sizeof(Name));
                SSTBuffer += sizeof(Name);
//                 if( !strcmp(Name,"global2\\model\\clerkb.gfx") ) {
//                     SkipFileSection(SSTFile,84);
//                     break;
//                 }
                DPrintf("Loading model %s\n",Name);
//                 Ret = RSCOpen(RSCData,Name,&Entry);
//                 if( Ret < 0 ) {
//                     Ret = RSCOpen(RSCData2,Name,&Entry);
//                     if( Ret < 0 ) {
//                         DPrintf("File was not found inside RSC...%s\n",Name);
//                         break;
//                     }
//                 }
//                 Models[NumModels].Model = GFXRead(Entry.Data);
                memcpy(&Name,SSTBuffer,sizeof(Name));
                SSTBuffer += sizeof(Name);
                DPrintf("Loading texture %s\n",Name);
//                 Ret = RSCOpen(RSCData,Name,&Entry);
//                 if( Ret < 0 ) {
//                     Ret = RSCOpen(RSCData2,Name,&Entry);
//                     if( Ret < 0 ) {
//                         DPrintf("File was not found inside RSC...%s\n",Name);
//                         break;
//                     }
//                 }
//                 int NumImages = 0;
//                 while( Entry.Data ) {
//                     Models[NumModels].Image = TIMLoadImageFromBuffer(&Entry.Data,NumImages);
//                     if( Models[NumModels].Image == NULL ) {
//                         DPrintf("Image is NULL skippin\n");
//                         break;
//                     }
//                     Models[NumModels].Image->Next = SST->ImageList;
//                     SST->ImageList = Models[NumModels].Image;
//                     NumImages++;
//                 }
                SSTBuffer += 56;
//                 exit(0);
                NumModels++;
                break;
            case 11:
                //After a GFX Model we have this token.
                SSTBuffer += 288;
                break;
            default:
                DPrintf("SSTLoad:Unknown token %i\n",Token);
                assert(1!=1);
                break;
        }
    }
    //qsort( &Label, NumLabels, sizeof(SSTLabel_t), SSTCompare );
    RSCFree(RSCData);
    return SST;
    // Test
}
