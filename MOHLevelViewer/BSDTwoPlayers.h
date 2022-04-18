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

#ifndef __BSD_TWOPLAYERS_H_
#define __BSD_TWOPLAYERS_H_

#include "Common.h"
#include "VAO.h"
#include "VRAM.h"
#include "BSD.h"

//Actually this is true for both SinglePlayer/TwoPlayers.
#define BSD2P_ENTRY_TABLE_POSITION 0x53C
#define BSD2P_ENTRY_TABLE_SIZE     0x68

typedef struct BSD2P_s {
    BSD_Header_t Header;
    int          NodeOffset;
    BSDRenderObjectBlock_t  RenderObjectTable;
    BSDNodeInfo_t NodeData;
    
    VAO_t       *NodeVAO;
//     BSDTSPInfo_t TSPInfo;
//     char Unknown[72];
//     BSDTable_t   PTable;
//     BSDEntryTable_t EntryTable;
//     BSDUBlock_t UBlock;
//     BSDNodeInfo_t NodeData;
//     BSDPropertySetFile_t PropertySetFile;
//     BSDUObject_t *UObjectList;
//     
//     BSDUObject_t *URealObjectList;
//     //
//     VAO_t       *NodeVAO;
//     VAO_t       *NodeBoxVAO;
//     //Easy lookup to see if the camera is in that node position so that
//     //we can update the TSPNumberRenderList.
//     BSDTSPStreamNode_t *TSPStreamNodeList;
} BSD2P_t;

typedef struct Level_s Level_t;
void      BSD2PVAOPointList(BSD2P_t *BSD);
void      BSD2PDraw(Level_t *Level);
BSD2P_t  *BSD2PLoad(char *FName,int MissionNumber);
void      BSD2PFree(BSD2P_t *BSD);
// void    BSDCheckCompartmentTrigger(Level_t *Level,Vec3_t CameraPosition);
// char   *BSDNodeGetEnumStringFromValue(unsigned int Value);
// Vec3_t  BSDGetPlayerSpawn(BSD_t *BSD);
// void    BSDVAOBoxList(BSD_t *BSD);
// void    BSDVAOPointList(BSD_t *BSD);
// void    BSDVAOObjectList(BSD_t *BSD);
// void    BSDVAOTexturedObjectList(BSD_t *BSD);
// void    BSDSpawnNodes(BSD_t *BSD);
// void    BSDDraw(Level_t *Level);
// unsigned int BSDNodeIDToUBlockID(unsigned int NodeID);
// unsigned int BSDMPNodeIDToUBlockID(unsigned int NodeID);
// int     BSDGetUBlockIndexByID(BSD_t *BSD,int ID);
// bool    BSDIsUBlockPresent(BSD_t *BSD,unsigned int UBlockID);


#endif //__BSD_TWOPLAYERS_H_
