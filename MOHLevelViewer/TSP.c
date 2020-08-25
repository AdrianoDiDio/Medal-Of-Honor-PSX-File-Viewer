/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
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

void CreateVAO(TSP_t *TSPList)
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

void DrawTSP(TSP_t *TSP)
{
#if 1
    GL_Shader_t *Shader;
    Vao_t *Iterator;
    int MVPMatrixID;
    
    Shader = Shader_Cache("TSPShader","Shaders/TSPVertexShader.glsl","Shaders/TSPFragmentShader.glsl");
    aglUseProgram(Shader->ProgramID);

    MVPMatrixID = aglGetUniformLocation(Shader->ProgramID,"MVPMatrix");
    aglUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
    
    if( Level->Settings.WireFrame ) {
       aglPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
       aglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    for( Iterator = TSP->VaoList; Iterator; Iterator = Iterator->Next ) {
        int VRamPage = Iterator->TSB & 0x1F;
        
#if 0
        //DO THIS ONLY IF ABE IS ENABLED...
        int Trans = (Iterator->TSB & 0x30) >> 4;
        aglEnable(GL_BLEND);
        if( Trans == 0 ) {
            aglBlendEquation(GL_FUNC_ADD);
            aglBlendColor(1.0, 1.0, 1.0, 0.5);
            aglBlendFunc(GL_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);
        } else if( Trans == 1 ) {
            aglBlendEquation(GL_FUNC_ADD);
            aglBlendFunc(GL_ONE, GL_ONE);
        } else if( Trans == 2 ) {
            aglBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
            aglBlendFunc(GL_ONE, GL_ONE);
        } else if ( Trans == 3 ) {
            aglBlendEquation(GL_FUNC_ADD);
            aglBlendColor(1.0, 1.0, 1.0, 0.25);
            aglBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
        } else {
            aglDisable(GL_BLEND);
        }
#endif
        
        if( (Iterator->TSB & 0xC0) >> 7 == 1) {
            aglBindTexture(GL_TEXTURE_2D, Level->VRam->Page8Bit[VRamPage].TextureID);
        } else {
            aglBindTexture(GL_TEXTURE_2D, Level->VRam->Page4Bit[VRamPage].TextureID);
        }
        aglBindVertexArray(Iterator->VaoID[0]);
        aglDrawArrays(GL_TRIANGLES, 0, 3);
        aglBindVertexArray(0);
        aglBindTexture(GL_TEXTURE_2D,0);
    }
    aglUseProgram(0);
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

void DrawTSPBox(TSPBBox_t Box)
{
#if 0
    aglBegin(GL_LINE_LOOP);
    aglVertex3s(Box.Min.x, Box.Min.y, Box.Min.z);
    aglVertex3s(Box.Min.x, Box.Min.y, Box.Max.z);
    aglVertex3s(Box.Max.x, Box.Min.y, Box.Max.z);
    aglVertex3s(Box.Max.x, Box.Min.y, Box.Min.z);
    aglEnd();
    
    aglBegin(GL_LINE_LOOP);
    aglVertex3s(Box.Min.x, Box.Max.y, Box.Min.z);
    aglVertex3s(Box.Min.x, Box.Max.y, Box.Max.z);
    aglVertex3s(Box.Max.x, Box.Max.y, Box.Max.z);
    aglVertex3s(Box.Max.x, Box.Max.y, Box.Min.z);
    aglEnd();
    
    aglBegin(GL_LINES);
    aglVertex3s(Box.Min.x, Box.Min.y, Box.Min.z);
    aglVertex3s(Box.Min.x, Box.Max.y, Box.Min.z);
    aglVertex3s(Box.Min.x, Box.Min.y, Box.Max.z);
    aglVertex3s(Box.Min.x, Box.Max.y, Box.Max.z);

    aglVertex3s(Box.Max.x, Box.Min.y, Box.Min.z);
    aglVertex3s(Box.Max.x, Box.Max.y, Box.Min.z);
    aglVertex3s(Box.Max.x, Box.Min.y, Box.Max.z);
    aglVertex3s(Box.Max.x, Box.Max.y, Box.Max.z);
    aglEnd();
#endif
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

void DrawTSPList(Level_t *Level)
{
    TSP_t *TSPData;
    TSP_t *Iterator;
    
    TSPData = Level->TSPList;
    
    if( !TSPData ) {
        printf("DrawTSP:Invalid TSP data\n");
        return;
    }
    
    if( Level->Settings.ShowMap ) {
        for( Iterator = TSPData; Iterator; Iterator = Iterator->Next ) {
//             if( !IsTSPInRenderArray(Level,Iterator->Number) ) {
//                 continue;
//             }
            DrawTSP(Iterator);
        }
    }
#if 0
    TSP_t *Iterator;
    float Width = 256.f;
    float Height = 256.f;
    int i;
    int j;
    aglEnable(GL_TEXTURE_2D);
    aglDepthMask(GL_TRUE);
    aglPushMatrix();
    aglRotatef(180.f,1.f,0.f,0.f);
    aglPolygonMode( GL_FRONT_AND_BACK, Level->Settings.IsWireFrame ? GL_LINE : GL_FILL);

    aglEnable(GL_ALPHA_TEST);
    aglEnable(GL_BLEND);

    if( Level->Settings.ShowMap ) {
        for( Iterator = TSPData; Iterator; Iterator = Iterator->Next ) {
            for( i = 0; i < Iterator->Header.NumFaces; i++ ) {
                int Vert0 = Iterator->Face[i].V0;
                int Vert1 = Iterator->Face[i].V1;
                int Vert2 = Iterator->Face[i].V2;
                int TexturePage = Iterator->Face[i].TSB.AsShort & 0x1F;
                float U0 = ((Iterator->Face[i].UV0.u)/Width);
                float V0 = /*255 -*/((Iterator->Face[i].UV0.v) / Height);
                float U1 = ((Iterator->Face[i].UV1.u) / Width);
                float V1 = /*255 -*/((Iterator->Face[i].UV1.v) /Height);
                float U2 = ((Iterator->Face[i].UV2.u) /Width);
                float V2 = /*255 -*/((Iterator->Face[i].UV2.v) / Height);
                aglBindTexture(GL_TEXTURE_2D,Level->VRam->Page[TexturePage].TextureID);
                aglBegin(GL_TRIANGLES);
                aglColor3ub(Iterator->Color[Vert0].r,Iterator->Color[Vert0].g,Iterator->Color[Vert0].b/*,Alpha*/);
                aglTexCoord2f(U0,V0);
                aglVertex3s(Iterator->Vertex[Vert0].Position.x,Iterator->Vertex[Vert0].Position.y,Iterator->Vertex[Vert0].Position.z);
                aglColor3ub(Iterator->Color[Vert1].r,Iterator->Color[Vert1].g,Iterator->Color[Vert1].b/*,Alpha*/);
                aglTexCoord2f(U1,V1);
                aglVertex3s(Iterator->Vertex[Vert1].Position.x,Iterator->Vertex[Vert1].Position.y,Iterator->Vertex[Vert1].Position.z);
                aglColor3ub(Iterator->Color[Vert2].r,Iterator->Color[Vert2].g,Iterator->Color[Vert2].b/*,Alpha*/);
                aglTexCoord2f(U2,V2);
                aglVertex3s(Iterator->Vertex[Vert2].Position.x,Iterator->Vertex[Vert2].Position.y,Iterator->Vertex[Vert2].Position.z);
                aglEnd();
            }
        }
    }
    aglBindTexture(GL_TEXTURE_2D,0);

    if( Level->Settings.ShowCollisionData ) {
        aglColor3f(0.37f,0.34f,0.5f);
        for( Iterator = TSPData; Iterator; Iterator = Iterator->Next ) {
            for( i = 0; i < Iterator->CollisionData->Header.NumFaces; i++ ) {
                int Vert0 = Iterator->CollisionData->Face[i].V0;
                int Vert1 = Iterator->CollisionData->Face[i].V1;
                int Vert2 = Iterator->CollisionData->Face[i].V2;
                aglBegin(GL_TRIANGLES);
                aglVertex3s(Iterator->CollisionData->Vertex[Vert0].Position.x,
                            Iterator->CollisionData->Vertex[Vert0].Position.y,Iterator->CollisionData->Vertex[Vert0].Position.z);
                aglVertex3s(Iterator->CollisionData->Vertex[Vert1].Position.x,
                            Iterator->CollisionData->Vertex[Vert1].Position.y,Iterator->CollisionData->Vertex[Vert1].Position.z);
                aglVertex3s(Iterator->CollisionData->Vertex[Vert2].Position.x,
                            Iterator->CollisionData->Vertex[Vert2].Position.y,Iterator->CollisionData->Vertex[Vert2].Position.z);
                aglEnd();
            }
            aglColor3f(255.f,0.f,0.f);
            aglBegin(GL_POINTS);
            for( i = 0; i < Iterator->CollisionData->Header.NumGs; i++ ) {
                aglVertex3s(Iterator->CollisionData->G[i].Position.x,Iterator->CollisionData->G[i].Position.y,
                            Iterator->CollisionData->G[i].Position.z);
            }
            aglEnd();
        }
    }
    if( Level->Settings.ShowAABBTree ) {
        for( Iterator = TSPData; Iterator; Iterator = Iterator->Next ) {
            for( i = 0; i < Iterator->Header.NumNodes; i++ ) {
                if( i == 0 ) {
                    aglLineWidth(2.f);
                } else {
                    aglLineWidth(1.f);
                }
                if( Iterator->Node[i].NumFaces == 0 ) {
                    continue;
                    aglColor3f(1,0.5,0);
                } else {
                    aglColor3f(1,1,0);
                }
                int Base = Iterator->Node[i].BaseData / sizeof(TSPFace_t);
                int Target = Base + Iterator->Node[i].NumFaces;
                DrawTSPBox(Iterator->Node[i].BBox);
                for( j = Base; j < Target; j++ ) {
                    int Vert0 = Iterator->Face[j].V0;
                    int Vert1 = Iterator->Face[j].V1;
                    int Vert2 = Iterator->Face[j].V2;
                    aglBegin(GL_TRIANGLES);
                    aglVertex3s(Iterator->Vertex[Vert0].Position.x,Iterator->Vertex[Vert0].Position.y,Iterator->Vertex[Vert0].Position.z);
                    aglVertex3s(Iterator->Vertex[Vert1].Position.x,Iterator->Vertex[Vert1].Position.y,Iterator->Vertex[Vert1].Position.z);
                    aglVertex3s(Iterator->Vertex[Vert2].Position.x,Iterator->Vertex[Vert2].Position.y,Iterator->Vertex[Vert2].Position.z);
                    aglEnd();
                }
            }
        }
    }
    aglPopMatrix();
#endif
}

int TSPGetNodeByChildOffset(TSP_t *TSP,int Offset)
{
    int i;
    for( i = 0; i < TSP->Header.NumNodes; i++ ) {
        if( Offset == TSP->Node[i].FileOffset.Offset ) {
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
    for( i = 0; i < TSP->Header.NumNodes; i++ ) {
        TSP->Node[i].FileOffset.Offset = ftell(InFile);
        DPrintf("TSPReadNodeChunk:Reading node %i at %i\n",i,TSP->Node[i].FileOffset.Offset);
        fread(&TSP->Node[i].BBox,sizeof(TSPBBox_t),1,InFile);
        fread(&TSP->Node[i].NumFaces,sizeof(TSP->Node[i].NumFaces),1,InFile);
        fread(&TSP->Node[i].U2.AsInt,sizeof(TSP->Node[i].U2.AsInt),1,InFile);
        fread(&TSP->Node[i].U3.AsInt,sizeof(TSP->Node[i].U3.AsInt),1,InFile);
        fread(&TSP->Node[i].BaseData,sizeof(TSP->Node[i].BaseData),1,InFile);
        DPrintf("TSPReadNodeChunk:Node has %i faces\n",TSP->Node[i].NumFaces);
        DPrintf("TSPReadNodeChunk:Node BaseData %i\n",TSP->Node[i].BaseData);
        if( TSP->Node[i].NumFaces == 0 ) {
            fread(&TSP->Node[i].FileOffset.Child1Offset,sizeof(TSP->Node[i].FileOffset.Child1Offset),1,InFile);
            fread(&TSP->Node[i].FileOffset.Child2Offset,sizeof(TSP->Node[i].FileOffset.Child2Offset),1,InFile);
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

void TSPReadDChunk(TSP_t *TSP,FILE *InFile)
{
    printf("TSPReadDChunk:Stub moving file offset to the next block.\n");
    if( TSP->Header.NumD != 0 ) {
        fseek(InFile,TSP->Header.CollisionOffset,SEEK_SET);
    }
#if 0
    int i;
    int j;
    
    if( !TSP || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        printf("TSPReadDChunk: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return;
    }
    if( TSP->Header.NumD == 0 ) {
        printf("TSPReadDChunk:No D block found in file %s.\n",TSP->FName);
        return;
    }
    
    TSP->DBlock = malloc(TSP->Header.NumD * sizeof(TSPD_t));
    printf("TSPReadDChunk:Loading %i D\n",TSP->Header.NumD);
    for( i = 0; i < 1/*TSP->Header.NumD*/; i++ ) {
        printf("-- BLOCK %i --\n",i);
        printf("File offset is %li\n",ftell(InFile));
        fread(&TSP->DBlock[i].Size,sizeof(TSP->DBlock[i].Size),1,InFile);
        fread(&TSP->DBlock[i].Pad,sizeof(TSP->DBlock[i].Pad),1,InFile);
        fread(&TSP->DBlock[i].U4,sizeof(TSP->DBlock[i].U4),1,InFile);
        fread(&TSP->DBlock[i].NumData,sizeof(TSP->DBlock[i].NumData),1,InFile);
        fread(&TSP->DBlock[i].U5,sizeof(TSP->DBlock[i].U5),1,InFile);
        fread(&TSP->DBlock[i].U6,sizeof(TSP->DBlock[i].U6),1,InFile);
        printf("TSPReadDChunk:Size %i\n",TSP->DBlock[i].Size);
        printf("TSPReadDChunk:U4 %i\n",TSP->DBlock[i].U4);
        printf("TSPReadDChunk:NumData %i\n",TSP->DBlock[i].NumData);
        printf("TSPReadDChunk:U5 %i\n",TSP->DBlock[i].U5);
        printf("TSPReadDChunk:U6 %u\n",TSP->DBlock[i].U6);
        int NumElements = (TSP->DBlock[i].Size - 24) / sizeof(short);
        TSP->DBlock[i].Data = malloc((TSP->DBlock[i].Size - 24) /** sizeof(short)*/);
        printf("TSPReadDChunk:NumElements are %i\n",NumElements);
        //Skip datasection for now...
        for( j = 0; j < NumElements; j++ ) {
            fread(&TSP->DBlock[i].Data[j],sizeof(short),1,InFile);
            printf("TSPReadDChunk:TSP DBlock %i Data %i is %u\n",i,j,TSP->DBlock[i].Data[j]);
        }
//         printf(" -- Color %i --\n",i);
//         PrintTSPColor(TSP->Color[i]);
    }
    printf("TSPReadDChunk:File offset at the end is %li\n",ftell(InFile));
#endif
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
    printf("U0|U1|U2|U3:%u %u %u %u\n",TSP->CollisionData->Header.U0,TSP->CollisionData->Header.U1,TSP->CollisionData->Header.U2,
        TSP->CollisionData->Header.U3
    );
    printf("NumGs:%u\n",TSP->CollisionData->Header.NumGs);
    printf("NumHs:%u\n",TSP->CollisionData->Header.NumHs);
    printf("NumVertices:%u\n",TSP->CollisionData->Header.NumVertices);
    printf("NumNormals:%u\n",TSP->CollisionData->Header.NumNormals);
    printf("NumFaces:%u\n",TSP->CollisionData->Header.NumFaces);
    //NOTE(Adriano):G data should be a list of planes in the form ax+by+cz+d where d is the pad....
    TSP->CollisionData->G = malloc(TSP->CollisionData->Header.NumGs * sizeof(TSPVert_t));
    for( i = 0; i < TSP->CollisionData->Header.NumGs; i++ ) {
        Ret = fread(&TSP->CollisionData->G[i],sizeof(TSP->CollisionData->G[i]),1,InFile);
        if( Ret != 1 ) {
            printf("TSPReadCollisionChunk:Early failure when reading G data.\n");
            return;
        }
//         printf(" -- G %i --\n",i);
//         PrintTSPVec3(TSP->CollisionData->G[i].Position);
//         printf("Pad is %i\n",TSP->CollisionData->G[i].Pad);
    }
    TSP->CollisionData->H = malloc(TSP->CollisionData->Header.NumHs * sizeof(short));
    for( i = 0; i < TSP->CollisionData->Header.NumHs; i++ ) {
        Ret = fread(&TSP->CollisionData->H[i],sizeof(TSP->CollisionData->H[i]),1,InFile);
        if( Ret != 1 ) {
            DPrintf("TSPReadCollisionChunk:Early failure when reading H data.\n");
            return;
        }
        DPrintf("-- H %i at %i --\n",i,GetCurrentFilePosition(InFile));
//         printf("%i\n",TSP->CollisionData->H[i]);
    }
    fread(&Pad,sizeof(Pad),1,InFile);
    if( Pad != 0 ) {
        //Undo the last read.
        fseek(InFile,-sizeof(Pad),SEEK_CUR);
        
    }
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
    TSP->CollisionData->Face = malloc(TSP->CollisionData->Header.NumFaces * sizeof(TSPCollisionFace_t));
    for( i = 0; i < TSP->CollisionData->Header.NumFaces; i++ ) {
        Ret = fread(&TSP->CollisionData->Face[i],sizeof(TSP->CollisionData->Face[i]),1,InFile);
        if( Ret != 1 ) {
            printf("TSPReadCollisionChunk:Early failure when reading face data.\n");
            return;
        }
//         printf("-- Face %i --\n",i);
//         printf("V0|V1|V2:%u %u %u\n",TSP->CollisionData->Face[i].V0,TSP->CollisionData->Face[i].V1,TSP->CollisionData->Face[i].V2);
//         printf("Normal Index:%u\n",TSP->CollisionData->Face[i].NormalIndex);
//         printf("Flags:%u\n",TSP->CollisionData->Face[i].Flags);
    }
    assert(ftell(InFile) == GetFileLength(InFile));
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
    DPrintf("NumD:%i DOffset:%i\n",TSP->Header.NumD,TSP->Header.DOffset);
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
    TSPReadDChunk(TSP,TSPFile);
    assert(ftell(TSPFile) == TSP->Header.CollisionOffset);
    TSPReadCollisionChunk(TSP,TSPFile);
    return TSP;
}
