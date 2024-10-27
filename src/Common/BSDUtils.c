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
#include "RenderObject.h"

Color3b_t BSDStarsColors[BSD_SKY_MAX_STAR_COLORS_NUMBER] = {
    {128,128,128},
    {240,96,64},
    {128,128,32},
    {240,64,64},
    {96,96,240},
    {255,255,255},
    {255,128,64},
    {128,64,255},
};

void BSDPositionToGLMVec3(BSDPosition_t In,vec3 Out)
{
    Out[0] = In.x;
    Out[1] = In.y;
    Out[2] = In.z;
}

void BSDGetProperty(BSDPropertySetFile_t PropertySetFile,int PropertyIndex)
{
    int i;
    DPrintf("BSDGetProperty:Property %i has %i nodes\n",PropertyIndex,PropertySetFile.Property[PropertyIndex].NumNodes);
    for( i = 0; i <  PropertySetFile.Property[PropertyIndex].NumNodes; i++ ) {
        DPrintf("BSDGetProperty Property Node %i\n", PropertySetFile.Property[PropertyIndex].NodeList[i]);
//            assert(BSD->PropertySetFile.Property[i].Data[j] < 158);
    }
}

int BSDGetRealOffset(int RelativeOffset)
{
    return RelativeOffset + BSD_HEADER_SIZE;
}

int BSDGetRenderObjectTableOffset(int GameEngine)
{
    return GameEngine == MOH_GAME_STANDARD ? BSD_RENDER_OBJECT_STARTING_OFFSET : BSD_MOH_UNDERGROUND_RENDER_OBJECT_STARTING_OFFSET;
}

bool BSDIsMoonEnabled(BSDSky_t SkyData)
{
    return SkyData.MoonZ != 0;
}

bool BSDAreStarsEnabled(BSDSky_t SkyData)
{
    return SkyData.StarRadius != 0;
}

void BSDGetObjectDefaultExportMatrix(RenderObject_t *RenderObject,mat4 Result)
{
    vec3 RotationAxis;
    vec3 Temp;
    mat4 RotationMatrix;
    mat4 ScaleMatrix;
    mat4 TranslationMatrix;
    mat4 RotScale;
    
    if( !RenderObject ) {
        return;
    }
    
    glm_mat4_identity(Result);
    glm_mat4_identity(TranslationMatrix);
    glm_mat4_identity(RotationMatrix);   
    
    RotationAxis[0] = 1;
    RotationAxis[1] = 0;
    RotationAxis[2] = 0;
    glm_rotate(RotationMatrix,glm_rad(180.f), RotationAxis);

    glm_scale_make(ScaleMatrix,RenderObject->Scale);
    glm_mat4_mul(RotationMatrix,ScaleMatrix,RotScale);
    glm_mat4_mul(TranslationMatrix,RotScale,Result);

}

void BSDUpdateAnimatedLights(BSDAnimatedLightTable_t *AnimatedLightsTable)
{
    BSDAnimatedLight_t *AnimatedLight;
    int Now;
    int i;
    
    if( !AnimatedLightsTable ) {
        return;
    }
    for( i = 0; i < AnimatedLightsTable->NumAnimatedLights; i++ ) {
        AnimatedLight = &AnimatedLightsTable->AnimatedLightsList[i];
        if( !AnimatedLight->NumColors ) {
            continue;
        }
        Now = SysMilliseconds();
        //NOTE(Adriano):Avoid running too fast...
        if( (Now - AnimatedLight->LastUpdateTime ) < 30 ) {
            continue;
        }
        AnimatedLight->LastUpdateTime = Now;
        AnimatedLight->Delay--;
        if( AnimatedLight->Delay <= 0 ) {
            AnimatedLight->ColorIndex++;
            if( AnimatedLight->ColorIndex >= AnimatedLight->NumColors ) {
                AnimatedLight->ColorIndex = 0;
            }
            AnimatedLight->Delay = AnimatedLight->ColorList[AnimatedLight->ColorIndex].rgba[3];
            AnimatedLight->CurrentColor = AnimatedLight->ColorList[AnimatedLight->ColorIndex].c;
        }
    }
}

void BSDUpdateStarsColors(BSDSky_t *SkyData)
{
    float Data[3];
    float DataSize;
    int BaseOffset;
    int Stride;
    int Random;
    int i;
    Color1i_t StarColor;
    
    if( !SkyData ) {
        return;
    }
    
    DataSize = 3 * sizeof(float);
    Stride = (3 + 3) * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, SkyData->StarsVAO->VBOId[0]);
    
    for( i = 0; i < BSD_SKY_MAX_STARS_NUMBER; i++ ) {
        Random = rand();
        BaseOffset = (i * Stride);
        StarColor = SkyData->StarsColors[i];
        Data[0] = StarColor.rgba[0];
        Data[1] = StarColor.rgba[1];
        Data[2] = StarColor.rgba[2];
        if( (Random & 3 ) != 0 ) {
            if( StarColor.rgba[0] >= 33 ) {
                Data[0] -= 32;
            }
            if( StarColor.rgba[1] >= 33 ) {
                Data[1] -= 32;
            }
            if( StarColor.rgba[2] >= 33 ) {
                Data[2] -= 32;
            }
        }
        Data[0] /= 255.f;
        Data[1] /= 255.f;
        Data[2] /= 255.f;
        glBufferSubData(GL_ARRAY_BUFFER, BaseOffset + (3*sizeof(float)), DataSize, &Data);
    }
     glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BSDWriteRenderObjectToPlyFile(RenderObject_t *RenderObject,VRAM_t *VRAM,int GameEngine,const char *OutDirectory)
{
    char *EngineName;
    char *PlyObjectFile;
    char *ObjectFileName;
    char *TextureFile;
    FILE *PlyObjectOutFile;
    char Buffer[256];
    mat4 ModelMatrix;
    int FaceCount;
    int VertexOffset;
    int Vert0;
    int Vert1;
    int Vert2;
    int i;
    
    if( !RenderObject || !OutDirectory ) {
        bool InvalidDirectory = (OutDirectory == NULL ? true : false);
        DPrintf("BSDWriteRenderObjectToPlyFile: Invalid %s\n",InvalidDirectory ? "Directory" : "RenderObject");
        return;
    }
    if( !VRAM ) {
        DPrintf("BSDWriteRenderObjectToPlyFile:Invalid VRAM data\n");
        return;
    }
    if( !RenderObject->IsStatic ) {
        DPrintf("BSDWriteRenderObjectToPlyFile:RenderObject is not static...\n");
        return;
    }
    
    asprintf(&EngineName,"%s",(GameEngine == MOH_GAME_STANDARD) ? "MOH" : "MOHUndergound");
    asprintf(&ObjectFileName,"%s-RenderObject-%i.ply",EngineName,RenderObject->Id);
    asprintf(&PlyObjectFile,"%s%c%s",OutDirectory,PATH_SEPARATOR,ObjectFileName);
    asprintf(&TextureFile,"%s%cvram.png",OutDirectory,PATH_SEPARATOR);
    DPrintf("BSDWriteRenderObjectToPlyFile:Writing RenderObject -> %s\n",PlyObjectFile);

    PlyObjectOutFile = fopen(PlyObjectFile,"w");
    if( !PlyObjectOutFile ) {
        DPrintf("BSDWriteRenderObjectToPlyFile:Failed to open %s for writing\n",PlyObjectFile);
        return;
    }
    
    sprintf(Buffer,"ply\nformat ascii 1.0\n");
    fwrite(Buffer,strlen(Buffer),1,PlyObjectOutFile);
    FaceCount = RenderObject->NumFaces;
    
    sprintf(Buffer,"element vertex %i\nproperty float x\nproperty float y\nproperty float z\nproperty float red\nproperty float green\n"
            "property float blue\nproperty float s\nproperty float t\n",FaceCount * 3);
    fwrite(Buffer,strlen(Buffer),1,PlyObjectOutFile);
    sprintf(Buffer,"element face %i\nproperty list uchar int vertex_indices\nend_header\n",FaceCount);
    fwrite(Buffer,strlen(Buffer),1,PlyObjectOutFile);

    glm_mat4_identity(ModelMatrix);
    BSDGetObjectDefaultExportMatrix(RenderObject,ModelMatrix);
    RenderObjectExportStaticFaceDataToPlyFile(RenderObject,ModelMatrix,VRAM,PlyObjectOutFile);
    
    for( i = 0; i < RenderObject->NumFaces; i++ ) {
        Vert0 = (i * 3) + 0;
        Vert1 = (i * 3) + 1;
        Vert2 = (i * 3) + 2;
        sprintf(Buffer,"3 %i %i %i\n",Vert0,Vert1,Vert2);
        fwrite(Buffer,strlen(Buffer),1,PlyObjectOutFile);
    }
    VRAMSave(VRAM,TextureFile);
    free(EngineName);
    free(PlyObjectFile);
    free(ObjectFileName);
    free(TextureFile);
    fclose(PlyObjectOutFile);
}

