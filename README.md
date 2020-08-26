* [Medal Of Honor PSX File Viewer](#medal-of-honor-psx-file-viewer)
* [Introduction](#introduction)
* [Common Formats](#common-formats)
   * [TSB](#tsb)
* [TSP Files](#tsp-files)
   * [BSP Nodes](#bsp-nodes)
   * [Vertex:](#vertex)
   * [Color:](#color)
   * [Faces:](#faces)
* [BSD Files](#bsd-files)
   * [File Format](#file-format)
   * [Node Table](#node-table)
   * [Node](#node)
   * [Build](#build)
   * [Usage](#usage)
* [RSC Files](#rsc-files)
   * [File Format](#file-format-1)
   * [Usage](#usage-1)
* [TAF Files](#taf-files)
* [TIM Files](#tim-files)
   * [File Format](#file-format-2)
   * [Usage](#usage-2)



## Introduction
This project contains a set of tools that can be used to view Medal Of Honor for PSX Level files and images.
It was tested under Linux but it should also run on any other platform since it uses SDL2.
At the moment I've been able to extract and render all the images and level data except for Enemies and Animation.
Game Files are not available into this repository and you need a copy of the game in order to use it.

## Common Formats
### TSB
It contains information about the texture.
It is a 16 bit number that has the following information:
Assuming we have 12299 as TSB number it can be seen in binary as:
0011000000001011
Starting from the left to right:
First 7 bits can be discarded leaving:
>000001011

First 2 bits represents the TPF or Color Mode (00 4 bit,01 8 bit,10 15 bit):
>00

Next 2 bits represents the Semi-Transparency rate.
> 00

Finally last 5 bits represent the VRam page number (11 in this specific case).
> 01011




## TSP Files
All TSP files starts with an header which contains the following data:

| Type | Size | Description |
| ---- | ---- | ----------- |
| short | 2 bytes  | ID |
| short  | 2 bytes  | Version |
| int  | 4 bytes  | Number of BSP Nodes|
| int  | 4 bytes  | BSP Nodes Data Offset |
| int  | 4 bytes  | Number of Faces|
| int  | 4 bytes  | Faces Data Offset |
| int  | 4 bytes  | Number of Vertices|
| int  | 4 bytes  | Vertices Offset |
| int  | 4 bytes  | Number of Unknown Data |
| int  | 4 bytes  | Unknown Data Offset |
| int  | 4 bytes  | Number of Colors|
| int  | 4 bytes  | Colors Data Offset |
| int  | 4 bytes  | Number of Unknown Data |
| int  | 4 bytes  | Unknown Data Offset |
| int  | 4 bytes  | Number of Unknown Data |
| int  | 4 bytes  | Unknown Data Offset |
| int  | 4 bytes  | Collision Data Offset |

**Note that all the offset starts from the beginning of the file.**
Thanks to this format we can read each chunk separetely by moving the file position to the wanted offset.
### BSP Nodes
The game uses a BSP tree probably for collision detection.
Each BSP node contains the following data:
#### Vector3:

| Type | Size | Description |
| ---- | ---- | ---- |
| short | 2 bytes | x coordinate  |
| short  | 2 bytes | y coordinate |
| short  | 2 bytes | z coordinate |

#### Bounding Box:

| Type | Size | Description |
| ---- | ---- | ----------- |
| [Vector3](#Vector3) | 6 bytes  | Min |
| [Vector3](#Vector3) | 6 bytes  | Max |

#### BSP Node:

| Type | Size | Description |
| ---- | ---- | ----------- |
| [BBox](#Bounding-Box) | 12 bytes  | Bounding Box |
| int  | 4 bytes  | Number of Faces |
| int  | 4 bytes  | Unknown |
| int  | 4 bytes  | Unknown |
| int  | 4 bytes  | Unknown |
| int  | 4 bytes  | Child1 Offset |
| int  | 4 bytes  | Child2 Offset |

If the Number of faces is zero then this node has child and they can be found by using the two offset.
Each child can have an offset equals to -1 in which case it means that it is **NULL**.

**Note that the child offset starts from the node declaration as seen in the header.**
E.G: BSD Node offset is 64, Child1 Offset is 24 then the child node will be at **64+24**.

### Vertex:

| Type | Size | Description |
| ---- | ---- | ----------- |
| [Vector3](#Vector3) | 6 byte  | Vertex coordinate |
| short | 2 byte  | Pad |

### Color:
This is used by each face in order to simulate lights.
| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned byte | 1 byte  | Red |
| unsigned byte | 1 byte  | Green |
| unsigned byte | 1 byte  | Blue |
| unsigned byte | 1 byte  | Pad |

### Faces:
##### UV Coordinates(UV):
Used for texture coordinates.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned char | 1 byte  | u coordinate |
| unsigned char | 1 byte  | v coordinate |

##### Face Data:

Each face is made by 3 vertices that forms a triangle.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned short | 2 byte  | V0 First vertex and color index in array |
| unsigned short | 2 byte  | V1 Second vertex and color index in array |
| unsigned short | 2 byte  | V2 Third vertex and color index in array |
| [UV](#UV-Coordinates) | 2 byte  | UV0 Texture coordinate for vertex 0  |
| short | 2 byte  | Unknown  |
| [UV](#UV-Coordinates) | 2 byte  | UV1 Texture coordinate for vertex 1  |
| short | 2 byte  | TSB that contains info about the used texture ( read [TSB](#TSB) for more information)|
| [UV](#UV-Coordinates) | 2 byte  | UV2 Texture coordinate for vertex 2  |




## BSD Files
Each BSD files can be seen as a container for multiple file types (Levels,Weapons,Players).
There are some minor differences between Single Player and MultiPlayer BSD file and the MOHLevelViewer contains two .c files that parses the two version.

### File Format
Every BSD file starts with a 2048 bytes header that can be ignored and probably contains a list of CD sectors that were required by the PSX in order to correctly read the file.
**Note that all the offset contained inside the file starts from 2048.**


Right after the header the information about the corresponding TSP file is found if it is a level file otherwise it's replaced by zeroes.

#### TSP Info Block

| Type | Size | Description |
| ---- | ---- | ----------- |
| char | 128 bytes  | TSP File Name |
| int  | 4 bytes  | Total Number of TSP Files |
| int  | 4 bytes  | Number of TSP Files that needs to be rendered at start |
| int  | 4 bytes  | Number of the first TSP File that needs to be rendered |

The other TSP are loaded in real time when hitting specific triggers contained into the level that unloads the previous one that were loaded in memory.

#### Entry Table Block
This block is found at position 1340 (excluding the header) or 3388 (including the header) contains information about the position of some elements that are contained inside the file along with the number of elements and has a fixed size of 104.

| Type | Size | Description |
| ---- | ---- | ----------- |
| int  | 4 bytes  | Node offset |
| int  | 4 bytes  | Unknown Offset0 |
| int  | 4 bytes  | Unknown Offset1 |
| int  | 4 bytes  | Number of elements at Offset1 |
| int  | 4 bytes  | Unknown Offset2 |
| int  | 4 bytes  | Number of elements at Offset2 |
| int  | 4 bytes  | Unknown Offset3 |
| int  | 4 bytes  | Number of elements at Offset3 |
| int  | 4 bytes  | Unknown Offset4 |
| int  | 4 bytes  | Number of elements at Offset4 |
| int  | 4 bytes  | Unknown Offset5 |
| int  | 4 bytes  | Number of elements at Offset5 |
| int  | 4 bytes  | Unknown Offset6 |
| int  | 4 bytes  | Number of elements at Offset6 |
| int  | 4 bytes  | Unknown Offset7 |
| int  | 4 bytes  | Number of elements at Offset7 |
| int  | 4 bytes  | Unknown Offset8 |
| int  | 4 bytes  | Number of elements at Offset8 |
| int  | 4 bytes  | Unknown Offset9 |
| int  | 4 bytes  | Number of elements at Offset9 |
| char | 24 bytes | More unknown offsets |

#### RenderObject Block
After the entry block we find the number of RenderObject stored as an int (4 bytes).
A RenderObject, as the name implies , are all the objects that can be seen inside the level like Windows,Doors,Enemies,Weapons,Boxes,MG42s, etc...
Each RenderObject has a fixed size of 256 bytes containing several fields that depending by the type of the RenderObject can be NULL or contains an offset to the data stored inside the BSD file.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned int  | 4 bytes  | ID |
| int  | 4 bytes  | Unknown Offset (Valid if != 0)|
| int  | 4 bytes  | Animation Data Offset (Valid if != -1) |
| int  | 4 bytes  | Unknown Offset (Valid if != 0)|
| char  | 20 bytes  | Unknown Data |
| int  | 4 bytes  | Unknown Offset (Valid if != 0)|
| char  | 4 bytes  | Unknown Data |
| int  | 4 bytes  | Face Data Offset |
| int  | 4 bytes  | Unknown Data Offset |
| int  | 4 bytes  | Unknown Data Offset |
| int  | 4 bytes  | Face Table Offset (Valid if != -1) (Used for animated objects)  |
| char  | 72 bytes  | Unknown Data |
| int  | 4 bytes  | Vertex Table Offset (Valid if != -1) (Used for animated objects)  |
| int  | 4 bytes  | Vertex Data Offset |
| unsigned short | 2 bytes | Number of Vertex Data |
| char  | 2 bytes  | Unknown Data |
| int  | 4 bytes  | Unknown Data Offset |
| char  | 8 bytes  | Unknown Data |
| int  | 4 bytes  | Root Bone Offset (Valid if != -1) (Used for animated objects) |
| char  | 20 bytes  | Unknown Data |
| int  | 4 bytes  | Unknown Data Offset |
| char  | 8 bytes  | Unknown Data |
| int  | 4 bytes  | Unknown Data Offset |
| int  | 4 bytes  | Unknown Data Offset (Probably an offset to a Matrix stored in the file that represent the model rotation) |
| char  | 52 bytes  | Unknown Data |
| int  | 4 bytes  | Type |

By trial and error I've discovered the following RenderObject Types:

| Type  | Description |
| ---- | ----------- |
| 0 | All the objects that can be carried such as Combat Helmet,Grenades etc... |
| 5122 | Enemies |
| 6000 | All the object that can be picked up such as Field Surgeon Kit, Medical Pack,Canteen and also Barrels,Boxes etc... |
| 6001 | AirPlane as seen at the start of Mission 1 Level 1 |
| 6002 | MG42 |
| 6006 | Doors |
| 5125 | Unknown|
| 6007 | Destructible Windows |
| 6005 | Unknown |
| 6008 | Explosive Charges |

### Node Table
This section of the BSD files contains the list of all the nodes along with their offset contained inside the level.
The position can be found thanks to the [Entry Table](#entry-table-block) and contains the following data:

#### Node Table Data

| Type | Size | Description |
| ---- | ---- | ----------- |
| int  | 4 bytes  | Number of Nodes |
| int  | 4 bytes | Table Size |
| char | 8 bytes | Unknown |

After this header we find the table entry containing the position for all the nodes inside the BSD file:

#### Node Table Entry

| Type | Size | Description |
| ---- | ---- | ----------- |
| int  | 4 bytes  | Unknown |
| int  | 4 bytes | Node Offset |

**Note that the Node offset refers to the position after the table entry list.**

### Node
After having loaded the table and all the table entries, we find the actual node data (First node position should be the same as the first offset inside the node table entry list).
Each Node represents either a phisical object (referencing a RenderObject ID) or logical such as spawn point which are not rendered.
Each node has the following structure:
unsigned int Id;
int Size;
int u2;
int Type;
BSDPosition_t Position;
BSDPosition_t Rotation;
#### Node Position

| Type | Size | Description |
| ---- | ---- | ----------- |
| short  | 2 bytes  | x position |
| short  | 2 bytes | y position |
| short  | 2 bytes | z position |
| short  | 2 bytes | Pad |

#### Node Data
| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned int  | 4 bytes  | ID |
| int  | 4 bytes | Size |
| int  | 4 bytes | Unknown |
| int  | 4 bytes | Type |
| [NodePosition](#node-position)  | 4 bytes | Position |
| [NodePosition](#node-position)  | 4 bytes | Rotation |

**Note that Rotation is stored in fixed math format where 4096 is 360 degrees**
### Build
> cd into the directory and type make
### Usage
> ./MOHLevelViewer <MOH Game Directory> MissionNumber LevelNumber
## RSC Files
### File Format
RSC are simple not compressed archive files that contains different files type.
Each RSC files starts with an header containing the following data:

##### RSC Header
| Type | Size | Description |
| ---- | ---- | ----------- |
| char  | 64 bytes  | Directory Name |
| long long  | 8 bytes  | Number of Entry |

##### RSC Entry

| Type | Size | Description |
| ---- | ---- | ----------- |
| char  | 68 bytes  | File Name |
| int  | 4 bytes  | File Length |
| long long | 8 bytes | Offset |

### Usage
#### Build
Compile:
  > gcc -o RSCUtils RSCUtils.c
#### Run
  > ./RSCUtils `<File.rsc>`

This command will extract the content of <File.rsc> creating all the required directories as declared in the RSC file.
## TAF Files
TAF files contains images and sounds for each level.
Inside each level folder there are two TAF files which have the following format:
>MissionNumber_LevelNumber0.TAF
>MissionNumber_LevelNumber1.TAF

Only one is used at any time and represents the used language:
0 for Deutsche(Default) while 1 is American(Can be activated in the Password menu).

File has not an header but it is just a collection of tim files and vab files.
## TIM Files
TIM is a file format used for storing all the images in the game.
### File Format

##### TIM Header:

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned int  | 4 bytes  | Magic (Always 0x10) |
| unsigned int  | 4 bytes  | BPP |
| unsigned int  | 4 bytes |  CLUTSize |
| unsigned short  | 2 bytes |  CLUTX |
| unsigned short  | 2 bytes |  CLUTY |
| unsigned short  | 2 bytes |  Number of CLUT Colors |
| unsigned short  | 2 bytes |  Number of CLUTs |

##### TIM CLUT Color:

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned char  | 1 bytes  | R (Red Component) |
| unsigned char  | 1 bytes  | G (Green Component) |
| unsigned char  | 1 bytes  | B (Blue Component) |
| unsigned char  | 1 bytes  | STP (Used for transparency) |


##### TIM Content:

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned int  | 4 bytes  | NumPixels |
| unsigned short  | 2 bytes  | RowCount |
| unsigned short  | 2 bytes |  Width |
| unsigned short  | 2 bytes |  Height |
| unsigned short  | 2 bytes |  FrameBufferX |
| unsigned short  | 2 bytes |  FrameBufferY |
| unsigned char   | 4 bytes |  CLUTColor |
| unsigned short  | Pointer | Data |

### Usage
#### Build
Compile:
  > gcc -o TIMWalker TimWalker.c
#### Run
  > ./TIMWalker `<File.tim>`

This command will read the content of <File.tim>, convert it to png and save it in the current folder.
If the file contains more than one TIM then it creates a folder with the same name as <File> (without the extension) that contains all the converted images that were found in the file.
