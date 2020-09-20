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

    for( Iterator = TSPList; Iterator; Iterator = Iterator->Next ) {
        for( i = 0; i < Iterator->Header.NumNodes; i++ ) {
            if( Iterator->Node[i].NumFaces != 0 ) {
                TotalFaceCount += Iterator->Node[i].NumFaces;
                int Base = Iterator->Node[i].BaseData / sizeof(TSPFace_t);
                int Target = Base + Iterator->Node[i].NumFaces;
                for( j = Base; j < Target; j++ ) {
                    Vao_t *Vao;
                    int Vert0 = Iterator->Face[j].V0;
                    int Vert1 = Iterator->Face[j].V1;
                    int Vert2 = Iterator->Face[j].V2;
                    
                    float U0 = (((float)Iterator->Face[j].UV0.u)/Width);
                    float V0 = /*255 -*/(((float)Iterator->Face[j].UV0.v) / Height);
                    float U1 = (((float)Iterator->Face[j].UV1.u) / Width);
                    float V1 = /*255 -*/(((float)Iterator->Face[j].UV1.v) /Height);
                    float U2 = (((float)Iterator->Face[j].UV2.u) /Width);
                    float V2 = /*255 -*/(((float)Iterator->Face[j].UV2.v) / Height);

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
                    
                    Vao = VaoInitXYZUVRGB(VertexData,VertexSize * 3,Stride,VertexOffset,TextureOffset,ColorOffset,
                                          Iterator->Face[j].TSB.AsShort,-1);            
                    Vao->Next = Iterator->Node[i].LeafFaceListVao;
                    Iterator->Node[i].LeafFaceListVao = Vao;
                    free(VertexData);
                    
//                     DPrintf("Node %i has collision V0: %i;%i;%i\n",i,Iterator->CollisionData->Vertex[Vert0].Position.x,
//                             Iterator->CollisionData->Vertex[Vert0].Position.y,Iterator->CollisionData->Vertex[Vert0].Position.z);
//                     DPrintf("Node %i has collision V1: %i;%i;%i\n",i,Iterator->CollisionData->Vertex[Vert1].Position.x,
//                             Iterator->CollisionData->Vertex[Vert1].Position.y,Iterator->CollisionData->Vertex[Vert1].Position.z);
//                     DPrintf("Node %i has collision V2: %i;%i;%i\n",i,Iterator->CollisionData->Vertex[Vert2].Position.x,
//                             Iterator->CollisionData->Vertex[Vert2].Position.y,Iterator->CollisionData->Vertex[Vert2].Position.z);
                }
//                 continue;
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
        int VRamPage = Iterator->TSB & 0x1F;
        
#if 0
        //DO THIS ONLY IF ABE IS ENABLED...
        int Trans = (Iterator->TSB & 0x30) >> 4;
        glEnable(GL_BLEND);
        if( Trans == 0 ) {
            glBlendEquation(GL_FUNC_ADD);
            glBlendColor(1.0, 1.0, 1.0, 0.5);
            glBlendFunc(GL_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);
        } else if( Trans == 1 ) {
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
        } else if( Trans == 2 ) {
            glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
            glBlendFunc(GL_ONE, GL_ONE);
        } else if ( Trans == 3 ) {
            glBlendEquation(GL_FUNC_ADD);
            glBlendColor(1.0, 1.0, 1.0, 0.25);
            glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
        } else {
            glDisable(GL_BLEND);
        }
#endif
        
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

static int TotalFaceCount2 = 0; 

void DrawNode(TSPNode_t *Node)
{
    GL_Shader_t *Shader;
    Vao_t *Iterator;
    int MVPMatrixID;
    int i;
    
    if( !Node ) {
        return;
    }
    
//     if( Node->NumFaces != 0 ) {
//                 Shader = Shader_Cache("TSPShader","Shaders/TSPVertexShader.glsl","Shaders/TSPFragmentShader.glsl");
//         glUseProgram(Shader->ProgramID);
// 
//         MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
//         glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
//                 
//         if( Level->Settings.WireFrame ) {
//             glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//         } else {
//             glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//         }
//         for( Iterator = Node->LeafFaceListVao; Iterator; Iterator = Iterator->Next ) {
//             DPrintf("DRawing\n");
//             int VRamPage = Iterator->TSB & 0x1F;
//                 
//             if( (Iterator->TSB & 0xC0) >> 7 == 1) {
//                 glBindTexture(GL_TEXTURE_2D, Level->VRam->Page8Bit[VRamPage].TextureID);
//             } else {
//                 glBindTexture(GL_TEXTURE_2D, Level->VRam->Page4Bit[VRamPage].TextureID);
//             }
//             glBindVertexArray(Iterator->VaoID[0]);
//             glDrawArrays(GL_TRIANGLES, 0, 3);
//             glBindVertexArray(0);
//             glBindTexture(GL_TEXTURE_2D,0);
//         }
//         glUseProgram(0);
//         return;
//     }

//     if( Node->NumFaces != 0 ) {
//                 TotalFaceCount2 += Node->NumFaces;
//         return;
//     }
//     
        if( Level->Settings.ShowAABBTree ) {
//             for( Iterator = TSPData; Iterator; Iterator = Iterator->Next ) {
//                 for( i = 0; i < Iterator->Header.NumNodes; i++ ) {
                    DrawTSPBox(*Node);
//                 }
//             }
        }
    if( Node->NumFaces != 0 ) {
        if( Level->Settings.ShowMap ) {
            Shader = Shader_Cache("TSPShader","Shaders/TSPVertexShader.glsl","Shaders/TSPFragmentShader.glsl");
            glUseProgram(Shader->ProgramID);

            MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
            glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
                    
            if( Level->Settings.WireFrame ) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            for( Iterator = Node->LeafFaceListVao; Iterator; Iterator = Iterator->Next ) {
                int VRamPage = Iterator->TSB & 0x1F;
                    
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
            glUseProgram(0);
        }
    } else {
        for( i = 0; i < 2; i++ ) {
            DrawNode(Node->Child[i]);
        }
        DrawNode(Node->Next);
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
    Vao_t *VaoIterator;
    int i;
    
    TSPData = Level->TSPList;
    
    if( !TSPData ) {
        printf("DrawTSP:Invalid TSP data\n");
        return;
    }
    
    for( Iterator = TSPData; Iterator; Iterator = Iterator->Next ) {
//             DrawNode(Iterator->BSDTree);
//             DrawTSP(Iterator);
//         for( i = 0; i < Iterator->Header.NumNodes; i++ ) {
            DrawNode(&Iterator->Node[0]);
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
static int vaoCount = 0;
static int StaticFaceCounter = 0;
TSPNode_t *ReadTSPTreeChunk(TSP_t *TSP,int NodeOffset,FILE *InFile)
{
    TSPNode_t *Node;
    int Child1Offset;
    int Child2Offset;
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
    int NodeFilePosition;
    
    
    if( !TSP || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        printf("TSPReadNodeChunk: Invalid %s\n",InvalidFile ? "file" : "tsp struct");
        return NULL;
    }
    
    if( NodeOffset == -1 ) {
        DPrintf("LEaf\n");
        return NULL;
    }
    
    Node = malloc(sizeof(TSPNode_t));
    DPrintf(" Loading Node at offset %i\n",NodeOffset);
    fseek(InFile,/*TSP->Header.NodeOffset + */NodeOffset,SEEK_SET);
    fread(&Node->BBox,sizeof(TSPBBox_t),1,InFile);
    fread(&Node->NumFaces,sizeof(Node->NumFaces),1,InFile);
    fread(&Node->U2.AsInt,sizeof(Node->U2.AsInt),1,InFile);
    fread(&Node->U3.AsInt,sizeof(Node->U3.AsInt),1,InFile);
    fread(&Node->BaseData,sizeof(Node->BaseData),1,InFile);
    
    if( Node->NumFaces == 0 ) {
        fread(&Node->FileOffset.Child1Offset,sizeof(Node->FileOffset.Child1Offset),1,InFile);
        fread(&Node->FileOffset.Child2Offset,sizeof(Node->FileOffset.Child2Offset),1,InFile);
        DPrintf("Node has Child1Offset: %i\n",Node->FileOffset.Child1Offset);
        DPrintf("Node has Child2Offset: %i\n",Node->FileOffset.Child2Offset);
        int ExtraOffset;
        if( Node->BaseData < 0 ) {
            ExtraOffset = 0;
        } else {
            ExtraOffset = NodeOffset + Node->BaseData;
        }
//         if( Node->FileOffset.Child1Offset < 0 ) {
//             Node->Child1 = NULL;
//         } else {
            DPrintf("Reading children 1\n");
            Node->Child1 = ReadTSPTreeChunk(TSP,/*Node->BaseData*/ TSP->Header.NodeOffset + Node->FileOffset.Child1Offset,InFile);
//         }
//         if( Node->FileOffset.Child2Offset < 0 ) {
//             Node->Child2 = NULL;
//         } else {
            DPrintf("Reading Children 2\n");
            Node->Child2 = ReadTSPTreeChunk(TSP,/*Node->BaseData*/ TSP->Header.NodeOffset + Node->FileOffset.Child2Offset,InFile);
//         }
    } else {
        int Base = Node->BaseData / sizeof(TSPFace_t);
        int Target = Base + Node->NumFaces;
        DPrintf("Leaf Node has %i faces!\n",Node->NumFaces);
        StaticFaceCounter += Node->NumFaces;
        Node->Child1 = NULL;
        Node->Child2 = NULL;
                
    }
    return Node;
}
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
