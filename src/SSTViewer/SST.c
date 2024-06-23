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
#include "../Common/TIM.h"
#include "SSTViewer.h" 

const SSTRSCMap_t MOHSSTRSCMap[] = {
    {
        "m_brief",
        28,
        (const char*[]) {
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
        (const char*[]) {
            "SCR1/MCALIB.RSC"
        }
    },
    {
        "m_config",
        1,
        (const char*[]) {
            "SCR1/MCONFIG.RSC"
        }
    },
    {
        "m_ctrl",
        1,
        (const char*[]) {
            "SCR1/MCTRL.RSC"
        }
    },
    {
        "m_dev",
        2,
        (const char*[]) {
            "SCR1/MDEV.RSC",
            "SCR2/MDEV.RSC"
        }
    },
    {
        "m_dwiemp",
        19,
        (const char*[]) {
            "SCR3/MDWIEMP/1/1.RSC",
            "SCR3/MDWIEMP/1/2.RSC",
            "SCR3/MDWIEMP/1/3.RSC",
            "SCR3/MDWIEMP/1/4.RSC",
            "SCR3/MDWIEMP/1/5.RSC",
            "SCR3/MDWIEMP/1/6.RSC",
            "SCR3/MDWIEMP/1/7.RSC",
            "SCR3/MDWIEMP/1/8.RSC",
            "SCR3/MDWIEMP/1/9.RSC",
            "SCR3/MDWIEMP/1/10.RSC",
            "SCR3/MDWIEMP/2/1.RSC",
            "SCR3/MDWIEMP/2/2.RSC",
            "SCR3/MDWIEMP/2/3.RSC",
            "SCR3/MDWIEMP/2/4.RSC",
            "SCR3/MDWIEMP/2/5.RSC",
            "SCR3/MDWIEMP/2/6.RSC",
            "SCR3/MDWIEMP/2/7.RSC",
            "SCR3/MDWIEMP/15/1.RSC",
            "SCR3/MDWIEMP/15/2.RSC",
        }
    },
    {
        "m_gallery",
        1,
        (const char*[]) {
            "SCR1/MGAL.RSC"
        }
    },
    {
        "m_legal",
        1,
        (const char*[]) {
            "SCR1/MLEGAL.RSC"
        }
    },
    {
        "m_load",
        1,
        (const char*[]) {
            "SCR1/MLOAD.RSC"
        }
    },
    {
        "m_mission",
                1,
        (const char*[]) {
            "SCR1/MMISSION.RSC"
        }
    },
    {
        "m_option",
        1,
        (const char*[]) {
            "SCR1/MOPTION.RSC"
        }
    },
    {
        "m_play",
        1,
        (const char*[]) {
            "SCR1/MPLAY.RSC"
        }
    },
    {
        "m_save",
        1,
        (const char*[]) {
            "SCR1/MSAVE.RSC"
        }
    },
    {
        "m_slide",
        80,
        (const char*[]) {
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
        (const char*[]) {
            "SCR1/MTITLE1.RSC",
            "SCR1/MTITLE2.RSC"
        }
    },
    {
        "m_war",
        1,
        (const char*[]) {
            "SCR1/MWAR.RSC"
        }
    },
    {
        "m_audio",
        1,
        (const char*[]) {
            "SCR2/MAUDIO.RSC"
        }
    },
    {
        "m_commend",
        1,
        (const char*[]) {
            "SCR2/MCOMMEND.RSC"
        }
    },
    {
        "m_dev2",
        1,
        (const char*[]) {
            "SCR2/MDEV.RSC"
        }
    },
    {
        "m_enigma",
        1,
        (const char*[]) {
            "SCR2/MENIGMA.RSC"
        }
    },
    {
        "m_eval",
        1,
        (const char*[]) {
            "SCR2/MEVAL.RSC"
        }
    },
    {
        "m_medals",
        1,
        (const char*[]) {
            "SCR2/MMEDALS.RSC"
        }
    },
    {
        "m_multopt",
        1,
        (const char*[]) {
            "SCR2/MMULTOPT.RSC"
        }
    },
    {
        "m_pmeval",                
        1,
        (const char*[]) {
            "SCR2/MPMEVAL.RSC"
        }
    },
    {
        "m_pmeval2",
        1,
        (const char*[]) {
            "SCR2/MPMEVAL2.RSC"
        }
    },
    {
        "m_pname",
        1,
        (const char*[]) {
            "SCR2/MPNAME.RSC"
        }
    },
    {
        "m_postmis",
        1,
        (const char*[]) {
            "SCR2/MPOSTMIS.RSC"
        }
    },
    {
        "m_secret",
        1,
        (const char*[]) {
            "SCR2/MSECRET.RSC"
        }
    },
    {
        "m_uniform",
        1,
        (const char*[]) {
            "SCR2/MUNIF.RSC"
        }
    },
    {
        "m_video",
        1,
        (const char*[]) {
            "SCR2/MVIDEO.RSC"
        }
    }
};
int NumMOHSSTRSCMapEntry = sizeof(MOHSSTRSCMap) / sizeof(MOHSSTRSCMap[0]);

const SSTRSCMap_t MOHUndergroundSSTRSCMap[] = {
    {
        "m_brief",
        28,
        (const char*[]) {
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
        (const char*[]) {
            "SCR1/MCALIB.RSC"
        }
    },
    {
        "m_config",
        1,
        (const char*[]) {
            "SCR1/MCONFIG.RSC"
        }
    },
    {
        "m_ctrl",
        1,
        (const char*[]) {
            "SCR1/MCTRL.RSC"
        }
    },
    {
        "m_dev",
        1,
        (const char*[]) {
            "SCR1/MDEV.RSC"
        }
    },
    {
        "m_dwiemp",
        12,
        (const char*[]) {
            "SCR3/MDWIEMP/1/1.RSC",
            "SCR3/MDWIEMP/1/2.RSC",
            "SCR3/MDWIEMP/1/3.RSC",
            "SCR3/MDWIEMP/1/4.RSC",
            "SCR3/MDWIEMP/1/5.RSC",
            "SCR3/MDWIEMP/1/6.RSC",
            "SCR3/MDWIEMP/1/7.RSC",
            "SCR3/MDWIEMP/1/8.RSC",
            "SCR3/MDWIEMP/1/9.RSC",
            "SCR3/MDWIEMP/1/10.RSC",
            "SCR3/MDWIEMP/2/1.RSC",
            "SCR3/MDWIEMP/2/2.RSC"
        }
    },
    {
        "m_diff",
        1,
        (const char*[]) {
            "SCR1/MDIFF.RSC"
        }
    },
    {
        "m_gallery",
        1,
        (const char*[]) {
            "SCR1/MGAL.RSC"
        }
    },
    {
        "m_legal",
        1,
        (const char*[]) {
            "SCR1/MLEGAL.RSC"
        }
    },
    {
        "m_load",
        1,
        (const char*[]) {
            "SCR1/MLOAD.RSC"
        }
    },
    {
        "m_mission",
                1,
        (const char*[]) {
        "SCR1/MMISSION.RSC"
        }
    },
    {
        "m_option",
        1,
        (const char*[]) {
            "SCR1/MOPTION.RSC"
        }
    },
    {
        "m_play",
        1,
        (const char*[]) {
            "SCR1/MPLAY.RSC"
        }
    },
    {
        "m_save",
        1,
        (const char*[]) {
            "SCR1/MSAVE.RSC"
        }
    },
    {
        "m_slide",
        70,
        (const char*[]) {
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
        (const char *[]) {
            "SCR1/MTITLE1.RSC",
            "SCR1/MTITLE2.RSC"
        }
    },
    {
        "m_war",
        1,
        (const char*[]) {
            "SCR1/MWAR.RSC"
        }
    },
    {
        "m_audio",
        1,
        (const char*[]) {
            "SCR2/MAUDIO.RSC"
        }
    },
    {
        "m_commend",
        1,
        (const char*[]) {
            "SCR2/MCOMMEND.RSC"
        }
    },
    {
        "m_dev2",
        1,
        (const char*[]) {
            "SCR2/MDEV.RSC"
        }
    },
    {
        "m_enigma",
        1,
        (const char*[]) {
            "SCR2/MENIGMA.RSC"
        }
    },
    {
        "m_eval",
        1,
        (const char*[]) {
            "SCR2/MEVAL.RSC"
        }
    },
    {
        "m_medals",
        1,
        (const char*[]) {
            "SCR2/MMEDALS.RSC"
        }
    },
    {
        "m_multopt",
        1,
        (const char*[]) {
            "SCR2/MMULTOPT.RSC"
        }
    },
    {
        "m_pmeval",                
        1,
        (const char*[]) {
            "SCR2/MPMEVAL.RSC"
        }
    },
    {
        "m_pmeval2",
        1,
        (const char*[]) {
            "SCR2/MPMEVAL2.RSC"
        }
    },
    {
        "m_pname",
        1,
        (const char*[]) {
            "SCR2/MPNAME.RSC"
        }
    },
    {
        "m_postmis",
        1,
        (const char*[]) {
            "SCR2/MPOSTMIS.RSC"
        }
    },
    {
        "m_secret",
        1,
        (const char*[]) {
            "SCR2/MSECRET.RSC"
        }
    },
    {
        "m_uniform",
        1,
        (const char*[]) {
            "SCR2/MUNIF.RSC"
        }
    },
    {
        "m_video",
        1,
        (const char*[]) {
            "SCR2/MVIDEO.RSC"
        }
    }
};
int NumMOHUndergroundSSTRSCMapEntry = sizeof(MOHUndergroundSSTRSCMap) / sizeof(MOHUndergroundSSTRSCMap[0]);

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
    
    if( !SST ) {
        return;
    }
    
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
        if( Temp->RSCList ) {
            RSCFree(Temp->RSCList);
        }
        if( Temp->ImageList ) {
            TIMImageListFree(Temp->ImageList);
        }
        if( Temp->GFXModelList ) {
            GFXFree(Temp->GFXModelList);
        }
        if( Temp->VRAM ) {
            VRAMFree(Temp->VRAM);
        }
        if( Temp->LabelsVAO ) {
            VAOFree(Temp->LabelsVAO);
        }
        SST->ClassList = SST->ClassList->Next;
        free(Temp);
    }
    if( SST->Name ) {
        free(SST->Name);
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

void SSTFillVertexBuffer(int *Buffer,int *BufferSize,int x,int y,int z,Color1i_t Color,int U,int V,int CLUTX,int CLUTY,int ColorMode)
{
    if( !Buffer ) {
        DPrintf("SSTFillVertexBuffer:Invalid Buffer\n");
        return;
    }
    if( !BufferSize ) {
        DPrintf("SSTFillVertexBuffer:Invalid BufferSize\n");
        return;
    }
    Buffer[*BufferSize] =   x;
    Buffer[*BufferSize+1] = y;
    Buffer[*BufferSize+2] = z;
    Buffer[*BufferSize+3] = U;
    Buffer[*BufferSize+4] = V;
    Buffer[*BufferSize+5] = Color.rgba[0];
    Buffer[*BufferSize+6] = Color.rgba[1];
    Buffer[*BufferSize+7] = Color.rgba[2];
    Buffer[*BufferSize+8] = CLUTX;
    Buffer[*BufferSize+9] = CLUTY;
    Buffer[*BufferSize+10] = ColorMode;
    *BufferSize += 11;
}
int SSTGetLabelStride()
{
    //      XYZ UV  RGB CLUT ColorMode
    return (3 + 2 + 3 + 2 + 1) * sizeof(int);
}
void SSTFillLabelsVAO(SSTLabel_t *Label,VRAM_t* VRAM,VAO_t *LabelsVAO)
{
    int x0;
    int y0;
    int u0;
    int v0;
    int x1;
    int y1;
    int u1;
    int v1;
    int x2;
    int y2;
    int u2;
    int v2;
    int x3;
    int y3;
    int u3;
    int v3;
    int CLUTPosX;
    int CLUTPosY;
    int CLUTDestX;
    int CLUTDestY;
    int CLUTPage;
    int VRAMPage;
    int BaseTextureX;
    int BaseTextureY;
    int *VertexData;
    int Stride;
    int DataSize;
    int VertexPointer;
    int Width;
    int Height;
    
    if( !Label ) {
        DPrintf("SSTFillLabelVAO: Invalid label data\n");
        return;
    }
    
    if( !VRAM ) {
        DPrintf("SSTFillLabelVAO: Invalid VRAM data\n");
        return;
    }
    
    if( !LabelsVAO ) {
        DPrintf("SSTFillLabelVAO: Invalid VAO data\n");
        return;
    }
    Stride = SSTGetLabelStride();
    //We need 6 vertices to describe a quad...
    DataSize = Stride * 6;
    
    VertexData = malloc(Stride * 6);
    VertexPointer = 0;
    
    
    CLUTPosX = Label->ImageInfo.CLUTX;
    CLUTPosY = Label->ImageInfo.CLUTY;
    
    CLUTPage = VRAMGetCLUTPage(CLUTPosX,CLUTPosY);
    
    CLUTDestX = VRAMGetCLUTPositionX(CLUTPosX,CLUTPosY,CLUTPage) + VRAMGetTexturePageX(CLUTPage);
    CLUTDestY = CLUTPosY + VRAMGetCLUTOffsetY(Label->ImageInfo.ColorMode);
    
    VRAMPage = Label->ImageInfo.TexturePage;
    
    //NOTE(Adriano): First get the position relative to the single page then map it to the whole VRAM texture that we have
    BaseTextureX =  VRAMGetTexturePageX(VRAMPage) + VRAMGetTexturePositionX(Label->ImageInfo.FrameBufferX,Label->ImageInfo.ColorMode);
    BaseTextureY = VRAMGetTexturePageY(VRAMPage,Label->ImageInfo.ColorMode) + VRAMGetTexturePositionY(Label->ImageInfo.FrameBufferY);

    //NOTE(Adriano):Rotate the label if necessary...
    if( Label->UseLabelSize ) {
        Width = Label->Width;
        Height = Label->Height;
    } else {
        Width = Label->FlipTexture ? Label->ImageInfo.Height : Label->ImageInfo.Width;
        Height = Label->FlipTexture ?Label->ImageInfo.Width : Label->ImageInfo.Height;
    }
    if( Label->FlipTexture == 0 ) {
        u0 = BaseTextureX;
        v0 = BaseTextureY;
        u1 = BaseTextureX;
        v1 = BaseTextureY + Label->ImageInfo.Height;
        u2 = BaseTextureX + Label->ImageInfo.Width;
        v2 = BaseTextureY + Label->ImageInfo.Height;
        u3 = BaseTextureX + Label->ImageInfo.Width;
        v3 = BaseTextureY;
    } else {
        u0 = BaseTextureX;
        v0 = BaseTextureY + Label->ImageInfo.Height;
        u1 = BaseTextureX + Label->ImageInfo.Width;
        v1 = BaseTextureY + Label->ImageInfo.Height;
        u2 = BaseTextureX + Label->ImageInfo.Width;
        v2 = BaseTextureY;
        u3 = BaseTextureX;
        v3 = BaseTextureY;
    }
    
    x0 = Label->x;
    y0 = Label->y;
    
    x1 = x0;
    y1 = Label->y + Height;
    
    x2 = Label->x  + Width;
    y2 = y1;
    
    x3 = x2;
    y3 = y0;
  
    SSTFillVertexBuffer(VertexData,&VertexPointer,x1,y1,Label->Depth,
                            Label->Color0,u1,v1,CLUTDestX,CLUTDestY,Label->ImageInfo.ColorMode);
    SSTFillVertexBuffer(VertexData,&VertexPointer,x0,y0,Label->Depth,
                            Label->Color1,u0,v0,CLUTDestX,CLUTDestY,Label->ImageInfo.ColorMode);
    SSTFillVertexBuffer(VertexData,&VertexPointer,x2,y2,Label->Depth,
                            Label->Color2,u2,v2,CLUTDestX,CLUTDestY,Label->ImageInfo.ColorMode);

    SSTFillVertexBuffer(VertexData,&VertexPointer,x2,y2,Label->Depth,
                            Label->Color1,u2,v2,CLUTDestX,CLUTDestY,Label->ImageInfo.ColorMode);
    SSTFillVertexBuffer(VertexData,&VertexPointer,x0,y0,Label->Depth,
                            Label->Color0,u0,v0,CLUTDestX,CLUTDestY,Label->ImageInfo.ColorMode);
    SSTFillVertexBuffer(VertexData,&VertexPointer,x3,y3,Label->Depth,
                            Label->Color2,u3,v3,CLUTDestX,CLUTDestY,Label->ImageInfo.ColorMode);

    VAOUpdate(LabelsVAO,VertexData,DataSize,6);
    free(VertexData);
}
void SSTModelRender(VRAM_t *VRAM)
{
//     int i;
//     for( i = 0; i < NumModels; i++ ) {
//         GFXRender(Models[i].Model,VRAM);
//     }
}
void SSTRenderClass(SSTClass_t *Class)
{
    if( !Class->VRAM ) {
        return;
    }
    if( !Class->LabelsVAO ) {
        return;
    }
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, Class->VRAM->TextureIndexPage.TextureId);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, Class->VRAM->PalettePage.TextureId);
    glBindVertexArray(Class->LabelsVAO->VAOId[0]);
    glDrawArrays(GL_TRIANGLES, 0, Class->LabelsVAO->Count);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D,0);

}
void SSTRender(SST_t *SST,mat4 ProjectionMatrix)
{
    Shader_t *Shader;
    SSTClass_t *ClassIterator;
    float PsxScreenWidth = 512.f;
    float PsxScreenHeight = 256.f;
    int PaletteTextureId;
    int TextureIndexId;
    int OrthoMatrixID;
    mat4 ModelViewMatrix;
    mat4 MVPMatrix;
    mat4 ViewMatrix;
    GFX_t *Model;
    vec3 v;

    if( !SST ) {
        DPrintf("SSTRender: Invalid SST script\n");
        return;
    }
    
    if( !SST->ClassList ) {
        DPrintf("SSTRender: No class list to draw\n");
        return;
    }
    
    Shader = ShaderCache("SSTShader","Shaders/SSTVertexShader.glsl","Shaders/SSTFragmentShader.glsl");
    
    if( Shader ) {
        glUseProgram(Shader->ProgramId);
        
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        OrthoMatrixID = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
        glm_mat4_identity(ModelViewMatrix);
        v[0] = (VidConfigWidth->IValue / PsxScreenWidth);
        v[1] = (VidConfigHeight->IValue / PsxScreenHeight);
        v[2] = 0;
        glm_scale(ModelViewMatrix,v);
        glm_mat4_mul(ProjectionMatrix,ModelViewMatrix,MVPMatrix);
        glUniformMatrix4fv(OrthoMatrixID,1,false,&MVPMatrix[0][0]);
        PaletteTextureId = glGetUniformLocation(Shader->ProgramId,"ourPaletteTexture");
        TextureIndexId = glGetUniformLocation(Shader->ProgramId,"ourIndexTexture");
        glUniform1i(TextureIndexId, 0);
        glUniform1i(PaletteTextureId,  1);
        for( ClassIterator = SST->ClassList; ClassIterator; ClassIterator = ClassIterator->Next ) {
            SSTRenderClass(ClassIterator);
        }
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glUseProgram(0);
    }
    glm_mat4_identity(ProjectionMatrix);
    glm_mat4_identity(ViewMatrix);
    glm_perspective(glm_rad(110.f),(float) VidConfigWidth->IValue / (float) VidConfigHeight->IValue,1.f, 4096.f,ProjectionMatrix);    
    for( ClassIterator = SST->ClassList; ClassIterator; ClassIterator = ClassIterator->Next ) {
        for( Model = ClassIterator->GFXModelList; Model; Model = Model->Next ) {
            GFXRender(Model,ClassIterator->VRAM,ProjectionMatrix,ViewMatrix);
        }
    }
    
//     SSTModelRender(VRam);
}
int SSTCreateLabelsVAO(SSTClass_t *Class)
{
    VAO_t *Result;
    int DataSize;
    int Stride;
    
    if( !Class->LabelList || Class->NumLabels == 0 ) {
        DPrintf("SSTCreateLabelsVAO: Invalid label list\n");
        return 0;
    }
    Stride = SSTGetLabelStride();
    //NOTE(Adriano): We need 6 vertices to describe a quad
    DataSize = 6 * Stride * Class->NumLabels;
    Result = VAOInitXYZUVRGBCLUTColorModeInteger(NULL,DataSize,Stride,0,3,5,8,10,Class->NumLabels * 6);
    if( !Result ) {
        DPrintf("SSTCreateLabelsVAO: Failed to initialize Label VAO\n");
        return 0;
    }
    Class->LabelsVAO = Result;
    return 1;
}
void SSTGenerateClassVAOs(SSTClass_t *Class)
{
    SSTLabel_t *Label;
    int Ret;
    
    if( !Class ) {
        DPrintf("SSTGenerateClassVAOs: Invalid class\n");
        return;
    }
    if( !Class->ImageList ) { 
        DPrintf("SSTGenerateClassVAOs: Failed to generate VAOs for class %s...no images were loaded...\n",Class->Name);
        return;
    }
    if( Class->VRAM ) {
        DPrintf("SSTGenerateClassVAOs: Class %s has already a VRAM structure allocated...freeing it\n",Class->Name);
        VRAMFree(Class->VRAM);
    }
    Class->VRAM = VRAMInit(Class->ImageList);
    if( !Class->VRAM ) {
        DPrintf("SSTGenerateClassVAOs: Failed to create VRAM for class %s\n",Class->Name);
        return;
    }
    Ret = SSTCreateLabelsVAO(Class);
    if( !Ret ) {
        DPrintf("SSTGenerateClassVAOs: Failed to create label VAO for class %s\n",Class->Name);
        VRAMFree(Class->VRAM);
        return;
    }
    DPrintf("SSTGenerateClassVAOs: Generating label VAOs for class %s\n",Class->Name);
    for( Label = Class->LabelList; Label; Label = Label->Next ) {
        SSTFillLabelsVAO(Label, Class->VRAM, Class->LabelsVAO);
    }
}
void SSTUnload(SST_t *SST)
{
    SSTClass_t *Class;
    GFX_t *Model;
    
    if( !SST ) {
        return;
    }
    if( !SST->ClassList ) {
        return;
    }
    DPrintf("SSTUnload: Unloading script %s\n",SST->Name);
    for( Class = SST->ClassList; Class; Class = Class->Next ) {
        if( Class->LabelsVAO ) {
            VAOFree(Class->LabelsVAO);
            Class->LabelsVAO = NULL;
        }
        for( Model = Class->GFXModelList; Model; Model = Model->Next ) {
            VAOFree(Model->VAO);
            Model->VAO = NULL;
        }
        if( Class->VRAM ) {
            VRAMFree(Class->VRAM);
            Class->VRAM = NULL;
        }
    }
}
void SSTGenerateVAOs(SST_t *SST)
{
    SSTClass_t *Class;
    GFX_t *Model;
    
    if( !SST ) {
        return;
    }
    if( !SST->ClassList ) {
        return;
    }

    for( Class = SST->ClassList; Class; Class = Class->Next ) {
        if( Class->LabelsVAO ) {
            continue;
        }
        SSTGenerateClassVAOs(Class);
        for( Model = Class->GFXModelList; Model; Model = Model->Next ) {
            if( Model->VAO ) {
                continue;
            }
            GFXPrepareVAO(Model);
        }
    }
}
void SSTAppendImageList(TIMImage_t **OriginalImageList,TIMImage_t *ImageList)
{
    TIMImage_t *LastNode;
    if( !*OriginalImageList ) {
        *OriginalImageList = ImageList;
    } else {
        LastNode = *OriginalImageList;
        while( LastNode->Next ) {
            LastNode = LastNode->Next;
        }
        LastNode->Next = ImageList;
    }
}
const char **SSTBuildRSCPathListFromClassName(const char *ClassName, const SSTRSCMap_t *Map, int NumMapEntries, int *NumRSCFile)
{
    int i;
    for( i = 0; i < NumMapEntries; i++ ) {
        if( !strcmp(ClassName,Map[i].ClassName) ) {
            if( NumRSCFile ) {
                *NumRSCFile = Map[i].NumRSC;
            }
            return Map[i].RSCList;
        }
    }
    return NULL;
}
const char **SSTGetRSCPathListFromClassName(SSTClass_t *Class, int GameEngine, int *NumRSCFile)
{    
    if( !Class ) {
        DPrintf("SSTGetRSCPathFromClassName: Invalid class\n");
        return NULL;
    }
    
    if( GameEngine == MOH_GAME_STANDARD ) {
        return SSTBuildRSCPathListFromClassName(Class->Name,MOHSSTRSCMap,NumMOHSSTRSCMapEntry,NumRSCFile);
    }
    
    return SSTBuildRSCPathListFromClassName(Class->Name,MOHUndergroundSSTRSCMap,NumMOHUndergroundSSTRSCMapEntry,NumRSCFile);
}
int SSTLoadAssetFromRSCList(const char *FileName,RSC_t *ClassRSCList,const RSC_t *GlobalRSCList,RSCEntry_t *Entry)
{
    int Ret;
    
    Ret = RSCOpen(ClassRSCList,FileName,Entry);
    if( Ret < 0 ) {
        DPrintf("SSTLoadAssetFromRSCList: File %s was not found in the class list...trying the global one...\n",FileName);
        Ret = RSCOpen(GlobalRSCList,FileName,Entry);
        if( Ret < 0 ) {
            DPrintf("SSTLoadAssetFromRSCList: File %s was not found in the global list\n",FileName);
            return 0;
        }
    }
    return 1;
}
void SSTAppendLabelToList(SSTLabel_t *Label,SSTLabel_t **LabelList)
{
    SSTLabel_t *Iterator;
    SSTLabel_t *Prev;
    
    //Base case - List is empty...just append it
    if (*LabelList == NULL) {
        Label->Next = *LabelList;
        *LabelList = Label;
        return;
    }
    
    Iterator = *LabelList;
    Prev = NULL;
    
    while( 1 ) {
        if( !Iterator ) {
            break;
        }
        //Found insertion point
        if( Iterator->Depth <= Label->Depth ) {
            break;
        }
        Prev = Iterator;
        Iterator = Iterator->Next;
    }
    //Insert it
    Label->Next = Iterator;
    if( Prev != NULL ) {
        Prev->Next = Label;
    } else {
        *LabelList = Label;
    }
}
void SSTLoadLabel(SST_t *SST, SSTClass_t *Class,SSTCallback_t *Callback,const RSC_t *GlobalRSCList,Byte **SSTBuffer,int GameEngine)
{
    SSTLabel_t *Label;
    TIMImage_t *Image;
    RSCEntry_t  Entry;
    int Ret;
    
    if( !SST ) {
        DPrintf("SSTLoadLabel: Invalid SST\n");
        return;
    }
    if( !Class ) {
        DPrintf("SSTLoadLabel: Invalid class\n");
        return;
    }
    if( !GlobalRSCList ) {
        DPrintf("SSTLoadLabel: Invalid global RSC list\n");
        return;
    }
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
    Label->FlipTexture = **(Byte **) SSTBuffer;
    *SSTBuffer += 1;
    Label->UseLabelSize = **(Byte **) SSTBuffer;
    *SSTBuffer += 1;
    Label->Unknown4 = **(Byte **) SSTBuffer;
    *SSTBuffer += 1;
    Label->Unknown5 = **(Byte **) SSTBuffer;
    *SSTBuffer += 1;
    Label->Depth = **(int **) SSTBuffer;
    *SSTBuffer += 4;
    memcpy(&Label->Color0,*SSTBuffer,sizeof(Label->Color0));
    *SSTBuffer += sizeof(Label->Color0);
    memcpy(&Label->Color1,*SSTBuffer,sizeof(Label->Color1));
    *SSTBuffer += sizeof(Label->Color1);
    memcpy(&Label->Color2,*SSTBuffer,sizeof(Label->Color2));
    *SSTBuffer += sizeof(Label->Color2);
    DPrintf("SSTLoadLabel:Label Texture:%s Unknown1:%i X:%i Y:%i Width:%i Height:%i Depth:%i FlipTexture:%i UseLabelSize:%i %i %i\n",Label->TextureFile,
            Label->Unknown,Label->x,
            Label->y,Label->Width,Label->Height,Label->Depth,Label->FlipTexture,
            Label->UseLabelSize,Label->Unknown4,Label->Unknown5);
    DPrintf("SSTLoadLabel: Color0: %i;%i;%i;%i Color1: %i;%i;%i;%i Color2:%i;%i;%i;%i\n",Label->Color0.rgba[0],Label->Color0.rgba[1],
            Label->Color0.rgba[2],Label->Color0.rgba[3],Label->Color1.rgba[0],Label->Color1.rgba[1],Label->Color1.rgba[2],Label->Color2.rgba[3],
            Label->Color2.rgba[0],Label->Color2.rgba[1],Label->Color2.rgba[2],Label->Color2.rgba[3]);
    //TODO(Adriano): Do we want to keep a reference to the original callback or we want to memcpy it (and manage the pointer by itself)?
    Label->Callback = Callback;

    if( Label->Callback ) {
        DPrintf("SSTLoadLabel: SrcEvent %s DestEvent %s Arg:%i\n",Label->Callback->SrcEvent,Label->Callback->DestEvent,Label->Callback->Unknown);
    } else {
        DPrintf("SSTLoadLabel: No callback has been specified\n");
    }
    //Link it in!
    SSTAppendLabelToList(Label,&Class->LabelList);
    Class->NumLabels++;

    if( strcmp(Label->TextureFile,"NULL") != 0 ) {
        if( Label->y > 512 ) {
            //Clamp to Height
            Label->y = 0;
        }
        Ret = SSTLoadAssetFromRSCList(Label->TextureFile, Class->RSCList, GlobalRSCList, &Entry);
        if( Ret ) {
            DPrintf("SSTLoadLabel:Texture is %s\n",Entry.Name);
            Image = TIMLoadAllImagesFromBuffer(Entry.Data);
            Label->ImageInfo.TexturePage = Image->TexturePage;
            Label->ImageInfo.CLUTX = Image->Header.CLUTOrgX;
            Label->ImageInfo.CLUTY = Image->Header.CLUTOrgY;
            Label->ImageInfo.CLUTPage = Image->CLUTTexturePage;
            Label->ImageInfo.FrameBufferX = Image->FrameBufferX;
            Label->ImageInfo.FrameBufferY = Image->FrameBufferY;
            Label->ImageInfo.Width = Image->Width;
            Label->ImageInfo.Height = Image->Height;
            Label->ImageInfo.ColorMode = Image->Header.BPP;
            SSTAppendImageList(&Class->ImageList,Image);
        } else {
            DPrintf("SSTLoadLabel:Failed to locate texture %s\n",Label->TextureFile);
        }
    }
    return;
}
SSTCallback_t *SSTLoadCallback(SST_t *SST, SSTClass_t *Class,Byte **SSTBuffer)
{
    SSTCallback_t *Callback;
    
    if( !SST ) {
        DPrintf("SSTLoadCallback: Invalid SST\n");
        return NULL;
    }
    if( !Class ) {
        DPrintf("SSTLoadCallback: Invalid class\n");
        return NULL;
    }
    if( !SSTBuffer ) {
        DPrintf("SSTLoadCallback: Invalid Buffer\n");
        return NULL;
    }
    Callback = malloc(sizeof(SSTCallback_t));
    if( !Callback ) {
        DPrintf("SSTLoadCallback: Failed to load callback for class %s\n",Class->Name);
        return NULL;
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
    return Callback;
}
void SSTLoadVideoInfo(SST_t *SST,SSTClass_t *Class,Byte **SSTBuffer)
{
    SSTVideoInfo_t *VideoInfo;
    
    if( !SST ) {
        DPrintf("SSTLoadVideoInfo:Invalid SST data\n");
        return;
    }
    if( !Class ) {
        DPrintf("SSTLoadVideoInfo:Invalid class\n");
        return;
    }
    if( !SSTBuffer ) {
        DPrintf("SSTLoadVideoInfo:Invalid buffer\n");
        return;
    }
    if( Class->VideoInfo ) {
        DPrintf("SSTLoadVideoInfo:Warning, class %s already has video info data allocated\n",Class->Name);
        return;
    }
    VideoInfo = malloc(sizeof(SSTVideoInfo_t));
    if( !VideoInfo ) {
        DPrintf("SSTLoadVideoInfo:Failed to allocate memory for video info for class %s\n",Class->Name);
        return;
    }
    memcpy(VideoInfo,*SSTBuffer,sizeof(SSTVideoInfo_t));
    *SSTBuffer += sizeof(SSTVideoInfo_t);
    DPrintf("SSTLoad:Callback STR file:%s Unknown:%i Unknown2: %i\n",VideoInfo->STRFile,
    VideoInfo->Unknown,VideoInfo->Unknown2);
    Class->VideoInfo = VideoInfo;
}
void SSTLoadGFXModel(SST_t *SST,SSTClass_t *Class,const RSC_t *GlobalRSCList,Byte **SSTBuffer)
{
    GFX_t *GFX;
    RSCEntry_t Entry;
    TIMImage_t *Image;
    char FileName[28];
    Byte UseGlobal2;
    int Ret;
    
    if( !SST ) {
        DPrintf("SSTLoadGFXModel:Invalid SST data\n");
        return;
    }
    if( !Class ) {
        DPrintf("SSTLoadGFXModel:Invalid class\n");
        return;
    }
    if( !GlobalRSCList ) {
        DPrintf("SSTLoadGFXModel: Invalid global RSC list\n");
        return;
    }
    if( !SSTBuffer ) {
        DPrintf("SSTLoadGFXModel:Invalid buffer\n");
        return;
    }
    
    memcpy(&FileName,*SSTBuffer,sizeof(FileName));
    *SSTBuffer += sizeof(FileName);
    DPrintf("SSTLoadGFXModel:Looking up model %s\n",FileName);
    Ret = SSTLoadAssetFromRSCList(FileName, Class->RSCList, GlobalRSCList, &Entry);
    if( Ret ) {
        DPrintf("SSTLoadGFXModel:Model is %s\n",Entry.Name);
        GFX = GFXRead(Entry.Data,Entry.Length);
    } else {
        DPrintf("SSTLoadGFXModel:Failed to locate model %s\n",FileName);
    }
    
    memcpy(&FileName,*SSTBuffer,sizeof(FileName));
    *SSTBuffer += sizeof(FileName);
    
    if( strcmp(FileName,"NULL") != 0 ) {
        Ret = SSTLoadAssetFromRSCList(FileName, Class->RSCList, GlobalRSCList, &Entry);
        if( Ret ) {
            DPrintf("SSTLoadGFXModel:Texture is %s\n",Entry.Name);
            Image = TIMLoadAllImagesFromBuffer(Entry.Data);
            SSTAppendImageList(&Class->ImageList,Image);
        } else {
            DPrintf("SSTLoadGFXModel:Failed to locate texture %s\n",FileName);
        }
    } else {
        DPrintf("SSTLoadGFXModel: NULL texture...\n");
    }
    *SSTBuffer += 32;
    UseGlobal2 = **(Byte **) SSTBuffer;
    *SSTBuffer += 4;
    GFX->RotationX = **(int **) SSTBuffer;
    *SSTBuffer += 4;
    GFX->RotationY = **(int **) SSTBuffer;
    *SSTBuffer += 4;
    GFX->RotationZ = **(int **) SSTBuffer;
    //NOTE(Adriano): Also skips the remaining bytes
    *SSTBuffer += 12;
    
    DPrintf("SSTLoadGFXModel: Asset should be loaded from Global2:%i\n",UseGlobal2);
    //Link it in!
    GFX->Next = Class->GFXModelList;
    Class->GFXModelList = GFX;
}
SSTClass_t *SSTLoadClass(SST_t *SST,Byte **SSTBuffer,const char *BasePath,int GameEngine)
{
    SSTClass_t *Class;
    RSC_t *RSC;
    char *DataPath;
    char *Temp;
    const char **RSCPathList;
    int NumRSCPath;
    
    if( !SST ) {
        DPrintf("SSTLoadClass:Invalid SST script data\n");
        return NULL;
    }
    if( !SSTBuffer ) {
        DPrintf("SSTLoadClass:Invalid SST Buffer\n");
        return NULL;
    }
    Class = malloc(sizeof(SSTClass_t));
    if( !Class ) {
        DPrintf("SSTLoadClass:Failed to allocate memory for class\n");
        return NULL;
    }
    Class->LabelList = NULL;
    Class->CallbackList = NULL;
    Class->VideoInfo = NULL;
    Class->RSCList = NULL;
    Class->ImageList = NULL;
    Class->GFXModelList = NULL;
    Class->VRAM = NULL;
    Class->Next = NULL;
    Class->LabelsVAO = NULL;
    Class->NumLabels = 0;
    memcpy(&Class->Name,*SSTBuffer,sizeof(Class->Name));
    *SSTBuffer += sizeof(Class->Name);
    DPrintf("SSTLoadClass:Class Name is %s\n",Class->Name);

    RSCPathList = SSTGetRSCPathListFromClassName(Class, GameEngine, &NumRSCPath);
    if( RSCPathList != NULL ) {
        for (int i = 0; i < NumRSCPath; i++ ) {
            Temp = NormalizePathSeparators(RSCPathList[i]);
            if( !Temp ) {
                DPrintf("SSTLoadClass: Failed to normalize path %s\n",RSCPathList[i]);
                continue;
            }
            DPrintf("SSTLoadClass: Assets should be loaded from %s\n",Temp );
            asprintf(&DataPath,"%s%cDATA%c%s",BasePath,PATH_SEPARATOR,PATH_SEPARATOR,Temp);
            DPrintf("SSTLoadClass:Loading asset file from %s\n",DataPath);
            RSC = RSCLoad(DataPath);
            if( RSC ) {
                //Link it in!
                RSCAppend(&Class->RSCList,RSC);
            } else {
                DPrintf("SSTLoadClass:Failed to load RSC file %s\n",DataPath);
            }
            free(Temp);
            free(DataPath);
        }
    } else {
        //NOTE(Adriano): Movies file are the only exception since they always use the CD path
        //movie/*/*.STR and are not inside any RSC file...
        if( !StringStartsWith(Class->Name, "m_movie" )) {
            DPrintf("SSTLoadClass: Missing map for class %s\n",Class->Name);
            assert(1!=1);
        }
    }
    if( SST->ClassList != NULL ) {
        DPrintf("SSTLoadClass: Multiple class detected for script %s...\n",SST->Name);
    }
    //Link it in!
    Class->Next = SST->ClassList;
    SST->ClassList = Class;
    return Class;
}

SST_t *SSTLoad(Byte *SSTBuffer,const char *ScriptName,const char *BasePath,const RSC_t *GlobalRSCList,int GameEngine)
{
    SST_t *SST;
    SSTClass_t *CurrentClass;
    SSTCallback_t *LastCallback;
    int Size;
    int Token;
    
    if( !SSTBuffer ) {
        DPrintf("SSTLoad:Invalid data.\n");
        return NULL;
    }
    DPrintf("SSTLoad:Loading it\n");
    
    SST = malloc(sizeof(SST_t));
    
    if( !SST ) {
        DPrintf("SSTLoad:Failed to allocate memory for struct\n");
        return NULL;
    }
    SST->Name = StringCopy(ScriptName);
    SST->ClassList = NULL;
    SST->Next = NULL;
    CurrentClass = NULL;

    DPrintf("SSTLoad:Loading script %s\n",SST->Name);
    
    while( 1 ) {
        if( !*SSTBuffer ) {
            DPrintf("SSTLoad:EOF reached or an error occurred...\n");
            break;
        }
        Token = *(int *) SSTBuffer;
        SSTBuffer += 4;
        DPrintf("SSTLoad:Got token %i\n",Token);
        switch( Token ) {
            case SST_CLASS_TOKEN:
                CurrentClass = SSTLoadClass(SST,&SSTBuffer,BasePath,GameEngine);
                if( !CurrentClass ) {
                    DPrintf("SSTLoad:Failed to allocate class...\n");
                    return NULL;
                }
                break;
            case SST_CALLBACK_TOKEN:
                LastCallback = SSTLoadCallback(SST,CurrentClass,&SSTBuffer);
                break;
            case SST_LABEL_TOKEN:
                SSTLoadLabel(SST,CurrentClass,LastCallback,GlobalRSCList,&SSTBuffer,GameEngine);
                break;
            case SST_BACKDROP_TOKEN:
                LastCallback = NULL;
                break;
            case SST_STR_FILE_TOKEN:
                SSTLoadVideoInfo(SST,CurrentClass,&SSTBuffer);
                break;
            case SST_UNKNOWN_1_TOKEN:
                DPrintf("SSTLoad:Skipping unk1 276\n");
                SSTBuffer += 276;
                break;
            case SST_UNKNOWN_2_TOKEN:
                memcpy(&Size,SSTBuffer,sizeof(Size));
                SSTBuffer += sizeof(Size);
                SSTBuffer += (4*Size) + 4;
                DPrintf("SSTLoad:Skipping unk2\n");
                break;
            case SST_GFX_TOKEN:
                SSTLoadGFXModel(SST,CurrentClass,GlobalRSCList,&SSTBuffer);
                break;
            case SST_UNKNOWN_3_TOKEN:
                //After a GFX Model we have this token.
                DPrintf("SSTLoad:Skipping unk3 288\n");
                SSTBuffer += 288;
                break;
            default:
                DPrintf("SSTLoad:Unknown token %i\n",Token);
                assert(1!=1);
                break;
        }
    }
    //qsort( &Label, NumLabels, sizeof(SSTLabel_t), SSTCompare );
    return SST;
}