int BSDGetCurrentAnimatedLightColorByIndex(BSDAnimatedLightTable_t AnimatedLightsTable,int Index)
{
    if( Index < 0 || Index >= BSD_ANIMATED_LIGHTS_TABLE_SIZE ) {
        DPrintf("BSDGetCurrentAnimatedLightColorByIndex:Invalid index %i\n",Index);
        return 0;
    }
    return AnimatedLightsTable.AnimatedLightsList[Index].CurrentColor;
}

void BSDGetNodeColorById(int NodeId,vec3 OutColor)
{
    switch( NodeId ) {
        case BSD_TSP_LOAD_TRIGGER:
            OutColor[0] = 0;
            OutColor[1] = 0;
            OutColor[2] = 1;
            break;
        case BSD_PLAYER_SPAWN:
            OutColor[0] = 0;
            OutColor[1] = 1;
            OutColor[2] = 0;
            break;
        case BSD_NODE_SCRIPT:
            OutColor[0] = 1;
            OutColor[1] = 1;
            OutColor[2] = 0;
            break;
        case BSD_ANIMATED_OBJECT:
            OutColor[0] = 1;
            OutColor[1] = 0;
            OutColor[2] = 1;
            break;
        case BSD_LADDER:
            OutColor[0] = 0;
            OutColor[1] = 0.64f;
            OutColor[2] = 0;
            break;
        default:
            OutColor[0] = 1;
            OutColor[1] = 0;
            OutColor[2] = 0;
            break;
    }
}

void BSDGetPlayerSpawn(BSDNodeInfo_t NodeData,int SpawnIndex,vec3 Position,vec3 Rotation)
{
    vec3 LocalRotation;

    int i;
    
    glm_vec3_zero(Position);
    
    for( i = 0; i < NodeData.Header.NumNodes; i++ ) {
        if( NodeData.Node[i].Id != BSD_PLAYER_SPAWN ) {
            continue;
        }
        if( NodeData.Node[i].SpawnIndex != SpawnIndex ) {
            continue;
        }
        BSDPositionToGLMVec3(NodeData.Node[i].Position,Position);
        glm_vec3_rotate(Position, DEGTORAD(180.f), GLM_XUP);
        if( Rotation ) {
            BSDPositionToGLMVec3(NodeData.Node[i].Rotation,LocalRotation);
            glm_vec3_scale(LocalRotation,360.f/4096.f,Rotation);
        }
        break;
    }
    return;
}



int BSDNodeIdToRenderObjectId(int NodeId)
{
    int RenderObjectId;
//  1821346472 -> 519275822
//  935865828 -> 2278882431
//  149114796 -> 1686008476
//  4262665218 -> 1771441436
//  2622993903 -> 1771441436
//  3832727321 -> 1335595487
//  3815705185 -> 137063914
//  1730263601 -> 1771441436
//  3363099888 -> 1771441436
//  648959524 -> 1771441436
//  427407355 -> 1771441436
//  2966651809 -> 1771441436
//  1444155995 -> 137063914
//  1088149327 -> 1771441436
//  1696717421 -> 1771441436
//  633670612 -> 3487282118
//  2470021088 -> 1909080654
//  2522430033 -> 2018764808
//  1247503060 -> 4294967295
//  3545379694 -> 4294967295
    switch ( NodeId ) {
        case 1821346472:
            RenderObjectId = 519275822;
            break;
        case 935865828:
            RenderObjectId = 2278882431;
            break;
        case 149114796:
            RenderObjectId = 1686008476;
            break;
        case 4262665218:
            RenderObjectId = 1771441436;
            break;
        case 2622993903:
            RenderObjectId = 1771441436;
            break;
        case 3832727321:
            RenderObjectId = 1335595487;
            break;
        case 3815705185:
        case 1444155995:
            RenderObjectId = 137063914;
            break;
        case 1730263601:
            RenderObjectId = 1771441436;
            break;
        case 3363099888:
            RenderObjectId = 1771441436;
            break;
        case 648959524:
            RenderObjectId = 1771441436;
            break;
        case 427407355:
            RenderObjectId = 1771441436;
            break;
        case 2966651809:
            RenderObjectId = 1771441436;
            break;
        case 1088149327:
            RenderObjectId = 1771441436;
            break;
        case 1696717421:
            RenderObjectId = 1771441436;
            break;
        case 633670612:
            RenderObjectId = 3487282118;
            break;
        case 2470021088:
            RenderObjectId = 1909080654;
            break;
        case 2522430033:
            RenderObjectId = 2018764808;
            break;
        case 1247503060:
        case 3545379694:
            RenderObjectId = 4294967295;
            break;
        case 0:
            RenderObjectId = 0;
            break;
        default:
            DPrintf("RenderObjectId was not in the mapping table...assuming it is the NodeId itself.\n");
            RenderObjectId = NodeId;
            break;
    }
    return RenderObjectId;
}

