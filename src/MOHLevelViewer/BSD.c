// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
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

#include "BSD.h"
#include "MOHLevelViewer.h"

void BSDGetObjectMatrix(BSDRenderObjectDrawable_t *RenderObjectDrawable,mat4 Result)
{
    vec3 temp;
    glm_mat4_identity(Result);
    
    glm_vec3_copy(RenderObjectDrawable->Position,temp);
    glm_vec3_rotate(temp, DEGTORAD(180.f), GLM_XUP);    
    glm_translate(Result,temp);
    
    temp[0] = 0;
    temp[1] = 1;
    temp[2] = 0;
    glm_rotate(Result,glm_rad(-RenderObjectDrawable->Rotation[1]), temp);
    temp[0] = 1;
    temp[1] = 0;
    temp[2] = 0;
    glm_rotate(Result,glm_rad(RenderObjectDrawable->Rotation[0]), temp);
    temp[0] = 0;
    temp[1] = 0;
    temp[2] = 1;
    glm_rotate(Result,glm_rad(RenderObjectDrawable->Rotation[2]), temp);
    glm_scale(Result,RenderObjectDrawable->Scale);
}

void BSDGetObjectExportMatrix(BSDRenderObjectDrawable_t *RenderObjectDrawable,mat4 Result)
{
    vec3 RotationAxis;
    vec3 Temp;
    mat4 RotationMatrix;
    mat4 ScaleMatrix;
    mat4 TranslationMatrix;
    mat4 RotScale;
    
    glm_mat4_identity(Result);
    glm_mat4_identity(TranslationMatrix);
    glm_vec3_copy(RenderObjectDrawable->Position,Temp);
    glm_vec3_rotate(Temp, DEGTORAD(180.f), GLM_XUP);    
    glm_translate(TranslationMatrix,Temp);
    
    glm_mat4_identity(RotationMatrix);
        
    RotationAxis[0] = 1;
    RotationAxis[1] = 0;
    RotationAxis[2] = 0;
    glm_rotate(RotationMatrix,glm_rad(180.f), RotationAxis);
    RotationAxis[0] = 0;
    RotationAxis[1] = 1;
    RotationAxis[2] = 0;
    glm_rotate(RotationMatrix,glm_rad(RenderObjectDrawable->Rotation[1]), RotationAxis);
    RotationAxis[0] = 1;
    RotationAxis[1] = 0;
    RotationAxis[2] = 0;
    glm_rotate(RotationMatrix,glm_rad(RenderObjectDrawable->Rotation[0]), RotationAxis);
    RotationAxis[0] = 0;
    RotationAxis[1] = 0;
    RotationAxis[2] = 1;
    glm_rotate(RotationMatrix,glm_rad(RenderObjectDrawable->Rotation[2]), RotationAxis);
    
    glm_scale_make(ScaleMatrix,RenderObjectDrawable->Scale);
    glm_mat4_mul(RotationMatrix,ScaleMatrix,RotScale);
    glm_mat4_mul(TranslationMatrix,RotScale,Result);

}

void BSDDumpDataToObjFile(BSD_t *BSD,VRAM_t *VRAM,int GameEngine,FILE *OutFile)
{
    char Buffer[256];
    BSDRenderObjectDrawable_t *RenderObjectIterator;
    mat4 ModelMatrix;

    
    if( !BSD || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        DPrintf("BSDDumpDataToFile: Invalid %s\n",InvalidFile ? "file" : "bsd struct");
        return;
    }
    
    if( !VRAM ) {
        DPrintf("BSDDumpDataToObjFile:Invalid VRAM data\n");
        return;
    }

    for( RenderObjectIterator = BSD->RenderObjectDrawableList; RenderObjectIterator; RenderObjectIterator = RenderObjectIterator->Next) {
        if( !RenderObjectIterator->RenderObject->IsStatic ) {
            continue;
        }
        sprintf(Buffer,"o BSD%i\n",RenderObjectIterator->RenderObject->Id);
        fwrite(Buffer,strlen(Buffer),1,OutFile);
        
        
        glm_mat4_identity(ModelMatrix);
        BSDGetObjectMatrix(RenderObjectIterator,ModelMatrix);
        RenderObjectExportStaticFaceDataToObjFile(RenderObjectIterator->RenderObject,ModelMatrix,VRAM,OutFile);
    }

}


void BSDDumpDataToPlyFile(BSD_t *BSD,VRAM_t *VRAM,int GameEngine,FILE *OutFile)
{
    char Buffer[256];
    BSDRenderObjectDrawable_t *RenderObjectIterator;
    mat4 ModelMatrix;
    int FaceCount;
    int VertexOffset;
    int i;
    if( !BSD || !OutFile ) {
        bool InvalidFile = (OutFile == NULL ? true : false);
        DPrintf("BSDDumpDataToPlyFile: Invalid %s\n",InvalidFile ? "file" : "bsd struct");
        return;
    }
    if( !VRAM ) {
        DPrintf("BSDDumpDataToPlyFile:Invalid VRAM data\n");
        return;
    }
    sprintf(Buffer,"ply\nformat ascii 1.0\n");
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    FaceCount = 0;
    for( RenderObjectIterator = BSD->RenderObjectDrawableList; RenderObjectIterator; RenderObjectIterator = RenderObjectIterator->Next) {
        if( !RenderObjectIterator->RenderObject->IsStatic ) {
            continue;
        }
        FaceCount += RenderObjectIterator->RenderObject->NumFaces;
    }
    
    sprintf(Buffer,"element vertex %i\nproperty float x\nproperty float y\nproperty float z\nproperty float red\nproperty float green\n"
            "property float blue\nproperty float s\nproperty float t\n",FaceCount * 3);
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    sprintf(Buffer,"element face %i\nproperty list uchar int vertex_indices\nend_header\n",FaceCount);
    fwrite(Buffer,strlen(Buffer),1,OutFile);
    for( RenderObjectIterator = BSD->RenderObjectDrawableList; RenderObjectIterator; RenderObjectIterator = RenderObjectIterator->Next) {
        if( !RenderObjectIterator->RenderObject->IsStatic ) {
            continue;
        }
        glm_mat4_identity(ModelMatrix);
        BSDGetObjectExportMatrix(RenderObjectIterator,ModelMatrix);
        RenderObjectExportStaticFaceDataToPlyFile(RenderObjectIterator->RenderObject,ModelMatrix,VRAM,OutFile);
    }
    VertexOffset = 0;
    for( RenderObjectIterator = BSD->RenderObjectDrawableList; RenderObjectIterator; RenderObjectIterator = RenderObjectIterator->Next) {
        for( i = 0; i < RenderObjectIterator->RenderObject->NumFaces; i++ ) {
            int Vert0 = VertexOffset + (i * 3) + 0;
            int Vert1 = VertexOffset + (i * 3) + 1;
            int Vert2 = VertexOffset + (i * 3) + 2;
            sprintf(Buffer,"3 %i %i %i\n",Vert0,Vert1,Vert2);
            fwrite(Buffer,strlen(Buffer),1,OutFile);
        }
        VertexOffset += RenderObjectIterator->RenderObject->NumFaces * 3;
    }
}

