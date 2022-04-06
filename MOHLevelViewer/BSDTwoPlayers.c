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

void BSD2PFree(BSD2P_t *BSD)
{
    free(BSD->NodeData.Table);
    free(BSD->NodeData.Node);
    free(BSD->RenderObjectTable.RenderObject);
    VAOFree(BSD->NodeVAO);
    free(BSD);
}

void BSD2PVAOPointList(BSD2P_t *BSD)
{
    int NumSkip;
    float *NodeData;
    int    NodeDataPointer;
    int    Stride;
    int    NodeDataSize;
    int    i;
    
    Stride = (3 + 3) * sizeof(float);
    NodeDataSize = Stride * BSD->NodeData.Header.NumNodes;
    NodeData = malloc(NodeDataSize);
    NodeDataPointer = 0;
    NumSkip = 0;
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        NodeData[NodeDataPointer] =   BSD->NodeData.Node[i].Position.x;
        NodeData[NodeDataPointer+1] = BSD->NodeData.Node[i].Position.y;
        NodeData[NodeDataPointer+2] = BSD->NodeData.Node[i].Position.z;
        NodeData[NodeDataPointer+3] = 0.18f;
        NodeData[NodeDataPointer+4] = 0.30f;
        NodeData[NodeDataPointer+5] = 0.30f;
        NodeDataPointer += 6;
    }
    BSD->NodeVAO = VAOInitXYZRGB(NodeData,NodeDataSize - (Stride * NumSkip),Stride,0,3);            
    free(NodeData);
}


void BSD2PDraw(Level_t *Level)
{
    Shader_t *Shader;
    int MVPMatrixID;
    
    Shader = ShaderCache("BSDShader","Shaders/BSDVertexShader.glsl","Shaders/BSDFragmentShader.glsl");
    glUseProgram(Shader->ProgramID);

    MVPMatrixID = glGetUniformLocation(Shader->ProgramID,"MVPMatrix");
    glUniformMatrix4fv(MVPMatrixID,1,false,&VidConf.MVPMatrix[0][0]);
    glBindVertexArray(Level->BSDTwoP->NodeVAO->VAOId[0]);
    glPointSize(10.f);
    glDrawArrays(GL_POINTS, 0, Level->BSDTwoP->NodeData.Header.NumNodes);
    glBindVertexArray(0);
    glUseProgram(0);
}
void BSD2PReadEntryTableChunk(BSD2P_t *BSD,FILE *InFile)
{
    if( !BSD || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("BSD2PReadEntryTableChunk: Invalid %s\n",InvalidFile ? "file" : "BSD2P struct");
        return;
    }
    fseek(InFile,BSD2P_ENTRY_TABLE_POSITION + sizeof(BSD_Header_t),SEEK_SET);
    //ATM we are interested at the node offset...
    fread(&BSD->NodeOffset,sizeof(BSD->NodeOffset),1,InFile);
    DPrintf("BSD2PReadEntryTableChunk:Node table is at %i (%i)\n",BSD->NodeOffset,BSD->NodeOffset + 2048);
    return;
}