int BSDMPNodeIdToRenderObjectId(int NodeId)
{
    int RenderObjectId;
    switch( NodeId ) {
        //NEW
        case 3815705185:
            //Bazooka Ammo.
            RenderObjectId = 3139577012;
            break;
        case 3832727321:
            //SMG Ammo.
            RenderObjectId = 3158305228;
            break;
        case 2622993903:
            //Pistol Ammo.
            RenderObjectId = 523223373;
            break;
        case 1730263601:
            //Grenade Ammo.
            RenderObjectId = 1064080612;
            break;
        case 3363099888:
            //Shotgun Ammo.
            RenderObjectId = 1322820526;
            break;
        case 648959524:
            //Rifle Ammo.
            RenderObjectId = 414830070;
            break;
        case 1444155995:
            //Silencer Ammo.
            RenderObjectId = 1479327691;
            break;
        case 1088149327:
            //German Grenade Ammo.
            RenderObjectId = 1529087242;
            break;
        case 1696717421:
            //BAR Ammo.
            RenderObjectId = 1857331800;
            break;
        case 427407355:
            //Salute Ammo.
            RenderObjectId = 1008432070;
            break;
        //Undefined but found....
        case 1656540076:
            RenderObjectId = 1771441436;
            break;
        case 475277581:
            RenderObjectId = 1771441436;
            break;
        case 1587751891:
            RenderObjectId = 1771441436;
            break;
        case 2044083958:
            RenderObjectId = 1771441436;
            break;
        case 1271113291:
            RenderObjectId = 1771441436;
            break;
        case 2774162518:
            RenderObjectId = 1771441436;
            break;
        case 586892869:
            RenderObjectId = 1771441436;
            break;
        case 807017850:
            //HEALTH
            RenderObjectId = 2278882431;
            break;
        case 254902066:
            RenderObjectId = 1686008476;
            break;
        case 1801304630:
            RenderObjectId = 519275822;
            break;
        case 3482129947:
            RenderObjectId = 1771441436;
            break;
        case 3538476007:
            RenderObjectId = 1771441436;
            break;
        case 3364936547:
            RenderObjectId = 1771441436;
            break;
        default:
            RenderObjectId = NodeId;
            break;
    }
    return RenderObjectId;
}

bool BSDIsRenderObjectPresent(BSDRenderObjectTable_t RenderObjectTable,int RenderObjectId) 
{
    if( BSDGetRenderObjectIndexById(&RenderObjectTable,RenderObjectId) == -1 ) {
        DPrintf("Render Object Id %i not found..\n",RenderObjectId);
        return false;
    }
    return true;
}
const char *BSDNodeGetEnumStringFromNodeId(int NodeId)
{
    switch( NodeId ) {
        case BSD_PLAYER_SPAWN:
            return "Player Spawn";
        case BSD_TSP_LOAD_TRIGGER:
            return "TSP Trigger";
        case BSD_ENEMY_SPAWN:
            return "Enemy Spawn";
        case BSD_DOOR:
            return "Door";
        case BSD_LADDER:
            return "Ladder";
        case BSD_PICKUP_OBJECT:
            return "Pickup Object";
        case BSD_NODE_SCRIPT:
            return "Scripted Node";
        case BSD_ANIMATED_OBJECT:
            return "Animated Object";
        case BSD_DESTRUCTIBLE_WINDOW:
            return "Destructible Window";
        default:
            return "Unknown";
    }
}




const char *BSDGetCollisionVolumeStringFromType(int CollisionVolumeType)
{
    switch( CollisionVolumeType ) {
        case BSD_COLLISION_VOLUME_TYPE_SPHERE:
            return "Sphere";
        case BSD_COLLISION_VOLUME_TYPE_CYLINDER:
            return "Cylinder";
        case BSD_COLLISION_VOLUME_TYPE_BOX:
        default:
            return "Box";
    }
}

bool BSDPointInSphere(vec3 Point,BSDPosition_t Center,float Radius)
{
    vec3  NodePosition;
    BSDPositionToGLMVec3(Center,NodePosition);
    glm_vec3_rotate(NodePosition, DEGTORAD(180.f), GLM_XUP);
    return (  glm_vec3_distance2(Point,NodePosition) <= Radius*Radius );
}

/*
    Cylinder is described using:
    Center Point
    Radius
    MinY/MaxY: Bottom and Top Y coordinate of the cylinder from the center position.
*/
bool BSDPointInCylinder(vec3 Point,BSDPosition_t Center,float Radius,float MinY,float MaxY)
{
    vec3  NodePosition;
    float DeltaX;
    float DeltaY;
    float DeltaZ;
    float Temp;
  
    BSDPositionToGLMVec3(Center,NodePosition);
    glm_vec3_rotate(NodePosition, DEGTORAD(180.f), GLM_XUP);

    //Make sure Min/Max are not swapped out.
    if( MaxY < MinY ) {
        Temp = MaxY;
        MaxY = MinY;
        MinY = Temp;
    }
    
    DeltaX = Point[0] - NodePosition[0];
    DeltaY = Point[1] - NodePosition[1];
    DeltaZ = Point[2] - NodePosition[2];
    
    if( DeltaX * DeltaX + DeltaZ * DeltaZ <= Radius * Radius ) {
        if( DeltaY >= MinY && DeltaY <= MaxY ) {
            return true;
        } 
    }
    return false;
}

bool BSDPointInBox(vec3 Point,BSDPosition_t Center,BSDPosition_t NodeRotation,float Width,float Height,float Depth)
{
    vec3 NodePosition;
    vec3 Delta;
    float HalfSizeX;
    float HalfSizeY;
    float HalfSizeZ;
    vec3  Axis;
    vec3  LocalRotation;
    mat4  RotationMatrix;
  
    BSDPositionToGLMVec3(Center,NodePosition);
    glm_vec3_rotate(NodePosition, DEGTORAD(180.f), GLM_XUP);
    
    HalfSizeX = fabs(Width) / 2.f;
    HalfSizeY = fabs(Height)/ 2.f;
    HalfSizeZ = fabs(Depth) / 2.f;

    Delta[0] = Point[0] - NodePosition[0];
    Delta[1] = Point[1] - NodePosition[1];
    Delta[2] = Point[2] - NodePosition[2];

    if( NodeRotation.x != 0 || NodeRotation.y != 0 || NodeRotation.z != 0 ) {
//         OOB Test...
        BSDPositionToGLMVec3(NodeRotation,LocalRotation);
        glm_vec3_scale(LocalRotation,360.f/4096.f,LocalRotation);
        glm_mat4_identity(RotationMatrix);
        Axis[0] = 0;
        Axis[1] = 1;
        Axis[2] = 0;
        glm_rotate(RotationMatrix,glm_rad(-LocalRotation[1]), Axis);
        Axis[0] = 1;
        Axis[1] = 0;
        Axis[2] = 0;
        glm_rotate(RotationMatrix,glm_rad(LocalRotation[0]), Axis);
        Axis[0] = 0;
        Axis[1] = 0;
        Axis[2] = 1;
        glm_rotate(RotationMatrix,glm_rad(LocalRotation[2]), Axis);
        glm_mat4_mulv3(RotationMatrix, Delta, 1, Delta);
    }
    
    if( fabs(Delta[0]) <= HalfSizeX && fabs(Delta[1]) <= HalfSizeY && fabs(Delta[2]) <= HalfSizeZ ) {
        return true;
    }
    return false;
}