void BSDFixRenderObjectPosition(Level_t *Level)
{
    BSDRenderObjectDrawable_t *RenderObjectIterator;
    int Result;
    int OutY;
    int Delta;
    int PropertySetIndex;
    
    for( RenderObjectIterator = Level->BSD->RenderObjectDrawableList; RenderObjectIterator; 
        RenderObjectIterator = RenderObjectIterator->Next ) {
        Result = TSPGetPointYComponentFromKDTree(RenderObjectIterator->Position,Level->TSPList,&PropertySetIndex,&OutY);
        if( Result == -1 ) {
            DPrintf("BSDFixRenderObjectPosition:Not found in KD Tree!\n");
            continue;
        }
        Delta = abs(RenderObjectIterator->Position[1] - OutY);
        DPrintf("Delta is %i (Limit 55) PropertySetIndex:%i Original Position:%f OutY:%i\n",Delta,PropertySetIndex,
                RenderObjectIterator->Position[1],OutY);
        BSDGetProperty(Level->BSD->PropertySetFile,PropertySetIndex);
        if( Delta < 55 ) {
            RenderObjectIterator->Position[1] = OutY;
        }
    }
}

void BSDRenderObjectListCleanUp(BSD_t *BSD)
{
    RenderObjectFreeList(BSD->RenderObjectList);
}

void BSDFree(BSD_t *BSD)
{
    BSDRenderObjectDrawable_t *Drawable;
    BSDAnimatedLight_t *AnimatedLight;
    int i;
    
    if( !BSD ) {
        return;
    }
    
    for( i = 0; i < BSD_ANIMATED_LIGHTS_TABLE_SIZE; i++ ) {
        AnimatedLight = &BSD->AnimatedLightsTable.AnimatedLightsList[i];
        if( AnimatedLight->NumColors == 0 ) {
            continue;
        }
        if( AnimatedLight->ColorList ) {
            free(AnimatedLight->ColorList);
        }
    }
    if( BSD->NodeData.Table ) {
        free(BSD->NodeData.Table);
    }
    if( BSD->NodeData.Node ) {
        free(BSD->NodeData.Node);
    }
    
    if( BSD->PropertySetFile.Property ) {
        for( i = 0; i < BSD->PropertySetFile.NumProperties; i++ ) {
            free(BSD->PropertySetFile.Property[i].NodeList);
        }
        free(BSD->PropertySetFile.Property);
    }
    if( BSD->RenderObjectTable.RenderObject ) {
        free(BSD->RenderObjectTable.RenderObject);
    }
    BSDRenderObjectListCleanUp(BSD);

    VAOFree(BSD->NodeVAO);
    VAOFree(BSD->RenderObjectPointVAO);
    
    for( i = 0; i < BSD_COLLISION_VOLUME_MAX_VALUE; i++ ) {
        VAOFree(BSD->CollisionVolumeVAO[i]);
    }
    if( BSDIsMoonEnabled(BSD->SkyData) ) {
        VAOFree(BSD->SkyData.MoonVAO);
    }
    
    if( BSDAreStarsEnabled(BSD->SkyData) ) {
        VAOFree(BSD->SkyData.StarsVAO);
    }
    
    while( BSD->RenderObjectDrawableList ) {
        Drawable = BSD->RenderObjectDrawableList;
        BSD->RenderObjectDrawableList = BSD->RenderObjectDrawableList->Next;
        free(Drawable);
    }
    
    free(BSD);
}

void BSDCreatePointListVAO(BSD_t *BSD)
{
    float *NodeData;
    int    NodeDataPointer;
    int    Stride;
    int    NodeDataSize;
    int    i;
    vec3   NodeColor;
    
    Stride = (3 + 3) * sizeof(float);
    NodeDataSize = Stride * BSD->NodeData.Header.NumNodes;
    NodeData = malloc(NodeDataSize);
    NodeDataPointer = 0;
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        NodeData[NodeDataPointer] =   BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer+1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer+2] = BSD->NodeData.Node[i].Position.z;
        BSDGetNodeColorById(BSD->NodeData.Node[i].Id,NodeColor);
        NodeData[NodeDataPointer+3] = NodeColor[0];
        NodeData[NodeDataPointer+4] = NodeColor[1];
        NodeData[NodeDataPointer+5] = NodeColor[2];
        NodeDataPointer += 6;
    }
    BSD->NodeVAO = VAOInitXYZRGB(NodeData,NodeDataSize,Stride,0,3,0);            
    free(NodeData);
}
void BSDCreateRenderObjectPointListVAO(BSD_t *BSD)
{
    BSDRenderObjectDrawable_t *Iterator;
    BSDRenderObjectElement_t RenderObjectElement;
    int NumSkip;
    float *RenderObjectData;
    int    RenderObjectDataPointer;
    int    Stride;
    int    RenderObjectDataSize;
    int    i;

    Stride = (3 + 3) * sizeof(float);
    RenderObjectDataSize = Stride * BSD->NumRenderObjectPoint;
    RenderObjectData = malloc(RenderObjectDataSize);
    RenderObjectDataPointer = 0;
    NumSkip = 0;
    i = 0;
    for( Iterator = BSD->RenderObjectDrawableList; Iterator; Iterator = Iterator->Next ) {
        RenderObjectData[RenderObjectDataPointer] =   Iterator->Position[0];
        RenderObjectData[RenderObjectDataPointer+1] = Iterator->Position[1];
        RenderObjectData[RenderObjectDataPointer+2] = Iterator->Position[2];
        RenderObjectElement = *Iterator->RenderObject->Data;
        if( RenderObjectElement.Type == RENDER_OBJECT_ENEMY ) {
            // BLUE
            RenderObjectData[RenderObjectDataPointer+3] = 0.f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        } else if (RenderObjectElement.Type == RENDER_OBJECT_DOOR ) {
            // GREEN
            RenderObjectData[RenderObjectDataPointer+3] = 0.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.f;
        } else if( RenderObjectElement.Type == RENDER_OBJECT_MG42 ) {
            // Yellow
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.f;
        } else if( RenderObjectElement.Type == RENDER_OBJECT_PLANE ) {
            // Fuchsia -- Plane.
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        } else if ( RenderObjectElement.Type == RENDER_OBJECT_UNKNOWN1 ) {
            // Maroon
            RenderObjectData[RenderObjectDataPointer+3] = 0.5f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.f;
        } else if( RenderObjectElement.Type == RENDER_OBJECT_DESTRUCTIBLE_WINDOW ) {
            // saddlebrown -- 
            RenderObjectData[RenderObjectDataPointer+3] = 0.54f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.27f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.07f;
        } else if( RenderObjectElement.Type == RENDER_OBJECT_VALVE ) {
            // White -- 
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        } else if( RenderObjectElement.Type == RENDER_OBJECT_EXPLOSIVE_CHARGE ) {
            // Pink
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 0.f;
            RenderObjectData[RenderObjectDataPointer+5] = 0.5f;
        } else {
            // RED
            RenderObjectData[RenderObjectDataPointer+3] = 1.f;
            RenderObjectData[RenderObjectDataPointer+4] = 1.f;
            RenderObjectData[RenderObjectDataPointer+5] = 1.f;
        }
        RenderObjectDataPointer += 6;
        i++;
    }
    BSD->RenderObjectPointVAO = VAOInitXYZRGB(RenderObjectData,RenderObjectDataSize - (Stride * NumSkip),Stride,0,3,0);            
    free(RenderObjectData);
}

