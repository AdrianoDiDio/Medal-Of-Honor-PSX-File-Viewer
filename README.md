# Medal Of Honor PSX File Viewer
* [BSD Files](#bsd-files)
   * [File Format](#file-format)
   * [Build](#build)
   * [Usage](#usage)
* [TSP Files](#tsp-files)
* [RSC Files](#rsc-files)
   * [File Format](#file-format-1)
   * [Usage](#usage-1)
* [TAF Files](#taf-files)
* [TIM Files](#tim-files)
   * [File Format](#file-format-2)
   * [Usage](#usage-2)


## BSD Files
Each BSD files can be seen as a container for multiple file types (Levels,Weapons,Players).
### File Format
It starts with a 2048 bytes header that can be ignored and probably contains a list of CD sectors that were required by the PSX in order to correctly read the file.

Right after the header the information about the corresponding TSP file is found if it is a level file.

| Type | Size | Description |
| ---- | ---- | ----------- |
| char | 128 bytes  | TSP File Name |
| int  | 4 bytes  | Total Number of TSP Files |
| int  | 4 bytes  | Number of TSP Files that needs to be rendered at start |
| int  | 4 bytes  | Number of the first TSP File that needs to be rendered |

The other TSP are loaded in real time when hitting specific triggers contained into the level that unloads the previous one that were loaded in memory.

### Build
### Usage

## TSP Files
## RSC Files
### File Format
RSC files are simple not compressed archive files that contains different files type.
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
## TIM Files
TIM is a file format used for all images in the game.
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

This command will read the content of <File.tim>, convert it to png and saving it in the current folder.
If the file contains more than one TIM then it creates a folder the same name as <File> (without the extension) that contains all the converted images that were found in the file.