bool BSDPointInNode(vec3 Position,const BSDNode_t *Node)
{
    switch( Node->CollisionVolumeType ) {
        case BSD_COLLISION_VOLUME_TYPE_SPHERE:
            return BSDPointInSphere(Position,Node->Position,Node->CollisionInfo0);
        case BSD_COLLISION_VOLUME_TYPE_CYLINDER:
            return BSDPointInCylinder(Position,Node->Position,Node->CollisionInfo0,Node->CollisionInfo1,Node->CollisionInfo2);
        case BSD_COLLISION_VOLUME_TYPE_BOX:
            return BSDPointInBox(Position,Node->Position,Node->Rotation,Node->CollisionInfo0,Node->CollisionInfo1,Node->CollisionInfo2);
        default:
            DPrintf("Unknown CollisionVolumeType %i for node %i\n",Node->CollisionVolumeType,Node->Id);
            return false;
    }
}

void BSDClearNodesFlag(BSDNodeInfo_t *NodeData)
{
    int i;
    if( !NodeData ) {
        return;
    }
    for( i = 0; i < NodeData->Header.NumNodes; i++ ) {
        NodeData->Node[i].Visited = 0;
    }
}

int BSDGetNodeDynamicDataFromPosition(BSDNodeInfo_t *NodeData,vec3 Position)
{
    int i;
    for( i = 0; i < NodeData->Header.NumNodes; i++ ) {
        if( NodeData->Node[i].Visited ) {
            continue;
        }
        if( NodeData->Node[i].MessageData == -1 ) {
            continue;
        }
        
        if( BSDPointInNode(Position,&NodeData->Node[i]) ) {
            if( NodeData->Node[i].Type == 3 || NodeData->Node[i].Type == 5 ||
                NodeData->Node[i].Type == 6 ) {
                NodeData->Node[i].Visited = 1;
                return NodeData->Node[i].DynamicBlockIndex;
            }
        }
    }
    return -1;
}

BSDRenderObjectElement_t *BSDGetRenderObjectById(const BSDRenderObjectTable_t *RenderObjectTable,int RenderObjectId)
{
    int i;
    
    if( !RenderObjectTable ) {
        return NULL;
    }
    
    for( i = 0; i < RenderObjectTable->NumRenderObject; i++ ) {
        if( RenderObjectTable->RenderObject[i].Id == RenderObjectId ) {
            return &RenderObjectTable->RenderObject[i];
        }
    }
    return NULL;
}

int BSDGetRenderObjectIndexById(const BSDRenderObjectTable_t *RenderObjectTable,int RenderObjectId)
{
    int i;
    
    if( !RenderObjectTable ) {
        return -1;
    }
    for( i = 0; i < RenderObjectTable->NumRenderObject; i++ ) {
        if( RenderObjectTable->RenderObject[i].Id == RenderObjectId ) {
            return i;
        }
    }
    return -1;
}

/*
 * NOTE(Adriano):
 * Some RenderObjects uses the 'ReferencedRenderObjectId' field to reference a RenderObject that contains common
 * informations shared by multiple RenderObjects.
 * In order to correctly parse these entry we need to copy the field from the 'ReferencedRenderObjectId' to the RenderObject that
 * requested it.
 * NOTE(Adriano):Make sure to update the data when new fields are added.
 */
void BSDPatchRenderObjects(FILE *BSDFile,BSDRenderObjectTable_t *RenderObjectTable)
{
    BSDRenderObjectElement_t *CurrentRenderObject;
    BSDRenderObjectElement_t *ReferencedRenderObject;
    int i;
        
    for( i = 0; i < RenderObjectTable->NumRenderObject; i++ ) {
        if( RenderObjectTable->RenderObject[i].ReferencedRenderObjectId == -1 ) {
            continue;
        }
        ReferencedRenderObject = BSDGetRenderObjectById(RenderObjectTable,RenderObjectTable->RenderObject[i].ReferencedRenderObjectId);
        CurrentRenderObject = &RenderObjectTable->RenderObject[i];
        if( !ReferencedRenderObject ) {
            DPrintf("BSDPatchRenderObjects:RenderObject Id %i not found\n",RenderObjectTable->RenderObject[i].ReferencedRenderObjectId);
            continue;
        }
        DPrintf("BSDPatchRenderObjects:Patching up RenderObject Id %i using Id %i\n",RenderObjectTable->RenderObject[i].Id,
            RenderObjectTable->RenderObject[i].ReferencedRenderObjectId
        );
        if(CurrentRenderObject->AnimationDataOffset == -1 ) {
            CurrentRenderObject->AnimationDataOffset = ReferencedRenderObject->AnimationDataOffset;
        }
        if(CurrentRenderObject->FaceOffset == -1 ) {
            CurrentRenderObject->FaceOffset = ReferencedRenderObject->FaceOffset;
        }
        if(CurrentRenderObject->FaceTableOffset == -1 ) {
            CurrentRenderObject->FaceTableOffset = ReferencedRenderObject->FaceTableOffset;
        }
        if(CurrentRenderObject->VertexTableIndexOffset == -1 ) {
            CurrentRenderObject->VertexTableIndexOffset = ReferencedRenderObject->VertexTableIndexOffset;
        }
        if(CurrentRenderObject->UnknownOffset4 == -1 ) {
            CurrentRenderObject->UnknownOffset4 = ReferencedRenderObject->UnknownOffset4;
        }
        if(CurrentRenderObject->VertexOffset == -1 ) {
            CurrentRenderObject->VertexOffset = ReferencedRenderObject->VertexOffset;
            CurrentRenderObject->NumVertex = ReferencedRenderObject->NumVertex;
        }
        if(CurrentRenderObject->HierarchyDataRootOffset == -1 ) {
            CurrentRenderObject->HierarchyDataRootOffset = ReferencedRenderObject->HierarchyDataRootOffset;
        }
        if(CurrentRenderObject->ColorOffset == -1 ) {
            CurrentRenderObject->ColorOffset = ReferencedRenderObject->ColorOffset;
        }
        if(CurrentRenderObject->AnimationDataOffset == -1 ) {
            CurrentRenderObject->AnimationDataOffset = ReferencedRenderObject->AnimationDataOffset;
        }
        if( CurrentRenderObject->ScaleX == 0 && CurrentRenderObject->ScaleY == 0 && CurrentRenderObject->ScaleZ == 0 ) {
            CurrentRenderObject->ScaleX = ReferencedRenderObject->ScaleX;
            CurrentRenderObject->ScaleY = ReferencedRenderObject->ScaleY;
            CurrentRenderObject->ScaleZ = ReferencedRenderObject->ScaleZ;

        }
        if( CurrentRenderObject->Type == -1 ) {
            CurrentRenderObject->Type = ReferencedRenderObject->Type;
        }
        
        if( CurrentRenderObject->FaceV2Offset == -1 ) {
            CurrentRenderObject->FaceV2Offset = ReferencedRenderObject->FaceV2Offset;
        }
        
        if( CurrentRenderObject->NumV2Faces == 0 ) {
            CurrentRenderObject->NumV2Faces = ReferencedRenderObject->NumV2Faces;
        }
        
        if( CurrentRenderObject->NumV2AnimatedFaces == 0 ) {
            CurrentRenderObject->NumV2AnimatedFaces = ReferencedRenderObject->NumV2AnimatedFaces;
        }
        
        if( CurrentRenderObject->AnimatedV2FaceOffset == -1 ) {
            CurrentRenderObject->AnimatedV2FaceOffset = ReferencedRenderObject->AnimatedV2FaceOffset;
        }
    }
}

