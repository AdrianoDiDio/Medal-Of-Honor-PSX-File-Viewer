# Medal Of Honor PSX File Viewer

* [Medal Of Honor PSX File Viewer](#medal-of-honor-psx-file-viewer)
   * [Introduction](#introduction)
   * [Common Formats](#common-formats)
      * [TSB](#tsb)
   * [TSP Files](#tsp-files)
      * [BSP Nodes](#bsp-nodes)
         * [Vector3](#vector3)
         * [Bounding Box](#bounding-box)
         * [BSP Node](#bsp-node)
      * [Vertex](#vertex)
      * [Color](#color)
      * [Faces](#faces)
         * [UV Coordinates(UV)](#uv-coordinatesuv)
         * [Face Data](#face-data)
      * [Dynamic Data](#dynamic-data)
         * [Dynamic Face Data](#dynamic-face-data)
      * [Collision Data](#collision-data)
         * [KDTree Nodes](#kdtree-nodes)
         * [Face Index Array](#face-index-array)
         * [Collision Vertices](#collision-vertices)
         * [Collision Normals](#collision-normals)
         * [Collision Faces](#collision-faces)
   * [BSD Files](#bsd-files)
      * [File Format](#file-format)
         * [TSP Info Block](#tsp-info-block)
         * [Entry Table Block](#entry-table-block)
         * [RenderObject Block](#renderobject-block)
            * [Color Mode](#color-mode)
            * [Texture Page](#texture-page)
            * [Vertex Data](#vertex-data)
               * [Vertex 0](#vertex-0)
               * [Vertex 1](#vertex-1)
               * [Vertex 2](#vertex-2)
         * [Node Table](#node-table)
            * [Node Table Data](#node-table-data)
            * [Node Table Entry](#node-table-entry)
         * [Node](#node)
            * [Node Position](#node-position)
            * [Node Data](#node-data)
               * [Node Type](#node-type)
         * [Property Set File](#property-set-file)
      * [Build](#build)
      * [Usage](#usage)
         * [Controls](#controls)
   * [RSC Files](#rsc-files)
      * [File Format](#file-format-1)
            * [RSC Header](#rsc-header)
            * [RSC Entry](#rsc-entry)
      * [Usage](#usage-1)
         * [Build](#build-1)
         * [Run](#run)
   * [TAF Files](#taf-files)
   * [TIM Files](#tim-files)
      * [File Format](#file-format-2)
            * [TIM Header](#tim-header)
            * [TIM CLUT Color](#tim-clut-color)
            * [TIM Content](#tim-content)
      * [Usage](#usage-2)
         * [Build](#build-2)
         * [Run](#run-1)
   * [VAB Extractor](#vab-extractor)
      * [Usage](#usage-3)
         * [Build](#build-3)
         * [Run](#run-2)



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
| int  | 4 bytes  | Dynamic Data Number |
| int  | 4 bytes  | Dynamic Data Offset |
| int  | 4 bytes  | Collision Data Offset |

**Note that all the offset starts from the beginning of the file.**
Thanks to this format we can read each chunk separetely by moving the file position to the wanted offset.
### BSP Nodes
The game uses a BSP tree for rendering all the level data.
Each BSP node contains the following data:
#### Vector3

| Type | Size | Description |
| ---- | ---- | ---- |
| short | 2 bytes | x coordinate  |
| short  | 2 bytes | y coordinate |
| short  | 2 bytes | z coordinate |

#### Bounding Box

| Type | Size | Description |
| ---- | ---- | ----------- |
| [Vector3](#Vector3) | 6 bytes  | Min |
| [Vector3](#Vector3) | 6 bytes  | Max |

#### BSP Node

| Type | Size | Description |
| ---- | ---- | ----------- |
| [BBox](#Bounding-Box) | 12 bytes  | Bounding Box |
| int  | 4 bytes  | Number of Faces |
| int  | 4 bytes  | Unknown |
| int  | 4 bytes  | Unknown |
| int  | 4 bytes  | Offset |
| int  | 4 bytes  | Child1 Offset |
| int  | 4 bytes  | Child2 Offset |

If the Number of faces is zero then this node could have a child and they can be found by using the two offset.
Each child can have an offset equals to -1 in which case it means that it is **NULL**.
Offset Field has two purposes:
If NumFaces != 0 then Offset represents the starting position where to load the face array that goes from
> [Offset;Offset + (NumFaces * sizeof(Face))]

Otherwise it represents the next node offset in the array that needs to be loaded.


**Note that the child and next node offset starts from the node declaration as seen in the header.**
E.G: TSP Node offset is 64, Child1 Offset is 24 then the child node will be at **64+24**.

### Vertex

| Type | Size | Description |
| ---- | ---- | ----------- |
| [Vector3](#Vector3) | 6 bytes  | Vertex coordinate |
| short | 2 bytes  | Pad |

### Color
This is used by each face in order to simulate lights.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned byte | 1 byte  | Red |
| unsigned byte | 1 byte  | Green |
| unsigned byte | 1 byte  | Blue |
| unsigned byte | 1 byte  | Pad |

### Faces

#### UV Coordinates(UV)
Used for texture coordinates.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned char | 1 byte  | u coordinate |
| unsigned char | 1 byte  | v coordinate |

#### Face Data

Each face is made by 3 vertices that forms a triangle.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned short | 2 bytes  | V0 First vertex and color index in array |
| unsigned short | 2 bytes  | V1 Second vertex and color index in array |
| unsigned short | 2 bytes  | V2 Third vertex and color index in array |
| [UV](#uv-coordinatesuv) | 2 byte  | UV0 Texture coordinate for vertex 0  |
| short | 2 bytes  | Unknown  |
| [UV](#uv-coordinatesuv) | 2 bytes  | UV1 Texture coordinate for vertex 1  |
| short | 2 bytes  | TSB that contains info about the used texture ( read [TSB](#TSB) for more information)|
| [UV](#uv-coordinatesuv) | 2 bytes  | UV2 Texture coordinate for vertex 2  |

### Dynamic Data

Dynamic data is used to swap textures from existing face in order to simulate different effects like when a window is hit by a bullet and explodes...
Every TSP can contain any number of dynamic blocks (even 0 if not used)
and each block has the following data:

Every block starts with an header that contains the following fields:

| Type | Size | Description |
| ---- | ---- | ----------- |
| int | 4 bytes  | Dynamic Block Size |
| int | 4 bytes  | Unknown |
| int | 4 bytes  | Unknown |
| int | 4 bytes  | Dynamic Data Index  |
| short | 2 bytes  | Face Data Multiplier  |
| short | 2 bytes  | Number of Faces Index  |
| short | 2 bytes  | Face Index Offset  |
| short | 2 bytes  | Face Data Offset  |
| short | n bytes  | Array of Faces Index |

**Note that Face Index is an index to the TSP face index**
Face data multiplier is used to indicate that we need to load at FaceDataOffset n faces where n=NumberofFacesIndex * FaceDataMultiplier.

#### Dynamic Face Data
After the index list we have the actual face data that can be used to
swap the original texture from the TSP face and contains the following data:

| Type | Size | Description |
| ---- | ---- | ----------- |
| [UV](#uv-coordinatesuv) | 2 bytes  | UV0 |
| short | 2 bytes  | CBA (Clut Data)  |
| [UV](#uv-coordinatesuv) | 2 bytes  | UV1 |
| short | 2 bytes  | TSB (Texture Data) |
| [UV](#uv-coordinatesuv) | 2 bytes  | UV2 |

### Collision Data

Collision data is found after the face block and has the following header:

| Type | Size | Description |
| ---- | ---- | ----------- |
| short | 2 bytes  | World Bound Min X |
| short | 2 bytes  | World Bound Min Z |
| short | 2 bytes  | World Bound Max X |
| short | 2 bytes  | World Bound Max Z  |
| unsigned short | 2 bytes  | Number of KDTree Nodes  |
| unsigned short | 2 bytes  | Number of Face Index array element  |
| unsigned short | 2 bytes  | Number of Vertices  |
| unsigned short | 2 bytes  | Number of Normals  |
| unsigned short | 2 bytes  | Number of Faces  |

WorldBoundMinX/Z are used to iterate over the KDTree.

#### KDTree Nodes

Each Node has the following data:

| Type | Size | Description |
| ---- | ---- | ----------- |
| short | 2 bytes  | Child0 |
| short | 2 bytes  | Child1 |
| short | 2 bytes  | Middle Split Value |
| short | 2 bytes  | Index to the [Property Set File](#property-set-file) as found in the BSD  |

If Child0 is less than 0 then the current node is a leaf and contains
(-Child0 - 1) faces starting from the index Child1 that is mapped to the Face Index array.

If the node is not a leaf one then Child0 and Child1 are used to iterate over the KDTree
Child1 has two function It represents the next node in the KDTree and also the split axis:
If Child1 < 0 => Z-Axis and the next node could be either Child0 or (-Child1 - 1)
Else X-Axis => Next node could be either Child0 or Child1.


#### Face Index Array

Face Index Array is a list of shorts that maps from the KDTree index to the collision face array.

#### Collision Vertices

Collision Vertex is a list that contains all the vertices used by the collision faces.

| Type | Size | Description |
| ---- | ---- | ----------- |
| [Vector3](#Vector3) | 6 bytes  | Vertex coordinate |
| short | 2 bytes  | Pad |

#### Collision Normals

Collision Normal is a list that contains all the normals used by the collision faces.

| Type | Size | Description |
| ---- | ---- | ----------- |
| [Vector3](#Vector3) | 6 bytes  | Normal |
| short | 2 bytes  | Pad |

#### Collision Faces

Every collision face has the following structure:

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned short | 2 bytes  | Vertex0 |
| unsigned short | 2 bytes  | Vertex1 |
| unsigned short | 2 bytes  | Vertex2 |
| unsigned short | 2 bytes  | Normal Index  |
| short | 2 bytes | Plane Distance |


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
| char | 12 bytes | More unknown offsets |
| int  | 4 bytes | Property Set File |
| char  | 8 bytes | More unknown offsets  |


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
| int  | 4 bytes  | Unknown Data |
| int  | 4 bytes  | Scale X |
| int  | 4 bytes  | Scale Y |
| int  | 4 bytes  | Scale Z |
| char  | 4 bytes  | Unknown Data |
| int  | 4 bytes  | Unknown Data Offset |
| char  | 8 bytes  | Unknown Data |
| int  | 4 bytes  | Unknown Data Offset |
| int  | 4 bytes  | Unknown Data Offset (Probably an offset to a Matrix stored in the file that represent the model rotation) |
| char  | 52 bytes  | Unknown Data |
| int  | 4 bytes  | Type |

**NOTE that ScaleX/Y/Z Values must be divided by 4 and the value found is in fixed point format where 4096 is equal to 1.**

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
| 6005 | Valve |
| 6008 | Explosive Charges |
| 6009 | Radio |
| 6004 | V2 Rocket |

There are 13 available weapons that can be used in game and are identified using specific IDs:

| ID  | Weapon Name |
| ---- | ----------- |
| 1878462241 | Pistol Type 1 |
| 1631105660 | SubMachineGun Type 1 |
| 509069799  | Bazooka |
| 424281247  | American Grenade |
| 2634331343 | Shotgun |
| 4284575011 | Sniper Rifle |
| 2621329551 | SubMachineGun Type 2 |
| 3147228851 | Document Papers (Used in stealth missions) |
| 860498661  | Pistol Type 2 |
| 1609048829 | Pistol Type 3 |
| 3097846808 | German Grenade |
| 2691923848 | SubMachineGun Type 3 |
| 1326598003 | M1 Garand \(Rifle\) |

If the Face Data Offset is not zero then the RenderObject can be rendered using the following face structure:

| Type | Size | Description |
| ---- | ---- | ----------- |
| [UV](#uv-coordinatesuv)  | 2 bytes  | UV Coordinates of Vertex 0 |
| short  | 2 bytes  | [TSB](#TSB) Info |
| [UV](#uv-coordinatesuv)  | 2 bytes  | UV Coordinates of Vertex 1 |
| short  | 2 bytes  | Texture Info |
| [UV](#uv-coordinatesuv)  | 2 bytes  | UV Coordinates of Vertex 2 |
| unsigned int  | 4 bytes  | Vertex Data |

Texture info contains all the information about the used texture for the current face and can be extracted in this way:
##### Color Mode
> (TexInfo & 0xC0) >> 7

For the result value read about [TSB](#TSB) since it uses the same format.
##### Texture Page
> TexInfo & 0x3f

For the result value read about [TSB](#TSB) since it uses the same format.

##### Vertex Data
Vertex Data contains the information about the indices used to create the triangle that represent the current face and It has the following format:

###### Vertex 0
> VertexData & 0xFF

###### Vertex 1
> (VertexData & 0x3fc00) >> 10

###### Vertex 2
> (VertexData & 0xFF00000) >> 20

#### Node Table
This section of the BSD files contains the list of all the nodes along with their offset contained inside the level.
The position can be found thanks to the [Entry Table](#entry-table-block) and contains the following data:

##### Node Table Data

| Type | Size | Description |
| ---- | ---- | ----------- |
| int  | 4 bytes  | Number of Nodes |
| int  | 4 bytes | Table Size |
| char | 8 bytes | Unknown |

After this header we find the table entry containing the position for all the nodes inside the BSD file:

##### Node Table Entry

| Type | Size | Description |
| ---- | ---- | ----------- |
| int  | 4 bytes  | Unknown |
| int  | 4 bytes | Node Offset |

**Note that the Node offset refers to the position after the table entry list.**

#### Node
After having loaded the table and all the table entries, we find the actual node data (First node position should be the same as the first offset inside the node table entry list).
Each Node represents either a phisical object (referencing a RenderObject ID) or logical such as spawn point which are not rendered.
Each node has the following structure:
##### Node Position

| Type | Size | Description |
| ---- | ---- | ----------- |
| short  | 2 bytes  | x position |
| short  | 2 bytes | y position |
| short  | 2 bytes | z position |
| short  | 2 bytes | Pad |

##### Node Data

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned int  | 4 bytes  | ID |
| int  | 4 bytes | Size |
| int  | 4 bytes | Unknown |
| int  | 4 bytes | Type |
| [NodePosition](#node-position)  | 8 bytes | Position |
| [NodePosition](#node-position)  | 8 bytes | Rotation |
| char | 8 bytes | Unknown |
| short | 2 bytes | Collision Volume Type |
| [Vector3](#Vector3) | 6 bytes | Collision Volume Half Extent |
| char | 8 bytes | Unknown |
| int  | 4 bytes | Message Data ID List |


**Note that Rotation is stored in fixed math format where 4096 is 360 degrees**

**Note that Collision Volume starts from Node Position and uses Half extent to construct a bounding box.**

###### Node Type

Node Type is used to understand what kind of data the node represents and at which offset it is found.

| Type | Data Offset |
| ---- | ---- |
| 2,4,6  | 96 bytes |
| 3  | 116 bytes |
| 0 | 0 bytes (no Data) |

All the offset starts from the node position in file.
**Note that If the Node has ID equals to 1292341027 and the type is 0 then It represents a TSP load node which contains information about the next TSP file that needs to be loaded and the information is found at 48 bytes.**

In all other cases the offset represents the information about the attached RenderObject that this node represents and can be read as a series of integers.

#### Property Set File

This data contains a list of nodes that are used to glue the TSP collision data to the node structure in the BSD file.
Each leaf of the [KD-Tree](#collision-data) found in the TSP file contains an index to this property array that is used to check
which node has to be checked in order to fire some event (Load the next TSP,Spawn an Object etc...).

At the start of the section there is a 4 bytes number that tells how many property we need to load.
Each Property contains the following data:

| Type | Size | Description |
| ---- | ---- | ----------- |
| Byte  | 1 byte  | Number of Nodes |
| short  | n bytes | Node List |

**IMPORTANT: The actual number of nodes is found by subtracting the value 255 to the one that was loaded.**


### Build
> cd MOHLevelViewer && make
### Usage
> ./MOHLevelViewer <MOH Game Directory> MissionNumber LevelNumber

#### Controls

| Key | Description |
| ---- | ---- |
| Esc  | Press to close the program |
| c  | Press to show/hide the collision data |
| b  | Press to show/hide the BSP tree nodes bounding boxes |
| i  | Press to show/hide the BSD Render Objects Showcase (Draws all the loaded RenderObjects) |
| l  | Press to show/hide the Level |
| n  | Press to show/hide the BSD Nodes as points |
| p  | Press to show/hide the BSD Render Objects |
| r  | Press to show/hide the BSD Render Objects as points |
| q  | Press to toggle between wireframe and Normal mode |
| f  | Press to toggle Frustum Culling using TSP BSP Tree |
| g  | Press to enable/disable Lighting |
| w,a,s,d | Press to move the camera anywhere on the loaded level|

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

##### TIM Header

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned int  | 4 bytes  | Magic (Always 0x10) |
| unsigned int  | 4 bytes  | BPP |
| unsigned int  | 4 bytes |  CLUTSize |
| unsigned short  | 2 bytes |  CLUTX |
| unsigned short  | 2 bytes |  CLUTY |
| unsigned short  | 2 bytes |  Number of CLUT Colors |
| unsigned short  | 2 bytes |  Number of CLUTs |

##### TIM CLUT Color

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned char  | 1 bytes  | R (Red Component) |
| unsigned char  | 1 bytes  | G (Green Component) |
| unsigned char  | 1 bytes  | B (Blue Component) |
| unsigned char  | 1 bytes  | STP (Used for transparency) |


##### TIM Content

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
  > gcc -o TIMExtractor TIMExtractor.c
#### Run
  > ./TIMExtractor `<File.tim>`

This command will read the content of <File.tim>, convert it to png and save it in the current folder.
If the file contains more than one TIM then it creates a folder with the same name as <File> (without the extension) that contains all the converted images that were found in the file.

## VAB Extractor

This utility can be used to extract music contained in *.vb files as well as TAF files.
At the moment the only requirements is that the taf file must have the tim section removed otherwise it won't work.

### Usage
#### Build
Compile:
  > gcc -o VABExtractor VABExtractor.c
#### Run
  > ./VABExtractor `<File.vab> <IsTaf> <Output Directory> <IsVag>`

This command will read the content of <File.vab>, convert it to wav and save it in the output directory.
If it is a TAF file it convert all the files that are found inside otherwise if IsVag is set to 1 will convert only one file to wav.
