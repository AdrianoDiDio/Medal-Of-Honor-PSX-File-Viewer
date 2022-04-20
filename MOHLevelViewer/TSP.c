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

#include "TSP.h"
#include "ShaderManager.h"
#include "MOHLevelViewer.h"

void TSPFree(TSP_t *TSP)
{
    TSPRenderingFace_t *Temp;
    int i;
//     TSP->Node
//     TSPRecursiveNodeFree(&TSP->Node[0]);
    for( i = 0; i < TSP->Header.NumNodes; i++ ) {
        VAOFree(TSP->Node[i].BBoxVAO);
        VAOFree(TSP->Node[i].OpaqueFacesVAO);
        VAOFree(TSP->Node[i].LeafCollisionFaceListVAO);
        while( TSP->Node[i].OpaqueFaceList ) {
            Temp = TSP->Node[i].OpaqueFaceList;
            TSP->Node[i].OpaqueFaceList = TSP->Node[i].OpaqueFaceList->Next;
            free(Temp);
        }
        if( TSP->Node[i].FaceList ) {
            free(TSP->Node[i].FaceList);
        }
        //Don't bother traversing the tree since we can walk the whole array and free all the pointers...
        TSP->Node[i].Child[0] = NULL;
        TSP->Node[i].Child[1] = NULL;
        TSP->Node[i].Next = NULL;
    }
    free(TSP->Node);
    if( TSP->Face ) {
        free(TSP->Face);
    }
    if( TSP->TextureData ) {
        free(TSP->TextureData);
    }
    free(TSP->Vertex);
    free(TSP->Color);
    
    if( TSP->Header.NumDynamicDataBlock != 0 ) {
        for( i = 0; i < TSP->Header.NumDynamicDataBlock; i++ ) {
            free(TSP->DynamicData[i].FaceIndexList);
            free(TSP->DynamicData[i].FaceDataList);
        }
        free(TSP->DynamicData);
    }
    
    free(TSP->CollisionData->KDTree);
    free(TSP->CollisionData->FaceIndexList);
    free(TSP->CollisionData->Vertex);
    free(TSP->CollisionData->Normal);
    free(TSP->CollisionData->Face);
    free(TSP->CollisionData);
    
    while( TSP->TransparentFaceList ) {
        Temp = TSP->TransparentFaceList;
        TSP->TransparentFaceList = TSP->TransparentFaceList->Next;
        free(Temp);
    }
    VAOFree(TSP->VAOList);
    VAOFree(TSP->CollisionVAOList);
    VAOFree(TSP->TransparentVAO);
    free(TSP);
}

void TSPFreeList(TSP_t *List)
{
    TSP_t *Temp;
    while( List ) {
        Temp = List;
        List = List->Next;
        TSPFree(Temp);
    }
}

bool TSPIsVersion3(TSP_t *TSP)
{
    return TSP->Header.Version == 3;
}