bool BSDReadPropertySetFileBlock(FILE *BSDFile, BSDPropertySetFile_t *PropertySetFile)
{
    int PropertySetFileOffset;
    int i;
    int j;
    
    if( !BSDFile ) {
        DPrintf("BSDReadPropertySetFileBlock: Invalid file\n");
        return false;
    }
    
    if( !PropertySetFile ) {
        DPrintf("BSDReadPropertySetFileBlock: Invalid data\n");
        return false;
    }
    
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_PROPERTY_SET_FILE_POSITION)) {
        fseek(BSDFile, BSDGetRealOffset(BSD_PROPERTY_SET_FILE_POSITION), SEEK_SET);
    }

    fread(&PropertySetFileOffset,sizeof(PropertySetFileOffset),1,BSDFile);
    
    if( PropertySetFileOffset == 0 ) {
        DPrintf("BSDReadPropertySetFileBlock:BSD File has no property file set.\n");
        return false;
    }
    
    fseek(BSDFile, BSDGetRealOffset(PropertySetFileOffset), SEEK_SET);
    fread(&PropertySetFile->NumProperties,sizeof(PropertySetFile->NumProperties),1,BSDFile);
    DPrintf("BSDReadPropertySetFile:Reading %i properties at %i (%i).\n",PropertySetFile->NumProperties,
            GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - BSD_HEADER_SIZE);
    PropertySetFile->NumProperties++;
    PropertySetFile->Property = malloc(sizeof(BSDProperty_t) * PropertySetFile->NumProperties);
    if( !PropertySetFile->Property ) {
        DPrintf("BSDReadPropertySetFileBlock:Couldn't allocate memory for Property array\n");
        return false;
    }
    for( i = 0; i < PropertySetFile->NumProperties; i++ ) {
        DPrintf("BSDReadPropertySetFileBlock:Reading property %i at %i (%i)\n",i,GetCurrentFilePosition(BSDFile),
            GetCurrentFilePosition(BSDFile) - BSD_HEADER_SIZE
        );
        fread(&PropertySetFile->Property[i].NumNodes,sizeof(PropertySetFile->Property[i].NumNodes),1,BSDFile);
        PropertySetFile->Property[i].NumNodes = (255 - PropertySetFile->Property[i].NumNodes) /*<< 1*/;
        DPrintf("Property contains %i nodes\n",PropertySetFile->Property[i].NumNodes);
        
        SkipFileSection(1,BSDFile);
        PropertySetFile->Property[i].NodeList = malloc(PropertySetFile->Property[i].NumNodes * sizeof(unsigned short));
        if( !PropertySetFile->Property[i].NodeList ) {
            DPrintf("BSDReadPropertySetFileBlock:Couldn't allocate memory for Property node array\n");
            return false;
        }
        DPrintf("BSDReadPropertySetFileBlock:Reading %li bytes.\n",PropertySetFile->Property[i].NumNodes * sizeof(unsigned short));
        for( j = 0; j <  PropertySetFile->Property[i].NumNodes; j++ ) {
            fread(&PropertySetFile->Property[i].NodeList[j],sizeof(PropertySetFile->Property[i].NodeList[j]),1,BSDFile);
            DPrintf("Short %u\n", PropertySetFile->Property[i].NodeList[j]);
        }
    }
    DPrintf("BSDReadPropertySetFileBlock:Property end at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - BSD_HEADER_SIZE);
    return true;
}

