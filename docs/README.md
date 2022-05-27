- [Medal Of Honor PSX File Viewer](#medal-of-honor-psx-file-viewer)
  * [Introduction](#introduction)
  * [Programs](#programs)
    + [MOHLevelViewer](#mohlevelviewer)
      - [Build](#build)
      - [Usage](#usage)
    + [RSC Extractor](#rsc-extractor)
      - [Build](#build-1)
      - [Run](#run)
    + [TIM Extractor](#tim-extractor)
      - [Build](#build-2)
      - [Run](#run-1)
    + [VAB Extractor](#vab-extractor)
      - [Build](#build-3)
      - [Run](#run-2)
  * [File Formats](#file-formats)
    + [Common Formats](#common-formats)
      - [TSB](#tsb)
    + [TSP Files](#tsp-files)
      - [BSP Nodes](#bsp-nodes)
        * [Vector3](#vector3)
        * [Bounding Box](#bounding-box)
        * [BSP Node](#bsp-node)
      - [Vertex](#vertex)
      - [Color](#color)
      - [Faces](#faces)
        * [UV Coordinates](#uv-coordinates)
      - [Texture Info](#texture-info)
        * [Face Data](#face-data)
      - [Dynamic Data](#dynamic-data)
        * [Dynamic Face Data](#dynamic-face-data)
      - [Collision Data](#collision-data)
        * [KDTree Nodes](#kdtree-nodes)
        * [Face Index Array](#face-index-array)
        * [Collision Vertices](#collision-vertices)
        * [Collision Normals](#collision-normals)
        * [Collision Faces](#collision-faces)
    + [BSD Files](#bsd-files)
      - [File Format](#file-format)
        * [TSP Info Block](#tsp-info-block)
        * [Animated Lights Block](#animated-lights-block)
        * [Entry Table Block](#entry-table-block)
        * [Sky Box Definitions](#sky-box-definitions)
        * [RenderObject Block](#renderobject-block)
          + [Color Mode](#color-mode)
          + [Texture Page](#texture-page)
          + [Vertex Data](#vertex-data)
        * [Node Table](#node-table)
          + [Node Table Data](#node-table-data)
          + [Node Table Entry](#node-table-entry)
        * [Node](#node)
          + [Node Position](#node-position)
          + [Node Data](#node-data)
          + [Node Type](#node-type)
        * [Property Set File](#property-set-file)
    + [RSC Files](#rsc-files)
      - [File Format](#file-format-1)
          + [RSC Header](#rsc-header)
          + [RSC Entry](#rsc-entry)
    + [TAF Files](#taf-files)
    + [TIM Files](#tim-files)
      - [File Format](#file-format-2)
          + [TIM Header](#tim-header)
          + [TIM CLUT Color](#tim-clut-color)
          + [TIM Content](#tim-content)



## Introduction
This project contains a set of tools that can be used to view Medal Of
Honor and Medal Of Honor Underground Level files and images.  
It was tested under Linux and Windows but it should also run on any other
platform.  
Game Files are not available into this repository and you need a copy of
the game in order to use it.  

Some sample screenshot taken from MOHLevelViewer:  

<img src="Images/MOH_MSN9LVL4_Screen1.png" width="600" />

*Screenshot taken from the last Mission of Medal Of Honor
using MOHLevelViewer.*

<img src="Images/MOHU_MSN2LVL1_Screen2.png" width="600" />

*Screenshot taken from the first Mission of Medal Of
Honor:Underground using MOHLevelViewer.*

## Programs
### MOHLevelViewer
MOHLevelViewer is able to load and render any level from
the games Medal Of Honor and Medal Of Honor:Underground.  
At the moment only the level files and objects are loaded
and rendered.  
It is able to load level (that can be exported along with
object to an Obj or Ply file) and the music (that can be
exported to wav).  

#### Build
Before building make sure to fetch all the submodules by typing:  
> git submodule --init --recursive  

then type:  

> cd MOHLevelViewer && mkdir Build && cmake .. && cmake --build . --target
MOHLevelViewer

to build it.  
**NOTE: Make sure to move the shader folder in the same directory as the
executable, otherwise the program will display only the GUI and not the
levels.**  
#### Usage
> ./MOHLevelViewer `<Optional Game Directory>`  

**NOTE: The configuration is stored in the User preference folder (.local/
share/MOHLevelViewer on Linux and %AppData% on Windows).**  
#### Credits
MOHLevelViewer uses the following libraries:  

**SDL2**: https://www.libsdl.org/  
**zlib**: https://github.com/madler/zlib  
**libpng**:http://www.libpng.org/  
**IMGUI**: https://github.com/ocornut/imgui/  
**IMGUI_FileDialog**:  https://github.com/aiekick/ImGuiFileDialog  
The font file shipped with the program is:  
**DroidSans.ttf**: https://www.fontsquirrel.com/fonts/droid-sans

### RSC Extractor
This tool is able to extract any RSC file that is available in the game
folder.  
#### Build
Compile:
  > gcc -o RSCUtils RSCUtils.c
#### Run
  > ./RSCUtils `<File.rsc>`

This command will extract the content of <File.rsc> creating all the required directories as declared in the RSC file.

### TIM Extractor
This tool is used to extract all the TIM images from different files and
convert them to png.
#### Build
Compile:
  > gcc -o TIMExtractor TIMExtractor.c
#### Run
  > ./TIMExtractor `<File.tim>`

This command will read the content of <File.tim>, convert it to png and
save it in the current folder.  
If the file contains more than one TIM then it creates a folder with the
same name as <File> (without the extension) and all the images that were
found in the file will be saved inside that folder.
#### Credits
TIMExtractor uses the following libraries:  
**libpng**: http://www.libpng.org/  

### VAB Extractor

This utility can be used to extract music contained in *.vb files as well
as TAF files.  
At the moment the only requirements is that the taf file must have the tim
section removed otherwise it won't work.

#### Build
Compile:
  > gcc -o VABExtractor VABExtractor.c
#### Run
  > ./VABExtractor `<File.vab> <IsTaf> <Output Directory> <IsVag>`

This command will read the content of <File.vab>, convert it to wav and
save it in the output directory.  
If it is a TAF file, then it will convert all the files that are found
inside otherwise if IsVag is set to 1 will convert only one file to
wav.
#### Credits
VABExtractor uses the following libraries:  
**libsndfile**: https://github.com/libsndfile/libsndfile  
## File Formats
### Common Formats
#### TSB
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

Finally last 5 bits represent the VRAM page number (11 in this specific
case).
> 01011

### TSP Files
All TSP files starts with an header which contains the following data:  
If Version is 1:

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

Otherwise, in version 3, two new fields are added:

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
| int  | 4 bytes  | Texture Info Number |
| int  | 4 bytes  | Texture Info Offset |


**Note that all the offset starts from the beginning of the file.**  
Thanks to this format we can read each chunk separately by moving the file
position to the wanted offset.
#### BSP Nodes
The game uses a BSP tree for rendering all the level data.
Each BSP node contains the following data:
##### Vector3

| Type | Size | Description |
| ---- | ---- | ---- |
| short | 2 bytes | x coordinate  |
| short  | 2 bytes | y coordinate |
| short  | 2 bytes | z coordinate |

##### Bounding Box

| Type | Size | Description |
| ---- | ---- | ----------- |
| [Vector3](#Vector3) | 6 bytes  | Min |
| [Vector3](#Vector3) | 6 bytes  | Max |

##### BSP Node

| Type | Size | Description |
| ---- | ---- | ----------- |
| [BBox](#Bounding-Box) | 12 bytes  | Bounding Box |
| int  | 4 bytes  | Number of Faces |
| int  | 4 bytes  | Unknown |
| int  | 4 bytes  | Unknown |
| int  | 4 bytes  | Offset |
| int  | 4 bytes  | Child1 Offset |
| int  | 4 bytes  | Child2 Offset |

If the Number of faces is zero then this node could have a child and they
can be found by using the two offset.
Each child can have an offset equals to -1 in which case it means that it
is **NULL**.

Offset Field has two purposes:  
If NumFaces != 0 then Offset represents the starting position where to load
the face array that goes from
> [Offset;Offset + (NumFaces * sizeof(Face))]

**Note that this is valid only in TSP version 1, TSP version 3 uses a
different algorithm based on what is described on
the [Face Section](#faces).**

Otherwise it represents the next node offset in the array that needs to be
loaded.


**Note that the child and next node offset starts from the node declaration
as seen in the header.**  
E.G: TSP Node offset is 64, Child1 Offset is 24 then the child node will be
at **64+24**.

#### Vertex

| Type | Size | Description |
| ---- | ---- | ----------- |
| [Vector3](#Vector3) | 6 bytes  | Vertex coordinate |
| short | 2 bytes  | Pad |

#### Color
This is used by each face in order to simulate lights.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned byte | 1 byte  | Red |
| unsigned byte | 1 byte  | Green |
| unsigned byte | 1 byte  | Blue |
| unsigned byte | 1 byte  | Pad |

#### Faces

##### UV Coordinates
Used for texture coordinates.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned char | 1 byte  | u coordinate |
| unsigned char | 1 byte  | v coordinate |


#### Texture Info

Starting from version 3 texture data is stored in a separate structure:  

| Type | Size | Description |
| ---- | ---- | ----------- |
| [UV](#uv-coordinates) | 2 byte  | UV0 |
| short | 2 byte  | CBA |
| [UV](#uv-coordinates) | 2 byte  | UV1|
| short | 2 byte  | [TSB](#tsb) (Texture Info) |
| [UV](#uv-coordinates) | 2 byte  | UV2|
| short | 2 byte  | Pad |

##### Face Data

Each face is made by 3 vertices that forms a triangle.

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned short | 2 bytes  | V0 First vertex and color index in array |
| unsigned short | 2 bytes  | V1 Second vertex and color index in array |
| unsigned short | 2 bytes  | V2 Third vertex and color index in array |
| [UV](#uv-coordinates) | 2 byte  | UV0 Texture coordinate for vertex 0  |
| short | 2 bytes  | CBA (Contains CLUT Data for TIM Images, Not Used)  |
| [UV](#uv-coordinates) | 2 bytes  | UV1 Texture coordinate for vertex 1  |
| short | 2 bytes  | TSB that contains info about the used texture ( read [TSB](#tsb) for more information)|
| [UV](#uv-coordinates) | 2 bytes  | UV2 Texture coordinate for vertex 2  |

Starting from Version 3 a different type of face format is used:

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned int | 4 bytes  | V0,V1 First and Second vertex/color index in array |
| unsigned short | 2 bytes  | V2 Third vertex and color index in array |
| unsigned short | 2 bytes  | Texture Index in texture array |

this format is not meant to be loaded directly but rather the [BSP Node struct](#bsp-node) must be used to load it.  
The way it works it's based on the offset found in the Node data that
signals the beginning of the face definition.  
After reading the data and storing it into an array, we need to iterate and
read the next 4-bytes that contains a vertex and a new face index.  
Note that unlike V2, that it is not encoded, V0 and V1 can be extracted
using bit shifting: (V0V1 & 0x1FFF) and (V0V1 >> 16 ) & 0X1FFF  
There are three possible cases that we can find when reading this int.  
If it is equal to 0x1fff1fff than this is the last data that we need to
read for that node.  
If, instead, is equal to 0x1FFF then we need to read a new face struct and
read the next int until one of the two markers is found.  
Otherwise it is a valid int and can be used to build a new face.  
The new face is made from the main face struct that we read at the
beginning where the vertex are updated based on the current integer.  
If the integer has the left-most bit set then we need to swap Vertex0 and
Vertex2, otherwise we need to set V0 equals to V1 and V1 equals to V2.  
Finally we update Vertex2 with the new value taken from the int.  
Note that the Value read is an int that contains two values: Vertex Number  
(Value & 0x1FFF) and Texture Index (Value >> 16 ).  
At the end of all the iterations we should find that the number of loaded
faces is equals to the one declared in the node.  


#### Dynamic Data

Dynamic data is used to swap textures from existing face in order to
simulate different effects like when a window is hit by a bullet and
explodes...  
Every TSP can contain any number of dynamic blocks (even 0 if not used)
and each block has the following data:

Every block starts with an header that contains the following fields:

| Type | Size | Description |
| ---- | ---- | ----------- |
| int | 4 bytes  | Dynamic Block Size |
| int | 4 bytes  | Unknown |
| int | 4 bytes  | Effect Type |
| int | 4 bytes  | Dynamic Data Index  |
| short | 2 bytes  | Face Data Multiplier  |
| short | 2 bytes  | Number of Faces Index  |
| short | 2 bytes  | Face Index Offset  |
| short | 2 bytes  | Face Data Offset  |
| short | n bytes  | Array of Faces Index |

**Note that Face Index is an index to the TSP face index only on Medal Of
Honor, Medal Of Honor:Underground uses this as the Face offset.**  
Face data multiplier is used to indicate that we need to load at
FaceDataOffset n faces where n=NumberofFacesIndex * FaceDataMultiplier.    

There are four types of effects that can be played when using Dynamic data:    

| Effect | Enum | Description |
| ---- | --------| ----------- |
| 0 | TSP_DYNAMIC_FACE_EFFECT_PLAY_AND_STOP_TO_LAST | Change the texture data until the last effect is reached then stops. |
| 1 | TSP_DYNAMIC_FACE_EFFECT_JUMP_TO_LAST | Change the texture data to the last index available in the array. |
| 2 |  TSP_DYNAMIC_FACE_EFFECT_CYCLE | Change the texture data continuously by resetting to zero when it reaches the last state |
| 3 |  TSP_DYNAMIC_FACE_EFFECT_PULSE | Change the texture data by increasing and decreasing the index causing a pulse-like effect.|



##### Dynamic Face Data
After the index list we have the actual face data that can be used to swap the original texture from the TSP face.  
Medal Of Honor uses a structure that contains several fields in order to update a specific face:  

| Type | Size | Description |
| ---- | ---- | ----------- |
| [UV](#uv-coordinatesuv) | 2 bytes  | UV0 |
| short | 2 bytes  | CBA (Clut Data)  |
| [UV](#uv-coordinatesuv) | 2 bytes  | UV1 |
| short | 2 bytes  | TSB (Texture Data) |
| [UV](#uv-coordinatesuv) | 2 bytes  | UV2 |  

Medal Of Honor:Underground stores this information as a list of short that
references the [Texture Data](#texture-info) that will be applied
to a particular face.  
#### Collision Data

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

##### KDTree Nodes

Each Node has the following data:

| Type | Size | Description |
| ---- | ---- | ----------- |
| short | 2 bytes  | Child0 |
| short | 2 bytes  | Child1 |
| short | 2 bytes  | Middle Split Value |
| short | 2 bytes  | Index to the [Property Set File](#property-set-file) as found in the BSD  |

If Child0 is less than 0 then the current node is a leaf and contains
(-Child0 - 1) faces starting from the index Child1 that is mapped to the
Face Index array.

If the node is not a leaf one then Child0 and Child1 are used to iterate
over the KDTree
Child1 has two function It represents the next node in the KDTree and also
the split axis:  
```
If (Child1 < 0) {
  Split along the Z axis and next node could be Child0 or (-Child1-1)
} else {
  Split along the X Axis and next node could be either Child0 or Child1.
}
```

##### Face Index Array

Face Index Array is a list of shorts that maps from the KDTree index to the collision face array.

##### Collision Vertices

Collision Vertex is a list that contains all the vertices used by the collision faces.

| Type | Size | Description |
| ---- | ---- | ----------- |
| [Vector3](#Vector3) | 6 bytes  | Vertex coordinate |
| short | 2 bytes  | Pad |

##### Collision Normals

Collision Normal is a list that contains all the normals used by the
collision faces.

| Type | Size | Description |
| ---- | ---- | ----------- |
| [Vector3](#Vector3) | 6 bytes  | Normal |
| short | 2 bytes  | Pad |

##### Collision Faces

Every collision face has the following structure:

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned short | 2 bytes  | Vertex0 |
| unsigned short | 2 bytes  | Vertex1 |
| unsigned short | 2 bytes  | Vertex2 |
| unsigned short | 2 bytes  | Normal Index  |
| short | 2 bytes | Plane Distance |


### BSD Files
Each BSD files can be seen as a container for multiple file types
(Levels,Weapons,Players).
There are some minor differences between Single Player and MultiPlayer BSD
file and the MOHLevelViewer contains two source files that parses the two
version.

#### File Format
Every BSD file starts with a 2048 bytes header that can be ignored and
probably contains a list of CD sectors that were required by the PSX in
order to correctly read the file.  
**Note that all the offset contained inside the file starts from 2048.**


Right after the header the information about the corresponding TSP file is
found if it is a level file otherwise it's replaced by zeroes.

##### TSP Info Block

| Type | Size | Description |
| ---- | ---- | ----------- |
| char | 128 bytes  | TSP File Name |
| int  | 4 bytes  | Total Number of TSP Files |
| int  | 4 bytes  | Number of TSP Files that needs to be rendered at start |
| int  | 4 bytes  | Number of the first TSP File that needs to be rendered |

The other TSP are loaded in real time when hitting specific triggers
contained into the level that unloads the previous one that were loaded in
memory.

##### Animated Lights Block
This block is found at position 216 (excluding the header) or 2264
(including the header) and contains information about animated lights that
can be used by the TSP in order to render special effects like running
water from a river,a blinking light etc...
Each BSD file can hold a maximum number of 40 animated lights where each
one is contained in a structure of 20 bytes:

| Type | Size | Description |
| ---- | ---- | ----------- |
| int  | 4 bytes  | NumColors |
| int  | 4 bytes  | StartingColorOffset |
| int  | 4 bytes  | ColorIndex |
| int  | 4 bytes  | CurrentColor |
| int  | 4 bytes  | Delay |

Every animated light has a number of colors that are loaded at the
specified StartingColorOffset (to which you would add 4-bytes until all
colors are read).  
Each color is just a 4-byte integer that represents the 3 components (RGB)
plus a constant value that it is used to restart the animation ( by setting
the Delay value to this constant ).  
Every frame the animated light structure is updated only if the Delay
reaches zero, after which the ColorIndex is incremented wrapping around
only when it reaches the maximum value represented by NumColors.  
ColorIndex is used to select the current color that will be used by the
surface during rendering time.  
The list of colors can be actually found after the RenderObject block and
it is not meant to be read sequentially but loaded when parsing this
block.   

##### Entry Table Block
This block is found at position 1340 (excluding the header) or 3388
(including the header) contains information about the position of some
elements that are contained inside the file along with the number of
elements and has a fixed size of 80 bytes.

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

##### Sky Box Definitions
This block is found at position 1420 ( excluding the header) or 3468
(including the header) and contains information about the position of the
moon and how the stars are projected across the screen.  

| Type | Size | Description |
| ---- | ---- | ----------- |
| Byte | 1 byte  | Unknown0 |
| Byte | 1 byte  | Unknown1 |
| Byte | 1 byte  | Unknown2 |
| Byte | 1 byte  | Star Circle Radius (Used when generating the star arrays as the radius for polar coordinates) |
| int  | 4 bytes  | Unknown3 |
| short  | 2 bytes  | Moon Position Z |
| short  | 2 bytes  | Moon Position Y |
| int  | 4 bytes  | Unknown4 |
| int  | 4 bytes  | Unknown5|
| int  | 4 bytes  | Unknown6 |

There can be a maximum number of 255 stars across the screen and they are
generated randomly using the Radius variable.  
The stars colors are selected from a fixed array of colors that has a size
of 8 and can be found inside the source file.  

##### RenderObject Block
After the entry block we find the number of RenderObject stored as an int
(4 bytes).  
A RenderObject, as the name implies , are all the objects that can be seen
inside the level like Windows,Doors,Enemies,Weapons,Boxes,MG42s, etc...  
Each RenderObject has a fixed size of 256 bytes (276 bytes for
MOH:Underground) containing several fields that depending by the type of
the RenderObject can be
NULL or contains an offset to the data stored inside the BSD file.    

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
| int  | 4 bytes  | Color Offset (Indices are the same as the one used for vertices) |
| char  | 52 bytes  | Unknown Data |
| int  | 4 bytes  | Type |

Medal Of Honor:Underground adds different new fields to the RenderObject
structure increasing the size to 276.  
In order to load the faces from these objects in MOH:Underground we need to
read both offset 256 and 260 in order to get the Face Offset and  the
Number of faces that needs to be drawn.  

**NOTE that ScaleX/Y/Z Values must be divided by 4 and the value found is
in fixed point format where 4096 is equal to 1.**

By trial and error the following RenderObject Types were found:

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

There are 13 available weapons that can be used in game and are identified
using specific IDs:

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

If the Face Data Offset is not zero then the RenderObject can be rendered
using the following face structure:

| Type | Size | Description |
| ---- | ---- | ----------- |
| [UV](#uv-coordinatesuv)  | 2 bytes  | UV Coordinates of Vertex 0 |
| short  | 2 bytes  | [TSB](#TSB) Info |
| [UV](#uv-coordinatesuv)  | 2 bytes  | UV Coordinates of Vertex 1 |
| short  | 2 bytes  | Texture Info |
| [UV](#uv-coordinatesuv)  | 2 bytes  | UV Coordinates of Vertex 2 |
| unsigned int  | 4 bytes  | Vertex Data |


this structure has been changed slightly in MOH:Underground and uses the
following format:  


| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned int | 4 bytes | Vertex 0 and Vertex 1 |
| unsigned short | 2 bytes | Vertex 2 |
| short  | 2 bytes  | [TSB](#TSB) Info |
| [UV](#uv-coordinatesuv)  | 2 bytes  | UV Coordinates of Vertex 0 |
| [UV](#uv-coordinatesuv)  | 2 bytes  | UV Coordinates of Vertex 1 |
| short | 2 bytes | TexInfo (Texture page and Color Mode) |
| [UV](#uv-coordinatesuv)  | 2 bytes  | UV Coordinates of Vertex 2 |

The algorithm for loading it is similar to what it is used on the TSP
version 3 but using a different structure for the face data.  
After reading the data and storing it into an array, we need to iterate and
read the next 4-bytes that contains a vertex and a new UV coordinate.  
Note that unlike V2, that it is not encoded, V0 and V1 can be extracted
using bit shifting: (V0V1 & 0x1FFF) and (V0V1 >> 16 ) & 0X1FFF.   
There are three possible cases that we can find when reading this int.  
If it is equal to 0x1fff1fff than this is the last data that we need to
read for that node.  
If, instead, is equal to 0x1FFF then we need to read a new face struct and
read the next int until one of the two markers is found.  
Otherwise it is a valid int and can be used to build a new face.  
The new face is made from the main face structure that we read at the
beginning with only the vertex updated using the current integer.  
If the integer has the left-most bit set then we need to swap Vertex0 and
Vertex2 (and the corresponding texture coordinates UV0 and UV2),
otherwise we need to set V0 equals to V1 and  V1 equals to V2 (this means
UV0 = UV1 and UV1 = UV2).  
Finally we update Vertex2 and UV2 with the new value taken from the int.    
Note that the Value read is an int that contains two information: Vertex
Number  (Value & 0x1FFF) and Texture Coordinates (Value >> 16) >> 8 for the
u coordinate while the v coordinate is equal to (Value >> 0x10) & 0xff.    
At the end of all the iterations we should find that the number of loaded
faces is equals to the one declared in the RenderObject.    

Texture info contains all the information about the used texture for the current face and can be extracted in this way:  
###### Color Mode
> (TexInfo & 0xC0) >> 7

For the result value read about [TSB](#TSB) since it uses the same format.
###### Texture Page
> TexInfo & 0x3f or TexInfo if loading FaceV2 (for MOH:Underground)

For the result value read about [TSB](#TSB) since it uses the same format.

###### Vertex Data
Vertex Data contains the information about the indices used to create the triangle that represent the current face and It has the following format:

####### Vertex 0
> VertexData & 0xFF

####### Vertex 1
> (VertexData & 0x3fc00) >> 10

####### Vertex 2
> (VertexData & 0xFF00000) >> 20

##### Node Table
This section of the BSD files contains the list of all the nodes along with
their offset contained inside the level.
The position can be found thanks to the [Entry Table](#entry-table-block)
and contains the following data:

###### Node Table Data

| Type | Size | Description |
| ---- | ---- | ----------- |
| int  | 4 bytes  | Number of Nodes |
| int  | 4 bytes | Table Size |
| char | 8 bytes | Unknown |

After this header we find the table entry containing the position for all the nodes inside the BSD file:

###### Node Table Entry

| Type | Size | Description |
| ---- | ---- | ----------- |
| int  | 4 bytes  | Unknown |
| int  | 4 bytes | Node Offset |

**Note that the Node offset refers to the position after the table entry
list.**

##### Node
After having loaded the table and all the table entries, we find the actual
node data (First node position should be the same as the first offset
inside the node table entry list).  
Each Node represents either a physical object (referencing a RenderObject
ID) or logical such as spawn point which are not rendered.
Each node has the following structure:
###### Node Position

| Type | Size | Description |
| ---- | ---- | ----------- |
| short  | 2 bytes  | x position |
| short  | 2 bytes | y position |
| short  | 2 bytes | z position |
| short  | 2 bytes | Pad |

###### Node Data

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
| short  | 2 bytes | When ID is equal to 2289546822 (Player Spawn) then this value is the Spawn Index (Player 1/2) |


**Note that Rotation is stored in fixed math format where 4096 is 360
degrees**

**Note that Collision Volume starts from Node Position and uses Half extent
to construct a bounding box.**

###### Node Type

Node Type is used to understand what kind of data the node represents and at which offset it is found.

| Type | Data Offset |
| ---- | ---- |
| 2,4,6  | 96 bytes |
| 3  | 116 bytes |
| 0 | 0 bytes (no Data) |

All the offset starts from the node position in file.  
**Note that If the Node has ID equals to 1292341027 and the type is 0 then
It represents a TSP load node which contains information about the next TSP
file that needs to be loaded and the information is found at an offset of
48 bytes.**

In all other cases the offset represents the information about the attached
RenderObject that this node represents and can be read as a series of
integers.

##### Property Set File

This data contains a list of nodes that are used to glue the TSP collision
data to the node structure in the BSD file.  
Each leaf of the [KD-Tree](#collision-data) found in the TSP file contains
an index to this property array that is used to check
which node has to be checked in order to fire some event (Load the next
TSP,Spawn an Object etc...).

At the start of the section there is a 4 bytes number that tells how many
property we need to load.  
Each Property contains the following data:

| Type | Size | Description |
| ---- | ---- | ----------- |
| Byte  | 1 byte  | Number of Nodes |
| short  | n bytes | Node List |

**IMPORTANT: The actual number of nodes is found by subtracting the value ù
255 to the one that was  loaded.**

### RSC Files
#### File Format
RSC are simple not compressed archive files that contains different files
type.
Each RSC files starts with an header containing the following data:

###### RSC Header
| Type | Size | Description |
| ---- | ---- | ----------- |
| char  | 64 bytes  | Directory Name |
| long long  | 8 bytes  | Number of Entry |

###### RSC Entry

| Type | Size | Description |
| ---- | ---- | ----------- |
| char  | 68 bytes  | File Name |
| int  | 4 bytes  | File Length |
| long long | 8 bytes | Offset |

### TAF Files
TAF files contains images and sounds for each level.
Inside each level folder there are two TAF files which have the following
format:
>MissionNumber_LevelNumber0.TAF
>MissionNumber_LevelNumber1.TAF

Only one is used at any time and represents the used language:
0 for German (Default) while 1 is American(Can be activated in the Password
menu).

File has not an header but it is just a collection of tim files and vab
files.
### TIM Files
TIM is a file format used for storing all the images in the game.
#### File Format

###### TIM Header

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned int  | 4 bytes  | Magic (Always 0x10) |
| unsigned int  | 4 bytes  | BPP |
| unsigned int  | 4 bytes |  CLUTSize |
| unsigned short  | 2 bytes |  CLUTX |
| unsigned short  | 2 bytes |  CLUTY |
| unsigned short  | 2 bytes |  Number of CLUT Colors |
| unsigned short  | 2 bytes |  Number of CLUTs |

###### TIM CLUT Color

| Type | Size | Description |
| ---- | ---- | ----------- |
| unsigned char  | 1 bytes  | R (Red Component) |
| unsigned char  | 1 bytes  | G (Green Component) |
| unsigned char  | 1 bytes  | B (Blue Component) |
| unsigned char  | 1 bytes  | STP (Used for transparency) |


###### TIM Content

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