void BSDAddNodeToRenderObjecDrawableList(BSD_t *BSD,int IsMultiplayer,int NodeId,vec3 Position,vec3 Rotation)
{
    BSDRenderObjectDrawable_t *Object;
    int RenderObjectId;
    RenderObject_t *RenderObject;

    if( IsMultiplayer ) {
        RenderObjectId = BSDMPNodeIdToRenderObjectId(NodeId);
    } else {
        RenderObjectId = BSDNodeIdToRenderObjectId(NodeId);
    }

    RenderObject = RenderObjectGetByIdFromList(BSD->RenderObjectList,RenderObjectId);

    if( !RenderObject ) {
        DPrintf("BSDAddNodeToRenderObjecDrawableList:Failed adding new object...Id %i doesn't match any.\n",RenderObjectId);
        return;
    }
    DPrintf("RenderObjectId %i for node %i -> %i\n",RenderObjectId,NodeId,RenderObject->Id);

    Object = malloc(sizeof(BSDRenderObjectDrawable_t));
    Object->RenderObject = RenderObject;
    glm_vec3_copy(Position,Object->Position);
    //PSX GTE Uses 4096 as unit value only when dealing with fixed math operation.
    //When dealing with rotation then 4096 = 360 degrees.
    //We need to map it back to OpenGL standard format [0;360].
    glm_vec3_scale(Rotation, 360.f / 4096.f, Object->Rotation);
    glm_vec3_copy(RenderObject->Scale,Object->Scale);
    
    Object->Next = BSD->RenderObjectDrawableList;
    BSD->RenderObjectDrawableList = Object;
    BSD->NumRenderObjectPoint++;
}

void BSDCreateMoonVAO(BSD_t *BSD,VRAM_t *VRAM)
{
    float x;
    float y;
    float z;
    float w;
    float h;
    float u0;
    float v0;
    float TexWidth;
    float TexHeight;
    float ImageWidth;
    float ImageHeight;
    float *VertexData;
    int Stride;
    int DataSize;
    int VertexPointer;
    vec3 MoonPosition;
    
    
    if( !BSDIsMoonEnabled(BSD->SkyData) ) {
        DPrintf("Moon is not enabled...\n");
        return;
    }
    
    //        XYZ  UV
    Stride = (3 + 2) * sizeof(float);
    DataSize = Stride * 6;
    
    ImageWidth = VRAM->Page.Width;
    ImageHeight = VRAM->Page.Height;

    u0 = ((float)BSD_MOON_TEXTURE_X + VRAMGetTexturePageX(BSD_MOON_VRAM_PAGE)) / ImageWidth;
    //Color Mode 0 => 4 BPP texture
    v0 = ((float)BSD_MOON_TEXTURE_Y + VRAMGetTexturePageY(BSD_MOON_VRAM_PAGE,0)) / ImageHeight;
    TexWidth = ((float)BSD_MOON_WIDTH) / ImageWidth;
    TexHeight = ((float)BSD_MOON_HEIGHT) / ImageHeight;
    
    VertexData = malloc(DataSize);
    VertexPointer = 0;
    MoonPosition[0] = (BSD->SkyData.MoonZ * 32.f) / 200.f;
    MoonPosition[1] = BSD->SkyData.MoonY;
    MoonPosition[2] = BSD->SkyData.MoonZ;
    glm_vec3_rotate(MoonPosition, DEGTORAD(180.f), GLM_XUP);

    x = MoonPosition[0];
    y = MoonPosition[1];
    z = MoonPosition[2];
    
    w = BSD_MOON_WIDTH * (BSD->SkyData.StarRadius * 4);
    h = BSD_MOON_HEIGHT * (BSD->SkyData.StarRadius * 4);

    VertexData[VertexPointer] =  x;
    VertexData[VertexPointer+1] = y + h;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0;
    VertexData[VertexPointer+4] = v0 + TexHeight;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x;
    VertexData[VertexPointer+1] = y;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0;
    VertexData[VertexPointer+4] = v0;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x + w;
    VertexData[VertexPointer+1] = y + h;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0 + TexWidth;
    VertexData[VertexPointer+4] = v0 + TexHeight;
    VertexPointer += 5;
            

    VertexData[VertexPointer] =  x + w;
    VertexData[VertexPointer+1] = y + h;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0 + TexWidth;
    VertexData[VertexPointer+4] = v0 + TexHeight;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x;
    VertexData[VertexPointer+1] = y;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0;
    VertexData[VertexPointer+4] = v0;
    VertexPointer += 5;
            
    VertexData[VertexPointer] =  x + w;
    VertexData[VertexPointer+1] = y;
    VertexData[VertexPointer+2] = z;
    VertexData[VertexPointer+3] = u0 + TexWidth;
    VertexData[VertexPointer+4] = v0;
    VertexPointer += 5;        
    BSD->SkyData.MoonVAO = VAOInitXYZUV(VertexData,DataSize,Stride,0,3,6);
    free(VertexData);
}

void BSDCreateStarsVAO(BSD_t *BSD)
{
    float *VertexData;
    float R;
    float Phi;
    float Theta;
    int VertexPointer;
    int Stride;
    int VertexSize;
    int i;
    int StarColorIndex;
    Color3b_t StarColor;

    if( !BSDAreStarsEnabled(BSD->SkyData) ) {
        DPrintf("Stars are not enabled...\n");
        return;
    }
    //        XYZ RGB
    Stride = (3 + 3) * sizeof(float);
    VertexSize = Stride * BSD_SKY_MAX_STARS_NUMBER;
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    StarColorIndex = RandRangeI(0,7);
    for( i = 0; i < BSD_SKY_MAX_STARS_NUMBER; i++ ) {
        R = (BSD->SkyData.StarRadius*256) * sqrt(Rand01());
        Theta = Rand01() * 2 * M_PI;
        Phi = acos(2.0 * Rand01() - 1.0);/*BSDRand01() * M_PI;*/
        StarColor = BSDStarsColors[StarColorIndex];
        StarColorIndex = (StarColorIndex + 1) & 7;
        BSD->SkyData.StarsColors[i].rgba[0] = StarColor.r;
        BSD->SkyData.StarsColors[i].rgba[1] = StarColor.g;
        BSD->SkyData.StarsColors[i].rgba[2] = StarColor.b;

        VertexData[VertexPointer] =  (R * sin(Phi) * cos(Theta) );
        VertexData[VertexPointer+1] = (R * sin(Theta) * sin(Phi) ) - (BSD->SkyData.StarRadius*264);
        VertexData[VertexPointer+2] = R * cos(Phi);
        VertexData[VertexPointer+3] = StarColor.r / 255.f;
        VertexData[VertexPointer+4] = StarColor.g / 255.f;
        VertexData[VertexPointer+5] = StarColor.b / 255.f;
        VertexPointer += 6;
    }
    BSD->SkyData.StarsVAO = VAOInitXYZRGB(VertexData,VertexSize,Stride,0,3,1);
    free(VertexData);
}