bool BSDReadNodeInfoBlock(FILE *BSDFile,int NodeInfoOffset, BSDNodeInfo_t *NodeInfo)
{
    int NodeFilePosition;
    int NodeTableEnd;
    int NextNodeOffset;
    int i;
    
    if( !BSDFile ) {
        DPrintf("BSDReadNodeInfoBlock: Invalid file\n");
        return false;
    }
    
    if( !NodeInfo ) {
        DPrintf("BSDReadNodeInfoBlock: Invalid data\n");
        return false;
    }
    
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(NodeInfoOffset)) {
        fseek(BSDFile, BSDGetRealOffset(NodeInfoOffset), SEEK_SET);
    }

    DPrintf("BSDReadNodeInfoBlock:Reading node table at %i...\n",GetCurrentFilePosition(BSDFile));
    fread(&NodeInfo->Header,sizeof(NodeInfo->Header),1,BSDFile);
    DPrintf("BSDReadNodeInfoBlock:Reading %i entries.\n",NodeInfo->Header.NumNodes);
    DPrintf("TableSize: %i\n",NodeInfo->Header.TableSize);
    DPrintf("U2: %i\n",NodeInfo->Header.u2);
    DPrintf("U3: %i\n",NodeInfo->Header.u3);
    DPrintf("U4: %i\n",NodeInfo->Header.u4);
    DPrintf("U5: %i\n",NodeInfo->Header.u5);
    NodeInfo->Table = malloc(NodeInfo->Header.NumNodes * sizeof(BSDNodeTableEntry_t));
    if( !NodeInfo->Table ) {
        DPrintf("BSDReadNodeChunk:Failed to allocate memory for node table\n");
        return false;
    }
    DPrintf("BSDReadNodeInfoBlock:Nodetable starts at %i\n",GetCurrentFilePosition(BSDFile));
    for( i = 0; i < NodeInfo->Header.NumNodes; i++ ) {
        fread(&NodeInfo->Table[i],sizeof(NodeInfo->Table[i]),1,BSDFile);
        DPrintf("-- NODE %i --\n",i);
        DPrintf("Pointer:%i\n",NodeInfo->Table[i].Pointer);
        DPrintf("Offset:%i\n",NodeInfo->Table[i].Offset);
    }
    NodeTableEnd = GetCurrentFilePosition(BSDFile);
    DPrintf("BSDReadNodeInfoBlock:Nodetable ends at %i\n",NodeTableEnd);
    //All the node offset are calculated from the 0 node...
    //So all the offset inside a node are Offset+AddressOfFirstNode.
    //TODO:Load each node entry one by one (not using a single fread) since
    //     there are many types of node that contains different data...
    NodeInfo->Node = malloc(NodeInfo->Header.NumNodes * sizeof(BSDNode_t));
    if( !NodeInfo->Node ) {
        DPrintf("BSDReadNodeInfoBlock:Failed to allocate memory for node array\n");
        return false;
    }
    for( i = 0; i < NodeInfo->Header.NumNodes; i++ ) {
        NodeFilePosition = GetCurrentFilePosition(BSDFile);
        DPrintf(" -- NODE %i (Pos %i PosNoHeader %i)-- \n",i,NodeFilePosition,NodeFilePosition - BSD_HEADER_SIZE);
        assert(GetCurrentFilePosition(BSDFile) == (NodeInfo->Table[i].Offset + NodeTableEnd));
        fread(&NodeInfo->Node[i].Id,sizeof(NodeInfo->Node[i].Id),1,BSDFile);
        fread(&NodeInfo->Node[i].Size,sizeof(NodeInfo->Node[i].Size),1,BSDFile);
        fread(&NodeInfo->Node[i].u2,sizeof(NodeInfo->Node[i].u2),1,BSDFile);
        fread(&NodeInfo->Node[i].Type,sizeof(NodeInfo->Node[i].Type),1,BSDFile);
        fread(&NodeInfo->Node[i].Position,sizeof(NodeInfo->Node[i].Position),1,BSDFile);
        fread(&NodeInfo->Node[i].Rotation,sizeof(NodeInfo->Node[i].Rotation),1,BSDFile);
        fread(&NodeInfo->Node[i].Pad,sizeof(NodeInfo->Node[i].Pad),1,BSDFile);
        fread(&NodeInfo->Node[i].CollisionVolumeType,sizeof(NodeInfo->Node[i].CollisionVolumeType),1,BSDFile);
        fread(&NodeInfo->Node[i].CollisionInfo0,sizeof(NodeInfo->Node[i].CollisionInfo0),1,BSDFile);
        fread(&NodeInfo->Node[i].CollisionInfo1,sizeof(NodeInfo->Node[i].CollisionInfo1),1,BSDFile);
        fread(&NodeInfo->Node[i].CollisionInfo2,sizeof(NodeInfo->Node[i].CollisionInfo2),1,BSDFile);
        NodeInfo->Node[i].FilePosition = NodeFilePosition;
        NextNodeOffset = NodeFilePosition + NodeInfo->Node[i].Size;
        fseek(BSDFile,NextNodeOffset,SEEK_SET);
    }
    return true;
}

