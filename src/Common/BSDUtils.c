// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
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

#include "BSDUtils.h"

void BSDRenderObjectResetFrameQuaternionList(BSDAnimationFrame_t *Frame)
{
    if( !Frame ) {
        return;
    }
    memcpy(Frame->CurrentQuaternionList,Frame->QuaternionList,Frame->NumQuaternions * sizeof(BSDQuaternion_t));
}

void BSDRecursivelyApplyHierachyData(const BSDHierarchyBone_t *Bone,const BSDQuaternion_t *QuaternionList,BSDVertexTable_t *VertexTable,
                                     mat4 TransformMatrix)
{
    versor Quaternion;
    mat4 LocalRotationMatrix;
    mat4 LocalTranslationMatrix;
    mat4 LocalTransformMatrix;
    vec3 TransformedBonePosition;
    vec3 TransformedVertexPosition;
    vec3 Temp;
    int i;
    
    if( !Bone ) {
        DPrintf("BSDRecursivelyApplyHierachyData:NULL Bone.\n");
        return;
    }
    
    if( !QuaternionList ) {
        DPrintf("BSDRecursivelyApplyHierachyData:Invalid Quaternion List.\n");
        return;
    }
    if( !VertexTable ) {
        DPrintf("BSDRecursivelyApplyHierachyData:Invalid Vertex Table.\n");
        return;
    }

    glm_mat4_identity(LocalRotationMatrix);
    glm_mat4_identity(LocalTranslationMatrix);
    glm_mat4_identity(LocalTransformMatrix);
    
    Quaternion[0] = QuaternionList[Bone->VertexTableIndex].x / 4096.f;
    Quaternion[1] = QuaternionList[Bone->VertexTableIndex].y / 4096.f;
    Quaternion[2] = QuaternionList[Bone->VertexTableIndex].z / 4096.f;
    Quaternion[3] = QuaternionList[Bone->VertexTableIndex].w / 4096.f;
    
    glm_quat_mat4t(Quaternion,LocalRotationMatrix);
    
    Temp[0] = Bone->Position.x;
    Temp[1] = Bone->Position.y;
    Temp[2] = Bone->Position.z;
    
    glm_mat4_mulv3(TransformMatrix,Temp,1.f,TransformedBonePosition);
    glm_translate_make(LocalTranslationMatrix,TransformedBonePosition);
    glm_mat4_mul(LocalTranslationMatrix,LocalRotationMatrix,LocalTransformMatrix);

    if( VertexTable[Bone->VertexTableIndex].Offset != -1 && VertexTable[Bone->VertexTableIndex].NumVertex != 0 ) {
        for( i = 0; i < VertexTable[Bone->VertexTableIndex].NumVertex; i++ ) {
            Temp[0] = VertexTable[Bone->VertexTableIndex].VertexList[i].x;
            Temp[1] = VertexTable[Bone->VertexTableIndex].VertexList[i].y;
            Temp[2] = VertexTable[Bone->VertexTableIndex].VertexList[i].z;
            glm_mat4_mulv3(LocalTransformMatrix,Temp,1.f,TransformedVertexPosition);
            VertexTable[Bone->VertexTableIndex].VertexList[i].x = TransformedVertexPosition[0];
            VertexTable[Bone->VertexTableIndex].VertexList[i].y = TransformedVertexPosition[1];
            VertexTable[Bone->VertexTableIndex].VertexList[i].z = TransformedVertexPosition[2];
        }
    }

    if( Bone->Child2 ) {
        BSDRecursivelyApplyHierachyData(Bone->Child2,QuaternionList,VertexTable,TransformMatrix);
    }
    if( Bone->Child1 ) {
        BSDRecursivelyApplyHierachyData(Bone->Child1,QuaternionList,VertexTable,LocalTransformMatrix);
    }
}

bool BSDReadTSPInfoBlock(FILE *BSDFile, BSDTSPInfo_t *BSDTSPInfo)
{
    
    if( !BSDFile ) {
        DPrintf("BSDReadTSPInfoBlock: Invalid file\n");
        return false;
    }
    if( !BSDTSPInfo ) {
        DPrintf("BSDReadTSPInfoBlock: Invalid data\n");
        return false;
    }
    if(GetCurrentFilePosition(BSDFile) != BSD_HEADER_SIZE ) {
        fseek(BSDFile, BSD_HEADER_SIZE, SEEK_SET);
    }
    fread(BSDTSPInfo,sizeof(BSDTSPInfo_t),1,BSDFile);
    DPrintf("BSDReadTSPInfoBlock:Reading TSP info...\n");
    DPrintf("Compartment pattern: %s\n",BSDTSPInfo->TSPPattern);
    DPrintf("Number of compartments: %i\n",BSDTSPInfo->NumTSP);
    DPrintf("TargetInitialCompartment: %i\n",BSDTSPInfo->TargetInitialCompartment);
    DPrintf("Starting Compartment: %i\n",BSDTSPInfo->StartingComparment);
    DPrintf("u3: %i\n",BSDTSPInfo->u3);
    DPrintf("TSP Block ends at %i\n",GetCurrentFilePosition(BSDFile));
    assert(BSDTSPInfo->u3 == 0);
    return true;
}