void BSD2PReadRenderObjectChunk(BSD2P_t *BSD,FILE *InFile)
{
    int StartinUAt;
    int i;
    if( !BSD || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("BSD2PReadRenderObjectChunk: Invalid %s\n",InvalidFile ? "file" : "BSD2P struct");
        return;
    }
    fseek(InFile,BSD2P_ENTRY_TABLE_POSITION + sizeof(BSD_Header_t) + BSD2P_ENTRY_TABLE_SIZE,SEEK_SET);
    fread(&BSD->RenderObjectTable.NumRenderObject,sizeof(BSD->RenderObjectTable.NumRenderObject),1,InFile);
    StartinUAt = GetCurrentFilePosition(InFile);
    DPrintf("BSD2PReadRenderObjectChunk: Reading %i RenderObject Elements...\n",BSD->RenderObjectTable.NumRenderObject);
    assert(sizeof(BSDRenderObjectElement_t) == 256);
    BSD->RenderObjectTable.RenderObject = malloc(BSD->RenderObjectTable.NumRenderObject * sizeof(BSDRenderObjectElement_t));
    for( i = 0; i < BSD->RenderObjectTable.NumRenderObject; i++ ) {
        assert(GetCurrentFilePosition(InFile) == StartinUAt + (i * 256));
        DPrintf("BSD2PReadRenderObjectChunk: Reading RenderObject Element %i at %i\n",i,GetCurrentFilePosition(InFile));
        fread(&BSD->RenderObjectTable.RenderObject[i],sizeof(BSD->RenderObjectTable.RenderObject[i]),1,InFile);
        DPrintf("BSD2PReadRenderObjectChunk: RenderObject ID:%u\n",BSD->RenderObjectTable.RenderObject[i].ID);
        DPrintf("BSD2PReadRenderObjectChunk: RenderObject Type:%i | %s\n",BSD->RenderObjectTable.RenderObject[i].Type,
            BSDRenderObjectGetEnumStringFromType(BSD->RenderObjectTable.RenderObject[i].Type)
        );
        DPrintf("BSD2PReadRenderObjectChunk: RenderObject Element Vertex Offset: %i (%i)\n",
                BSD->RenderObjectTable.RenderObject[i].VertOffset,
                BSD->RenderObjectTable.RenderObject[i].VertOffset + 2048);
        DPrintf("BSD2PReadRenderObjectChunk: RenderObject Element NumVertex: %i\n",BSD->RenderObjectTable.RenderObject[i].NumVertex);
        //Those offset are relative to the EntryTable.
        DPrintf("BSD2PReadRenderObjectChunk: RenderObject Element UnknownOffset1: %i (%i)\n",
                BSD->RenderObjectTable.RenderObject[i].UnknownOffset1,
                BSD->RenderObjectTable.RenderObject[i].UnknownOffset1 + 2048);
        DPrintf("BSD2PReadRenderObjectChunk: RenderObject Element UnknownOffset2: %i (%i)\n",
                BSD->RenderObjectTable.RenderObject[i].UnknownOffset2,
                BSD->RenderObjectTable.RenderObject[i].UnknownOffset2 + 2048);
        DPrintf("BSD2PReadRenderObjectChunk: RenderObject Element RootBoneOffset: %i (%i)\n",
                BSD->RenderObjectTable.RenderObject[i].RootBoneOffset,
                BSD->RenderObjectTable.RenderObject[i].RootBoneOffset + 2048);
        DPrintf("BSD2PReadRenderObjectChunk: RenderObject Element FaceOffset: %i (%i)\n",
                BSD->RenderObjectTable.RenderObject[i].FaceOffset,
                BSD->RenderObjectTable.RenderObject[i].FaceOffset + 2048);
    }
    return;
}
void BSD2PReadNodeChunk(BSD2P_t *BSD,FILE *InFile)
{
    int j;
    int Jump;
    int NodeTableEnd;
    int NodeFilePosition;
    int Offset;
    int Delta;
    int NodeNumReferencedRenderObjectIDOffset;
    int NumReferencedRenderObjectID;
    unsigned int NodeRenderObjectID;
    int PrevPos;
    int i;
    if( !BSD || !InFile ) {
        bool InvalidFile = (InFile == NULL ? true : false);
        DPrintf("BSD2PReadNodeChunk: Invalid %s\n",InvalidFile ? "file" : "BSD2P struct");
        return;
    }
    fseek(InFile,BSD->NodeOffset + sizeof(BSD_Header_t),SEEK_SET);
    DPrintf("BSD2PReadNodeChunk: Reading at %i.\n",GetCurrentFilePosition(InFile));
    fread(&BSD->NodeData.Header,sizeof(BSD->NodeData.Header),1,InFile);
    DPrintf("BSD2PReadNodeChunk: Reading %i entries.\n",BSD->NodeData.Header.NumNodes);
    DPrintf("BSD2PReadNodeChunk: TableSize: %i\n",BSD->NodeData.Header.TableSize);
    DPrintf("BSD2PReadNodeChunk: U2: %i\n",BSD->NodeData.Header.u2);
    DPrintf("BSD2PReadNodeChunk: U3: %i\n",BSD->NodeData.Header.u3);
    DPrintf("BSD2PReadNodeChunk: U4: %i\n",BSD->NodeData.Header.u4);
    DPrintf("BSD2PReadNodeChunk: U5: %i\n",BSD->NodeData.Header.u5);
    BSD->NodeData.Table = malloc(BSD->NodeData.Header.NumNodes * sizeof(BSDNodeTableEntry_t));
    DPrintf("BSD2PReadNodeChunk: Nodetable starts at %i\n",GetCurrentFilePosition(InFile));
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        fread(&BSD->NodeData.Table[i],sizeof(BSD->NodeData.Table[i]),1,InFile);
        DPrintf("BSD2PReadNodeChunk: -- NODE %i --\n",i);
        DPrintf("BSD2PReadNodeChunk: Pointer:%i\n",BSD->NodeData.Table[i].Pointer);
        DPrintf("BSD2PReadNodeChunk: Offset:%i\n",BSD->NodeData.Table[i].Offset);
    }
    NodeTableEnd = GetCurrentFilePosition(InFile);
    DPrintf("BSD2PReadNodeChunk: Nodetable ends at %i\n",NodeTableEnd);
    //All the node offset are calculated from the 0 node...
    //So all the offset inside a node are Offset+AddressOfFirstNode.
    BSD->NodeData.Node = malloc(BSD->NodeData.Header.NumNodes * sizeof(BSDNode_t));
    for( i = 0; i < BSD->NodeData.Header.NumNodes; i++ ) {
        NodeFilePosition = GetCurrentFilePosition(InFile);
        DPrintf("BSD2PReadNodeChunk: -- NODE %i (Pos %i PosNoHeader %i)-- \n",i,NodeFilePosition,NodeFilePosition - 2048);
        assert(GetCurrentFilePosition(InFile) == (BSD->NodeData.Table[i].Offset + NodeTableEnd));
        fread(&BSD->NodeData.Node[i],sizeof(BSDNode_t),1,InFile);
        DPrintf("BSD2PReadNodeChunk: ID:%u | ID:%s\n",BSD->NodeData.Node[i].Id,BSDNodeGetEnumStringFromNodeID(BSD->NodeData.Node[i].Id));
        DPrintf("BSD2PReadNodeChunk: Size:%i\n",BSD->NodeData.Node[i].Size);
        DPrintf("BSD2PReadNodeChunk: U2:%i\n",BSD->NodeData.Node[i].u2);
        DPrintf("BSD2PReadNodeChunk: Type:%i\n",BSD->NodeData.Node[i].Type);
        DPrintf("BSD2PReadNodeChunk: Position:(%i;%i;%i) Pad %i\n",BSD->NodeData.Node[i].Position.x,BSD->NodeData.Node[i].Position.y,
                BSD->NodeData.Node[i].Position.z,BSD->NodeData.Node[i].Position.Pad);
        DPrintf("BSD2PReadNodeChunk: Rotation:(%i;%i;%i) Pad %i\n",BSD->NodeData.Node[i].Rotation.x,BSD->NodeData.Node[i].Rotation.y,
                BSD->NodeData.Node[i].Rotation.z,BSD->NodeData.Node[i].Rotation.Pad);
        assert(BSD->NodeData.Node[i].Position.Pad == 0);
        assert(BSD->NodeData.Node[i].Rotation.Pad == 0);
                
        PrevPos = GetCurrentFilePosition(InFile);
        int MessageRegIndex;
        fseek(InFile,PrevPos + 52,SEEK_SET);
        fread(&MessageRegIndex,sizeof(MessageRegIndex),1,InFile);
        DPrintf("BSD2PReadNodeChunk: Node has message reg index %i\n",MessageRegIndex);
        fseek(InFile,PrevPos,SEEK_SET);
        
        if( BSD->NodeData.Node[i].Type == 2 || BSD->NodeData.Node[i].Type == 4 || BSD->NodeData.Node[i].Type == 6 ) {
            //0x74
            Offset = 96;
        } else if ( BSD->NodeData.Node[i].Type == 3 ) {
            //0x60
            Offset = 116;
        } else if( BSD->NodeData.Node[i].Type == 0 && BSD->NodeData.Node[i].Id == BSD_TSP_LOAD_TRIGGER ) {
            Offset = 48;
        } else {
            Offset = 0;
        }
        if( Offset != 0 ) {
            PrevPos = GetCurrentFilePosition(InFile);
            Delta = Offset - sizeof(BSDNode_t);
            DPrintf("BSD2PReadNodeChunk: Node type %i has offset at %i.\n",BSD->NodeData.Node[i].Type,Offset);
            fseek(InFile,GetCurrentFilePosition(InFile) + Delta,SEEK_SET);
            if( BSD->NodeData.Node[i].Id == BSD_TSP_LOAD_TRIGGER ) {
                DPrintf("BSD2PReadNodeChunk: Node is a BSD_TSP_LOAD_TRIGGER.\n");
            } else {
                fread(&NodeNumReferencedRenderObjectIDOffset,sizeof(NodeNumReferencedRenderObjectIDOffset),1,InFile);
                DPrintf("BSD2PReadNodeChunk: Node has RenderObject offset %i.\n",NodeNumReferencedRenderObjectIDOffset);
                if( NodeNumReferencedRenderObjectIDOffset != 0 ) {
                    if( BSD->NodeData.Node[i].Type == 4 ) {
                        DPrintf("BSD2PReadNodeChunk: Node has Type 4 so the RenderObject is %u.\n",NodeNumReferencedRenderObjectIDOffset);
//                         BSDAddNodeToUObjectList(BSD,MissionNumber,NodeNumReferencedRenderObjectIDOffset,NodePosition);
                    } else {
                        fseek(InFile,NodeFilePosition + NodeNumReferencedRenderObjectIDOffset,SEEK_SET);
                        fread(&NumReferencedRenderObjectID,sizeof(NumReferencedRenderObjectID),1,InFile);
                        DPrintf("BSD2PReadNodeChunk: Node is referencing %i RenderObjects.\n",NumReferencedRenderObjectID);
                        for( j = 0; j < NumReferencedRenderObjectID; j++ ) {
                            fread(&NodeRenderObjectID,sizeof(NodeRenderObjectID),1,InFile);
                            if( BSD->NodeData.Node[i].Id == BSD_ENEMY_SPAWN && NodeRenderObjectID != 3817496448 && j == 0 ) {
                                DPrintf("BSD2PReadNodeChunk: We have a different RenderObject for this enemy spawn...\n");
                            }
//                             BSDAddNodeToUObjectList(BSD,MissionNumber,NodeUBlockID,NodePosition);
                        }
                    }
                }
            }
            fseek(InFile,PrevPos,SEEK_SET);
        } else {
            DPrintf("Zero Offset.\n");
        }
        Jump = BSD->NodeData.Node[i].Size - sizeof(BSDNode_t);
        DPrintf("Skipping %i bytes of data from %i.\n",Jump,GetCurrentFilePosition(InFile));
        SkipFileSection(Jump,InFile);
    }
    DPrintf("NodeList ends at %i\n",GetCurrentFilePosition(InFile));
}
BSD2P_t *BSD2PLoad(char *FName,int MissionNumber)
{
    FILE *BSDFile;
    BSD2P_t *BSD;
    int i;
    
    DPrintf("BSD2PLoad:Loading file %s...\n",FName);
    
    BSDFile = fopen(FName,"rb");
    
    if( BSDFile == NULL ) {
        DPrintf("BSD2PLoad:Failed opening BSD File %s.\n",FName);
        return NULL;
    }
    BSD = malloc(sizeof(BSD2P_t));
    BSD->NodeVAO = NULL;
    assert(sizeof(BSD->Header) == 2048);
    fread(&BSD->Header,sizeof(BSD->Header),1,BSDFile);
    DPrintf("BSD2PLoad:Header contains %i(%#02x) element.\n",BSD->Header.NumHeadElements,BSD->Header.NumHeadElements);
    for( i = 0; i < BSD->Header.NumHeadElements; i++ ) {
        printf("BSD2PLoad:Got %i(%#02x)(%i)\n",BSD->Header.Sector[i],BSD->Header.Sector[i],BSD->Header.Sector[i] >> 0xb);
    }
    
    BSD2PReadEntryTableChunk(BSD,BSDFile);
    BSD2PReadRenderObjectChunk(BSD,BSDFile);
    BSD2PReadNodeChunk(BSD,BSDFile);
    fclose(BSDFile);
    return BSD;
}

#ifdef __STANDALONE
int main(int argc,char **argv) {
    FILE *BSDFile;
    BSD_t BSD;
    BSDTableElement_t *Element;
    int Jump;
    int NodeTableEnd;
    int i;

    if( argc == 1 ) {
        printf("%s <Input.bsd> will extract the content of Input.bsd in the current folder.\n",argv[0]);
        return -1;
    }
    BSDParseFile(argv[1]);
    return 0;
}
#endif