bool BSDReadRenderObjectTable(FILE *BSDFile,int GameEngine, BSDRenderObjectTable_t *RenderObjectTable)
{
    int FirstRenderObjectFilePosition;
    int PreviousFilePosition;
    int Result;
    int i;
    int TableOffset;
    int RenderObjectSize;
    
    if( !BSDFile ) {
        DPrintf("BSDReadRenderObjectTable: Invalid file\n");
        return false;
    }
    
    if( !RenderObjectTable ) {
        DPrintf("BSDReadRenderObjectTable: Invalid data\n");
        return false;
    }
    
    TableOffset = BSDGetRenderObjectTableOffset(GameEngine);
    
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(TableOffset)) {
        fseek(BSDFile, BSDGetRealOffset(TableOffset), SEEK_SET);
    }
    
    fread(&RenderObjectTable->NumRenderObject,sizeof(RenderObjectTable->NumRenderObject),1,BSDFile);
    FirstRenderObjectFilePosition = GetCurrentFilePosition(BSDFile);
    
    DPrintf("BSDReadRenderObjectTable:Reading %i RenderObject Elements...\n",RenderObjectTable->NumRenderObject);
        
    RenderObjectTable->RenderObject = malloc(RenderObjectTable->NumRenderObject * sizeof(BSDRenderObjectElement_t));
    if( !RenderObjectTable->RenderObject ) {
        DPrintf("BSDReadRenderObjectTable:Failed to allocate memory for RenderObject Array\n");
        return 0;
    }
    RenderObjectSize = GameEngine == MOH_GAME_UNDERGROUND ? MOH_UNDERGROUND_RENDER_OBJECT_SIZE : MOH_RENDER_OBJECT_SIZE;
    for( i = 0; i < RenderObjectTable->NumRenderObject; i++ ) {
        assert(GetCurrentFilePosition(BSDFile) == FirstRenderObjectFilePosition + (i * RenderObjectSize));
        DPrintf("BSDReadRenderObjectTable:Reading RenderObject %i at %i...\n",i,GetCurrentFilePosition(BSDFile));
        fread(&RenderObjectTable->RenderObject[i],RenderObjectSize,1,BSDFile);
        DPrintf("RenderObject Id:%i\n",RenderObjectTable->RenderObject[i].Id);
        if( RenderObjectTable->RenderObject[i].Type == 1 ) {
            DPrintf("RenderObject Type:%i | %s\n",RenderObjectTable->RenderObject[i].Type,
                    RenderObjectGetWeaponNameFromId(RenderObjectTable->RenderObject[i].Id));
        } else {
            DPrintf("RenderObject Type:%i | %s\n",RenderObjectTable->RenderObject[i].Type,
                    RenderObjectGetStringFromType(RenderObjectTable->RenderObject[i].Type));
        }
        DPrintf("RenderObject Element Vertex Offset: %i (%i)\n",RenderObjectTable->RenderObject[i].VertexOffset,
                RenderObjectTable->RenderObject[i].VertexOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Element NumVertex: %i\n",RenderObjectTable->RenderObject[i].NumVertex);
        //Those offset are relative to the EntryTable.
        DPrintf("RenderObject FaceTableOffset: %i (%i)\n",RenderObjectTable->RenderObject[i].FaceTableOffset,
                RenderObjectTable->RenderObject[i].FaceTableOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject VertexTableIndexOffset: %i (%i)\n",RenderObjectTable->RenderObject[i].VertexTableIndexOffset,
                RenderObjectTable->RenderObject[i].VertexTableIndexOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Hierarchy Data Root Offset: %i (%i)\n",RenderObjectTable->RenderObject[i].HierarchyDataRootOffset,
                RenderObjectTable->RenderObject[i].HierarchyDataRootOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject FaceOffset: %i (%i)\n",RenderObjectTable->RenderObject[i].FaceOffset,
                RenderObjectTable->RenderObject[i].FaceOffset + BSD_HEADER_SIZE);
        DPrintf("RenderObject Scale: %i;%i;%i (4096 is 1 meaning no scale)\n",
                RenderObjectTable->RenderObject[i].ScaleX / 4,
                RenderObjectTable->RenderObject[i].ScaleY / 4,
                RenderObjectTable->RenderObject[i].ScaleZ / 4);
        if( RenderObjectTable->RenderObject[i].ReferencedRenderObjectId != -1 ) {
            DPrintf("RenderObject References RenderObject Id:%i\n",RenderObjectTable->RenderObject[i].ReferencedRenderObjectId);
        } else {
            DPrintf("RenderObject No Reference set...\n");
        }
        if( GameEngine == MOH_GAME_UNDERGROUND ) {
            DPrintf("RenderObject FaceV2Offset: %i (%i)\n",RenderObjectTable->RenderObject[i].FaceV2Offset,
                RenderObjectTable->RenderObject[i].FaceV2Offset + BSD_HEADER_SIZE);
            DPrintf("RenderObject NumV2Faces: %i\n",RenderObjectTable->RenderObject[i].NumV2Faces);
            DPrintf("RenderObject NumAnimatedFaces for V2: %i\n",RenderObjectTable->RenderObject[i].NumV2AnimatedFaces);
            DPrintf("RenderObject Animated FaceOffset V2: %i (%i)\n",RenderObjectTable->RenderObject[i].AnimatedV2FaceOffset,
                RenderObjectTable->RenderObject[i].AnimatedV2FaceOffset + BSD_HEADER_SIZE);
            DPrintf("RenderObject Unknown1 and 2: %i %i\n",RenderObjectTable->RenderObject[i].Unknown1,
                RenderObjectTable->RenderObject[i].Unknown2);

        }
    }
    // Patch up the data using the referenced renderobjects ids...
    BSDPatchRenderObjects(BSDFile,RenderObjectTable);
    return 1;
}
bool BSDReadSkyBlock(FILE *BSDFile,BSDSky_t *Sky)
{    
    if( !BSDFile ) {
        DPrintf("BSDReadSkyBlock: Invalid file\n");
        return false;
    }
    
    if( !Sky ) {
        DPrintf("BSDReadSkyBlock: Invalid data\n");
        return false;
    }

    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_SKY_DATA_FILE_POSITION)) {
        fseek(BSDFile, BSDGetRealOffset(BSD_SKY_DATA_FILE_POSITION), SEEK_SET);
    }

    fread(&Sky->U0,sizeof(Sky->U0),1,BSDFile);
    fread(&Sky->U1,sizeof(Sky->U1),1,BSDFile);
    fread(&Sky->U2,sizeof(Sky->U2),1,BSDFile);
    fread(&Sky->StarRadius,sizeof(Sky->StarRadius),1,BSDFile);
    fread(&Sky->U3,sizeof(Sky->U3),1,BSDFile);
    fread(&Sky->MoonZ,sizeof(Sky->MoonZ),1,BSDFile);
    fread(&Sky->MoonY,sizeof(Sky->MoonY),1,BSDFile);
    fread(&Sky->U4,sizeof(Sky->U4),1,BSDFile);
    fread(&Sky->U5,sizeof(Sky->U5),1,BSDFile);
    fread(&Sky->U6,sizeof(Sky->U6),1,BSDFile);
    
    DPrintf("BSDReadSkyBlock:MoonY:%i MoonZ:%i\n",Sky->MoonY,Sky->MoonZ);
    DPrintf("BSDReadSkyBlock:Star Radius:%i\n",Sky->StarRadius);
    return true;
}

bool BSDReadEntryTableBlock(FILE *BSDFile,BSDEntryTable_t *EntryTable)
{    
    if( !BSDFile ) {
        DPrintf("BSDReadEntryTableBlock: Invalid file\n");
        return false;
    }
    if( !EntryTable ) {
        DPrintf("BSDReadEntryTableBlock: Invalid data\n");
        return false;
    }

    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_ENTRY_TABLE_FILE_POSITION)) {
        fseek(BSDFile, BSDGetRealOffset(BSD_ENTRY_TABLE_FILE_POSITION), SEEK_SET);
    }
    
    assert(sizeof(BSDEntryTable_t) == 80);
    DPrintf("BSDReadEntryTableBlock: Reading EntryTable at %i (%i)\n",GetCurrentFilePosition(BSDFile),GetCurrentFilePosition(BSDFile) - BSD_HEADER_SIZE);
    fread(EntryTable,sizeof(BSDEntryTable_t),1,BSDFile);
    DPrintf("Node table is at %i (%i)\n",EntryTable->NodeTableOffset,EntryTable->NodeTableOffset + BSD_HEADER_SIZE);
    DPrintf("Unknown data is at %i (%i)\n",EntryTable->UnknownDataOffset,EntryTable->UnknownDataOffset + BSD_HEADER_SIZE);
    DPrintf("AnimationTableOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationTableOffset,
            EntryTable->AnimationTableOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationTableEntries);
    DPrintf("AnimationDataOffset is at %i (%i) contains %i elements.\n",EntryTable->AnimationDataOffset,
            EntryTable->AnimationDataOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationData);
    DPrintf("AnimationQuaternionDataOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationQuaternionDataOffset,
            EntryTable->AnimationQuaternionDataOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationQuaternionData);
    DPrintf("AnimationHierarchyDataOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationHierarchyDataOffset,
            EntryTable->AnimationHierarchyDataOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationHierarchyData);
    DPrintf("AnimationFaceTableOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationFaceTableOffset,
            EntryTable->AnimationFaceTableOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationFaceTables);
    DPrintf("AnimationFaceDataOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationFaceDataOffset,
            EntryTable->AnimationFaceDataOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationFaces);
    DPrintf("AnimationVertexTableIndexOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationVertexTableIndexOffset,
            EntryTable->AnimationVertexTableIndexOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationVertexTableIndex);
    DPrintf("AnimationVertexTableOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationVertexTableOffset,
            EntryTable->AnimationVertexTableOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationVertexTableEntry);
    DPrintf("AnimationVertexDataOffset is at %i (%i) and contains %i elements.\n",EntryTable->AnimationVertexDataOffset,
            EntryTable->AnimationVertexDataOffset + BSD_HEADER_SIZE,EntryTable->NumAnimationVertex);
    return true;
}

