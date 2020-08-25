# Medal Of Honor PSX File Viewer
* [Common Formats](#common-formats)
   * [TSB](#tsb)
* [TSP Files](#tsp-files)
   * [BSP Nodes](#bsp-nodes)
   * [Faces:](#faces)
* [BSD Files](#bsd-files)
   * [File Format](#file-format)
   * [Build](#build)
   * [Usage](#usage)
* [RSC Files](#rsc-files)
   * [File Format](#file-format-1)
   * [Usage](#usage-1)
* [TAF Files](#taf-files)
* [TIM Files](#tim-files)
   * [File Format](#file-format-2)
   * [Usage](#usage-2)


## Common Formats
### TSB
It contains information about the texture.  
It is a 16 bit number that has the following information:
Assuming we have 12299 as TSB number it can be seen in binary as:  
0011000000001011  
Starting from the left to right:  
First 7 bits can be discarded leaving:  
000001011
00 => First 2 bits represents the TPF or Color Mode (00 4 bit,01 8 bit,10 15 bit).  
00 => Semi-Transparency rate.  
01011 => Last 5 bits represent the VRam page number (11 in this specific case).  




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

#### BBox(Bounding Box):

| Type | Size | Description |
| ---- | ---- | ----------- |
| Vector3 | 6 bytes  | Min |
| Vector3 | 6 bytes  | Max |

#### BSP Node:

| Type | Size | Description |
| ---- | ---- | ----------- |
| BBox | 12 bytes  | Bounding Box |
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
### Faces:
##### UV Coordinates(UV):
Used for texture coordinates.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned char | 1 byte  | u coordinate |
| unsigned char | 1 byte  | v coordinate |

##### Face:

Each face is made by 3 vertices that forms a triangle.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned short | 2 byte  | V0 First vertex index in array |
| unsigned short | 2 byte  | V1 Second vertex index in array |
| unsigned short | 2 byte  | V2 Third vertex index in array |
| UV | 2 byte  | UV0 Texture coordinate for vertex 0  |
| short | 2 byte  | Unknown  |
| UV | 2 byte  | UV1 Texture coordinate for vertex 1  |
| short | 2 byte  | TSB that contains info about the used texture [TSB](#TSB) for more information)|
| UV | 2 byte  | UV2 Texture coordinate for vertex 2  |


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

### Build
### Usage
## RSC Files
### File Format
RSC are simple not compressed archive files that contains different files type.
Each RSC files starts with an header containing the following data:

##### RSC Header
| Type | Size | Description |
| ---- | ---- | ----------- |
| char  | 64 bytes  | Directory Name |
| long long  | 8 bytes  | Number of Entry |

Where each entry is:

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