void BSDCreateSkyVAOs(BSD_t *BSD,VRAM_t *VRAM)
{
    BSDCreateMoonVAO(BSD,VRAM);
    BSDCreateStarsVAO(BSD);
}

VAO_t *BSDCreateSphereCollisionVAO()
{
    float *VertexData;
    int VertexPointer;
    int IndexSize;
    int *IndexData;
    int IndexPointer;
    unsigned int k1, k2;
    int Stride;
    int VertexSize;
    float xz;
    float y;
    int i;
    int j;
    int NumSectors;
    int NumStack;
    float SectorStep;
    float StackStep;
    float SectorAngle;
    float StackAngle;
    VAO_t *Result;

    NumSectors = 36;
    NumStack = 18;
    
    Stride = 3 * sizeof(float);
    VertexSize = ((NumSectors+1) * 3) * (NumStack+1) * sizeof(float);
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    
    SectorStep = 2.0f * M_PI / (float)(NumSectors);
    StackStep  =  M_PI / (float)(NumStack);
    
    for( i = 0; i <= NumStack; i++ ) {
        StackAngle = M_PI / 2 - i * StackStep;
        xz = cos(StackAngle);
        y = sin(StackAngle);
        for( j = 0; j <= NumSectors; j++ ) {
            SectorAngle = j * SectorStep;
            VertexData[VertexPointer++] =  xz * cos(SectorAngle);
            VertexData[VertexPointer++] = y;
            VertexData[VertexPointer++] = xz * sin(SectorAngle);
        }
    }
    IndexSize = ( NumStack * ( ( (NumSectors - 2) * 6) + (6) ) ) * sizeof(int);
    IndexData = malloc(IndexSize);
    IndexPointer = 0;
    for(i = 0; i < NumStack; ++i) {
        k1 = i * (NumSectors + 1);
        k2 = k1 + NumSectors + 1;

        for(j = 0; j < NumSectors; j++, k1++, k2++) {
            if(i != 0) {
                IndexData[IndexPointer++] = k1;
                IndexData[IndexPointer++] = k2;
                IndexData[IndexPointer++] = k1+1;
            }

            if(i != (NumStack-1)) {
                IndexData[IndexPointer++] = k1+1;
                IndexData[IndexPointer++] = k2;
                IndexData[IndexPointer++] = k2+1;
            }
        }
    }
    Result = VAOInitXYZIBO(VertexData,VertexSize,Stride,IndexData,IndexSize,IndexPointer);
    free(VertexData);
    free(IndexData);
    return Result;
}

VAO_t *BSDCreateCylinderCollisionVAO()
{
    float *VertexData;
    int VertexPointer;
    int VertexCount;
    int *IndexData;
    int IndexPointer;
    int IndexSize;
    float Radius;
    float Height;
    int k1;
    int k2;
    int Stride;
    int VertexSize;
    int i;
    int k;
    int j;
    int NumSlice;
    float SliceAngleStep;
    float CurrentSliceAngle;
    int TopVertexIndex;
    int BottomVertexIndex;
    float *CosineTable;
    float *SineTable;
    VAO_t *Result;

    //        XYZ
    Stride = (3) * sizeof(float);
    NumSlice = 36;
    SliceAngleStep = 2.0f * M_PI / (float)(NumSlice);
    CurrentSliceAngle = 0.f;
    //                      SIDE                        TOP                     BOTTOM                  CAPS
    VertexCount = ( ( ( (NumSlice + 1) * 3 ) * 2) + ( (NumSlice + 1) * 3 ) + ( (NumSlice + 1) * 3 ) + (2 * 3) );
    VertexSize = Stride * VertexCount;
    VertexData = malloc(VertexSize);
    VertexPointer = 0;
    Radius = 1;
        
    CosineTable = malloc((NumSlice + 1) *sizeof(float));
    SineTable = malloc((NumSlice + 1) *sizeof(float));
    for(i = 0; i <= NumSlice; i++) {
        CosineTable[i] = cos(CurrentSliceAngle);
        SineTable[i] = sin(CurrentSliceAngle);
        CurrentSliceAngle += SliceAngleStep;
    }

    for(i = 0; i <= 1; i++) {
        Height = -0.5 + i;
        for(j = 0; j <= NumSlice; j++) {
            VertexData[VertexPointer++] =  CosineTable[j] * Radius;
            VertexData[VertexPointer++] = Height;
            VertexData[VertexPointer++] = SineTable[j] * Radius;
        }
    }
    
    BottomVertexIndex = VertexPointer / 3;
    VertexData[VertexPointer++] =  0;
    VertexData[VertexPointer++] = -Height;
    VertexData[VertexPointer++] = 0;
    for( i = 0; i <= NumSlice; i++ ) {
        VertexData[VertexPointer++] =  CosineTable[i] * Radius;
        VertexData[VertexPointer++] = -Height;
        VertexData[VertexPointer++] = -SineTable[i] * Radius;
    }
    TopVertexIndex = VertexPointer / 3;
    VertexData[VertexPointer++] =  0;
    VertexData[VertexPointer++] = Height;
    VertexData[VertexPointer++] = 0;

    for( i = 0; i <= NumSlice; i++ ) {
        VertexData[VertexPointer++] =  CosineTable[i] * Radius;
        VertexData[VertexPointer++] = Height;
        VertexData[VertexPointer++] = SineTable[i] * Radius;
    }

    //                 SIDE             TOP             BOTTOM
    IndexSize = ( (NumSlice * 6) + (NumSlice * 3) + (NumSlice * 3) ) * sizeof(int);
    IndexData = malloc(IndexSize);
    k1 = 0;
    k2 = NumSlice + 1;
    IndexPointer = 0;
    for(j = 0; j < NumSlice; j++, k1++, k2++) {
        IndexData[IndexPointer++] = k1;
        IndexData[IndexPointer++] = k1+1;
        IndexData[IndexPointer++] = k2;
        IndexData[IndexPointer++] = k2;
        IndexData[IndexPointer++] = k1+1;
        IndexData[IndexPointer++] = k2+1;
    }
    
    
    for(i = 0, k = BottomVertexIndex + 1; i < NumSlice; i++, k++) {
        if(i < (NumSlice - 1)){
            IndexData[IndexPointer++] = BottomVertexIndex;
            IndexData[IndexPointer++] = k+1;
            IndexData[IndexPointer++] = k;
        } else {
            IndexData[IndexPointer++] = BottomVertexIndex;
            IndexData[IndexPointer++] = BottomVertexIndex+1;
            IndexData[IndexPointer++] = k;
        }
    }
    for(i = 0, k = TopVertexIndex + 1; i < NumSlice; i++, k++) {
        if(i < (NumSlice - 1)){
            IndexData[IndexPointer++] = TopVertexIndex;
            IndexData[IndexPointer++] = k;
            IndexData[IndexPointer++] = k+1;
        } else {
            IndexData[IndexPointer++] = TopVertexIndex;
            IndexData[IndexPointer++] = k;
            IndexData[IndexPointer++] = TopVertexIndex+1;
        }
    }
    Result = VAOInitXYZIBO(VertexData,VertexSize,Stride,IndexData,IndexSize,IndexPointer);
    free(VertexData);
    free(IndexData);
    free(CosineTable);
    free(SineTable);
    return Result;
}