bool BSDReadAnimatedLightTableBlock(FILE *BSDFile, BSDAnimatedLightTable_t *AnimatedLightTable)
{
    BSDAnimatedLight_t *AnimatedLight;
    int PreviousFilePosition;
    int i;
    int j;
    
    if( !BSDFile ) {
        DPrintf("BSDReadAnimatedLightTableBlock: Invalid file\n");
        return false;
    }
    if( !AnimatedLightTable ) {
        DPrintf("BSDReadAnimatedLightTableBlock: Invalid data\n");
        return false;
    }
    
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_ANIMATED_LIGHTS_FILE_POSITION)) {
        fseek(BSDFile, BSDGetRealOffset(BSD_ANIMATED_LIGHTS_FILE_POSITION), SEEK_SET);
    }
    DPrintf("BSDReadAnimatedLightTableBlock:AnimatedLightsTable is at %li\n",ftell(BSDFile));
    fread(&AnimatedLightTable->NumAnimatedLights,sizeof(AnimatedLightTable->NumAnimatedLights),1,BSDFile);
    DPrintf("BSDReadAnimatedLightTableBlock:AnimatedLightsTable:Reading %i colors at %li\n",AnimatedLightTable->NumAnimatedLights,ftell(BSDFile));

    for( i = 0; i < BSD_ANIMATED_LIGHTS_TABLE_SIZE; i++ ) {
        AnimatedLight = &AnimatedLightTable->AnimatedLightsList[i];
        fread(&AnimatedLight->NumColors,sizeof(AnimatedLight->NumColors),1,BSDFile);
        fread(&AnimatedLight->StartingColorOffset,sizeof(AnimatedLight->StartingColorOffset),1,BSDFile);
        fread(&AnimatedLight->ColorIndex,sizeof(AnimatedLight->ColorIndex),1,BSDFile);
        fread(&AnimatedLight->CurrentColor,sizeof(AnimatedLight->CurrentColor),1,BSDFile);
        fread(&AnimatedLight->Delay,sizeof(AnimatedLight->Delay),1,BSDFile);
        if( AnimatedLight->NumColors == 0 ) {
            continue;
        }
        AnimatedLight->LastUpdateTime = 0;
        AnimatedLight->ColorList = malloc(AnimatedLight->NumColors * sizeof(Color1i_t));
        if( !AnimatedLight->ColorList ) {
            DPrintf("BSDReadAnimatedLightTableBlock:Failed to allocate memory for Color List\n");
            return false;
        }
        DPrintf("BSDReadAnimatedLightTableBlock: Reading Animated Light %i...\n",i);
        DPrintf("StartingColorOffset:%i\n",AnimatedLight->StartingColorOffset);
        DPrintf("StartingColorOffset No Header:%i\n",BSDGetRealOffset(AnimatedLight->StartingColorOffset));
        DPrintf("CurrentColor:%i\n",AnimatedLight->CurrentColor);
        DPrintf("ColorIndex:%i\n",AnimatedLight->ColorIndex);
        DPrintf("Delay:%i\n",AnimatedLight->Delay);
        PreviousFilePosition = GetCurrentFilePosition(BSDFile);
        fseek(BSDFile,BSDGetRealOffset(AnimatedLight->StartingColorOffset),SEEK_SET);
        DPrintf("Reading color at %i\n",GetCurrentFilePosition(BSDFile));
        for( j = 0; j < AnimatedLight->NumColors; j++ ) {
            fread(&AnimatedLight->ColorList[j],sizeof(AnimatedLight->ColorList[j]),1,BSDFile);
            DPrintf("Color %i %i %i %i %i (As Int %u)\n",j,AnimatedLight->ColorList[j].rgba[0],
                    AnimatedLight->ColorList[j].rgba[1],AnimatedLight->ColorList[j].rgba[2],AnimatedLight->ColorList[j].rgba[3],
                    AnimatedLight->ColorList[j].c
            );
        }
        fseek(BSDFile,PreviousFilePosition,SEEK_SET);
    }
    return true;
}

bool BSDReadSceneInfoBlock(FILE *BSDFile, BSDSceneInfo_t *SceneInfo)
{
    if( !BSDFile ) {
        DPrintf("BSDReadSceneInfoBlock: Invalid file\n");
        return false;
    }
    if( !SceneInfo ) {
        DPrintf("BSDReadSceneInfoBlock: Invalid data\n");
        return false;
    }
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_SCENE_INFO_BLOCK_POSITION)) {
        fseek(BSDFile, BSDGetRealOffset(BSD_SCENE_INFO_BLOCK_POSITION), SEEK_SET);
    }
    fread(SceneInfo,sizeof(BSDSceneInfo_t),1,BSDFile);
    DPrintf("BSDReadSceneInfoBlock:Reading scene info...\n");
    DPrintf("Fog Near: %i\n",SceneInfo->FogNear);
    DPrintf("Clear Color: %i;%i;%i\n",SceneInfo->ClearColor.r,SceneInfo->ClearColor.g,SceneInfo->ClearColor.b);
    return true;
}

bool BSDReadTSPInfoBlock(FILE *BSDFile, BSDTSPInfo_t *TSPInfo)
{
    
    if( !BSDFile ) {
        DPrintf("BSDReadTSPInfoBlock: Invalid file\n");
        return false;
    }
    if( !TSPInfo ) {
        DPrintf("BSDReadTSPInfoBlock: Invalid data\n");
        return false;
    }
    if(GetCurrentFilePosition(BSDFile) != BSDGetRealOffset(BSD_TSP_INFO_BLOCK_POSITION) ) {
        fseek(BSDFile, BSDGetRealOffset(BSD_TSP_INFO_BLOCK_POSITION), SEEK_SET);
    }
    fread(TSPInfo,sizeof(BSDTSPInfo_t),1,BSDFile);
    DPrintf("BSDReadTSPInfoBlock:Reading TSP info...\n");
    DPrintf("Compartment pattern: %s\n",TSPInfo->TSPPattern);
    DPrintf("Number of compartments: %i\n",TSPInfo->NumTSP);
    DPrintf("TargetInitialCompartment: %i\n",TSPInfo->TargetInitialCompartment);
    DPrintf("Starting Compartment: %i\n",TSPInfo->StartingComparment);
    DPrintf("u3: %i\n",TSPInfo->u3);
    DPrintf("TSP Block ends at %i\n",GetCurrentFilePosition(BSDFile));
    assert(TSPInfo->u3 == 0);
    return true;
}