void Vec4FromXYZ(float x,float y,float z,vec4 Out)
{
    Out[0] = x;
    Out[1] = y;
    Out[2] = z;
    Out[3] = 1;
}
void TSPDumpFaceDataToFile(TSP_t *TSP,FILE *OutFile)
{
    char Buffer[256];
    float TextureWidth;
    float TextureHeight;
    int i;
    
    if( !TSP || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        printf("TSPDumpFaceDataToFile: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    
    TextureWidth = LevelManager->CurrentLevel->VRAM->Page.Width;
    TextureHeight = LevelManager->CurrentLevel->VRAM->Page.Height;

    for( i = TSP->Header.NumFaces - 1; i >= 0 ; i-- ) {
        int ColorMode = (TSP->Face[i].TSB >> 7) & 0x3;
        int VRAMPage = TSP->Face[i].TSB & 0x1F;
        float U0 = (((float)TSP->Face[i].UV0.u + VRAMGetTexturePageX(VRAMPage))/TextureWidth);
        float V0 = /*255 -*/1.f-(((float)TSP->Face[i].UV0.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        float U1 = (((float)TSP->Face[i].UV1.u + VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
        float V1 = /*255 -*/1.f-(((float)TSP->Face[i].UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) /TextureHeight);
        float U2 = (((float)TSP->Face[i].UV2.u + VRAMGetTexturePageX(VRAMPage)) /TextureWidth);
        float V2 = /*255 -*/1.f-(((float)TSP->Face[i].UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
        sprintf(Buffer,"vt %f %f\nvt %f %f\nvt %f %f\n",U0,V0,U1,V1,U2,V2);
        fwrite(Buffer,strlen(Buffer),1,OutFile);    
    }
    for( i = 0; i < TSP->Header.NumFaces; i++ ) {
        int Vert0 = TSP->Face[i].V0;
        int Vert1 = TSP->Face[i].V1;
        int Vert2 = TSP->Face[i].V2;
        int BaseFaceUV = i * 3;
        sprintf(Buffer,"usemtl vram\n");
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        sprintf(Buffer,"f %i/%i %i/%i %i/%i\n",-(Vert0+1),-(BaseFaceUV+3),-(Vert1+1),-(BaseFaceUV+2),-(Vert2+1),-(BaseFaceUV+1));
        fwrite(Buffer,strlen(Buffer),1,OutFile);
    }
}
void TSPDumpFaceV3DataToFile(TSP_t *TSP,FILE *OutFile)
{
    char Buffer[256];
    TSPTextureInfo_t TextureInfo;
    float TextureWidth;
    float TextureHeight;
    int i;
    int j;
    
    if( !TSP || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        printf("TSPDumpFaceV3DataToFile: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    
    TextureWidth = LevelManager->CurrentLevel->VRAM->Page.Width;
    TextureHeight = LevelManager->CurrentLevel->VRAM->Page.Height;

    
    for( i = 0; i < TSP->Header.NumNodes; i++ ) {
        if( TSP->Node[i].NumFaces == 0 ) {
            continue;
        }
        for( j = TSP->Node[i].NumFaces - 1; j >= 0 ; j-- ) {
            TextureInfo = TSP->TextureData[TSP->Node[i].FaceList[j].TextureDataIndex];
            int ColorMode = (TextureInfo.TSB >> 7) & 0x3;
            int VRAMPage = TextureInfo.TSB & 0x1F;
            float U0 = (((float)TextureInfo.UV0.u + VRAMGetTexturePageX(VRAMPage))/TextureWidth);
            float V0 = /*255 -*/1.f-(((float)TextureInfo.UV0.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
            float U1 = (((float)TextureInfo.UV1.u + VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
            float V1 = /*255 -*/1.f-(((float)TextureInfo.UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) /TextureHeight);
            float U2 = (((float)TextureInfo.UV2.u + VRAMGetTexturePageX(VRAMPage)) /TextureWidth);
            float V2 = /*255 -*/1.f-(((float)TextureInfo.UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
            sprintf(Buffer,"vt %f %f\nvt %f %f\nvt %f %f\n",U0,V0,U1,V1,U2,V2);
            fwrite(Buffer,strlen(Buffer),1,OutFile);
        }
        for( j = 0; j < TSP->Node[i].NumFaces; j++ ) {

            int Vert0 = TSP->Node[i].FaceList[j].Vert0;
            int Vert1 = TSP->Node[i].FaceList[j].Vert1;
            int Vert2 = TSP->Node[i].FaceList[j].Vert2;
            int BaseFaceUV = j * 3;
            sprintf(Buffer,"usemtl vram\n");
            fwrite(Buffer,strlen(Buffer),1,OutFile);
            sprintf(Buffer,"f %i/%i %i/%i %i/%i\n",-(Vert0+1),-(BaseFaceUV+3),-(Vert1+1),-(BaseFaceUV+2),-(Vert2+1),-(BaseFaceUV+1));
            fwrite(Buffer,strlen(Buffer),1,OutFile);
        }
    }
}
void TSPDumpDataToFile(TSP_t *TSPList,FILE* OutFile)
{
    TSP_t *Iterator;
    char Buffer[256];
    int i;
    int t;
    Vec3_t NewPos;
    
    if( !TSPList || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        printf("TSPDumpDataToFile: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    t = 1;
    for( Iterator = TSPList; Iterator; Iterator = Iterator->Next, t++ ) {
        sprintf(Buffer,"o TSP%i\n",t);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        for( i = Iterator->Header.NumVertices - 1; i >= 0 ; i-- ) {
            NewPos = Vec3Build(Iterator->Vertex[i].Position.x,Iterator->Vertex[i].Position.y,Iterator->Vertex[i].Position.z);
            Vec3RotateXAxis(DEGTORAD(180.f),&NewPos);
            sprintf(Buffer,"v %f %f %f %f %f %f\n",NewPos.x / 4096.f,NewPos.y / 4096.f,NewPos.z / 4096.f,
                Iterator->Color[i].rgba[0] / 255.f,Iterator->Color[i].rgba[1] / 255.f,Iterator->Color[i].rgba[2] / 255.f
            );
            fwrite(Buffer,strlen(Buffer),1,OutFile);            
        }
        if( TSPIsVersion3(Iterator) ) {
            TSPDumpFaceV3DataToFile(Iterator,OutFile);
        } else {
            TSPDumpFaceDataToFile(Iterator,OutFile);
        }
//         fwrite(Buffer,strlen(Buffer),1,OutFile);
//         break;
    }
}
void TSPDumpFaceDataToPlyFile(TSP_t *TSP,int VertexOffset,FILE *OutFile)
{
    char Buffer[256];

    int i;

    if( !TSP || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        printf("TSPDumpFaceDataToFile: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    for( i = 0; i < TSP->Header.NumFaces; i++ ) {

        int Vert0 = VertexOffset + (i * 3) + 0;
        int Vert1 = VertexOffset + (i * 3) + 1;
        int Vert2 = VertexOffset + (i * 3) + 2;
        sprintf(Buffer,"3 %i %i %i\n",Vert0,Vert1,Vert2);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
    }
}
int TSPDumpFaceV3DataToPlyFile(TSP_t *TSP,int VertexOffset,FILE *OutFile)
{
    char Buffer[256];
    int NumRenderedNodes;
    int i;
    int j;
    int FaceOffset;
    if( !TSP || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        printf("TSPDumpFaceDataToFile: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return -1;
    }
    
    NumRenderedNodes = 0;
    FaceOffset = 0;
    for( i = 0; i < TSP->Header.NumNodes; i++ ) {
        if( TSP->Node[i].NumFaces == 0 ) {
            continue;
        }

          for( j = 0; j < TSP->Node[i].NumFaces; j++ ) {
              int Vert0 = VertexOffset + FaceOffset + (j * 3) + 0;
              int Vert1 = VertexOffset + FaceOffset + (j * 3) + 1;
              int Vert2 = VertexOffset + FaceOffset + (j * 3) + 2;
              sprintf(Buffer,"3 %i %i %i\n",Vert0,Vert1,Vert2);
              fwrite(Buffer,strlen(Buffer),1,OutFile);
          }
        FaceOffset += TSP->Node[i].NumFaces * 3;
        NumRenderedNodes++;
    }
    return FaceOffset;
}

void TSPDumpDataToPlyFile(TSP_t *TSPList,FILE* OutFile)
{
    TSP_t *Iterator;
    char Buffer[256];
    int i;
    int j;
    Vec3_t NewPos;
    int FaceCount;
    int VertexCount;
    int VertexOffset;
    
    if( !TSPList || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        printf("TSPDumpDataToPlyFile: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    sprintf(Buffer,"ply\nformat ascii 1.0\n");
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    VertexCount = 0;
    FaceCount = 0;
    for( Iterator = TSPList; Iterator; Iterator = Iterator->Next ) {
        VertexCount += Iterator->Header.NumVertices;
        if( TSPIsVersion3(Iterator) ) {
            for( i = 0; i < Iterator->Header.NumNodes; i++ ) {
                FaceCount += Iterator->Node[i].NumFaces;
            }
        } else {
            FaceCount += Iterator->Header.NumFaces;
        }
    }
    sprintf(Buffer,
        "element vertex %i\nproperty float x\nproperty float y\nproperty float z\nproperty float red\nproperty float green\nproperty float blue\nproperty float s\nproperty float t\n",FaceCount * 3);
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    sprintf(Buffer,"element face %i\nproperty list uchar int vertex_indices\nend_header\n",FaceCount);
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    VertexOffset = 0;
    for( Iterator = TSPList; Iterator; Iterator = Iterator->Next ) {
        float TextureWidth;
        float TextureHeight;
        TextureWidth = LevelManager->CurrentLevel->VRAM->Page.Width;
        TextureHeight = LevelManager->CurrentLevel->VRAM->Page.Height;
        if( TSPIsVersion3(Iterator) ) {
            for( i = 0; i < Iterator->Header.NumNodes; i++ ) {
                if( Iterator->Node[i].NumFaces == 0 ) {
                    continue;
                }
                for( j = 0; j < Iterator->Node[i].NumFaces; j++ ) {
                    TSPTextureInfo_t TextureInfo = Iterator->TextureData[Iterator->Node[i].FaceList[j].TextureDataIndex];
                    int ColorMode = (TextureInfo.TSB >> 7) & 0x3;
                    int VRAMPage = TextureInfo.TSB & 0x1F;
                    float U0 = (((float)TextureInfo.UV0.u + VRAMGetTexturePageX(VRAMPage))/TextureWidth);
                    float V0 = /*255 -*/1.f-(((float)TextureInfo.UV0.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
                    float U1 = (((float)TextureInfo.UV1.u + VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
                    float V1 = /*255 -*/1.f-(((float)TextureInfo.UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) /TextureHeight);
                    float U2 = (((float)TextureInfo.UV2.u + VRAMGetTexturePageX(VRAMPage)) /TextureWidth);
                    float V2 = /*255 -*/1.f-(((float)TextureInfo.UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
                    int Vert0 = Iterator->Node[i].FaceList[j].Vert0;
                    int Vert1 = Iterator->Node[i].FaceList[j].Vert1;
                    int Vert2 = Iterator->Node[i].FaceList[j].Vert2;
                    NewPos = Vec3Build(Iterator->Vertex[Vert0].Position.x,Iterator->Vertex[Vert0].Position.y,Iterator->Vertex[Vert0].Position.z);
                    Vec3RotateXAxis(DEGTORAD(180.f),&NewPos);
                    sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",NewPos.x / 4096.f,NewPos.y / 4096.f,NewPos.z / 4096.f,
                            Iterator->Color[Vert0].rgba[0] / 255.f,Iterator->Color[Vert0].rgba[1] / 255.f,Iterator->Color[Vert0].rgba[2] / 255.f,
                            U0,V0
                    );
                    fwrite(Buffer,strlen(Buffer),1,OutFile);
                    NewPos = Vec3Build(Iterator->Vertex[Vert1].Position.x,Iterator->Vertex[Vert1].Position.y,Iterator->Vertex[Vert1].Position.z);
                    Vec3RotateXAxis(DEGTORAD(180.f),&NewPos);
                    sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",NewPos.x / 4096.f,NewPos.y / 4096.f,NewPos.z / 4096.f,
                            Iterator->Color[Vert1].rgba[0] / 255.f,Iterator->Color[Vert1].rgba[1] / 255.f,Iterator->Color[Vert1].rgba[2] / 255.f,
                            U1,V1
                    );
                    fwrite(Buffer,strlen(Buffer),1,OutFile);      
                    NewPos = Vec3Build(Iterator->Vertex[Vert2].Position.x,Iterator->Vertex[Vert2].Position.y,Iterator->Vertex[Vert2].Position.z);
                    Vec3RotateXAxis(DEGTORAD(180.f),&NewPos);
                    sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",NewPos.x / 4096.f,NewPos.y / 4096.f,NewPos.z / 4096.f,
                            Iterator->Color[Vert2].rgba[0] / 255.f,Iterator->Color[Vert2].rgba[1] / 255.f,Iterator->Color[Vert2].rgba[2] / 255.f,
                            U2,V2
                    );
                    fwrite(Buffer,strlen(Buffer),1,OutFile); 
                }
            }
        } else {
            for( i = 0; i < Iterator->Header.NumFaces; i++ ) {
                int Vert0 = Iterator->Face[i].V0;
                int Vert1 = Iterator->Face[i].V1;
                int Vert2 = Iterator->Face[i].V2;
                int ColorMode = (Iterator->Face[i].TSB >> 7) & 0x3;
                int VRAMPage = Iterator->Face[i].TSB & 0x1F;
                float U0 = (((float)Iterator->Face[i].UV0.u + VRAMGetTexturePageX(VRAMPage))/TextureWidth);
                float V0 = /*255 -*/1.f-(((float)Iterator->Face[i].UV0.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);
                float U1 = (((float)Iterator->Face[i].UV1.u + VRAMGetTexturePageX(VRAMPage)) / TextureWidth);
                float V1 = /*255 -*/1.f-(((float)Iterator->Face[i].UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) /TextureHeight);
                float U2 = (((float)Iterator->Face[i].UV2.u + VRAMGetTexturePageX(VRAMPage)) /TextureWidth);
                float V2 = /*255 -*/1.f-(((float)Iterator->Face[i].UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode)) / TextureHeight);

                NewPos = Vec3Build(Iterator->Vertex[Vert0].Position.x,Iterator->Vertex[Vert0].Position.y,Iterator->Vertex[Vert0].Position.z);
                Vec3RotateXAxis(DEGTORAD(180.f),&NewPos);
                sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",NewPos.x / 4096.f,NewPos.y / 4096.f,NewPos.z / 4096.f,
                        Iterator->Color[Vert0].rgba[0] / 255.f,Iterator->Color[Vert0].rgba[1] / 255.f,Iterator->Color[Vert0].rgba[2] / 255.f,
                        U0,V0
                );
                fwrite(Buffer,strlen(Buffer),1,OutFile);
                NewPos = Vec3Build(Iterator->Vertex[Vert1].Position.x,Iterator->Vertex[Vert1].Position.y,Iterator->Vertex[Vert1].Position.z);
                Vec3RotateXAxis(DEGTORAD(180.f),&NewPos);
                sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",NewPos.x / 4096.f,NewPos.y / 4096.f,NewPos.z / 4096.f,
                        Iterator->Color[Vert1].rgba[0] / 255.f,Iterator->Color[Vert1].rgba[1] / 255.f,Iterator->Color[Vert1].rgba[2] / 255.f,
                        U1,V1
                );
                fwrite(Buffer,strlen(Buffer),1,OutFile);      
                NewPos = Vec3Build(Iterator->Vertex[Vert2].Position.x,Iterator->Vertex[Vert2].Position.y,Iterator->Vertex[Vert2].Position.z);
                Vec3RotateXAxis(DEGTORAD(180.f),&NewPos);
                sprintf(Buffer,"%f %f %f %f %f %f %f %f\n",NewPos.x / 4096.f,NewPos.y / 4096.f,NewPos.z / 4096.f,
                        Iterator->Color[Vert2].rgba[0] / 255.f,Iterator->Color[Vert2].rgba[1] / 255.f,Iterator->Color[Vert2].rgba[2] / 255.f,
                        U2,V2
                );
                fwrite(Buffer,strlen(Buffer),1,OutFile);      
            }
        }
   }
    
    for( Iterator = TSPList; Iterator; Iterator = Iterator->Next ) {
        if( TSPIsVersion3(Iterator) ) {
            VertexOffset += TSPDumpFaceV3DataToPlyFile(Iterator,VertexOffset,OutFile);
        } else {
            TSPDumpFaceDataToPlyFile(Iterator,VertexOffset,OutFile);
            VertexOffset += Iterator->Header.NumFaces * 3;
        }
    }

}

bool TSPBoxInFrustum(ViewParm_t Camera,TSPBBox_t BBox)
{
    vec4 BoxCornerList[8];
    int BoxOutsideCount;
    int i;
    int j;
    
    Vec4FromXYZ(BBox.Min.x,BBox.Min.y,BBox.Min.z,BoxCornerList[0]);
    Vec4FromXYZ(BBox.Max.x,BBox.Min.y,BBox.Min.z,BoxCornerList[1]);
    Vec4FromXYZ(BBox.Min.x,BBox.Max.y,BBox.Min.z,BoxCornerList[2]);
    Vec4FromXYZ(BBox.Max.x,BBox.Max.y,BBox.Min.z,BoxCornerList[3]);
    Vec4FromXYZ(BBox.Min.x,BBox.Min.y,BBox.Max.z,BoxCornerList[4]);
    Vec4FromXYZ(BBox.Max.x,BBox.Min.y,BBox.Max.z,BoxCornerList[5]);
    Vec4FromXYZ(BBox.Min.x,BBox.Max.y,BBox.Max.z,BoxCornerList[6]);
    Vec4FromXYZ(BBox.Max.x,BBox.Max.y,BBox.Max.z,BoxCornerList[7]);

    for( i = 0; i < 6; i++ ) {
        BoxOutsideCount = 0;
        for( j = 0; j < 8; j++ ) {
            if( glm_vec4_dot(Camera.FrustumPlaneList[i],BoxCornerList[j]) < 0.f ) {
                BoxOutsideCount++;
            }
        }
        if( BoxOutsideCount == 8 ) {
            return false;
        }
    }
    return true;
}
TSPDynamicFaceData_t *GetDynamicDataByFaceIndex(TSP_t *TSP,int FaceIndex,int Stride)
{
    int i;
    int j;
    int FaceDataStride;
    if( TSP->Header.NumDynamicDataBlock == 0 ) {
        return NULL;
    }
    for( i = 0; i < TSP->Header.NumDynamicDataBlock; i++ ) {
        for( j = 0; j < TSP->DynamicData[i].Header.NumFacesIndex; j++ ) {
            if( TSP->DynamicData[i].FaceIndexList[j] == FaceIndex ) {
//                 if( Stride < TSP->DynamicData[i].Header.FaceDataSizeMultiplier ) {
//                     FaceDataStride = Stride;
//                 } else {
                    FaceDataStride = Stride - 1;
//                 }
                return &TSP->DynamicData[i].FaceDataList[j + (TSP->DynamicData[i].Header.NumFacesIndex * FaceDataStride)];
            }
        }
    }
    return NULL;
}
int TSPGetNodeTransparentFaceCount(TSP_t *TSP,TSPNode_t *Node)
{
    TSPTextureInfo_t TextureInfo;
    int Base;
    int Target;
    int i;
    int Result;
    if( !Node ) {
        DPrintf("TSPGetTransparentNodeFaceCount:Invalid Node data\n");
        return 0;
    }
    if( Node->NumFaces <= 0 ) {
        DPrintf("TSPGetTransparentNodeFaceCount:Node is not a leaf...\n");
        return 0;
    }
    Result = 0;
    if( TSPIsVersion3(TSP) ) {
        for( i = 0; i < Node->NumFaces; i++ ) {
            TextureInfo = TSP->TextureData[Node->FaceList[i].TextureDataIndex];
            if( (TextureInfo.TSB & 0x4000) != 0 ) {
                Result++;
            }
        }
    } else {
        Base = Node->BaseData / sizeof(TSPFace_t);
        Target = Base + Node->NumFaces;
        for( i = Base; i < Target; i++ ) {
            if( (TSP->Face[i].TSB & 0x4000 ) != 0 ) {
                Result++;
            }
        }
    }
    return Result;
}

int TSPGetColorIndex(int Color)
{
    return Color & 0xFF00FF;
}

void TSPCreateFaceVAO(TSP_t *TSP,TSPNode_t *Node)
{
    int Base;
    int Target;
    int Stride;
    int Vert0;
    int Vert1;
    int Vert2;
    int U0,V0;
    int U1,V1;
    int U2,V2;
    int *VertexData;
    int *TransparentVertexData;
    int TotalVertexSize;
    int VertexSize;
    int TransparentVertexSize;
    int VertexPointer;
    int TransparentVertexPointer;
    int VertexOffset;
    int TextureOffset;
    int ColorOffset;
    int CLUTOffset;
    int ColorModeOffset;
    int CLUTPosX;
    int CLUTPosY;
    int CLUTDestX;
    int CLUTDestY;
    int CLUTPage;
    int NumTransparentFaces;
    int ColorMode;
    int VRAMPage;
    int ABRRate;
//     TSPDynamicFaceData_t *DynamicData;
    TSPRenderingFace_t *RenderingFace;
    VAO_t *VAO;
    int i;
    
    Base = Node->BaseData / sizeof(TSPFace_t);
    Target = Base + Node->NumFaces;
//            XYZ UV RGB CLUT ColorMode
    Stride = (3 + 2 + 3 + 2 + 1) * sizeof(int);
                
    VertexOffset = 0;
    TextureOffset = 3;
    ColorOffset = 5;
    CLUTOffset = 8;
    ColorModeOffset = 10;
                
    NumTransparentFaces = TSPGetNodeTransparentFaceCount(TSP,Node);
    VertexSize = Stride * 3;
    TotalVertexSize = VertexSize * (Node->NumFaces - NumTransparentFaces);
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    TransparentVertexSize = Stride * 3;
    TransparentVertexData = malloc(TransparentVertexSize);
    TransparentVertexPointer = 0;
    VAO = VAOInitXYZUVRGBCLUTColorModeInteger(NULL,TotalVertexSize,Stride,VertexOffset,TextureOffset,ColorOffset,CLUTOffset,ColorModeOffset,
                                              (Node->NumFaces - NumTransparentFaces) * 3);
    Node->OpaqueFacesVAO = VAO;
    for( i = Base; i < Target; i++ ) {
        ColorMode = (TSP->Face[i].TSB >> 7) & 0x3;
        VRAMPage = TSP->Face[i].TSB & 0x1F;
        ABRRate = (TSP->Face[i].TSB & 0x60) >> 5;
        
        RenderingFace = malloc(sizeof(TSPRenderingFace_t));
        RenderingFace->Flags = 0;
        RenderingFace->Next = NULL;
        
        CLUTPosX = (TSP->Face[i].CBA << 4) & 0x3F0;
        CLUTPosY = (TSP->Face[i].CBA >> 6) & 0x1ff;
        CLUTPage = VRAMGetCLUTPage(CLUTPosX,CLUTPosY);
        CLUTDestX = VRAMGetCLUTPositionX(CLUTPosX,CLUTPosY,CLUTPage);
        CLUTDestY = CLUTPosY + VRAMGetCLUTOffsetY(ColorMode);
        CLUTDestX += VRAMGetTexturePageX(CLUTPage);

        DPrintf("TSB is %u\n",TSP->Face[i].TSB);
        DPrintf("Expected VRam Page:%i\n",VRAMPage);
        DPrintf("Expected Color Mode:%i\n",ColorMode);
        DPrintf("Expected ABR rate:%i\n",ABRRate);
        
        Vert0 = TSP->Face[i].V0;
        Vert1 = TSP->Face[i].V1;
        Vert2 = TSP->Face[i].V2;
        
//         DynamicData = GetDynamicDataByFaceIndex(TSP,j,2);
//         if( DynamicData != NULL ) {
//             U0 = (((float)DynamicData->UV0.u)/Width);
//             V0 = /*255 -*/(((float)DynamicData->UV0.v) / Height);
//             U1 = (((float)DynamicData->UV1.u) / Width);
//             V1 = /*255 -*/(((float)DynamicData->UV1.v) /Height);
//             U2 = (((float)DynamicData->UV2.u) /Width);
//             V2 = /*255 -*/(((float)DynamicData->UV2.v) / Height);
//             TSB = DynamicData->TSB;
//             CBA = DynamicData->CBA;
//         } else {
            U0 = TSP->Face[i].UV0.u + VRAMGetTexturePageX(VRAMPage);
            V0 = TSP->Face[i].UV0.v + VRAMGetTexturePageY(VRAMPage,ColorMode);
            U1 = TSP->Face[i].UV1.u + VRAMGetTexturePageX(VRAMPage);
            V1 = TSP->Face[i].UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode);
            U2 = TSP->Face[i].UV2.u + VRAMGetTexturePageX(VRAMPage);
            V2 = TSP->Face[i].UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode);
//         }
        
        if( TSPGetColorIndex(TSP->Color[Vert0].c) < 40 || TSPGetColorIndex(TSP->Color[Vert1].c) < 40 || TSPGetColorIndex(TSP->Color[Vert2].c) < 40 ) {
            RenderingFace->Flags |= TSP_FX_ANIMATED;
        }
                    
        DPrintf("Tex Coords are %i;%i %i;%i %i;%i\n",
                    TSP->Face[i].UV0.u,TSP->Face[i].UV0.v,
                    TSP->Face[i].UV1.u,TSP->Face[i].UV1.v,
                    TSP->Face[i].UV2.u,TSP->Face[i].UV2.v);
        if( (TSP->Face[i].TSB & 0x4000) != 0) {
            TransparentVertexData[TransparentVertexPointer] =   TSP->Vertex[Vert0].Position.x;
            TransparentVertexData[TransparentVertexPointer+1] = TSP->Vertex[Vert0].Position.y;
            TransparentVertexData[TransparentVertexPointer+2] = TSP->Vertex[Vert0].Position.z;
            TransparentVertexData[TransparentVertexPointer+3] = U0;
            TransparentVertexData[TransparentVertexPointer+4] = V0;
            TransparentVertexData[TransparentVertexPointer+5] = TSP->Color[Vert0].rgba[0];
            TransparentVertexData[TransparentVertexPointer+6] = TSP->Color[Vert0].rgba[1];
            TransparentVertexData[TransparentVertexPointer+7] = TSP->Color[Vert0].rgba[2];
            TransparentVertexData[TransparentVertexPointer+8] = CLUTDestX;
            TransparentVertexData[TransparentVertexPointer+9] = CLUTDestY;
            TransparentVertexData[TransparentVertexPointer+10] = ColorMode;
            TransparentVertexPointer += 11;
                        
            TransparentVertexData[TransparentVertexPointer] =   TSP->Vertex[Vert1].Position.x;
            TransparentVertexData[TransparentVertexPointer+1] = TSP->Vertex[Vert1].Position.y;
            TransparentVertexData[TransparentVertexPointer+2] = TSP->Vertex[Vert1].Position.z;
            TransparentVertexData[TransparentVertexPointer+3] = U1;
            TransparentVertexData[TransparentVertexPointer+4] = V1;
            TransparentVertexData[TransparentVertexPointer+5] = TSP->Color[Vert1].rgba[0];
            TransparentVertexData[TransparentVertexPointer+6] = TSP->Color[Vert1].rgba[1];
            TransparentVertexData[TransparentVertexPointer+7] = TSP->Color[Vert1].rgba[2];
            TransparentVertexData[TransparentVertexPointer+8] = CLUTDestX;
            TransparentVertexData[TransparentVertexPointer+9] = CLUTDestY;
            TransparentVertexData[TransparentVertexPointer+10] = ColorMode;
            TransparentVertexPointer += 11;
                        
            TransparentVertexData[TransparentVertexPointer] =   TSP->Vertex[Vert2].Position.x;
            TransparentVertexData[TransparentVertexPointer+1] = TSP->Vertex[Vert2].Position.y;
            TransparentVertexData[TransparentVertexPointer+2] = TSP->Vertex[Vert2].Position.z;
            TransparentVertexData[TransparentVertexPointer+3] = U2;
            TransparentVertexData[TransparentVertexPointer+4] = V2;
            TransparentVertexData[TransparentVertexPointer+5] = TSP->Color[Vert2].rgba[0];
            TransparentVertexData[TransparentVertexPointer+6] = TSP->Color[Vert2].rgba[1];
            TransparentVertexData[TransparentVertexPointer+7] = TSP->Color[Vert2].rgba[2];
            TransparentVertexData[TransparentVertexPointer+8] = CLUTDestX;
            TransparentVertexData[TransparentVertexPointer+9] = CLUTDestY;
            TransparentVertexData[TransparentVertexPointer+10] = ColorMode;
            TransparentVertexPointer += 11;
            
            RenderingFace->VAOBufferOffset = TSP->TransparentVAO->CurrentSize;
            RenderingFace->BlendingMode = (TSP->Face[i].TSB >> 5 ) & 3;
            RenderingFace->Flags |= TSP_FX_TRANSPARENCY;
            VAOUpdate(TSP->TransparentVAO,TransparentVertexData,TransparentVertexSize,3);
            RenderingFace->Next = TSP->TransparentFaceList;
            TSP->TransparentFaceList = RenderingFace;
            TransparentVertexPointer = 0;

        } else {
            VertexData[VertexPointer] =   TSP->Vertex[Vert0].Position.x;
            VertexData[VertexPointer+1] = TSP->Vertex[Vert0].Position.y;
            VertexData[VertexPointer+2] = TSP->Vertex[Vert0].Position.z;
            VertexData[VertexPointer+3] = U0;
            VertexData[VertexPointer+4] = V0;
            VertexData[VertexPointer+5] = TSP->Color[Vert0].rgba[0];
            VertexData[VertexPointer+6] = TSP->Color[Vert0].rgba[1];
            VertexData[VertexPointer+7] = TSP->Color[Vert0].rgba[2];
            VertexData[VertexPointer+8] = CLUTDestX;
            VertexData[VertexPointer+9] = CLUTDestY;
            VertexData[VertexPointer+10] = ColorMode;
            VertexPointer += 11;
                        
            VertexData[VertexPointer] =   TSP->Vertex[Vert1].Position.x;
            VertexData[VertexPointer+1] = TSP->Vertex[Vert1].Position.y;
            VertexData[VertexPointer+2] = TSP->Vertex[Vert1].Position.z;
            VertexData[VertexPointer+3] = U1;
            VertexData[VertexPointer+4] = V1;
            VertexData[VertexPointer+5] = TSP->Color[Vert1].rgba[0];
            VertexData[VertexPointer+6] = TSP->Color[Vert1].rgba[1];
            VertexData[VertexPointer+7] = TSP->Color[Vert1].rgba[2];
            VertexData[VertexPointer+8] = CLUTDestX;
            VertexData[VertexPointer+9] = CLUTDestY;
            VertexData[VertexPointer+10] = ColorMode;
            VertexPointer += 11;      
            VertexData[VertexPointer] =   TSP->Vertex[Vert2].Position.x;
            VertexData[VertexPointer+1] = TSP->Vertex[Vert2].Position.y;
            VertexData[VertexPointer+2] = TSP->Vertex[Vert2].Position.z;
            VertexData[VertexPointer+3] = U2;
            VertexData[VertexPointer+4] = V2;
            VertexData[VertexPointer+5] = TSP->Color[Vert2].rgba[0];
            VertexData[VertexPointer+6] = TSP->Color[Vert2].rgba[1];
            VertexData[VertexPointer+7] = TSP->Color[Vert2].rgba[2];
            VertexData[VertexPointer+8] = CLUTDestX;
            VertexData[VertexPointer+9] = CLUTDestY;
            VertexData[VertexPointer+10] = ColorMode;
            VertexPointer += 11;
            
            RenderingFace->VAOBufferOffset = Node->OpaqueFacesVAO->CurrentSize;
            RenderingFace->Flags |= TSP_FX_NONE;
            VAOUpdate(Node->OpaqueFacesVAO,VertexData,VertexSize,3);
            RenderingFace->Next = Node->OpaqueFaceList;
            Node->OpaqueFaceList = RenderingFace;
            VertexPointer = 0;
        }
        if( RenderingFace->Flags & TSP_FX_ANIMATED ) {
            RenderingFace->ColorIndex[0] = (TSPGetColorIndex(TSP->Color[Vert0].c) < 40) ? (TSP->Color[Vert0].c & 0xFF) : -1;
            RenderingFace->ColorIndex[1] = (TSPGetColorIndex(TSP->Color[Vert1].c) < 40) ? (TSP->Color[Vert1].c & 0xFF) : -1; 
            RenderingFace->ColorIndex[2] = (TSPGetColorIndex(TSP->Color[Vert2].c) < 40) ? (TSP->Color[Vert2].c & 0xFF) : -1;
            //Save the orignal color in order to interpolate it later...
            RenderingFace->OriginalColor[0] = TSP->Color[Vert0].c;
            RenderingFace->OriginalColor[1] = TSP->Color[Vert1].c;
            RenderingFace->OriginalColor[2] =  TSP->Color[Vert2].c;
        }
    }
    free(VertexData);
    free(TransparentVertexData);
}

void TSPCreateFaceV3VAO(TSP_t *TSP,TSPNode_t *Node)
{
    int Stride;
    int Vert0;
    int Vert1;
    int Vert2;
    int U0,V0;
    int U1,V1;
    int U2,V2;
    int *VertexData;
    int *TransparentVertexData;
    int TotalVertexSize;
    int VertexSize;
    int TransparentVertexSize;
    int VertexPointer;
    int TransparentVertexPointer;
    int VertexOffset;
    int TextureOffset;
    int ColorOffset;
    int CLUTOffset;
    int ColorModeOffset;
    int NumTransparentFaces;
    int CLUTPosX;
    int CLUTPosY;
    int CLUTDestX;
    int CLUTDestY;
    int CLUTPage;
    int ColorMode;
    int VRAMPage;
    int ABRRate;
//     TSPDynamicFaceData_t *DynamicData;
    TSPTextureInfo_t TextureInfo;
    TSPRenderingFace_t *RenderingFace;
    VAO_t *VAO;

    int i;
    
//            XYZ UV RGB CLUT(X/Y) ColorMode
    Stride = (3 + 2 + 3 + 2 + 1) * sizeof(int);
                
    VertexOffset = 0;
    TextureOffset = 3;
    ColorOffset = 5;
    CLUTOffset = 8;
    ColorModeOffset = 10;
    
    NumTransparentFaces = TSPGetNodeTransparentFaceCount(TSP,Node);
    VertexSize = Stride * 3;
    TotalVertexSize = VertexSize * (Node->NumFaces - NumTransparentFaces);
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    TransparentVertexSize = Stride * 3;
    TransparentVertexData = malloc(TransparentVertexSize);
    TransparentVertexPointer = 0;
    VAO = VAOInitXYZUVRGBCLUTColorModeInteger(NULL,TotalVertexSize,Stride,VertexOffset,TextureOffset,ColorOffset,CLUTOffset,ColorModeOffset,
                                     (Node->NumFaces - NumTransparentFaces) * 3);
    Node->OpaqueFacesVAO = VAO;
    for( i = 0; i < Node->NumFaces; i++ ) {
        TextureInfo = TSP->TextureData[Node->FaceList[i].TextureDataIndex];
        ColorMode = (TextureInfo.TSB >> 7) & 0x3;
        VRAMPage = TextureInfo.TSB & 0x1F;
        ABRRate = (TextureInfo.TSB & 0x60) >> 5;
        
        RenderingFace = malloc(sizeof(TSPRenderingFace_t));
        RenderingFace->Flags = 0;
        RenderingFace->Next = NULL;
        
        CLUTPosX = (TextureInfo.CBA << 4) & 0x3F0;
        CLUTPosY = (TextureInfo.CBA >> 6) & 0x1ff;
        
        CLUTPage = VRAMGetCLUTPage(CLUTPosX,CLUTPosY);
        CLUTDestX = VRAMGetCLUTPositionX(CLUTPosX,CLUTPosY,CLUTPage);
        CLUTDestY = CLUTPosY + VRAMGetCLUTOffsetY(ColorMode);
        CLUTDestX += VRAMGetTexturePageX(CLUTPage);
        
        DPrintf("TSB is %u\n",TextureInfo.TSB);
        DPrintf("Expected VRam Page:%i\n",VRAMPage);
        DPrintf("Expected Color Mode:%i\n",ColorMode);
        DPrintf("Expected ABR rate:%i\n",ABRRate);
        
        Vert0 = Node->FaceList[i].Vert0;
        if( Node->FaceList[i].SwapV1V2 ) {
            Vert1 = Node->FaceList[i].Vert2;
            Vert2 = Node->FaceList[i].Vert1;
        } else {
            Vert1 = Node->FaceList[i].Vert1;
            Vert2 = Node->FaceList[i].Vert2;
        }
        
//         DynamicData = GetDynamicDataByFaceIndex(TSP,j,2);
//         if( DynamicData != NULL ) {
//             U0 = (((float)DynamicData->UV0.u)/Width);
//             V0 = /*255 -*/(((float)DynamicData->UV0.v) / Height);
//             U1 = (((float)DynamicData->UV1.u) / Width);
//             V1 = /*255 -*/(((float)DynamicData->UV1.v) /Height);
//             U2 = (((float)DynamicData->UV2.u) /Width);
//             V2 = /*255 -*/(((float)DynamicData->UV2.v) / Height);
//             TSB = DynamicData->TSB;
//             CBA = DynamicData->CBA;
//         } else {
            U0 = (TextureInfo.UV0.u + VRAMGetTexturePageX(VRAMPage));
            V0 = TextureInfo.UV0.v + VRAMGetTexturePageY(VRAMPage,ColorMode);
            if( Node->FaceList[i].SwapV1V2 ) {
                U2 = (TextureInfo.UV1.u + VRAMGetTexturePageX(VRAMPage));
                V2 = (TextureInfo.UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode));
                U1 = (TextureInfo.UV2.u + VRAMGetTexturePageX(VRAMPage));
                V1 = (TextureInfo.UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode));
            } else {
                U1 = (TextureInfo.UV1.u + VRAMGetTexturePageX(VRAMPage));
                V1 = (TextureInfo.UV1.v + VRAMGetTexturePageY(VRAMPage,ColorMode));
                U2 = (TextureInfo.UV2.u + VRAMGetTexturePageX(VRAMPage));
                V2 = (TextureInfo.UV2.v + VRAMGetTexturePageY(VRAMPage,ColorMode));
            }
//         }
                    
        if( TSPGetColorIndex(TSP->Color[Vert0].c) < 40 || TSPGetColorIndex(TSP->Color[Vert1].c) < 40 || TSPGetColorIndex(TSP->Color[Vert2].c) < 40 ) {
            RenderingFace->Flags |= TSP_FX_ANIMATED;
        }  
        DPrintf("Tex Coords are %i;%i %i;%i %i;%i\n",
                TextureInfo.UV0.u,TextureInfo.UV0.v,
                TextureInfo.UV1.u,TextureInfo.UV1.v,
                TextureInfo.UV2.u,TextureInfo.UV2.v);
        if( (TextureInfo.TSB & 0x4000) != 0) {
            TransparentVertexData[TransparentVertexPointer] =   TSP->Vertex[Vert0].Position.x;
            TransparentVertexData[TransparentVertexPointer+1] = TSP->Vertex[Vert0].Position.y;
            TransparentVertexData[TransparentVertexPointer+2] = TSP->Vertex[Vert0].Position.z;
            TransparentVertexData[TransparentVertexPointer+3] = U0;
            TransparentVertexData[TransparentVertexPointer+4] = V0;
            TransparentVertexData[TransparentVertexPointer+5] = TSP->Color[Vert0].rgba[0];
            TransparentVertexData[TransparentVertexPointer+6] = TSP->Color[Vert0].rgba[1];
            TransparentVertexData[TransparentVertexPointer+7] = TSP->Color[Vert0].rgba[2];
            TransparentVertexData[TransparentVertexPointer+8] = CLUTDestX;
            TransparentVertexData[TransparentVertexPointer+9] = CLUTDestY;
            TransparentVertexData[TransparentVertexPointer+10] = ColorMode;
            TransparentVertexPointer += 11;
                        
            TransparentVertexData[TransparentVertexPointer] =   TSP->Vertex[Vert1].Position.x;
            TransparentVertexData[TransparentVertexPointer+1] = TSP->Vertex[Vert1].Position.y;
            TransparentVertexData[TransparentVertexPointer+2] = TSP->Vertex[Vert1].Position.z;
            TransparentVertexData[TransparentVertexPointer+3] = U1;
            TransparentVertexData[TransparentVertexPointer+4] = V1;
            TransparentVertexData[TransparentVertexPointer+5] = TSP->Color[Vert1].rgba[0];
            TransparentVertexData[TransparentVertexPointer+6] = TSP->Color[Vert1].rgba[1];
            TransparentVertexData[TransparentVertexPointer+7] = TSP->Color[Vert1].rgba[2];
            TransparentVertexData[TransparentVertexPointer+8] = CLUTDestX;
            TransparentVertexData[TransparentVertexPointer+9] = CLUTDestY;
            TransparentVertexData[TransparentVertexPointer+10] = ColorMode;
            TransparentVertexPointer += 11;
                        
            TransparentVertexData[TransparentVertexPointer] =   TSP->Vertex[Vert2].Position.x;
            TransparentVertexData[TransparentVertexPointer+1] = TSP->Vertex[Vert2].Position.y;
            TransparentVertexData[TransparentVertexPointer+2] = TSP->Vertex[Vert2].Position.z;
            TransparentVertexData[TransparentVertexPointer+3] = U2;
            TransparentVertexData[TransparentVertexPointer+4] = V2;
            TransparentVertexData[TransparentVertexPointer+5] = TSP->Color[Vert2].rgba[0];
            TransparentVertexData[TransparentVertexPointer+6] = TSP->Color[Vert2].rgba[1];
            TransparentVertexData[TransparentVertexPointer+7] = TSP->Color[Vert2].rgba[2];
            TransparentVertexData[TransparentVertexPointer+8] = CLUTDestX;
            TransparentVertexData[TransparentVertexPointer+9] = CLUTDestY;
            TransparentVertexData[TransparentVertexPointer+10] = ColorMode;
            TransparentVertexPointer += 11;

            RenderingFace->VAOBufferOffset = TSP->TransparentVAO->CurrentSize;
            RenderingFace->BlendingMode = (TextureInfo.TSB >> 5) & 3;
            RenderingFace->Flags |= TSP_FX_TRANSPARENCY;
            VAOUpdate(TSP->TransparentVAO,TransparentVertexData,TransparentVertexSize,3);
            RenderingFace->Next = TSP->TransparentFaceList;
            TSP->TransparentFaceList = RenderingFace;
            TransparentVertexPointer = 0;
        } else {
            VertexData[VertexPointer] =   TSP->Vertex[Vert0].Position.x;
            VertexData[VertexPointer+1] = TSP->Vertex[Vert0].Position.y;
            VertexData[VertexPointer+2] = TSP->Vertex[Vert0].Position.z;
            VertexData[VertexPointer+3] = U0;
            VertexData[VertexPointer+4] = V0;
            VertexData[VertexPointer+5] = TSP->Color[Vert0].rgba[0];
            VertexData[VertexPointer+6] = TSP->Color[Vert0].rgba[1];
            VertexData[VertexPointer+7] = TSP->Color[Vert0].rgba[2];
            VertexData[VertexPointer+8] = CLUTDestX;
            VertexData[VertexPointer+9] = CLUTDestY;
            VertexData[VertexPointer+10] = ColorMode;
            VertexPointer += 11;
                        
            VertexData[VertexPointer] =   TSP->Vertex[Vert1].Position.x;
            VertexData[VertexPointer+1] = TSP->Vertex[Vert1].Position.y;
            VertexData[VertexPointer+2] = TSP->Vertex[Vert1].Position.z;
            VertexData[VertexPointer+3] = U1;
            VertexData[VertexPointer+4] = V1;
            VertexData[VertexPointer+5] = TSP->Color[Vert1].rgba[0];
            VertexData[VertexPointer+6] = TSP->Color[Vert1].rgba[1];
            VertexData[VertexPointer+7] = TSP->Color[Vert1].rgba[2];
            VertexData[VertexPointer+8] = CLUTDestX;
            VertexData[VertexPointer+9] = CLUTDestY;
            VertexData[VertexPointer+10] = ColorMode;
            VertexPointer += 11;
                        
            VertexData[VertexPointer] =   TSP->Vertex[Vert2].Position.x;
            VertexData[VertexPointer+1] = TSP->Vertex[Vert2].Position.y;
            VertexData[VertexPointer+2] = TSP->Vertex[Vert2].Position.z;
            VertexData[VertexPointer+3] = U2;
            VertexData[VertexPointer+4] = V2;
            VertexData[VertexPointer+5] = TSP->Color[Vert2].rgba[0];
            VertexData[VertexPointer+6] = TSP->Color[Vert2].rgba[1];
            VertexData[VertexPointer+7] = TSP->Color[Vert2].rgba[2];
            VertexData[VertexPointer+8] = CLUTDestX;
            VertexData[VertexPointer+9] = CLUTDestY;
            VertexData[VertexPointer+10] = ColorMode;
            VertexPointer += 11;

            RenderingFace->VAOBufferOffset = Node->OpaqueFacesVAO->CurrentSize;
            RenderingFace->Flags |= TSP_FX_NONE;
            VAOUpdate(Node->OpaqueFacesVAO,VertexData,VertexSize,3);
            RenderingFace->Next = Node->OpaqueFaceList;
            Node->OpaqueFaceList = RenderingFace;
            VertexPointer = 0;
        }
        if( RenderingFace->Flags & TSP_FX_ANIMATED ) {
            RenderingFace->ColorIndex[0] = (TSPGetColorIndex(TSP->Color[Vert0].c) < 40) ? (TSP->Color[Vert0].c & 0xFF) : -1;
            RenderingFace->ColorIndex[1] = (TSPGetColorIndex(TSP->Color[Vert1].c) < 40) ? (TSP->Color[Vert1].c & 0xFF) : -1; 
            RenderingFace->ColorIndex[2] = (TSPGetColorIndex(TSP->Color[Vert2].c) < 40) ? (TSP->Color[Vert2].c & 0xFF) : -1;
            //Save the orignal color in order to interpolate it later...
            RenderingFace->OriginalColor[0] = TSP->Color[Vert0].c;
            RenderingFace->OriginalColor[1] = TSP->Color[Vert1].c;
            RenderingFace->OriginalColor[2] =  TSP->Color[Vert2].c;
        }
    }
    free(VertexData);
    free(TransparentVertexData);
}
void TSPCreateNodeBBoxVAO(TSP_t *TSPList)
{
    TSP_t *Iterator;
    float *VertexData;
    int VertexSize;
    int TransparentVertexSize;
    int VertexPointer;
    int Stride;
    int NumTransparentFaces;
    vec4 BoxColor;
    int i;
    
    unsigned short BBoxIndices[16] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    
    for( Iterator = TSPList; Iterator; Iterator = Iterator->Next ) {
        NumTransparentFaces = 0;
        for( i = 0; i < Iterator->Header.NumNodes; i++ ) {
                NumTransparentFaces += TSPGetNodeTransparentFaceCount(Iterator,&Iterator->Node[i]);
        }
        Stride = (3 + 2 + 3 + 2 + 1) * sizeof(int);
        TransparentVertexSize = Stride * 3 * NumTransparentFaces;
        Iterator->TransparentVAO = VAOInitXYZUVRGBCLUTColorModeInteger(NULL,TransparentVertexSize,Stride,0,3,5,8,10,NumTransparentFaces * 3);
        
        for( i = 0; i < Iterator->Header.NumNodes; i++ ) {
            if( Iterator->Node[i].NumFaces != 0 ) {
                if( TSPIsVersion3(Iterator) ) {
                    TSPCreateFaceV3VAO(Iterator,&Iterator->Node[i]);
                } else {
                    TSPCreateFaceVAO(Iterator,&Iterator->Node[i]);
                }
            }
            
          //       XYZ RGB
            Stride = (3 + 3) * sizeof(float);
            VertexSize = Stride;
            VertexData = malloc(VertexSize * 8/** sizeof(float)*/);
            VertexPointer = 0;
                    
            if( Iterator->Node[i].NumFaces != 0 ) {
                //Leaf -- Yellow
                BoxColor[0] = 1;
                BoxColor[1] = 1;
                BoxColor[2] = 0;
                BoxColor[3] = 1;
            } else {
                //Splitter -- Red
                BoxColor[0] = 1;
                BoxColor[1] = 0;
                BoxColor[2] = 0;
                BoxColor[3] = 1;
            }
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Min.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Min.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Min.z;
            VertexData[VertexPointer+3] = BoxColor[0];
            VertexData[VertexPointer+4] = BoxColor[1];
            VertexData[VertexPointer+5] = BoxColor[2];

            VertexPointer += 6;
                        
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Min.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Min.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Max.z;
            VertexData[VertexPointer+3] = BoxColor[0];
            VertexData[VertexPointer+4] = BoxColor[1];
            VertexData[VertexPointer+5] = BoxColor[2];

            VertexPointer += 6;
            
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Max.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Min.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Max.z;
            VertexData[VertexPointer+3] = BoxColor[0];
            VertexData[VertexPointer+4] = BoxColor[1];
            VertexData[VertexPointer+5] = BoxColor[2];

            VertexPointer += 6;
            
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Max.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Min.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Min.z;
            VertexData[VertexPointer+3] = BoxColor[0];
            VertexData[VertexPointer+4] = BoxColor[1];
            VertexData[VertexPointer+5] = BoxColor[2];

            VertexPointer += 6;
            
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Min.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Max.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Min.z;
            VertexData[VertexPointer+3] = BoxColor[0];
            VertexData[VertexPointer+4] = BoxColor[1];
            VertexData[VertexPointer+5] = BoxColor[2];

            VertexPointer += 6;
            
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Min.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Max.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Max.z;
            VertexData[VertexPointer+3] = BoxColor[0];
            VertexData[VertexPointer+4] = BoxColor[1];
            VertexData[VertexPointer+5] = BoxColor[2];

            VertexPointer += 6;
                        
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Max.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Max.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Max.z;
            VertexData[VertexPointer+3] = BoxColor[0];
            VertexData[VertexPointer+4] = BoxColor[1];
            VertexData[VertexPointer+5] = BoxColor[2];

            VertexPointer += 6;
            
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Max.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Max.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Min.z;
            VertexData[VertexPointer+3] = BoxColor[0];
            VertexData[VertexPointer+4] = BoxColor[1];
            VertexData[VertexPointer+5] = BoxColor[2];

            VertexPointer += 6;
            
            Iterator->Node[i].BBoxVAO = VAOInitXYZRGBIBO(VertexData,VertexSize * 8,Stride,BBoxIndices,sizeof(BBoxIndices),0,3);            
            free(VertexData);
        }
    }
}

void TSPCreateCollisionVAO(TSP_t *TSPList)
{
    TSP_t *Iterator;
    int i;
    float *VertexData;
    int VertexSize;
    int VertexPointer;
    int Stride;

    for( Iterator = TSPList; Iterator; Iterator = Iterator->Next ) {
        VAO_t *VAO;
        //       XYZ
        Stride = (3) * sizeof(float);
                
        VertexSize = Stride;
        VertexData = malloc(VertexSize * 3 * Iterator->CollisionData->Header.NumFaces);
        VertexPointer = 0;
        for( i = 0; i < Iterator->CollisionData->Header.NumFaces; i++ ) {
            int Vert0 = Iterator->CollisionData->Face[i].V0;
            int Vert1 = Iterator->CollisionData->Face[i].V1;
            int Vert2 = Iterator->CollisionData->Face[i].V2;

                    
            VertexData[VertexPointer] =   Iterator->CollisionData->Vertex[Vert0].Position.x;
            VertexData[VertexPointer+1] = Iterator->CollisionData->Vertex[Vert0].Position.y;
            VertexData[VertexPointer+2] = Iterator->CollisionData->Vertex[Vert0].Position.z;
            VertexPointer += 3;
            
            VertexData[VertexPointer] =   Iterator->CollisionData->Vertex[Vert1].Position.x;
            VertexData[VertexPointer+1] = Iterator->CollisionData->Vertex[Vert1].Position.y;
            VertexData[VertexPointer+2] = Iterator->CollisionData->Vertex[Vert1].Position.z;
            VertexPointer += 3;
            
            VertexData[VertexPointer] =   Iterator->CollisionData->Vertex[Vert2].Position.x;
            VertexData[VertexPointer+1] = Iterator->CollisionData->Vertex[Vert2].Position.y;
            VertexData[VertexPointer+2] = Iterator->CollisionData->Vertex[Vert2].Position.z;
            VertexPointer += 3;
        }
        VAO = VAOInitXYZ(VertexData,VertexSize * 3 * Iterator->CollisionData->Header.NumFaces,Stride,0,Iterator->CollisionData->Header.NumFaces * 3);            
        VAO->Next = Iterator->CollisionVAOList;
        Iterator->CollisionVAOList = VAO;
        free(VertexData);
    }

}

Vec3_t Vec3_FromTSPVec3(TSPVec3_t In)
{
    return Vec3Build(In.x,In.y,In.z);
}

void TSPPrintVec3(TSPVec3_t Vector)
{
    printf("(%i;%i;%i)\n",Vector.x,Vector.y,Vector.z);
}

void TSPPrintColor(Color1i_t Color)
{
    printf("RGBA:(%i;%i;%i;%i)\n",Color.rgba[0],Color.rgba[1],Color.rgba[2],Color.rgba[3]);
}

void DrawTSPBox(TSPNode_t Node)
{
    Shader_t *Shader;
    int MVPMatrixId;
    
    Shader = ShaderCache("TSPBBoxShader","Shaders/TSPBBoxVertexShader.glsl","Shaders/TSPBBoxFragmentShader.glsl");
    glUseProgram(Shader->ProgramId);
    
    MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
    glUniformMatrix4fv(MVPMatrixId,1,false,&VidConf.MVPMatrix[0][0]);
    
    glBindVertexArray(Node.BBoxVAO->VAOId[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Node.BBoxVAO->IBOId[0]);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(unsigned short)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(unsigned short)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glUseProgram(0);
}

void DrawTSPCollisionData(TSP_t *TSP)
{
    VAO_t *Iterator;
    Shader_t *Shader;
    int MVPMatrixId;
    
    if( !TSP ) {
        DPrintf("Invalid TSP...\n");
        return;
    }
    
    Shader = ShaderCache("TSPCollisionShader","Shaders/TSPCollisionVertexShader.glsl","Shaders/TSPCollisionFragmentShader.glsl");
    glUseProgram(Shader->ProgramId);

    MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
    glUniformMatrix4fv(MVPMatrixId,1,false,&VidConf.MVPMatrix[0][0]);
    
    for( Iterator = TSP->CollisionVAOList; Iterator; Iterator = Iterator->Next ) {
        glBindVertexArray(Iterator->VAOId[0]);
        glDrawArrays(GL_TRIANGLES, 0, Iterator->Count);
        glBindVertexArray(0);
    }
    glUseProgram(0);

}
bool IsTSPInRenderArray(Level_t *Level,int TSPNumber)
{
    int i;
    for( i = 0; i < 4; i++ ) {
        if( TSPNumber == Level->TSPNumberRenderList[i] ) {
            return true;
        }
    }
    return false;
}

void DrawNode(TSPNode_t *Node,LevelSettings_t LevelSettings)
{
    Shader_t *Shader;
    int MVPMatrixId;
    int EnableLightingId;
    int PaletteTextureId;
    int TextureIndexId;
    
    if( !Node ) {
        return;
    }
    
    if( LevelSettings.EnableFrustumCulling && !TSPBoxInFrustum(Camera,Node->BBox) ) {
        return;
    }
    
    if( LevelSettings.ShowAABBTree ) {
        DrawTSPBox(*Node);
    }

    if( Node->NumFaces != 0 ) {
        if( LevelSettings.ShowMap ) {
            Shader = ShaderCache("TSPShader","Shaders/TSPVertexShader.glsl","Shaders/TSPFragmentShader.glsl");
            glUseProgram(Shader->ProgramId);

            MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
            glUniformMatrix4fv(MVPMatrixId,1,false,&VidConf.MVPMatrix[0][0]);
            EnableLightingId = glGetUniformLocation(Shader->ProgramId,"EnableLighting");
            PaletteTextureId = glGetUniformLocation(Shader->ProgramId,"ourPaletteTexture");
            TextureIndexId = glGetUniformLocation(Shader->ProgramId,"ourIndexTexture");
            glUniform1i(TextureIndexId, 0);
            glUniform1i(PaletteTextureId,  1);
            glUniform1i(EnableLightingId, LevelSettings.EnableLighting);
            if( LevelSettings.WireFrame ) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, LevelManager->CurrentLevel->VRAM->TextureIndexPage.TextureId);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, LevelManager->CurrentLevel->VRAM->PalettePage.TextureId);

            glDisable(GL_BLEND);
            glBindVertexArray(Node->OpaqueFacesVAO->VAOId[0]);
            glDrawArrays(GL_TRIANGLES, 0, Node->OpaqueFacesVAO->Count);
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D,0);
            glDisable(GL_BLEND);
            glBlendColor(1.f, 1.f, 1.f, 1.f);
            glUseProgram(0);
        }
    } else {
        DrawNode(Node->Child[1],LevelSettings);
        DrawNode(Node->Next,LevelSettings);
        DrawNode(Node->Child[0],LevelSettings);

    }
}
void TSPUpdateAnimatedRenderingFace(TSPRenderingFace_t *Face,VAO_t *VAO,BSD_t *BSD,int Reset)
{
    Color1i_t OriginalColor;
    Color1i_t FinalColor;
    int ColorData[3];
    int Stride;
    int CurrentColor;
    int BaseOffset;
    if( !Face ) {
        DPrintf("TSPUpdateAnimatedRenderingFace:Invalid face data\n");
        return;
    }
    
    if( !(Face->Flags & TSP_FX_ANIMATED) ) {
        return;
    }
    
    Stride = (3 + 2 + 3 + 2 + 1) * sizeof(int);
    glBindBuffer(GL_ARRAY_BUFFER, VAO->VBOId[0]);
    BaseOffset = (Face->VAOBufferOffset * Stride );
    
    for( int i = 0; i < 3; i++ ) {
        if( Face->ColorIndex[i] == -1 ) {
            continue;
        }
        CurrentColor = BSDGetCurrentAnimatedLightColorByIndex(BSD,Face->ColorIndex[i]);
        OriginalColor.c = Face->OriginalColor[i];
        FinalColor.c = (OriginalColor.c & 0xFF00) | (CurrentColor & 0xFFFFFF);
        if( Reset ) {
            ColorData[0] = OriginalColor.rgba[0];
            ColorData[1] = OriginalColor.rgba[1];
            ColorData[2] = OriginalColor.rgba[2];    
        } else {
            ColorData[0] = FinalColor.rgba[0];
            ColorData[1] = FinalColor.rgba[1];
            ColorData[2] = FinalColor.rgba[2];    
        }
        //The offset in which we write the color is based on the current VAO Offset to which
        //we add the stride times i which moves the pointer to one of the three vertices (each vertex takes Stride amount of bytes)
        //and finally we add 5 times sizeof(int) in order to grab the starting definition of the color data inside the vertex array.
        glBufferSubData(GL_ARRAY_BUFFER, BaseOffset + (Stride * i) + (5*sizeof(int)), 3 * sizeof(int), &ColorData);
    }
}
void TSPUpdateAnimatedFaceNodes(TSPNode_t *Node,BSD_t *BSD,int Reset)
{
    TSPRenderingFace_t *Iterator;

    if( !Node ) {
        return;
    }
    
    if( !TSPBoxInFrustum(Camera,Node->BBox) ) {
        return;
    }
    
    if( Node->NumFaces != 0 ) {
        for( Iterator = Node->OpaqueFaceList; Iterator; Iterator = Iterator->Next ) {
           TSPUpdateAnimatedRenderingFace(Iterator,Node->OpaqueFacesVAO,BSD,Reset);
        }

    } else {
        TSPUpdateAnimatedFaceNodes(Node->Child[1],BSD,Reset);
        TSPUpdateAnimatedFaceNodes(Node->Next,BSD,Reset);
        TSPUpdateAnimatedFaceNodes(Node->Child[0],BSD,Reset);
    }
}
void TSPUpdateTransparentAnimatedFaces(TSP_t *TSP,BSD_t *BSD,int Reset)
{
    TSPRenderingFace_t *Iterator;
    
    for( Iterator = TSP->TransparentFaceList; Iterator; Iterator = Iterator->Next ) {
        TSPUpdateAnimatedRenderingFace(Iterator,TSP->TransparentVAO,BSD,Reset);
    }
}
void TSPUpdateAnimatedFaces(TSP_t *TSPList,BSD_t *BSD,int Reset)
{
    TSP_t *Iterator;
    for( Iterator = TSPList; Iterator; Iterator = Iterator->Next ) {
        TSPUpdateAnimatedFaceNodes(&Iterator->Node[0],BSD,Reset);
        TSPUpdateTransparentAnimatedFaces(Iterator,BSD,Reset);
    }
}
void TSPDrawTransparentFaces(TSP_t *TSP,LevelSettings_t Settings)
{
    Shader_t *Shader;
    int MVPMatrixId;
    int EnableLightingId;
    int PaletteTextureId;
    int TextureIndexId;
    TSPRenderingFace_t *TransparentFaceIterator;

    
    Shader = ShaderCache("TSPShader","Shaders/TSPVertexShader.glsl","Shaders/TSPFragmentShader.glsl");
    glUseProgram(Shader->ProgramId);

    MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
    glUniformMatrix4fv(MVPMatrixId,1,false,&VidConf.MVPMatrix[0][0]);
    EnableLightingId = glGetUniformLocation(Shader->ProgramId,"EnableLighting");
    PaletteTextureId = glGetUniformLocation(Shader->ProgramId,"ourPaletteTexture");
    TextureIndexId = glGetUniformLocation(Shader->ProgramId,"ourIndexTexture");
    glUniform1i(TextureIndexId, 0);
    glUniform1i(PaletteTextureId,  1);
    glUniform1i(EnableLightingId, Settings.EnableLighting);
    if( Settings.WireFrame ) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, LevelManager->CurrentLevel->VRAM->TextureIndexPage.TextureId);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, LevelManager->CurrentLevel->VRAM->PalettePage.TextureId);
    glBindVertexArray(TSP->TransparentVAO->VAOId[0]);
    if( !Settings.EnableSemiTransparency ) {
        glDrawArrays(GL_TRIANGLES, 0, TSP->TransparentVAO->Count);
    } else {
        glDepthMask(0);
        glEnable(GL_BLEND);
        glBlendColor(0.25f, 0.25f, 0.25f, 0.5f);
        for( TransparentFaceIterator = TSP->TransparentFaceList; TransparentFaceIterator; TransparentFaceIterator = TransparentFaceIterator->Next) {
            switch( TransparentFaceIterator->BlendingMode ) {
                case 0:
                    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                    glBlendFunc(GL_ONE, GL_SRC_ALPHA);
                    break;
                case 1:
                    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                    glBlendFunc(GL_ONE, GL_SRC_ALPHA);
                    break;
                case 2:
                    glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
                    glBlendFunc(GL_ONE, GL_SRC_ALPHA);
                    break;
                case 3:
                    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                    glBlendFunc(GL_CONSTANT_COLOR, GL_SRC_ALPHA);
                    break;
            }
            glDrawArrays(GL_TRIANGLES, TransparentFaceIterator->VAOBufferOffset, 3);
        }
    }
    glBindVertexArray(0);
    glDepthMask(1);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D,0);
    glDisable(GL_BLEND);
    glBlendColor(1.f, 1.f, 1.f, 1.f);
    glUseProgram(0);
}
void TSPDrawList(LevelManager_t *LevelManager)
{
    TSP_t *TSPData;
    TSP_t *Iterator;

    TSPData = LevelManager->CurrentLevel->TSPList;
    
    if( !TSPData ) {
        printf("DrawTSP:Invalid TSP data\n");
        return;
    }

    for( Iterator = TSPData; Iterator; Iterator = Iterator->Next ) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);  
        DrawNode(&Iterator->Node[0],LevelManager->Settings);
        glDisable(GL_CULL_FACE);
    }

    // Alpha pass.
    for( Iterator = TSPData; Iterator; Iterator = Iterator->Next ) {
        TSPDrawTransparentFaces(Iterator,LevelManager->Settings);
    }
    if( LevelManager->Settings.ShowCollisionData ) {
        for( Iterator = TSPData; Iterator; Iterator = Iterator->Next ) {
            DrawTSPCollisionData(Iterator);
        }
    }
}

int TSPGetNodeByChildOffset(TSP_t *TSP,int Offset)
{
    int i;
    for( i = 0; i < TSP->Header.NumNodes; i++ ) {
        if( Offset == TSP->Node[i].FileOffset.Offset ) {
            DPrintf("Offset %i match node %i\n",Offset,i);
            return i;
        }
    }
    printf("Offset %i doesn't match any node!\n",Offset);
    return -1;
}

void TSPLookUpChildNode(TSP_t *TSP,FILE *InFile)
{
    int i;
    int Offset;
    int Index;
    for( i = 0; i < TSP->Header.NumNodes; i++ ) {
        if( TSP->Node[i].NumFaces != 0 ) {
            continue;
        }
        //Each child has an offset relative to his parent section that goes from [NodeOffset;FaceOffset]
        //So by adding TSP->Header.NodeOffset we should be able to map it file-wise.
        if( TSP->Node[i].FileOffset.Child1Offset != -1 ) {
            Offset = TSP->Node[i].FileOffset.Child1Offset + TSP->Header.NodeOffset;
            Index = TSPGetNodeByChildOffset(TSP,Offset);
            assert(Index != -1);
            TSP->Node[i].Child[0] = &TSP->Node[Index];
        } else {
            TSP->Node[i].Child[0] = NULL;
        }
        if( TSP->Node[i].FileOffset.Child2Offset != -1 ) {
            Offset = TSP->Node[i].FileOffset.Child2Offset + TSP->Header.NodeOffset;
            Index = TSPGetNodeByChildOffset(TSP,Offset);
            assert(Index != -1);
            TSP->Node[i].Child[1] = &TSP->Node[Index];
        } else {
            TSP->Node[i].Child[1] = NULL;
        }
        if( TSP->Node[i].BaseData > 0 ) {
            Offset = TSP->Node[i].BaseData + TSP->Header.NodeOffset;
            Index = TSPGetNodeByChildOffset(TSP,Offset);
            assert(Index != -1);
            TSP->Node[i].Next = &TSP->Node[Index];
        } else {
            TSP->Node[i].Next = NULL;
        }
    }
}
/*
    The U4 entry in TSPNode_t can be calculated by using this formula:
        if( TSP->Node[i].NumFaces == 0 ) {
            //NOTE(Adriano):int offset isn't part of the node struct!
            NextU4 = TSP->Node[i].BaseData + sizeof(TSPNode_t) - sizeof(int) - sizeof(Child);
        } else {
            NextU4 = TSP->Node[i].BaseData + (TSP->Node[i].NumFaces * sizeof(TSPFace_t));
            printf("Drawing face from %i to %i\n",Base,Target);
        }

*/

void TSPReadNodeChunk(TSP_t *TSP,FILE *InFile)
{
    TSPFaceV3_t TempFace;
    int CurrentFaceIndex;
    int Marker;
    int PrevFilePosition;
    int i;
    
    if( !TSP || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        printf("TSPReadNodeChunk: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    if( TSP->Header.NumNodes == 0 ) {
        printf("TSPReadNodeChunk:0 nodes found in file %s.\n",TSP->FName);
        return;
    }
    TSP->Node = malloc(TSP->Header.NumNodes * sizeof(TSPNode_t));
    memset(TSP->Node,0,TSP->Header.NumNodes * sizeof(TSPNode_t));
    for( i = 0; i < TSP->Header.NumNodes; i++ ) {
        DPrintf(" -- NODE %i -- \n",i);
        TSP->Node[i].FileOffset.Offset = ftell(InFile);
        DPrintf("TSPReadNodeChunk:Reading node %i at %i\n",i,TSP->Node[i].FileOffset.Offset);
        fread(&TSP->Node[i].BBox,sizeof(TSPBBox_t),1,InFile);
        fread(&TSP->Node[i].NumFaces,sizeof(TSP->Node[i].NumFaces),1,InFile);
        fread(&TSP->Node[i].U2,sizeof(TSP->Node[i].U2),1,InFile);
        fread(&TSP->Node[i].U3,sizeof(TSP->Node[i].U3),1,InFile);
        fread(&TSP->Node[i].BaseData,sizeof(TSP->Node[i].BaseData),1,InFile);
        TSP->Node[i].FaceList = NULL;
        DPrintf("TSPReadNodeChunk:Node has %i faces\n",TSP->Node[i].NumFaces);
        DPrintf("TSPReadNodeChunk:Node BaseData %i (References offset %i)\n",TSP->Node[i].BaseData,
                TSP->Node[i].BaseData + TSP->Header.NodeOffset);
        DPrintf("TSPReadNodeChunk:Node U2 %i\n",TSP->Node[i].U2);
        DPrintf("TSPReadNodeChunk:Node U3 %i\n",TSP->Node[i].U3);
        int Base = TSP->Node[i].BaseData / sizeof(TSPFace_t);
        int Target = Base + TSP->Node[i].NumFaces;
        DPrintf("We need to render %i faces starting from offset %i\n",TSP->Node[i].NumFaces,TSP->Header.FaceOffset + TSP->Node[i].BaseData);
        DPrintf("Face Index goes then from %i to %i\n",Base,Target);
        if( TSP->Node[i].NumFaces == 0 ) {
            fread(&TSP->Node[i].FileOffset.Child1Offset,sizeof(TSP->Node[i].FileOffset.Child1Offset),1,InFile);
            fread(&TSP->Node[i].FileOffset.Child2Offset,sizeof(TSP->Node[i].FileOffset.Child2Offset),1,InFile);
            DPrintf("TSPReadNodeChunk:Child1:%i\n",TSP->Node[i].FileOffset.Child1Offset + TSP->Header.NodeOffset);
            DPrintf("TSPReadNodeChunk:Child2:%i\n",TSP->Node[i].FileOffset.Child2Offset + TSP->Header.NodeOffset);
        } else {
            TSP->Node[i].OpaqueFaceList = NULL;
            if( TSPIsVersion3(TSP) ) {
                TSP->Node[i].FaceList = malloc(TSP->Node[i].NumFaces * sizeof(TSPFaceV3_t));
                PrevFilePosition = ftell(InFile);
                fseek(InFile,TSP->Node[i].BaseData + TSP->Header.FaceOffset,SEEK_SET);
                CurrentFaceIndex = 0;
                while( CurrentFaceIndex < TSP->Node[i].NumFaces ) {
                    DPrintf("TSPReadNodeChunk:Reading face %i for node %i at %li\n",CurrentFaceIndex,i,ftell(InFile) - TSP->Header.FaceOffset);
                    fread(&TempFace.V0V1,sizeof(TempFace.V0V1),1,InFile);
                    fread(&TempFace.V2,sizeof(TempFace.V2),1,InFile);
                    fread(&TempFace.TextureDataIndex,sizeof(TempFace.TextureDataIndex),1,InFile);
                    
                    TSP->Node[i].FaceList[CurrentFaceIndex].V0V1 = TempFace.V0V1;
                    TSP->Node[i].FaceList[CurrentFaceIndex].V2 = TempFace.V2;
                    TSP->Node[i].FaceList[CurrentFaceIndex].TextureDataIndex = TempFace.TextureDataIndex;
                    TSP->Node[i].FaceList[CurrentFaceIndex].Vert0 = TempFace.Vert0 = TSP->Node[i].FaceList[CurrentFaceIndex].V0V1 & 0x1FFF;
                    TSP->Node[i].FaceList[CurrentFaceIndex].Vert1 = TempFace.Vert1 = ( TSP->Node[i].FaceList[CurrentFaceIndex].V0V1 >> 16 ) & 0X1FFF;
                    TSP->Node[i].FaceList[CurrentFaceIndex].Vert2 = TempFace.Vert2 = TSP->Node[i].FaceList[CurrentFaceIndex].V2 & 0X1FFF;
                    TSP->Node[i].FaceList[CurrentFaceIndex].SwapV1V2 = 0;
                    DPrintf("TSPReadNodeChunk:Got Vert0:%i %i %i\n",TSP->Node[i].FaceList[CurrentFaceIndex].Vert0,TSP->Node[i].FaceList[CurrentFaceIndex].Vert1,
                        TSP->Node[i].FaceList[CurrentFaceIndex].Vert2
                    );
                    assert(TSP->Node[i].FaceList[CurrentFaceIndex].Vert0 < TSP->Header.NumVertices);
                    assert(TSP->Node[i].FaceList[CurrentFaceIndex].Vert1 < TSP->Header.NumVertices);
                    assert(TSP->Node[i].FaceList[CurrentFaceIndex].Vert2 < TSP->Header.NumVertices);
                    CurrentFaceIndex++;
                    while( 1 ) {
                        DPrintf("TSPReadNodeChunk:Reading additional face %i for node %i\n",CurrentFaceIndex,i);
                        fread(&Marker,sizeof(Marker),1,InFile);
                        DPrintf("TSPReadNodeChunk:Found Marker %i (Vertex %i) Texture:%i Mask %i\n",Marker,Marker & 0x1FFF,Marker >> 16,0x1FFF);

                        if( ( Marker & 0x1FFF ) == 0x1FFF || Marker == 0x1fff1fff ) {
                            DPrintf("TSPReadNodeChunk:Aborting since a marker was found\n");
                            break;
                        }
                        TSP->Node[i].FaceList[CurrentFaceIndex].TextureDataIndex = Marker >> 16;
                        if( (Marker & 0x8000) != 0 ) {
                            TempFace.Vert0 = TempFace.Vert2;
                        } else {
                            TempFace.Vert0 = TempFace.Vert1;
                            TempFace.Vert1 = TempFace.Vert2;
                        }
                        TempFace.Vert2 = Marker & 0x1FFF;
                        //NOTE(Adriano):If this bit is set, we need to swap vert1 and vert2 in order
                        //to keep the face winding clockwise and make culling works properly.
                        if( Marker & 0x4000 ) {
                            TSP->Node[i].FaceList[CurrentFaceIndex].SwapV1V2 = 1;
                        } else {
                            TSP->Node[i].FaceList[CurrentFaceIndex].SwapV1V2 = 0;
                        }
                        TSP->Node[i].FaceList[CurrentFaceIndex].Vert0 = TempFace.Vert0;
                        TSP->Node[i].FaceList[CurrentFaceIndex].Vert1 = TempFace.Vert1;
                        TSP->Node[i].FaceList[CurrentFaceIndex].Vert2 = TempFace.Vert2;
                        assert(TSP->Node[i].FaceList[CurrentFaceIndex].Vert0 < TSP->Header.NumVertices);
                        assert(TSP->Node[i].FaceList[CurrentFaceIndex].Vert1 < TSP->Header.NumVertices);
                        assert(TSP->Node[i].FaceList[CurrentFaceIndex].Vert2 < TSP->Header.NumVertices);
                        CurrentFaceIndex++;
                    }
                    if( Marker == 0x1fff1fff ) {
                        DPrintf("TSPReadNodeChunk:Sentinel Face found Done reading faces for node %i\n",i);
                        DPrintf("TSPReadNodeChunk:Loaded %i faces (Expected %i)\n",CurrentFaceIndex,TSP->Node[i].NumFaces);
                        assert(CurrentFaceIndex == TSP->Node[i].NumFaces);
                        break;
                    }
                }
                fseek(InFile,PrevFilePosition,SEEK_SET);
            }
        }
    }
    TSPLookUpChildNode(TSP,InFile);
    DPrintf("Current file offset is %li\n",ftell(InFile));
}

void TSPReadFaceChunk(TSP_t *TSP,FILE *InFile)
{
    int Ret;
    int i;
    int NumFaces;
    
    if( !TSP || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        printf("TSPReadFaceChunk: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    if( TSP->Header.NumFaces == 0 ) {
        printf("TSPReadFaceChunk:0 faces found in file %s.\n",TSP->FName);
        return;
    }
    //HACK:For the moment we calculate the number of faces by using the vertex offset
    //     Ignoring the NumFace param in the TSP header
    //     Since it doesn't cause any weird issue with the 3D rendering.
    NumFaces = (TSP->Header.VertexOffset - TSP->Header.FaceOffset) / sizeof(TSPFace_t);
    if( NumFaces != TSP->Header.NumFaces ) {
        printf("Fixed face count from %i to %i\n",TSP->Header.NumFaces,NumFaces);
        TSP->Header.NumFaces = NumFaces;
    }
    TSP->Face = malloc(TSP->Header.NumFaces * sizeof(TSPFace_t));
    for( i = 0; i < TSP->Header.NumFaces; i++ ) {
        DPrintf("Reading Face %i at %li\n",i,ftell(InFile));
        Ret = fread(&TSP->Face[i],sizeof(TSPFace_t),1,InFile);
        if( Ret != 1 ) {
            printf("TSPReadFaceChunk:Early failure when reading face %i\n",i);
            return;
        }
#if 1
//     if( i <= 4 ) {
        printf(" -- Face %i --\n",i);
        printf("V0:%u\n",TSP->Face[i].V0);
        printf("V1:%u\n",TSP->Face[i].V1);
        printf("V2:%u\n",TSP->Face[i].V2);
        printf("CBA:%u\n",TSP->Face[i].CBA);
        printf("TSB:%u\n",TSP->Face[i].TSB);
//     }
#endif
    }
}

void TSPReadVertexChunk(TSP_t *TSP,FILE *InFile)
{
    int Ret;
    int i;
    
    if( !TSP || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        printf("TSPReadVertexChunk: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    if( TSP->Header.NumVertices == 0 ) {
        printf("TSPReadVertexChunk:No vertices found in file %s.\n",TSP->FName);
        return;
    }
    
    TSP->Vertex = malloc(TSP->Header.NumVertices * sizeof(TSPVert_t));
    
    for( i = 0; i < TSP->Header.NumVertices; i++ ) {
        Ret = fread(&TSP->Vertex[i],sizeof(TSPVert_t),1,InFile);
        if( Ret != 1 ) {
            printf("TSPReadVertexChunk:Early failure when reading vertex %i\n",i);
            return;
        }
//         printf(" -- Vertex %i --\n",i);
//         PrintTSPVec3(TSP->Vertex[i].Position);
        assert(TSP->Vertex[i].Pad == 104 || TSP->Vertex[i].Pad == 105);
    }
}

void TSPReadColorChunk(TSP_t *TSP,FILE *InFile)
{
    int Ret;
    int i;
    
    if( !TSP || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        printf("TSPReadColorChunk: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    if( TSP->Header.NumVertices == 0 ) {
        printf("TSPReadColorChunk:0 colors found in file %s.\n",TSP->FName);
        return;
    }
    
    TSP->Color = malloc(TSP->Header.NumColors * sizeof(Color1i_t));
    
    for( i = 0; i < TSP->Header.NumColors; i++ ) {
        Ret = fread(&TSP->Color[i],sizeof(Color1i_t),1,InFile);
        if( Ret != 1 ) {
            printf("TSPReadColorChunk:Early failure when reading normal %i\n",i);
            return;
        }
    }
}

void TSPReadDynamicDataChunk(TSP_t *TSP,FILE *InFile)
{
    int i;
    int j;
    int DynamicBlockStart;
    int DynamicBlockEnd;
    int Delta;
    
    if( !TSP || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        printf("TSPReadDynamicDataChunk: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        fseek(InFile,TSP->Header.CollisionOffset,SEEK_SET);
        return;
    }
    if( TSP->Header.NumDynamicDataBlock == 0 ) {
        DPrintf("TSPReadDynamicDataChunk:TSP has no Dynamic Data set.\n");
        if( TSPIsVersion3(TSP) ) {
            fseek(InFile,TSP->Header.TextureInfoOffset,SEEK_SET);
        } else {
            fseek(InFile,TSP->Header.CollisionOffset,SEEK_SET);
        }
        return;
    }
    DPrintf("TSPReadDynamicDataChunk:TSP has %i Dynamic Data Block.\n",TSP->Header.NumDynamicDataBlock);
    TSP->DynamicData = malloc(TSP->Header.NumDynamicDataBlock * sizeof(TSPDynamicData_t));
    for( i = 0; i < TSP->Header.NumDynamicDataBlock; i++ ) {
        DPrintf("-- Dynamic Data Block %i --\n",i);
        DPrintf("File offset is %li\n",ftell(InFile));
        DynamicBlockStart = ftell(InFile);
        fread(&TSP->DynamicData[i].Header.Size,sizeof(TSP->DynamicData[i].Header.Size),1,InFile);
        fread(&TSP->DynamicData[i].Header.Unk0,sizeof(TSP->DynamicData[i].Header.Unk0),1,InFile);
        fread(&TSP->DynamicData[i].Header.Unk1,sizeof(TSP->DynamicData[i].Header.Unk1),1,InFile);
        fread(&TSP->DynamicData[i].Header.DynamicDataIndex,sizeof(TSP->DynamicData[i].Header.DynamicDataIndex),1,InFile);
        fread(&TSP->DynamicData[i].Header.FaceDataSizeMultiplier,sizeof(TSP->DynamicData[i].Header.FaceDataSizeMultiplier),1,InFile);
        fread(&TSP->DynamicData[i].Header.NumFacesIndex,sizeof(TSP->DynamicData[i].Header.NumFacesIndex),1,InFile);
        fread(&TSP->DynamicData[i].Header.FaceIndexOffset,sizeof(TSP->DynamicData[i].Header.FaceIndexOffset),1,InFile);
        fread(&TSP->DynamicData[i].Header.FaceDataOffset,sizeof(TSP->DynamicData[i].Header.FaceDataOffset),1,InFile);
        DPrintf("Size:%i\n",TSP->DynamicData[i].Header.Size);
        DPrintf("Unk0:%i || Unk1:%i\n",TSP->DynamicData[i].Header.Unk0,TSP->DynamicData[i].Header.Unk1);
        DPrintf("Dynamic Data Index:%i\n",TSP->DynamicData[i].Header.DynamicDataIndex);
        DPrintf("FaceDataSizeMultiplier:%i\n",TSP->DynamicData[i].Header.FaceDataSizeMultiplier);
        DPrintf("NumFacesIndex:%i\n",TSP->DynamicData[i].Header.NumFacesIndex);
        DPrintf("Face Index Offset:%i\n",TSP->DynamicData[i].Header.FaceIndexOffset);
        DPrintf("Face Data Offset:%i\n",TSP->DynamicData[i].Header.FaceDataOffset);
        TSP->DynamicData[i].FaceIndexList = malloc(TSP->DynamicData[i].Header.NumFacesIndex * sizeof(short));
        for( j = 0; j < TSP->DynamicData[i].Header.NumFacesIndex; j++ ) {
            fread(&TSP->DynamicData[i].FaceIndexList[j],sizeof(TSP->DynamicData[i].FaceIndexList[j]),1,InFile);
        }
        DPrintf("Position after face index list is %li\n",ftell(InFile));
        TSP->DynamicData[i].FaceDataList = malloc(TSP->DynamicData[i].Header.NumFacesIndex * sizeof(TSPDynamicFaceData_t)  * 
            TSP->DynamicData[i].Header.FaceDataSizeMultiplier);
        for( j = 0; j < TSP->DynamicData[i].Header.NumFacesIndex * TSP->DynamicData[i].Header.FaceDataSizeMultiplier; j++ ) {
            fread(&TSP->DynamicData[i].FaceDataList[j],sizeof(TSP->DynamicData[i].FaceDataList[j]),1,InFile);
        }
        DynamicBlockEnd = ftell(InFile);
        Delta = DynamicBlockEnd - DynamicBlockStart;
        DPrintf("Position after face data list is %i\n",DynamicBlockEnd);
        //Some blocks may not be aligned to the size 10 boundaries...
        if( Delta != TSP->DynamicData[i].Header.Size ) {
            DPrintf("Fixing unaligned block (Missing %i bytes)...\n",TSP->DynamicData[i].Header.Size - Delta);
            fseek(InFile,TSP->DynamicData[i].Header.Size - Delta,SEEK_CUR);
        }
    }
}

void TSPReadTextureInfoChunk(TSP_t *TSP,FILE *InFile)
{
    int i;
    int Ret;
    
    if( !TSP || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("TSPReadTextureInfoChunk: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    if( TSP->Header.NumTextureInfo == 0 || TSP->Header.NumTextureInfo == -1 ) {
        DPrintf("TSPReadTextureInfoChunk:0 textures found in file %s.\n",TSP->FName);
        return;
    }
    assert(sizeof(TSPTextureInfo_t) == 12);
    TSP->TextureData = malloc(TSP->Header.NumTextureInfo * sizeof(TSPTextureInfo_t));
    for( i = 0; i < TSP->Header.NumTextureInfo; i++ ) {
        DPrintf("Reading texture info at %li\n",ftell(InFile));
        Ret = fread(&TSP->TextureData[i],sizeof(TSPTextureInfo_t),1,InFile);
            if( Ret != 1 ) {
                DPrintf("TSPReadTextureInfoChunk:Early failure when reading face texture data %i\n",i);
                return;
            }
            DPrintf(" -- Texture for Face V3 %i --\n",i);
            DPrintf("TSB:%u\n",TSP->TextureData[i].TSB);
            DPrintf("TSB:%u\n",TSP->TextureData[i].CBA);
            DPrintf("U0:V0:%i %i\n",TSP->TextureData[i].UV0.u,TSP->TextureData[i].UV0.v);
            DPrintf("U1:V1:%i %i\n",TSP->TextureData[i].UV1.u,TSP->TextureData[i].UV1.v);
            DPrintf("U2:V2:%i %i\n",TSP->TextureData[i].UV2.u,TSP->TextureData[i].UV2.v);
            DPrintf("Padding:%i\n",TSP->TextureData[i].Padding);
    }

}

void TSPReadCollisionChunk(TSP_t *TSP,FILE *InFile)
{
    short Pad;
    int Ret;
    int i;
    
    if( !TSP || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        printf("TSPReadCollisionChunk: Invalid %s.\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    
    TSP->CollisionData = malloc(sizeof(TSPCollision_t));
    Ret = fread(&TSP->CollisionData->Header,sizeof(TSPCollisionHeader_t),1,InFile);
    if( Ret != 1 ) {
        printf("TSPReadCollisionChunk:Early failure when reading collision header.\n");
    }
    printf("TSPReadCollisionChunk:Header\n");
//     printf("U0|U1|U2|U3:%u %u %u %u\n",TSP->CollisionData->Header.U0,TSP->CollisionData->Header.U1,TSP->CollisionData->Header.U2,
//         TSP->CollisionData->Header.U3
//     );
    printf("CollisionBoundMinX:%i\n",TSP->CollisionData->Header.CollisionBoundMinX);
    printf("CollisionBoundMinZ:%i\n",TSP->CollisionData->Header.CollisionBoundMinZ);
    printf("CollisionBoundMaxX:%i\n",TSP->CollisionData->Header.CollisionBoundMaxX);
    printf("CollisionBoundMaxZ:%i\n",TSP->CollisionData->Header.CollisionBoundMaxZ);
    printf("Num Collision KDTree Nodes:%u\n",TSP->CollisionData->Header.NumCollisionKDTreeNodes);
    printf("Num Collision Face Index:%u\n",TSP->CollisionData->Header.NumCollisionFaceIndex);
    printf("NumVertices:%u\n",TSP->CollisionData->Header.NumVertices);
    printf("NumNormals:%u\n",TSP->CollisionData->Header.NumNormals);
    printf("NumFaces:%u\n",TSP->CollisionData->Header.NumFaces);

    TSP->CollisionData->KDTree = malloc(TSP->CollisionData->Header.NumCollisionKDTreeNodes * sizeof(TSPCollisionKDTreeNode_t));
    for( i = 0; i < TSP->CollisionData->Header.NumCollisionKDTreeNodes; i++ ) {
        Ret = fread(&TSP->CollisionData->KDTree[i],sizeof(TSP->CollisionData->KDTree[i]),1,InFile);
        if( Ret != 1 ) {
            printf("TSPReadCollisionChunk:Early failure when reading KDTree nodes.\n");
            return;
        }
    }
    TSP->CollisionData->FaceIndexList = malloc(TSP->CollisionData->Header.NumCollisionFaceIndex * sizeof(short));
    for( i = 0; i < TSP->CollisionData->Header.NumCollisionFaceIndex; i++ ) {
        Ret = fread(&TSP->CollisionData->FaceIndexList[i],sizeof(TSP->CollisionData->FaceIndexList[i]),1,InFile);
        if( Ret != 1 ) {
            DPrintf("TSPReadCollisionChunk:Early failure when reading Collison Face Index data.\n");
            return;
        }
//         DPrintf("-- H %i at %i --\n",i,GetCurrentFilePosition(InFile));
//         printf("%i\n",TSP->CollisionData->H[i]);
    }
    fread(&Pad,sizeof(Pad),1,InFile);
    if( Pad != 0 ) {
        //Undo the last read.
        fseek(InFile,-sizeof(Pad),SEEK_CUR);
        
    }
    DPrintf("TSPReadCollisionChunk:Vertex at %li\n",ftell(InFile));
    TSP->CollisionData->Vertex = malloc(TSP->CollisionData->Header.NumVertices * sizeof(TSPVert_t));
    for( i = 0; i < TSP->CollisionData->Header.NumVertices; i++ ) {
        Ret = fread(&TSP->CollisionData->Vertex[i],sizeof(TSP->CollisionData->Vertex[i]),1,InFile);
        if( Ret != 1 ) {
            printf("TSPReadCollisionChunk:Early failure when reading vertex data.\n");
            return;
        }
//         printf("-- Vertex %i --\n",i);
//         PrintTSPVec3(TSP->CollisionData->Vertex[i].Position);
//         printf("Pad is %i\n",TSP->CollisionData->Vertex[i].Pad);
        assert(TSP->CollisionData->Vertex[i].Pad == 104 || TSP->CollisionData->Vertex[i].Pad == 105);
    }
    DPrintf("TSPReadCollisionChunk:Normals at %li\n",ftell(InFile));
    TSP->CollisionData->Normal = malloc(TSP->CollisionData->Header.NumNormals * sizeof(TSPVert_t));
    for( i = 0; i < TSP->CollisionData->Header.NumNormals; i++ ) {
        Ret = fread(&TSP->CollisionData->Normal[i],sizeof(TSP->CollisionData->Normal[i]),1,InFile);
        if( Ret != 1 ) {
            printf("TSPReadCollisionChunk:Early failure when reading normal data.\n");
            return;
        }
        printf("-- Normal %i --\n",i);
        printf("%i;%i;%i\n",TSP->CollisionData->Normal[i].Position.x,TSP->CollisionData->Normal[i].Position.y,TSP->CollisionData->Normal[i].Position.z);
//         printf("Pad is %i\n",TSP->CollisionData->Normal[i].Pad);
        assert(TSP->CollisionData->Normal[i].Pad == 0);
    }
    DPrintf("TSPReadCollisionChunk:Faces at %li\n",ftell(InFile));
    TSP->CollisionData->Face = malloc(TSP->CollisionData->Header.NumFaces * sizeof(TSPCollisionFace_t));
    for( i = 0; i < TSP->CollisionData->Header.NumFaces; i++ ) {
        Ret = fread(&TSP->CollisionData->Face[i],sizeof(TSP->CollisionData->Face[i]),1,InFile);
        if( Ret != 1 ) {
            printf("TSPReadCollisionChunk:Early failure when reading face data.\n");
            return;
        }
        printf("-- Face %i --\n",i);
//         printf("V0|V1|V2:%u %u %u\n",TSP->CollisionData->Face[i].V0,TSP->CollisionData->Face[i].V1,TSP->CollisionData->Face[i].V2);
        if( TSPIsVersion3(TSP) ) {
            TSP->CollisionData->Face[i].NormalIndex = TSP->CollisionData->Face[i].NormalIndex & 0xFFF;
        }
        assert(TSP->CollisionData->Face[i].NormalIndex < TSP->CollisionData->Header.NumNormals);
        printf("Normal Index:%u\n",TSP->CollisionData->Face[i].NormalIndex);

    }
//     assert(ftell(InFile) == GetFileLength(InFile));
}

TSPCollision_t *TSPGetCollisionDataFromPoint(TSP_t *TSPList,TSPVec3_t Point)
{
    TSP_t *TSP;

    for( TSP = TSPList; TSP; TSP = TSP->Next ) {
        if( Point.x >= TSP->CollisionData->Header.CollisionBoundMinX && Point.x <= TSP->CollisionData->Header.CollisionBoundMaxX &&
        Point.z >= TSP->CollisionData->Header.CollisionBoundMinZ && Point.z <= TSP->CollisionData->Header.CollisionBoundMaxZ ) {
            return TSP->CollisionData;
        }
    }
    return NULL;
}

void TSPVec3ToVec2(TSPVec3_t Point,vec2 Out)
{
    Out[0] = Point.x;
    Out[1] = Point.z;
}

float TSPFixedToFloat(int input,int FixedShift)
{
    return ((float)input / (float)(1 << FixedShift));
}

int TSPGetYFromCollisionFace(TSPCollision_t *CollisionData,TSPVec3_t Point,TSPCollisionFace_t *Face)
{
    float OutY;
    float SolveFaceY;
    TSPVec3_t Normal;
    vec2 Point2D;
    
    TSPVec3ToVec2(Point,Point2D);

    
    Normal = CollisionData->Normal[Face->NormalIndex].Position;
    if( abs(Normal.y) < 257 ) {
        printf("TSPGetYFromCollisionFace:Returning it normal...\n");
        OutY = TSPFixedToFloat(Normal.y,15);
    } else {
        printf("Normal fixed is:%i;%i;%i\n",Normal.x,Normal.y,Normal.z);
        float NormalX;
        float NormalY;
        float NormalZ;
        float PointX;
        float PointZ;
        printf("Normal as int is:%i;%i;%i\n",Normal.x,Normal.y,Normal.z);
        NormalX = /*FixedToInt*/TSPFixedToFloat(Normal.x,15);
        NormalY = /*FixedToInt*/TSPFixedToFloat(Normal.y,15);
        NormalZ = /*FixedToInt*/TSPFixedToFloat(Normal.z,15);
        PointX = /*fixed_to_float*/(Point2D[0]);
        PointZ = /*fixed_to_float*/(Point2D[1]);
        printf("Normal as float is:%f;%f;%f\n",NormalX,NormalY,NormalZ);
//         DistanceOffset = ;
        printf("TSPGetYFromCollisionFace:DistanceOffset Fixed:%i Real:%i\n",Face->PlaneDistance << 0xf,Face->PlaneDistance );
        SolveFaceY = -(NormalX * PointX + NormalZ * PointZ + (Face->PlaneDistance /*<< 0xf*/));
        OutY = SolveFaceY / NormalY;
    }
    return (int) OutY;
}

//Cross product that returns the sign of the new vector.
float TSPSign (vec2 p1, vec2 p2, vec2 p3)
{
    return (p1[0] - p3[0]) * (p2[1] - p3[1]) - (p2[0] - p3[0]) * (p1[1] - p3[1]);
}

bool TSPPointInTriangle (TSPCollision_t *CollisionData,TSPVec3_t Point,TSPCollisionFace_t *Face)
{
    float d1, d2, d3;
    bool HasNegative, HasPositive;
    vec2 v1;
    vec2 v2;
    vec2 v3;
    vec2 pt;
 
    TSPVec3ToVec2(Point,pt);
    TSPVec3ToVec2(CollisionData->Vertex[Face->V0].Position,v1);
    TSPVec3ToVec2(CollisionData->Vertex[Face->V1].Position,v2);
    TSPVec3ToVec2(CollisionData->Vertex[Face->V2].Position,v3);
    
    if( CollisionData->Normal[Face->NormalIndex].Position.y > 0 ) {
        return false;
    }

    d1 = TSPSign(pt, v1, v2);
    d2 = TSPSign(pt, v2, v3);
    d3 = TSPSign(pt, v3, v1);

    HasNegative = (d1 < 0) || (d2 < 0) || (d3 < 0);
    HasPositive = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(HasNegative && HasPositive);
}

int TSPCheckCollisionFaceIntersection(TSPCollision_t *CollisionData,TSPVec3_t Point,int StartingFaceListIndex,int NumFaces,int *OutY)
{
    int i;
    TSPCollisionFace_t *CurrentFace;
    int FaceListIndex;
    int FaceIndex;
    int Y;
    int MinY;
    
    MinY = 99999;
    printf("TSPCheckCollisionFaceIntersection:Checking %i faces\n",NumFaces);
    FaceListIndex = StartingFaceListIndex;
    i = 0;

    while( i < NumFaces ) {
        //First fetch the face index from Face Index array...
        FaceIndex = CollisionData->FaceIndexList[FaceListIndex];
        //Then grab the corresponding face from the face array...
        CurrentFace = &CollisionData->Face[FaceIndex];
        printf("Iteration %i\n",i);
        if( TSPPointInTriangle(CollisionData,Point,CurrentFace) == 1) {
            printf("Point is in face %i...grabbing Y value\n",FaceIndex);
            Y = TSPGetYFromCollisionFace(CollisionData,Point,CurrentFace);
            printf("Got %i as Y PointY was:%i\n",Y,Point.y);
            if( Y < MinY ) {
                MinY = Y;
            }
        } else {
            printf("Missed face %i...\n",FaceIndex);
        }
        //Make sure to increment it in order to fetch the next face.
        FaceListIndex++;
        i++;
    }
    if( MinY != 99999 ) {
        *OutY = MinY;
        return 1;
    }
    return -1;
}

int TSPGetPointYComponentFromKDTree(TSPVec3_t Point,TSP_t *TSPList,int *PropertySetFileIndex,int *OutY)
{
    TSPCollision_t *CollisionData;
    TSPCollisionKDTreeNode_t *Node;
    int WorldBoundMinX;
    int WorldBoundMinZ;
    int MinValue;
    int CurrentNode;
    
    CollisionData = TSPGetCollisionDataFromPoint(TSPList,Point);
    
    if( CollisionData == NULL ) {
        DPrintf("TSPGetPointYComponentFromKDTree:Point wasn't in any collision data...\n");
        return -1;
    }
    
    WorldBoundMinX = CollisionData->Header.CollisionBoundMinX;
    WorldBoundMinZ = CollisionData->Header.CollisionBoundMinZ;
    
    CurrentNode = 0;
    
    while( 1 ) {
//         CurrentPlaneIndex = (CurrentPlane - GOffset) / sizeof(TSPCollisionG_t);
        printf("Node Index %i\n",CurrentNode);
        Node = &CollisionData->KDTree[CurrentNode];
        if( Node->Child0 < 0 ) {
            printf("Done...found a leaf...node %i FaceIndex:%i Child0:%i NumFaces:%i Child1:%i PropertySetFileIndex:%i\n",CurrentNode,
                   CollisionData->FaceIndexList[Node->Child1],
                   Node->Child0,~Node->Child0,Node->Child1,Node->PropertySetFileIndex);
            if( PropertySetFileIndex != NULL ) {
                *PropertySetFileIndex = Node->PropertySetFileIndex;
            }
            return TSPCheckCollisionFaceIntersection(CollisionData,Point,Node->Child1,~Node->Child0,OutY);
        }
        if( Node->Child1 < 0 ) {
            MinValue = WorldBoundMinZ + Node->MaxX;
            if (Point.z < MinValue) {
                CurrentNode = Node->Child0;
            } else {
                CurrentNode = ~Node->Child1;
                WorldBoundMinZ = MinValue;
            }
        } else {
            MinValue = WorldBoundMinX + Node->MaxX;
            if( Point.x < MinValue ) {
                CurrentNode = Node->Child0;
            } else {
                CurrentNode = Node->Child1;
                WorldBoundMinX = MinValue;
            }
        }
    }
    return -1;
}

TSP_t *TSPLoad(char *FName,int TSPNumber)
{
    FILE *TSPFile;
    TSP_t *TSP;
    
    TSPFile = fopen(FName,"rb");
    
    if( TSPFile == NULL ) {
        DPrintf("Failed opening TSP File %s.\n",FName);
        return NULL;
    }
    TSP = malloc(sizeof(TSP_t));
    TSP->VAOList = NULL;
    TSP->CollisionVAOList = NULL;
    TSP->Next = NULL;
    TSP->Number = TSPNumber;
    TSP->Face = NULL;
    TSP->TransparentFaceList = NULL;
    TSP->TransparentVAO = NULL;
    strcpy(TSP->FName,GetBaseName(FName));
    
    fread(&TSP->Header.Id,sizeof(TSP->Header.Id),1,TSPFile);
    fread(&TSP->Header.Version,sizeof(TSP->Header.Version),1,TSPFile);
    fread(&TSP->Header.NumNodes,sizeof(TSP->Header.NumNodes),1,TSPFile);
    fread(&TSP->Header.NodeOffset,sizeof(TSP->Header.NodeOffset),1,TSPFile);
    fread(&TSP->Header.NumFaces,sizeof(TSP->Header.NumFaces),1,TSPFile);
    fread(&TSP->Header.FaceOffset,sizeof(TSP->Header.FaceOffset),1,TSPFile);
    fread(&TSP->Header.NumVertices,sizeof(TSP->Header.NumVertices),1,TSPFile);
    fread(&TSP->Header.VertexOffset,sizeof(TSP->Header.VertexOffset),1,TSPFile);
    fread(&TSP->Header.NumB,sizeof(TSP->Header.NumB),1,TSPFile);
    fread(&TSP->Header.BOffset,sizeof(TSP->Header.BOffset),1,TSPFile);
    fread(&TSP->Header.NumColors,sizeof(TSP->Header.NumColors),1,TSPFile);
    fread(&TSP->Header.ColorOffset,sizeof(TSP->Header.ColorOffset),1,TSPFile);
    fread(&TSP->Header.NumC,sizeof(TSP->Header.NumC),1,TSPFile);
    fread(&TSP->Header.COffset,sizeof(TSP->Header.COffset),1,TSPFile);
    fread(&TSP->Header.NumDynamicDataBlock,sizeof(TSP->Header.NumDynamicDataBlock),1,TSPFile);
    fread(&TSP->Header.DynamicDataOffset,sizeof(TSP->Header.DynamicDataOffset),1,TSPFile);
    fread(&TSP->Header.CollisionOffset,sizeof(TSP->Header.CollisionOffset),1,TSPFile);
    
    if( TSPIsVersion3(TSP) ) {
        fread(&TSP->Header.NumTextureInfo,sizeof(TSP->Header.NumTextureInfo),1,TSPFile);
        fread(&TSP->Header.TextureInfoOffset,sizeof(TSP->Header.TextureInfoOffset),1,TSPFile);
    } else {
        TSP->Header.NumTextureInfo = -1;
        TSP->Header.TextureInfoOffset = -1;
        TSP->TextureData = NULL;
    }
    
    DPrintf("Sizeof TSPHeader is %li\n",sizeof(TSPHeader_t));
    DPrintf(" -- TSP HEADER --\n");
    DPrintf("TSP Number: %i\n",TSP->Number);
    DPrintf("TSP File: %s\n",TSP->FName);
    DPrintf("Id:%u\n",TSP->Header.Id);
    DPrintf("Version:%u\n",TSP->Header.Version);
    DPrintf("NumNodes:%i NodeOffset:%i\n",TSP->Header.NumNodes,TSP->Header.NodeOffset);
    DPrintf("NumFaces:%i FaceOffset:%i\n",TSP->Header.NumFaces,TSP->Header.FaceOffset);
    DPrintf("NumVertices:%i VertexOffset:%i\n",TSP->Header.NumVertices,TSP->Header.VertexOffset);
    DPrintf("NumB:%i BOffset:%i\n",TSP->Header.NumB,TSP->Header.BOffset);
    DPrintf("NumColors:%i ColorOffset:%i\n",TSP->Header.NumColors,TSP->Header.ColorOffset);
    DPrintf("NumC:%i COffset:%i\n",TSP->Header.NumC,TSP->Header.COffset);
    DPrintf("NumDynamicDataBlock:%i DynamicDataOffset:%i\n",TSP->Header.NumDynamicDataBlock,TSP->Header.DynamicDataOffset);
    DPrintf("CollisionOffset:%i\n",TSP->Header.CollisionOffset);
    DPrintf("NumTextureInfo:%i TextureInfoOffset:%i\n",TSP->Header.NumTextureInfo,TSP->Header.TextureInfoOffset);

    assert(ftell(TSPFile) == TSP->Header.NodeOffset);
    TSPReadNodeChunk(TSP,TSPFile);
    if( TSPIsVersion3(TSP) ) {
        fseek(TSPFile,TSP->Header.VertexOffset,SEEK_SET);
    } else {
        assert(ftell(TSPFile) == TSP->Header.FaceOffset);
        TSPReadFaceChunk(TSP,TSPFile);
    }
    assert(ftell(TSPFile) == TSP->Header.VertexOffset);
    TSPReadVertexChunk(TSP,TSPFile);
    assert(TSP->Header.NumB == 0);
    assert(ftell(TSPFile) == TSP->Header.ColorOffset);
    TSPReadColorChunk(TSP,TSPFile);
    assert(TSP->Header.NumC == 0);
    TSPReadDynamicDataChunk(TSP,TSPFile);
    if( TSPIsVersion3(TSP) ) {
        assert(ftell(TSPFile) == TSP->Header.TextureInfoOffset);
        TSPReadTextureInfoChunk(TSP,TSPFile);
    }
    assert(ftell(TSPFile) == TSP->Header.CollisionOffset);
    TSPReadCollisionChunk(TSP,TSPFile);
    fclose(TSPFile);
    return TSP;
}