VAO_t *BSDCreateBoxCollisionVAO()
{
    VAO_t *Result;
    float BoxVertices[24] = {
        -1.0, -1.0, -1.0,
        1.0,  -1.0,  -1.0,
        1.0,   1.0,  -1.0,
        -1.0,  1.0, -1.0,
        -1.0, -1.0, 1.0,
        1.0,  -1.0,  1.0,
        1.0,   1.0,  1.0,
        -1.0,  1.0, 1.0
    };
    int BoxIndices[36] = {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        7, 6, 5,
        5, 4, 7,
        4, 0, 3,
        3, 7, 4,
        4, 5, 1,
        1, 0, 4,
        3, 2, 6,
        6, 7, 3
    };

    Result = VAOInitXYZIBO(BoxVertices,sizeof(BoxVertices),3*sizeof(float),BoxIndices,sizeof(BoxIndices),36);
    return Result;
}

void BSDCreateCollisionVolumeVAOs(BSD_t *BSD)
{
    BSD->CollisionVolumeVAO[BSD_COLLISION_VOLUME_TYPE_SPHERE] = BSDCreateSphereCollisionVAO();
    BSD->CollisionVolumeVAO[BSD_COLLISION_VOLUME_TYPE_CYLINDER] = BSDCreateCylinderCollisionVAO();
    BSD->CollisionVolumeVAO[BSD_COLLISION_VOLUME_TYPE_BOX] = BSDCreateBoxCollisionVAO();


}
void BSDCreateVAOs(BSD_t *BSD,int GameEngine,VRAM_t *VRAM)
{
    RenderObject_t *RenderObjectIterator;
  
    for( RenderObjectIterator = BSD->RenderObjectList; RenderObjectIterator; RenderObjectIterator = RenderObjectIterator->Next ) {
        RenderObjectGenerateVAO(RenderObjectIterator);
    }
    BSDCreatePointListVAO(BSD);
    BSDCreateRenderObjectPointListVAO(BSD);
    BSDCreateSkyVAOs(BSD,VRAM);
    BSDCreateCollisionVolumeVAOs(BSD);
}

