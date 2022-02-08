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

#include "MOHLevelViewer.h"

void TSPFree(TSP_t *TSP)
{
    int i;
//     TSP->Node
//     TSPRecursiveNodeFree(&TSP->Node[0]);
    for( i = 0; i < TSP->Header.NumNodes; i++ ) {
        VaoFree(TSP->Node[i].BBoxVao);
        VaoFree(TSP->Node[i].LeafFaceListVao);
        VaoFree(TSP->Node[i].LeafCollisionFaceListVao);
        //Don't bother traversing the tree since we can walk the whole array and free all the pointers...
        TSP->Node[i].Child[0] = NULL;
        TSP->Node[i].Child[1] = NULL;
        TSP->Node[i].Next = NULL;
    }
    free(TSP->Node);
    free(TSP->Face);
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
    
    VaoFree(TSP->VaoList);
    VaoFree(TSP->CollisionVaoList);
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

void Vec4FromXYZ(float x,float y,float z,vec4 Out)
{
    Out[0] = x;
    Out[1] = y;
    Out[2] = z;
    Out[3] = 1;
}

void TSPDumpDataToFile(TSP_t *TSPList,FILE* OutFile)
{
    TSP_t *Iterator;
    char Buffer[256];
    int i;
    int t;
    float Width = 256.f;
    float Height = 256.f;
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
            NewPos = Vec3_Build(Iterator->Vertex[i].Position.x,Iterator->Vertex[i].Position.y,Iterator->Vertex[i].Position.z);
            Vec_RotateXAxis(DEGTORAD(180.f),&NewPos);
            sprintf(Buffer,"v %f %f %f %f %f %f\n",NewPos.x / 4096.f,NewPos.y / 4096.f,NewPos.z / 4096.f,
                Iterator->Color[i].r / 255.f,Iterator->Color[i].g / 255.f,Iterator->Color[i].b / 255.f
            );
            fwrite(Buffer,strlen(Buffer),1,OutFile);            
        }
        for( i = Iterator->Header.NumFaces - 1; i >= 0 ; i-- ) {
            float U0 = (((float)Iterator->Face[i].UV0.u)/Width);
            float V0 = /*255 -*/1.f-(((float)Iterator->Face[i].UV0.v) / Height);
            float U1 = (((float)Iterator->Face[i].UV1.u) / Width);
            float V1 = /*255 -*/1.f-(((float)Iterator->Face[i].UV1.v) /Height);
            float U2 = (((float)Iterator->Face[i].UV2.u) /Width);
            float V2 = /*255 -*/1.f-(((float)Iterator->Face[i].UV2.v) / Height);
            sprintf(Buffer,"vt %f %f\nvt %f %f\nvt %f %f\n",U0,V0,U1,V1,U2,V2);
            fwrite(Buffer,strlen(Buffer),1,OutFile);    
        }
        for( i = 0; i < Iterator->Header.NumFaces; i++ ) {
            int Vert0 = Iterator->Face[i].V0;
            int Vert1 = Iterator->Face[i].V1;
            int Vert2 = Iterator->Face[i].V2;
            int BaseFaceUV = i * 3;
            int ColorMode = (Iterator->Face[i].TSB.AsShort & 0x80) >> 7;
            int VRamPage = Iterator->Face[i].TSB.AsShort & 0x1F;
            if( ColorMode == 1 ) {
                sprintf(Buffer,"usemtl vram_8_page_%i\n",VRamPage);
            } else {
                sprintf(Buffer,"usemtl vram_4_page_%i\n",VRamPage);
            }
            fwrite(Buffer,strlen(Buffer),1,OutFile);
            sprintf(Buffer,"f %i/%i %i/%i %i/%i\n",-(Vert0+1),-(BaseFaceUV+3),-(Vert1+1),-(BaseFaceUV+2),-(Vert2+1),-(BaseFaceUV+1));
            fwrite(Buffer,strlen(Buffer),1,OutFile);
        }
//         fwrite(Buffer,strlen(Buffer),1,OutFile);
//         break;
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
void TSPCreateVAO(TSP_t *TSPList)
{
    TSP_t *Iterator;
    float Width = 256.f;
    float Height = 256.f;
    int i;
    float *VertexData;
    int VertexSize;
    int VertexPointer;
    int Stride;
    int VertexOffset;
    int TextureOffset;
    int ColorOffset;

    for( Iterator = TSPList; Iterator; Iterator = Iterator->Next ) {
        for( i = 0; i < Iterator->Header.NumFaces; i++ ) {
            Vao_t *Vao;
            int Vert0 = Iterator->Face[i].V0;
            int Vert1 = Iterator->Face[i].V1;
            int Vert2 = Iterator->Face[i].V2;

            float U0 = (((float)Iterator->Face[i].UV0.u)/Width);
            float V0 = /*255 -*/(((float)Iterator->Face[i].UV0.v) / Height);
            float U1 = (((float)Iterator->Face[i].UV1.u) / Width);
            float V1 = /*255 -*/(((float)Iterator->Face[i].UV1.v) /Height);
            float U2 = (((float)Iterator->Face[i].UV2.u) /Width);
            float V2 = /*255 -*/(((float)Iterator->Face[i].UV2.v) / Height);

//             int TexturePage = Iterator->Face[i].TSB.AsShort & 0x1F;

            //            XYZ UV RGB
            Stride = (3 + 2 + 3) * sizeof(float);
        
            VertexOffset = 0;
            TextureOffset = 3;
            ColorOffset = 5;
        
            VertexSize = Stride;
            VertexData = malloc(VertexSize * 3/** sizeof(float)*/);
            VertexPointer = 0;
                    
            VertexData[VertexPointer] =   Iterator->Vertex[Vert0].Position.x;
            VertexData[VertexPointer+1] = Iterator->Vertex[Vert0].Position.y;
            VertexData[VertexPointer+2] = Iterator->Vertex[Vert0].Position.z;
            VertexData[VertexPointer+3] = U0;
            VertexData[VertexPointer+4] = V0;
            VertexData[VertexPointer+5] = Iterator->Color[Vert0].r / 255.f;
            VertexData[VertexPointer+6] = Iterator->Color[Vert0].g / 255.f;
            VertexData[VertexPointer+7] = Iterator->Color[Vert0].b / 255.f;
            VertexPointer += 8;
            
            VertexData[VertexPointer] =   Iterator->Vertex[Vert1].Position.x;
            VertexData[VertexPointer+1] = Iterator->Vertex[Vert1].Position.y;
            VertexData[VertexPointer+2] = Iterator->Vertex[Vert1].Position.z;
            VertexData[VertexPointer+3] = U1;
            VertexData[VertexPointer+4] = V1;
            VertexData[VertexPointer+5] = Iterator->Color[Vert1].r / 255.f;
            VertexData[VertexPointer+6] = Iterator->Color[Vert1].g / 255.f;
            VertexData[VertexPointer+7] = Iterator->Color[Vert1].b / 255.f;
            VertexPointer += 8;
            
            VertexData[VertexPointer] =   Iterator->Vertex[Vert2].Position.x;
            VertexData[VertexPointer+1] = Iterator->Vertex[Vert2].Position.y;
            VertexData[VertexPointer+2] = Iterator->Vertex[Vert2].Position.z;
            VertexData[VertexPointer+3] = U2;
            VertexData[VertexPointer+4] = V2;
            VertexData[VertexPointer+5] = Iterator->Color[Vert2].r / 255.f;
            VertexData[VertexPointer+6] = Iterator->Color[Vert2].g / 255.f;
            VertexData[VertexPointer+7] = Iterator->Color[Vert2].b / 255.f;
            VertexPointer += 8;
            
            Vao = VaoInitXYZUVRGB(VertexData,VertexSize * 3,Stride,VertexOffset,TextureOffset,ColorOffset,Iterator->Face[i].TSB.AsShort,-1);            
            Vao->Next = Iterator->VaoList;
            Iterator->VaoList = Vao;
            free(VertexData);
        }
    }

}

void TSPCreateNodeBBoxVAO(TSP_t *TSPList)
{
    TSP_t *Iterator;
    float Width = 256.f;
    float Height = 256.f;
    int VertexOffset;
    int TextureOffset;
    int ColorOffset;
    int i;
    int j;
    float *VertexData;
    int VertexSize;
    int VertexPointer;
    int Stride;
    int TotalFaceCount = 0;
    int Vert0;
    int Vert1;
    int Vert2;
    float U0,V0;
    float U1,V1;
    float U2,V2;
    short TSB;
    short CBA;
    TSPDynamicFaceData_t *DynamicData;
    Vao_t *Vao;
    
    for( Iterator = TSPList; Iterator; Iterator = Iterator->Next ) {
        for( i = 0; i < Iterator->Header.NumNodes; i++ ) {
            if( Iterator->Node[i].NumFaces != 0 ) {
                TotalFaceCount += Iterator->Node[i].NumFaces;
                int Base = Iterator->Node[i].BaseData / sizeof(TSPFace_t);
                int Target = Base + Iterator->Node[i].NumFaces;
                for( j = Base; j < Target; j++ ) {
                    int ColorMode = (Iterator->Face[j].TSB.AsShort & 0x80) >> 7;
                    int VRamPage = Iterator->Face[j].TSB.AsShort & 0x1F;
                    int ABRRate = (Iterator->Face[j].TSB.AsShort & 0x60) >> 5;
                    if( VRamPage != 8 || ColorMode != 1 ) {
//                         continue;
                    }
                    DPrintf("TSB is %u\n",Iterator->Face[j].TSB.AsShort);
                    DPrintf("Expected VRam Page:%i\n",VRamPage);
                    DPrintf("Expected Color Mode:%i\n",ColorMode);
                    DPrintf("Expected ABR rate:%i\n",ABRRate);
                    Vert0 = Iterator->Face[j].V0;
                    Vert1 = Iterator->Face[j].V1;
                    Vert2 = Iterator->Face[j].V2;

//                     DynamicData = GetDynamicDataByFaceIndex(Iterator,j,2);
//                     if( DynamicData != NULL ) {
//                         U0 = (((float)DynamicData->UV0.u)/Width);
//                         V0 = /*255 -*/(((float)DynamicData->UV0.v) / Height);
//                         U1 = (((float)DynamicData->UV1.u) / Width);
//                         V1 = /*255 -*/(((float)DynamicData->UV1.v) /Height);
//                         U2 = (((float)DynamicData->UV2.u) /Width);
//                         V2 = /*255 -*/(((float)DynamicData->UV2.v) / Height);
//                         TSB = DynamicData->TSB;
//                         CBA = DynamicData->CBA;
//                     } else {
                        U0 = (((float)Iterator->Face[j].UV0.u)/Width);
                        V0 = /*255 -*/(((float)Iterator->Face[j].UV0.v) / Height);
                        U1 = (((float)Iterator->Face[j].UV1.u) / Width);
                        V1 = /*255 -*/(((float)Iterator->Face[j].UV1.v) /Height);
                        U2 = (((float)Iterator->Face[j].UV2.u) /Width);
                        V2 = /*255 -*/(((float)Iterator->Face[j].UV2.v) / Height);
                        TSB = Iterator->Face[j].TSB.AsShort;
                        CBA = Iterator->Face[j].CBA.AsShort;
//                     }
                    
                    
                    DPrintf("Tex Coords are %i;%i %i;%i %i;%i\n",
                            Iterator->Face[j].UV0.u,Iterator->Face[j].UV0.v,
                            Iterator->Face[j].UV1.u,Iterator->Face[j].UV1.v,
                            Iterator->Face[j].UV2.u,Iterator->Face[j].UV2.v);
            //            XYZ UV RGB
                    Stride = (3 + 2 + 3) * sizeof(float);
                
                    VertexOffset = 0;
                    TextureOffset = 3;
                    ColorOffset = 5;
                
                    VertexSize = Stride;
                    VertexData = malloc(VertexSize * 3/** sizeof(float)*/);
                    VertexPointer = 0;
                            
                    VertexData[VertexPointer] =   Iterator->Vertex[Vert0].Position.x;
                    VertexData[VertexPointer+1] = Iterator->Vertex[Vert0].Position.y;
                    VertexData[VertexPointer+2] = Iterator->Vertex[Vert0].Position.z;
                    VertexData[VertexPointer+3] = U0;
                    VertexData[VertexPointer+4] = V0;
                    VertexData[VertexPointer+5] = Iterator->Color[Vert0].r / 255.f;
                    VertexData[VertexPointer+6] = Iterator->Color[Vert0].g / 255.f;
                    VertexData[VertexPointer+7] = Iterator->Color[Vert0].b / 255.f;
                    VertexPointer += 8;
                    
                    VertexData[VertexPointer] =   Iterator->Vertex[Vert1].Position.x;
                    VertexData[VertexPointer+1] = Iterator->Vertex[Vert1].Position.y;
                    VertexData[VertexPointer+2] = Iterator->Vertex[Vert1].Position.z;
                    VertexData[VertexPointer+3] = U1;
                    VertexData[VertexPointer+4] = V1;
                    VertexData[VertexPointer+5] = Iterator->Color[Vert1].r / 255.f;
                    VertexData[VertexPointer+6] = Iterator->Color[Vert1].g / 255.f;
                    VertexData[VertexPointer+7] = Iterator->Color[Vert1].b / 255.f;
                    VertexPointer += 8;
                    
                    VertexData[VertexPointer] =   Iterator->Vertex[Vert2].Position.x;
                    VertexData[VertexPointer+1] = Iterator->Vertex[Vert2].Position.y;
                    VertexData[VertexPointer+2] = Iterator->Vertex[Vert2].Position.z;
                    VertexData[VertexPointer+3] = U2;
                    VertexData[VertexPointer+4] = V2;
                    VertexData[VertexPointer+5] = Iterator->Color[Vert2].r / 255.f;
                    VertexData[VertexPointer+6] = Iterator->Color[Vert2].g / 255.f;
                    VertexData[VertexPointer+7] = Iterator->Color[Vert2].b / 255.f;
                    VertexPointer += 8;
                    
                    Vao = VaoInitXYZUVRGB(VertexData,VertexSize * 3,Stride,VertexOffset,TextureOffset,ColorOffset,
                                          TSB,CBA);            
                    Vao->Next = Iterator->Node[i].LeafFaceListVao;
                    Iterator->Node[i].LeafFaceListVao = Vao;
                    free(VertexData);                    
                }
            }
            
            //       XYZ
            Stride = (3) * sizeof(float);
        
            VertexSize = Stride;
            VertexData = malloc(VertexSize * 8/** sizeof(float)*/);
            VertexPointer = 0;
                    
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Min.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Min.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Min.z;
            VertexPointer += 3;
                        
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Min.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Min.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Max.z;
            VertexPointer += 3;
            
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Max.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Min.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Max.z;
            VertexPointer += 3;
            
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Max.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Min.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Min.z;
            VertexPointer += 3;
            
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Min.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Max.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Min.z;
            VertexPointer += 3;
            
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Min.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Max.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Max.z;
            VertexPointer += 3;
                        
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Max.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Max.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Max.z;
            VertexPointer += 3;
            
            VertexData[VertexPointer] =   Iterator->Node[i].BBox.Max.x;
            VertexData[VertexPointer+1] = Iterator->Node[i].BBox.Max.y;
            VertexData[VertexPointer+2] = Iterator->Node[i].BBox.Min.z;
            VertexPointer += 3;

            
            unsigned short Index[16] = {
                0, 1, 2, 3,
                4, 5, 6, 7,
                0, 4, 1, 5, 2, 6, 3, 7
            };
            
            Iterator->Node[i].BBoxVao = VaoInitXYZIBO(VertexData,VertexSize * 8,Stride,Index,sizeof(Index),0);            
            free(VertexData);
        }
        DPrintf("Linearly we got to draw %i faces (%i NumFaces in header) for %s.\n",TotalFaceCount,Iterator->Header.NumFaces,Iterator->FName);
        TotalFaceCount = 0;
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
        for( i = 0; i < Iterator->CollisionData->Header.NumFaces; i++ ) {
            Vao_t *Vao;
            int Vert0 = Iterator->CollisionData->Face[i].V0;
            int Vert1 = Iterator->CollisionData->Face[i].V1;
            int Vert2 = Iterator->CollisionData->Face[i].V2;
            //       XYZ
            Stride = (3) * sizeof(float);
                
            VertexSize = Stride;
            VertexData = malloc(VertexSize * 3/** sizeof(float)*/);
            VertexPointer = 0;
                    
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
            
            Vao = VaoInitXYZ(VertexData,VertexSize * 3,Stride,0);            
            Vao->Next = Iterator->CollisionVaoList;
            Iterator->CollisionVaoList = Vao;
            free(VertexData);
        }
    }

}

void DrawTSP(TSP_t *TSP)
{
#if 1
    GL_Shader_t *Shader;
    Vao_t *Iterator;
    int MVPMatrixID;
    
    Shader = Shader_Cache("TSPShader","Shaders/TSPVertexShader.glsl","Shaders/TSPFragmentShader.glsl");
    glUseProgram(Shader->ProgramID);

    MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
    glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
    
    if( Level->Settings.WireFrame ) {
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    for( Iterator = TSP->VaoList; Iterator; Iterator = Iterator->Next ) {
//         int VRamPage = Iterator->TSB & 0x1F;
        int ColorMode = (Iterator->TSB & 0x80) >> 7;
        int VRamPage = Iterator->TSB & 0x1F;
//         int ABRRate = (Iterator->TSB & 0x60) >> 5;
#if 1
        //DO THIS ONLY IF ABE IS ENABLED...
//         int Trans = (Iterator->TSB & 0x30) >> 4;
//         glEnable(GL_BLEND);
//         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//         if( ABRRate == 0 ) {
//             glBlendEquation(GL_FUNC_ADD);
//             glBlendColor(1.0, 1.0, 1.0, 0.5);
//             glBlendFunc(GL_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);
//         } else if( ABRRate == 1 ) {
//             glBlendEquation(GL_FUNC_ADD);
//             glBlendFunc(GL_ONE, GL_ONE);
//         } else if( ABRRate == 2 ) {
//             glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
//             glBlendFunc(GL_ONE, GL_ONE);
//         } else if ( ABRRate == 3 ) {
//             glBlendEquation(GL_FUNC_ADD);
//             glBlendColor(1.0, 1.0, 1.0, 0.25);
//             glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
//         } else {
//             glDisable(GL_BLEND);
//         }
#endif
        
        if( ColorMode == 1) {
            glBindTexture(GL_TEXTURE_2D, Level->VRam->Page8Bit[VRamPage].TextureID);
        } else {
            glBindTexture(GL_TEXTURE_2D, Level->VRam->Page4Bit[VRamPage].TextureID);
        }
        glBindVertexArray(Iterator->VaoID[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D,0);
        glDisable(GL_BLEND);
    }
    glUseProgram(0);
#endif

/*
    glBindBuffer(GL_ARRAY_BUFFER, VboID);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 5*sizeof(GLfloat), NULL);
    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 5*sizeof(GLfloat), ((char*)NULL)+3*sizeof(GLfloat) );
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
}

Vec3_t Vec3_FromTSPVec3(TSPVec3_t In)
{
    return Vec3_Build(In.x,In.y,In.z);
}

void TSPPrintVec3(TSPVec3_t Vector)
{
    printf("(%i;%i;%i)\n",Vector.x,Vector.y,Vector.z);
}

void TSPPrintColor(TSPColor_t Color)
{
    printf("RGBA:(%i;%i;%i;%i)\n",Color.r,Color.g,Color.b,Color.a);
}

void DrawTSPBox(TSPNode_t Node)
{
    GL_Shader_t *Shader;
    vec4 BoxColor;
    int MVPMatrixID;
    int ColorID;
    

    
    if( Node.NumFaces != 0 ) {
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
    
    
    Shader = Shader_Cache("TSPBBoxShader","Shaders/TSPBBoxVertexShader.glsl","Shaders/TSPBBoxFragmentShader.glsl");
    glUseProgram(Shader->ProgramID);
    
    MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
    glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
        
    ColorID = glGetUniformLocation(Shader->ProgramID,"Color");
    glUniform4fv(ColorID,1,BoxColor);
    
    glBindVertexArray(Node.BBoxVao->VaoID[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Node.BBoxVao->IboID[0]);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(unsigned short)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(unsigned short)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glUseProgram(0);
}

void DrawTSPCollisionData(TSP_t *TSP)
{
    Vao_t *Iterator;
    GL_Shader_t *Shader;
    int MVPMatrixID;
    
    if( !TSP ) {
        DPrintf("Invalid TSP...\n");
        return;
    }
    
    Shader = Shader_Cache("TSPCollisionShader","Shaders/TSPCollisionVertexShader.glsl","Shaders/TSPCollisionFragmentShader.glsl");
    glUseProgram(Shader->ProgramID);

    MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
    glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
    
    for( Iterator = TSP->CollisionVaoList; Iterator; Iterator = Iterator->Next ) {
        glBindVertexArray(Iterator->VaoID[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
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
    GL_Shader_t *Shader;
    Vao_t *Iterator;
    int MVPMatrixID;
    int EnableLightingID;
    int i;
    
    if( !Node ) {
        return;
    }
    
    if( LevelSettings.EnableFrustumCulling && !TSPBoxInFrustum(Camera,Node->BBox) ) {
        return;
    }
    
    if( Level->Settings.ShowAABBTree ) {
        DrawTSPBox(*Node);
    }

    if( Node->NumFaces != 0 ) {
        if( Level->Settings.ShowMap ) {
            Shader = Shader_Cache("TSPShader","Shaders/TSPVertexShader.glsl","Shaders/TSPFragmentShader.glsl");
            glUseProgram(Shader->ProgramID);

            MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
            glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
            EnableLightingID = glGetUniformLocation(Shader->ProgramID,"EnableLighting");
            glUniform1i(EnableLightingID, LevelSettings.EnableLighting);
            if( Level->Settings.WireFrame ) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            for( Iterator = Node->LeafFaceListVao; Iterator; Iterator = Iterator->Next ) {
                int VRamPage = Iterator->TSB & 0x1F;
                
//         int Trans = (Iterator->TextureID & 0x30) >> 4;
//         if( Trans == 0 ) {
                if( (Iterator->TSB & 0xC0) >> 7 == 1) {
                    glBindTexture(GL_TEXTURE_2D, Level->VRam->Page8Bit[VRamPage].TextureID);
                } else {
                    glBindTexture(GL_TEXTURE_2D, Level->VRam->Page4Bit[VRamPage].TextureID);
                }
                glBindVertexArray(Iterator->VaoID[0]);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D,0);
            }
            glDisable(GL_BLEND);
            glUseProgram(0);
        }
    } else {
        for( i = 0; i < 2; i++ ) {
            DrawNode(Node->Child[i],LevelSettings);
        }
        DrawNode(Node->Next,LevelSettings);
    }
//     if( Node->Child[0] != NULL ) {
//         DrawNode(Node->Child1);
        
//     }
//     if( Node->NumFaces != 0 ) {

//     }

//     if( Node->Child[1] != NULL ) {
//         DrawNode(Node->Child2);
//     }
}

void DrawTSPList(Level_t *Level)
{
    TSP_t *TSPData;
    TSP_t *Iterator;
    
    TSPData = Level->TSPList;
    
    if( !TSPData ) {
        printf("DrawTSP:Invalid TSP data\n");
        return;
    }
    
    for( Iterator = TSPData; Iterator; Iterator = Iterator->Next ) {
//             DrawNode(Iterator->BSDTree);
//             DrawTSP(Iterator);
//         for( i = 0; i < Iterator->Header.NumNodes; i++ ) {
        DrawNode(&Iterator->Node[0],Level->Settings);
//                 DPrintf("Drawing %i faces for %s root %i\n",TotalFaceCount2,Iterator->FName,i);
//                 TotalFaceCount2 = 0;
//         }
//             exit(0);
    }
    
    
    if( Level->Settings.ShowCollisionData ) {
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
        fread(&TSP->Node[i].U2.AsInt,sizeof(TSP->Node[i].U2.AsInt),1,InFile);
        fread(&TSP->Node[i].U3.AsInt,sizeof(TSP->Node[i].U3.AsInt),1,InFile);
        fread(&TSP->Node[i].BaseData,sizeof(TSP->Node[i].BaseData),1,InFile);
        DPrintf("TSPReadNodeChunk:Node has %i faces\n",TSP->Node[i].NumFaces);
        DPrintf("TSPReadNodeChunk:Node BaseData %i (References offset %i)\n",TSP->Node[i].BaseData,
                TSP->Node[i].BaseData + TSP->Header.NodeOffset);
        DPrintf("TSPReadNodeChunk:Node U2 %i\n",TSP->Node[i].U2.AsInt);
        DPrintf("TSPReadNodeChunk:Node U3 %i\n",TSP->Node[i].U3.AsInt);
        int Base = TSP->Node[i].BaseData / sizeof(TSPFace_t);
        int Target = Base + TSP->Node[i].NumFaces;
        DPrintf("We need to render %i faces starting from offset %i\n",TSP->Node[i].NumFaces,TSP->Header.FaceOffset + TSP->Node[i].BaseData);
        DPrintf("Face Index goes then from %i to %i\n",Base,Target);
        if( TSP->Node[i].NumFaces == 0 ) {
            fread(&TSP->Node[i].FileOffset.Child1Offset,sizeof(TSP->Node[i].FileOffset.Child1Offset),1,InFile);
            fread(&TSP->Node[i].FileOffset.Child2Offset,sizeof(TSP->Node[i].FileOffset.Child2Offset),1,InFile);
            DPrintf("TSPReadNodeChunk:Child1:%i\n",TSP->Node[i].FileOffset.Child1Offset + TSP->Header.NodeOffset);
            DPrintf("TSPReadNodeChunk:Child2:%i\n",TSP->Node[i].FileOffset.Child2Offset + TSP->Header.NodeOffset);
        }
    }
    TSPLookUpChildNode(TSP,InFile);
    printf("Current file offset is %li\n",ftell(InFile));
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
        printf("CBA:%u\n",TSP->Face[i].CBA.AsShort);
        printf("TSB:%u\n",TSP->Face[i].TSB.AsShort);
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
        assert(TSP->Vertex[i].Pad == 104);
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
    
    TSP->Color = malloc(TSP->Header.NumColors * sizeof(TSPColor_t));
    
    for( i = 0; i < TSP->Header.NumColors; i++ ) {
        Ret = fread(&TSP->Color[i],sizeof(TSPColor_t),1,InFile);
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
        fseek(InFile,TSP->Header.CollisionOffset,SEEK_SET);
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
        assert(TSP->CollisionData->Vertex[i].Pad == 104);
    }
    DPrintf("TSPReadCollisionChunk:Normals at %li\n",ftell(InFile));
    TSP->CollisionData->Normal = malloc(TSP->CollisionData->Header.NumNormals * sizeof(TSPVert_t));
    for( i = 0; i < TSP->CollisionData->Header.NumNormals; i++ ) {
        Ret = fread(&TSP->CollisionData->Normal[i],sizeof(TSP->CollisionData->Normal[i]),1,InFile);
        if( Ret != 1 ) {
            printf("TSPReadCollisionChunk:Early failure when reading normal data.\n");
            return;
        }
//         printf("-- Normal %i --\n",i);
//         PrintTSPVec3(TSP->CollisionData->Normal[i].Position);
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
//         printf("Normal Index:%u\n",TSP->CollisionData->Face[i].NormalIndex);

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
    TSP->VaoList = NULL;
    TSP->CollisionVaoList = NULL;
    TSP->Next = NULL;
    TSP->Number = TSPNumber;
    strcpy(TSP->FName,GetBaseName(FName));
    fread(&TSP->Header,sizeof(TSPHeader_t),1,TSPFile);
    DPrintf("Sizeof TSPHeader is %li\n",sizeof(TSPHeader_t));
    DPrintf(" -- TSP HEADER --\n");
    DPrintf("TSP Number: %i\n",TSP->Number);
    DPrintf("TSP File: %s\n",TSP->FName);
    DPrintf("ID:%u\n",TSP->Header.ID);
    DPrintf("Version:%u\n",TSP->Header.Version);
    DPrintf("NumNodes:%i NodeOffset:%i\n",TSP->Header.NumNodes,TSP->Header.NodeOffset);
    DPrintf("NumFaces:%i FaceOffset:%i\n",TSP->Header.NumFaces,TSP->Header.FaceOffset);
    DPrintf("NumVertices:%i VertexOffset:%i\n",TSP->Header.NumVertices,TSP->Header.VertexOffset);
    DPrintf("NumB:%i BOffset:%i\n",TSP->Header.NumB,TSP->Header.BOffset);
    DPrintf("NumColors:%i ColorOffset:%i\n",TSP->Header.NumColors,TSP->Header.ColorOffset);
    DPrintf("NumC:%i COffset:%i\n",TSP->Header.NumC,TSP->Header.COffset);
    DPrintf("NumDynamicDataBlock:%i DynamicDataOffset:%i\n",TSP->Header.NumDynamicDataBlock,TSP->Header.DynamicDataOffset);
    DPrintf("CollisionOffset:%i\n",TSP->Header.CollisionOffset);

    assert(ftell(TSPFile) == TSP->Header.NodeOffset);
    TSPReadNodeChunk(TSP,TSPFile);
    assert(ftell(TSPFile) == TSP->Header.FaceOffset);
    TSPReadFaceChunk(TSP,TSPFile);
    assert(ftell(TSPFile) == TSP->Header.VertexOffset);
    TSPReadVertexChunk(TSP,TSPFile);
    assert(TSP->Header.NumB == 0);
    assert(ftell(TSPFile) == TSP->Header.ColorOffset);
    TSPReadColorChunk(TSP,TSPFile);
    assert(TSP->Header.NumC == 0);
    TSPReadDynamicDataChunk(TSP,TSPFile);
    assert(ftell(TSPFile) == TSP->Header.CollisionOffset);
    TSPReadCollisionChunk(TSP,TSPFile);
    fclose(TSPFile);
    return TSP;
}
