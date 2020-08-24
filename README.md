# Medal-Of-Honor-PSX-File-Viewer
- [Build](#build)
- [Usage](#usage)
- [BSD Files](#bsd-files)
- [TSP Files](#tsp-files)
- [RSC Files](#rsc-files)
- [TAF Files](#taf-files)

## Dependencies
  The project uses SDL2,LibPNG and OpenGL 3.
## Build
  To build it you just need to go into the folder and type:
  > make

  This will generate the executable in the current folder.
## Usage
  > ./MOHLevelViewer `<Root Medal of Honor Folder>` `<MissionNumber>` `<LevelNumber>`

  Example:  
  Assuming Medal Of Honor is extracted at /home/adriano/mohtest and you want to see Mission 1 Level 1:

  > ./MOHLevelViewer /home/adriano/mohtest 1 1
## BSD Files
Each BSD files can be seen as a container for multiple file types (Levels,Weapons,Players).  
It starts with a 2048 bytes header that can be ignored and probably contains a list of CD sectors that were required by the PSX in order to correctly read the file.  

Right after the header the information about the corresponding TSP file is found if it is a level file.  
| Type  | Size | Description |
| ------------- | ------------- | ------------- |
| char  | 128 bytes  | TSP File Name |
| int  | 4 bytes  | Total Number of TSP Files |
| int  | 4 bytes  | Number of TSP Files that needs to be rendered at start |
| int  | 4 bytes  | Number of the first TSP File that needs to be rendered |  
The other TSP are loaded in real time when hitting specific triggers contained into the level that unloads the previous one that were loaded in memory.

## TSP Files
## RSC Files
### File Format
RSC files are simple not compressed archive files that contains different files type.
Each RSC files starts with an header containing the following data:  
| Type | Size | Description |
| ------------- | ------------- | ------------- |
| char  | 64 bytes  | Directory Name |
| long long  | 8 bytes  | Number of Entry |  
Each RSC Entry has the following data:
| Type | Size | Description |
| ------------- | ------------- | ------------- |
| char  | 68 bytes  | File Name |
| int  | 4 bytes  | Length |  
| long long | 8 bytes | Offset |
### Usage
#### Build
Compile:
  > gcc -o RSCUtils RSCUtils.c
#### Run
  > ./RSCUtils `<File.rsc>`
  
This command will extract the content of <File.rsc> creating all the required directories as declared in the RSC file.  
## TAF Files