void BSDDrawSphereCollisionVolume(BSDNode_t *Node,mat4 ViewMatrix,mat4 ProjectionMatrix,int MVPMatrixId,int ColorId,int IndexCount)
{
    mat4 ModelMatrix;
    mat4 ModelViewMatrix;
    mat4 MVPMatrix;
    vec3 NodePosition;
    vec3 Scale;
    vec3 Color;
    float Radius;
    if( !Node ) {
        return;
    }
    Radius = Node->CollisionInfo0;
    BSDPositionToGLMVec3(Node->Position,NodePosition);
    glm_mat4_identity(ModelMatrix);
    glm_mat4_identity(ModelViewMatrix);
    glm_vec3_rotate(NodePosition, DEGTORAD(180.f), GLM_XUP);
    glm_translate(ModelMatrix,NodePosition);
    Scale[0] = Radius;
    Scale[1] = Radius;
    Scale[2] = Radius;
    glm_scale(ModelMatrix,Scale);
    glm_mat4_mul(ViewMatrix,ModelMatrix,ModelViewMatrix);
    glm_mat4_mul(ProjectionMatrix,ModelViewMatrix,MVPMatrix);
    glUniformMatrix4fv(MVPMatrixId,1,false,&MVPMatrix[0][0]);
    BSDGetNodeColorById(Node->Id,Color);
    glUniform3fv(ColorId,1,Color);
    glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, 0);
}
void BSDDrawCylinderCollisionVolume(BSDNode_t *Node,mat4 ViewMatrix,mat4 ProjectionMatrix,int MVPMatrixId,int ColorId,int IndexCount)
{
    mat4 ModelMatrix;
    mat4 ModelViewMatrix;
    mat4 MVPMatrix;
    vec3 NodePosition;
    vec3 Scale;
    vec3 Color;
    float Radius;
    float Height;
    if( !Node ) {
        return;
    }
    Radius = Node->CollisionInfo0;
    Height = fabs(Node->CollisionInfo2 - Node->CollisionInfo1);
    BSDPositionToGLMVec3(Node->Position,NodePosition);
    glm_mat4_identity(ModelMatrix);
    glm_mat4_identity(ModelViewMatrix);
    glm_vec3_rotate(NodePosition, DEGTORAD(180.f), GLM_XUP);
    glm_translate(ModelMatrix,NodePosition);
    Scale[0] = Radius;
    Scale[1] = Height;
    Scale[2] = Radius;
    glm_scale(ModelMatrix,Scale);
    glm_mat4_mul(ViewMatrix,ModelMatrix,ModelViewMatrix);
    glm_mat4_mul(ProjectionMatrix,ModelViewMatrix,MVPMatrix);
    glUniformMatrix4fv(MVPMatrixId,1,false,&MVPMatrix[0][0]);
    BSDGetNodeColorById(Node->Id,Color);
    glUniform3fv(ColorId,1,Color);
    glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, 0);
}
void BSDDrawBoxCollisionVolume(BSDNode_t *Node,mat4 ViewMatrix,mat4 ProjectionMatrix,int MVPMatrixId,int ColorId,int IndexCount)
{
    mat4 ModelMatrix;
    mat4 ModelViewMatrix;
    mat4 MVPMatrix;
    vec3 NodePosition;
    vec3 Axis;
    vec3 Scale;
    vec3 LocalRotation;
    vec3 Color;
    float Width;
    float Height;
    float Depth;
    
    if( !Node ) {
        return;
    }
    Width = Node->CollisionInfo0;
    Height = Node->CollisionInfo1;
    Depth = Node->CollisionInfo2;
    
    BSDPositionToGLMVec3(Node->Position,NodePosition);
    glm_mat4_identity(ModelMatrix);
    glm_mat4_identity(ModelViewMatrix);
    glm_vec3_rotate(NodePosition, DEGTORAD(180.f), GLM_XUP);
    glm_translate(ModelMatrix,NodePosition);
    BSDPositionToGLMVec3(Node->Rotation,LocalRotation);
    glm_vec3_scale(LocalRotation,360.f/4096.f,LocalRotation);
    Axis[0] = 0;
    Axis[1] = 1;
    Axis[2] = 0;
    glm_rotate(ModelMatrix,glm_rad(-LocalRotation[1]), Axis);
    Axis[0] = 1;
    Axis[1] = 0;
    Axis[2] = 0;
    glm_rotate(ModelMatrix,glm_rad(LocalRotation[0]), Axis);
    Axis[0] = 0;
    Axis[1] = 0;
    Axis[2] = 1;
    glm_rotate(ModelMatrix,glm_rad(LocalRotation[2]), Axis);
    Scale[0] = Width;
    Scale[1] = Height;
    Scale[2] = Depth;
    glm_scale(ModelMatrix,Scale);
    glm_mat4_mul(ViewMatrix,ModelMatrix,ModelViewMatrix);
    glm_mat4_mul(ProjectionMatrix,ModelViewMatrix,MVPMatrix);
    glUniformMatrix4fv(MVPMatrixId,1,false,&MVPMatrix[0][0]);
    BSDGetNodeColorById(Node->Id,Color);
    glUniform3fv(ColorId,1,Color);
    glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, 0);
}
void BSDDrawCollisionVolumes(BSD_t *BSD,Camera_t *Camera,mat4 ProjectionMatrix)
{
    Shader_t *Shader;
    int MVPMatrixId;
    int ColorId;
    int i;

    if( !LevelDrawBSDNodesCollisionVolumes->IValue ) {
        return;
    }
    Shader = ShaderCache("BSDCollisionShader","Shaders/BSDCollisionVertexShader.glsl","Shaders/BSDCollisionFragmentShader.glsl");
    if( !Shader ) {
        return;
    }
    glUseProgram(Shader->ProgramId);
    MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
    ColorId = glGetUniformLocation(Shader->ProgramId,"InColor");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        if( BSD->NodeData.Node[i].CollisionVolumeType < 0 || BSD->NodeData.Node[i].CollisionVolumeType > BSD_COLLISION_VOLUME_MAX_VALUE ) {
            continue;
        }
        glBindVertexArray(BSD->CollisionVolumeVAO[BSD->NodeData.Node[i].CollisionVolumeType]->VAOId[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BSD->CollisionVolumeVAO[BSD->NodeData.Node[i].CollisionVolumeType]->IBOId[0]);

        switch( BSD->NodeData.Node[i].CollisionVolumeType ) {
            case BSD_COLLISION_VOLUME_TYPE_SPHERE:
                BSDDrawSphereCollisionVolume(&BSD->NodeData.Node[i],Camera->ViewMatrix,ProjectionMatrix,MVPMatrixId,ColorId,
                                             BSD->CollisionVolumeVAO[BSD->NodeData.Node[i].CollisionVolumeType]->Count);
                break;
            case BSD_COLLISION_VOLUME_TYPE_CYLINDER:
                BSDDrawCylinderCollisionVolume(&BSD->NodeData.Node[i],Camera->ViewMatrix,ProjectionMatrix,MVPMatrixId,ColorId,
                                             BSD->CollisionVolumeVAO[BSD->NodeData.Node[i].CollisionVolumeType]->Count);
                break;
            case BSD_COLLISION_VOLUME_TYPE_BOX:
                BSDDrawBoxCollisionVolume(&BSD->NodeData.Node[i],Camera->ViewMatrix,ProjectionMatrix,MVPMatrixId,ColorId,
                                             BSD->CollisionVolumeVAO[BSD->NodeData.Node[i].CollisionVolumeType]->Count);
                break;
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
    glDisable(GL_BLEND);

}
//TODO:Spawn the RenderObject when loading node data!
//     Some nodes don't have a corresponding RenderObject like the PlayerSpawn.
void BSDDraw(BSD_t *BSD,VRAM_t *VRAM,Camera_t *Camera,RenderObjectShader_t *RenderObjectShader,mat4 ProjectionMatrix)
{
    Shader_t *Shader;
    BSDRenderObjectDrawable_t *RenderObjectDrawableIterator;
    RenderObject_t *RenderObjectIterator;
    mat4 MVPMatrix;
    mat4 ModelMatrix;
    vec3 Temp;
    vec3 PSpawn;
    int MVPMatrixId;
    int i;
    
    glm_mat4_mul(ProjectionMatrix,Camera->ViewMatrix,MVPMatrix);
    
    //Emulate PSX Coordinate system...
    glm_rotate_x(MVPMatrix,glm_rad(180.f), MVPMatrix);
        
    if( LevelDrawBSDNodesAsPoints->IValue ) {    
        Shader = ShaderCache("BSDShader","Shaders/BSDVertexShader.glsl","Shaders/BSDFragmentShader.glsl");
        if( Shader ) {
            glUseProgram(Shader->ProgramId);

            MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
            glUniformMatrix4fv(MVPMatrixId,1,false,&MVPMatrix[0][0]);
            glBindVertexArray(BSD->NodeVAO->VAOId[0]);
            glPointSize(10.f);
            glDrawArrays(GL_POINTS, 0, BSD->NodeData.Header.NumNodes);
            glBindVertexArray(0);
            glUseProgram(0);
        }
    }
    
    if( LevelDrawBSDRenderObjectsAsPoints->IValue ) {    
        Shader = ShaderCache("BSDShader","Shaders/BSDVertexShader.glsl","Shaders/BSDFragmentShader.glsl");
        if( Shader ) {
            glUseProgram(Shader->ProgramId);
            MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
            glUniformMatrix4fv(MVPMatrixId,1,false,&MVPMatrix[0][0]);
            glBindVertexArray(BSD->RenderObjectPointVAO->VAOId[0]);
            glPointSize(10.f);
            glDrawArrays(GL_POINTS, 0, BSD->NumRenderObjectPoint);
            glBindVertexArray(0);
            glUseProgram(0);
        }
    }
    
    
    if( LevelDrawBSDRenderObjects->IValue ) {
        RenderObjectBeginDraw(VRAM,RenderObjectShader,LevelEnableAmbientLight->IValue,
                              LevelEnableWireFrameMode->IValue,false);
        for( RenderObjectDrawableIterator = BSD->RenderObjectDrawableList; RenderObjectDrawableIterator; 
            RenderObjectDrawableIterator = RenderObjectDrawableIterator->Next ) {
            glm_mat4_identity(ModelMatrix);
            BSDGetObjectMatrix(RenderObjectDrawableIterator,ModelMatrix);
            RenderObjectDraw(RenderObjectDrawableIterator->RenderObject,RenderObjectShader,ModelMatrix,Camera->ViewMatrix,ProjectionMatrix);
        }
        RenderObjectEndDraw(LevelEnableWireFrameMode->IValue);
    }
    
    if( LevelDrawBSDShowcase->IValue ) {
        BSDGetPlayerSpawn(BSD->NodeData,0,PSpawn,NULL);
        i = 0;
        RenderObjectBeginDraw(VRAM,RenderObjectShader,LevelEnableAmbientLight->IValue,
                              LevelEnableWireFrameMode->IValue,false);
        for( RenderObjectIterator = BSD->RenderObjectList; RenderObjectIterator; RenderObjectIterator = RenderObjectIterator->Next ) {
            glm_mat4_identity(ModelMatrix);
            Temp[0] = ((PSpawn[0] - (i * 200.f)));
            Temp[1] = (-PSpawn[1]);
            Temp[2] = (PSpawn[2]);
            glm_translate(ModelMatrix,Temp);
            RenderObjectDraw(RenderObjectIterator,RenderObjectShader,ModelMatrix,Camera->ViewMatrix,ProjectionMatrix);
            i++;
        }
        RenderObjectEndDraw(LevelEnableWireFrameMode->IValue);
    }
    BSDDrawCollisionVolumes(BSD,Camera,ProjectionMatrix);
}

void BSDDrawSky(BSD_t *BSD,VRAM_t *VRAM,Camera_t *Camera,mat4 ProjectionMatrix)
{
    Shader_t *Shader;
    mat4 SkyMatrix;
    mat4 MVPMatrix;
    int MVPMatrixId;
    
    glm_mat4_identity(SkyMatrix);
    //NOTE(Adriano):Zero-Out the translation component from the Matrix.
    SkyMatrix[3][0]=0;
    SkyMatrix[3][1]=0;
    SkyMatrix[3][2]=0;
    SkyMatrix[3][3]=0;
    glm_mat4_mul(Camera->ViewMatrix,SkyMatrix,SkyMatrix);
    glm_mat4_mul(ProjectionMatrix,SkyMatrix,MVPMatrix);
     //Emulate PSX Coordinate system...
    glm_rotate_x(MVPMatrix,glm_rad(180.f), MVPMatrix);
    
    glDepthMask(0);
    if( BSDIsMoonEnabled(BSD->SkyData) ) {
        Shader = ShaderCache("MoonShader","Shaders/MoonVertexShader.glsl","Shaders/MoonFragmentShader.glsl");
        if( Shader ) {
            glUseProgram(Shader->ProgramId);
            MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
            glBindTexture(GL_TEXTURE_2D,VRAM->Page.TextureId);

            glUniformMatrix4fv(MVPMatrixId,1,false,&MVPMatrix[0][0]);

            glBindVertexArray(BSD->SkyData.MoonVAO->VAOId[0]);
            glDrawArrays(GL_TRIANGLES, 0, BSD->SkyData.MoonVAO->Count);
            glBindVertexArray(0);
                    
            glBindTexture(GL_TEXTURE_2D,0);
            glUseProgram(0);
        }
    }
    if( BSDAreStarsEnabled(BSD->SkyData) ) {
        BSDUpdateStarsColors(&BSD->SkyData);
        Shader = ShaderCache("StarsShader","Shaders/StarsVertexShader.glsl","Shaders/StarsFragmentShader.glsl");
        if( Shader ) {
            glUseProgram(Shader->ProgramId);
            MVPMatrixId = glGetUniformLocation(Shader->ProgramId,"MVPMatrix");
            
            glUniformMatrix4fv(MVPMatrixId,1,false,&MVPMatrix[0][0]);
            glPointSize(2.f);
            glBindVertexArray(BSD->SkyData.StarsVAO->VAOId[0]);
            glDrawArrays(GL_POINTS, 0, BSD_SKY_MAX_STARS_NUMBER);
            glBindVertexArray(0);
            glUseProgram(0);
        }
    }
    glDepthMask(1);
}

bool BSDParseRenderObjectData(BSD_t *BSD,FILE *BSDFile,int GameEngine)
{
    BSD->RenderObjectList = RenderObjectLoadAllFromTable(BSD->EntryTable,BSD->RenderObjectTable,BSDFile,GameEngine,false);
    if( !BSD->RenderObjectList ) {
        DPrintf("BSDParseRenderObjectData: Failed to load render objects...\n"); 
        return false;
    }
    return true;
}
/*
    NOTE(Adriano): 
    BSD File can roughly be divided in 2 sections:
    The Header which has a fixed size of 2048 and the actual BSD File.
    So all the offset which are found below the header needs to be fixed since they are off by 2048.
    This means that the file is not meant to be read sequentially but rather there are offset that tells you
    which part of the file you need to read...only the TSP info section seems to be static.
    
*/

int BSDGetTSPDynamicIndexOffsetFromNodeType(int Type)
{
    switch( Type ) {
        case 3:
            return 90;
        case 5:
            return 88;
        case 6:
            return 108;
        default:
            return -1;
    }
}

void BSDParseNodeChunk(BSDNode_t *Node,BSD_t *BSD,int IsMultiplayer,FILE *BSDFile)
{
    int Offset;
    int NodeNumReferencedRenderObjectIdOffset;
    int NumReferencedRenderObjectId;
    int NodeRenderObjectId;
    vec3 NodePosition;
    vec3 NodeRotation;
    int PrevPos;
    short CompartmentNumber;
    int i;

    int DynamicIndexOffset;
    if( !Node || !BSDFile) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDParseNodeChunk: Invalid %s\n",InvalidFile ? "file" : "node struct");
        return;
    }
    
    fseek(BSDFile, Node->FilePosition, SEEK_SET);
    
    DPrintf("BSDParseNodeChunk:Parsing node with Id:%i | Id:%s\n",Node->Id,BSDNodeGetEnumStringFromNodeId(Node->Id));
    DPrintf("Size:%i\n",Node->Size);
    DPrintf("U2:%i\n",Node->u2);
    DPrintf("Type:%i\n",Node->Type);
    DPrintf("Position:(%i;%i;%i) Pad %i\n",Node->Position.x,Node->Position.y,
            Node->Position.z,Node->Position.Pad);
    DPrintf("Rotation:(%i;%i;%i) Pad %i\n",Node->Rotation.x,Node->Rotation.y,
            Node->Rotation.z,Node->Rotation.Pad);
    if( Node->CollisionVolumeType == 0 ) {
        DPrintf("CollisionType:%s Radius:%i\n",BSDGetCollisionVolumeStringFromType(Node->CollisionVolumeType),
                Node->CollisionInfo0);
    } else if( Node->CollisionVolumeType == 1 ) {
        DPrintf("CollisionType:%s Radius:%i BottomY:%i TopY:%i \n",
                BSDGetCollisionVolumeStringFromType(Node->CollisionVolumeType),
                Node->CollisionInfo0,Node->CollisionInfo1,
                Node->CollisionInfo2);
    } else {
        DPrintf("CollisionType:%s Width:%i Height:%i Depth:%i \n",
                BSDGetCollisionVolumeStringFromType(Node->CollisionVolumeType),
                Node->CollisionInfo0,Node->CollisionInfo1,
                Node->CollisionInfo2);
    }
    
    if( Node->Id == BSD_PLAYER_SPAWN ) {
        PrevPos = GetCurrentFilePosition(BSDFile);
        fseek(BSDFile,Node->FilePosition + 52,SEEK_SET);
        fread(&Node->SpawnIndex,sizeof(Node->SpawnIndex),1,BSDFile);
        fseek(BSDFile,PrevPos,SEEK_SET);
    } else {
        Node->SpawnIndex = 0;
    }

    assert(Node->Position.Pad == 0);
    assert(Node->Rotation.Pad == 0);
        
    if( (DynamicIndexOffset = BSDGetTSPDynamicIndexOffsetFromNodeType(Node->Type)) != -1 ) {
        PrevPos = GetCurrentFilePosition(BSDFile);
        fseek(BSDFile,Node->FilePosition + DynamicIndexOffset,SEEK_SET);
        fread(&Node->DynamicBlockIndex,sizeof(Node->DynamicBlockIndex),1,BSDFile);
        fseek(BSDFile,PrevPos,SEEK_SET);
        DPrintf("Node has Dynamic Index %i\n",Node->DynamicBlockIndex);
    } else {
        Node->DynamicBlockIndex = -1;
    }
        
    BSDPositionToGLMVec3(Node->Position,NodePosition);
    BSDPositionToGLMVec3(Node->Rotation,NodeRotation);
    
    if( Node->Id != BSD_TSP_LOAD_TRIGGER && Node->Size > 48 ) {
        PrevPos = GetCurrentFilePosition(BSDFile);
        fseek(BSDFile,Node->FilePosition + 84,SEEK_SET);
        fread(&Node->MessageData,sizeof(Node->MessageData),1,BSDFile);
        DPrintf("Node has message reg index %i\n",Node->MessageData);
        fseek(BSDFile,PrevPos,SEEK_SET);
    } else {
        Node->MessageData = 0;
    }
    
    if( Node->Type == 2 || Node->Type == 4 || Node->Type == 6 ) {
        //0x74
        Offset = 96;
    } else if ( Node->Type == 3 ) {
        //0x60
        Offset = 116;
    } else if( Node->Type == 0 && Node->Id == BSD_TSP_LOAD_TRIGGER ) {
        Offset = 48;
    } else {
        Offset = 0;
    }
    if( Offset != 0 ) {
        PrevPos = GetCurrentFilePosition(BSDFile);
        DPrintf("Node type %i has offset at %i Position:%i.\n",Node->Type,Offset,Node->FilePosition + Offset);
        fseek(BSDFile,Node->FilePosition + Offset,SEEK_SET);
        if( Node->Id == BSD_TSP_LOAD_TRIGGER ) {
            DPrintf("Node is a BSD_TSP_LOAD_TRIGGER.\n");
            for( i = 0; i < 4; i++ ) {
                fread(&CompartmentNumber,sizeof(short),1,BSDFile);
                assert(CompartmentNumber != -1);
                DPrintf("Node will ask TSP to draw Compartment %i\n",CompartmentNumber);
            }
        } else {
            fread(&NodeNumReferencedRenderObjectIdOffset,sizeof(NodeNumReferencedRenderObjectIdOffset),1,BSDFile);
            DPrintf("Node has RenderObject offset %i.\n",NodeNumReferencedRenderObjectIdOffset);
            if( NodeNumReferencedRenderObjectIdOffset != 0 ) {
                if( Node->Type == 4 ) {
                    DPrintf("Node has Type 4 so the RenderObject Id is %i.\n",NodeNumReferencedRenderObjectIdOffset);
                    BSDAddNodeToRenderObjecDrawableList(BSD,IsMultiplayer,NodeNumReferencedRenderObjectIdOffset,NodePosition,NodeRotation);
                } else {
                    fseek(BSDFile,Node->FilePosition + NodeNumReferencedRenderObjectIdOffset,SEEK_SET);
                    fread(&NumReferencedRenderObjectId,sizeof(NumReferencedRenderObjectId),1,BSDFile);
                    DPrintf("Node is referencing %i RenderObjects.\n",NumReferencedRenderObjectId);
                    for( i = 0; i < NumReferencedRenderObjectId; i++ ) {
                        fread(&NodeRenderObjectId,sizeof(NodeRenderObjectId),1,BSDFile);
                        if( Node->Id == BSD_ENEMY_SPAWN && NodeRenderObjectId != 3817496448 && i == 0 ) {
                            DPrintf("We have a different RenderObject for this enemy spawn...\n");
                        }
                        BSDAddNodeToRenderObjecDrawableList(BSD,IsMultiplayer,NodeRenderObjectId,NodePosition,NodeRotation);
                    }
                }
            }
        }
        fseek(BSDFile,PrevPos,SEEK_SET);
    } else {
        DPrintf("Zero Offset.\n");
    }
}
int BSDParseNodes(BSD_t *BSD,int IsMultiplayer,FILE *BSDFile)
{
    int i;
    
    if( !BSD || !BSDFile ) {
        bool InvalidFile = (BSDFile == NULL ? true : false);
        printf("BSDReadNodeTableChunk: Invalid %s\n",InvalidFile ? "file" : "BSD struct");
        return 0;
    }
    
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        BSDParseNodeChunk(&BSD->NodeData.Node[i],BSD,IsMultiplayer,BSDFile);
    }
    return 1;
}

int BSDLoad(BSD_t *BSD,int GameEngine,int IsMultiplayer,FILE *BSDFile)
{       
    if( !BSDReadSceneInfoBlock(BSDFile, &BSD->SceneInfo) ) {
        return 0;
    }
    
    if( !BSDReadAnimatedLightTableBlock(BSDFile, &BSD->AnimatedLightsTable) ) {
        return 0;
    }
    
    //This file section of 80 bytes contains the begin/end offset of some entries.
    //Like the NodeListTableStart/NodeListEnd after the header (+2048).
    //We can have a max of 20 offsets or 10 Begin/End Definitions.
    if( !BSDReadEntryTableBlock(BSDFile, &BSD->EntryTable) ) {
        return 0;
    }
    
    if( !BSDReadSkyBlock(BSDFile, &BSD->SkyData) ) {
        return 0;
    }
        
    if( !BSDReadRenderObjectTable(BSDFile,GameEngine, &BSD->RenderObjectTable) ) {
        return 0;
    }

    if( !BSDParseRenderObjectData(BSD,BSDFile,GameEngine) ) {
        return 0;
    }

    if( !BSDReadNodeInfoBlock(BSDFile,BSD->EntryTable.NodeTableOffset,&BSD->NodeData) ) {
        return 0;
    }
    
    if( !BSDParseNodes(BSD,IsMultiplayer,BSDFile) ){
        return 0;
    }
    
    if( !BSDReadPropertySetFileBlock(BSDFile,&BSD->PropertySetFile) ) {
        return 0;
    }
    fclose(BSDFile);
    return 1;
}


FILE *BSDEarlyInit(BSD_t **BSD,const char *MissionPath,int MissionNumber,int LevelNumber)
{
    BSD_t *LocalBSD;
    FILE *BSDFile;
    char Buffer[512];
    int i;
    
    snprintf(Buffer,sizeof(Buffer),"%s/%i_%i.BSD",MissionPath,MissionNumber,LevelNumber);

    DPrintf("BSDEarlyInit:Loading BSD file %s...\n",Buffer);
    
    BSDFile = fopen(Buffer,"rb");
    
    if( BSDFile == NULL ) {
        DPrintf("BSDEarlyInit:Failed opening BSD File.\n");
        return NULL;
    }
    LocalBSD = malloc(sizeof(BSD_t));
    
    if( !LocalBSD ) {
        DPrintf("BSDEarlyInit:Failed allocating memory for BSD data\n");
        fclose(BSDFile);
        return NULL;
    }
    
    LocalBSD->RenderObjectList = NULL;
    LocalBSD->RenderObjectDrawableList = NULL;
    
    LocalBSD->NodeVAO = NULL;
    LocalBSD->RenderObjectPointVAO = NULL;
    LocalBSD->NumRenderObjectPoint = 0;
    LocalBSD->SkyData.MoonVAO = NULL;
    LocalBSD->SkyData.StarsVAO = NULL;
    
    LocalBSD->NodeData.Table = NULL;
    LocalBSD->NodeData.Node = NULL;
    
    LocalBSD->PropertySetFile.Property = NULL;
    
    for( i = 0; i < BSD_ANIMATED_LIGHTS_TABLE_SIZE; i++ ) {
        LocalBSD->AnimatedLightsTable.AnimatedLightsList[i].ColorList = NULL;
    }

    //At position 152 after the header we have the SPRITE definitions...
    //Maybe hud/ammo...
    if( !BSDReadTSPInfoBlock(BSDFile,&LocalBSD->TSPInfo) ) {
        free(LocalBSD);
        fclose(BSDFile);
        return NULL;
    }
    *BSD = LocalBSD;
    return BSDFile;
}
