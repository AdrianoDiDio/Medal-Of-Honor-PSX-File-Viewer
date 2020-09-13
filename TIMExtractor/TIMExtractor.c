/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
    TIMExtractor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TIMExtractor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TIMExtractor.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <png.h>

typedef unsigned char Byte;

typedef enum {
    false,
    true
} bool;

typedef enum {
    BPP_4 = 8,
    BPP_4_NO_CLUT = 0,
    BPP_8 = 9,
    BPP_8_NO_CLUT = 1,
    BPP_16 = 2,
    BPP_24 = 3
} TIMBPP;

typedef struct TIMHeader_s {
    unsigned int Magic; // Always 10 00 00 00 or 0x10
    unsigned int BPP;
    unsigned int CLUTSize;
    unsigned short CLUTOrgX; // Hard-Wired clut memory location.
    unsigned short CLUTOrgY; // Hard-Wired clut memory location.
    unsigned short NumClutColors;
    unsigned short NumCluts;
} TIMHeader_t;

typedef struct CLUTColor_s {
    Byte R;
    Byte G;
    Byte B;
    Byte STP; //Transparency - Not used at the moment.
} CLUTColor_t;

typedef struct TIMImage_s {
    TIMHeader_t Header;
    unsigned int NumPixels;
    unsigned short RowCount; // in Direct color mode (16/24 BPP) is equal to the Width.
    unsigned short Width;
    unsigned short Height;
    unsigned short FrameBufferX; // Hard-Wired texture memory location.
    unsigned short FrameBufferY; // Hard-Wired texture memory location.
    CLUTColor_t *CLUT;
    unsigned short /*Byte*/ *Data;
    unsigned short *BData;
    struct TIMImage_s *Next;
} TIMImage_t;

void CreateDirIfNotExists(char *DirName) {
    struct stat FileStat;

    if (stat(DirName, &FileStat) == -1) {
#ifdef _WIN32
        mkdir(DirName);
#else
        mkdir(DirName, 0700);
#endif
    }
}

char *SwitchExt(const char *In, const char *Ext)
{
    char *NewExt;
    int i;
    
    if ( !In || !Ext ) {
        return NULL;
    }

    NewExt = malloc(strlen(In) + 1);

    for ( i = 0; In[i] != '.'; i++ ) {
        NewExt[i] = In[i];
    }
    NewExt[i] = '\0';

    //Now append the extension to the string.
    strncat(NewExt, Ext, strlen(NewExt));
    return NewExt;
}

void PrintClut(TIMImage_t *Image,int ClutIndex)
{
    printf("CLUT %i => (%i;%i;%i)\n",ClutIndex,Image->CLUT[ClutIndex].R,Image->CLUT[ClutIndex].G,Image->CLUT[ClutIndex].B);
}

Byte GetR(short Color)
{
    return (Color & 0x1f) << 3;
}
Byte GetG(short Color)
{
    return (Color & 0x3e0 ) >> 2;
}
Byte GetB(short Color)
{
    return (Color & 0x7C00 ) >> 7;
}
Byte GetSTP(short Color)
{
    return (Color & 0x8000) >> 15;
}

Byte **SetRowPointer(png_structp PNGPtr,TIMImage_t *Image)
{
    Byte **RowPointer;
    int x;
    int y;
    int NumComponent;
    //Used in 24BPP mode
    CLUTColor_t RGB1;
    CLUTColor_t RGB2;
    RowPointer = png_malloc  (PNGPtr, Image->Height * sizeof (Byte *));
    if( Image->Header.BPP == BPP_4  ) {
        for (y = 0; y < Image->Height; y++) {
            Byte *Row = png_malloc (PNGPtr, sizeof (Byte) * Image->Width * 3);
            RowPointer[y] = Row;
            for (x = 0; x < Image->RowCount; x++) {
//                 printf("Pixel is %u 0x%08x\n",Image->Data[x+Image->RowCount*y],Image->Data[x+Image->RowCount*y]);
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] >> 4) & 0xF;
                Byte ClutIndex2 = (Image->Data[x+Image->RowCount*y] >> 8) & 0xF;
                Byte ClutIndex3 = (Image->Data[x+Image->RowCount*y] >> 12) & 0xF;
                *Row++ = Image->CLUT[ClutIndex0].R;
                *Row++ = Image->CLUT[ClutIndex0].G;
                *Row++ = Image->CLUT[ClutIndex0].B;
                *Row++ = Image->CLUT[ClutIndex1].R;
                *Row++ = Image->CLUT[ClutIndex1].G;
                *Row++ = Image->CLUT[ClutIndex1].B;
                *Row++ = Image->CLUT[ClutIndex2].R;
                *Row++ = Image->CLUT[ClutIndex2].G;
                *Row++ = Image->CLUT[ClutIndex2].B;
                *Row++ = Image->CLUT[ClutIndex3].R;
                *Row++ = Image->CLUT[ClutIndex3].G;
                *Row++ = Image->CLUT[ClutIndex3].B;
            }
        }
    }
    if( Image->Header.BPP == BPP_8  ) {
        for (y = 0; y < Image->Height; y++) {
            Byte *Row = png_malloc (PNGPtr, sizeof (Byte) * Image->Width * 3);
            RowPointer[y] = Row;
            for (x = 0; x < Image->RowCount; x++) {
//             printf("Pixel is %u 0x%08x\n",Image->Data[x+Image->RowCount*y],Image->Data[x+Image->RowCount*y]);
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xFF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] & 0xFF00) >> 8;
                *Row++ = Image->CLUT[ClutIndex0].R;
                *Row++ = Image->CLUT[ClutIndex0].G;
                *Row++ = Image->CLUT[ClutIndex0].B;
                *Row++ = Image->CLUT[ClutIndex1].R;
                *Row++ = Image->CLUT[ClutIndex1].G;
                *Row++ = Image->CLUT[ClutIndex1].B;
            }
        }
    }
    
    if( Image->Header.BPP == BPP_16  ) {
        for (y = 0; y < Image->Height; y++) {
            Byte *Row = png_malloc (PNGPtr, sizeof (Byte) * Image->Width * 3);
            RowPointer[y] = Row;
            for (x = 0; x < Image->RowCount; x++) {
//             printf("Pixel is %u 0x%08x\n",Image->Data[x+Image->RowCount*y],Image->Data[x+Image->RowCount*y]);
                Byte R = GetR(Image->Data[x+Image->RowCount*y]);
                Byte G = GetG(Image->Data[x+Image->RowCount*y]);
                Byte B = GetB(Image->Data[x+Image->RowCount*y]);
                *Row++ = R;
                *Row++ = G;
                *Row++ = B;
            }
        }
    }
    if( Image->Header.BPP == BPP_24 ) {
        NumComponent = 0;
        for (y = 0; y < Image->Height; y++) {
            Byte *Row = png_malloc (PNGPtr, sizeof (Byte) * Image->Width * 3);
            RowPointer[y] = Row;
            for (x = 0; x < Image->RowCount; x++) {
                switch( NumComponent ) {
                    case 0:
                        RGB1.R = Image->Data[x+Image->RowCount*y] & 0xFF;
                        RGB1.G = (Image->Data[x+Image->RowCount*y] >> 8) & 0xFF;
                        NumComponent++;
                        break;
                    case 1:
                        RGB1.B = Image->Data[x+Image->RowCount*y] & 0xFF;
                        RGB2.R = (Image->Data[x+Image->RowCount*y] >> 8) & 0xFF;
                        NumComponent++;
                        break;
                    case 2:
                        RGB2.G = Image->Data[x+Image->RowCount*y] & 0xFF;
                        RGB2.B = (Image->Data[x+Image->RowCount*y] >> 8) & 0xFF;
                        NumComponent = 0;
                        *Row++ = RGB1.R;
                        *Row++ = RGB1.G;
                        *Row++ = RGB1.B;
                        *Row++ = RGB2.R;
                        *Row++ = RGB2.G;
                        *Row++ = RGB2.B;
                        break;
                    default:
                        printf("SetRowPointer: Uneven component count detected aborting....!\n");
                        exit(0);
                        break;
                }

            }
        }
    }
    
    return RowPointer;
}

void WritePNGImage(char *OutName,TIMImage_t *Image)
{
    FILE *PNGImage;
    png_structp PNGPtr;
    png_infop PNGInfoPtr;
    Byte **RowPointer;
    int y;

    
    PNGImage = fopen(OutName,"wb");
    
    if( PNGImage == NULL ) {
        printf("Error creating image %s!\n",OutName);
        return;
    }

    PNGPtr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (PNGPtr == NULL) {
        printf("PNG: Couldn't create write struct!\n");
        return;
    }
    
    PNGInfoPtr = png_create_info_struct (PNGPtr);
    if (PNGInfoPtr == NULL) {
        printf("PNG: Couldn't create info struct!\n");
    }

    png_set_IHDR (PNGPtr,
                  PNGInfoPtr,
                  Image->Width,
                  Image->Height,
                  8, // Depth
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    RowPointer = SetRowPointer(PNGPtr,Image);
    
    png_init_io (PNGPtr, PNGImage);
    png_set_rows (PNGPtr, PNGInfoPtr, RowPointer);
    png_write_png (PNGPtr, PNGInfoPtr, PNG_TRANSFORM_IDENTITY, NULL);
    
    for (y = 0; y < Image->Height; y++) {
        png_free (PNGPtr, RowPointer[y]);
    }
    png_free (PNGPtr, RowPointer);
    fclose(PNGImage);
}

void GetPalette(FILE *TIMIMage,TIMImage_t *Image)
{
    unsigned short i;
    
    fread(&Image->Header.CLUTSize,sizeof(Image->Header.CLUTSize),1,TIMIMage);
    fread(&Image->Header.CLUTOrgX,sizeof(Image->Header.CLUTOrgX),1,TIMIMage);
    fread(&Image->Header.CLUTOrgY,sizeof(Image->Header.CLUTOrgY),1,TIMIMage);
    fread(&Image->Header.NumClutColors,sizeof(Image->Header.NumClutColors),1,TIMIMage);
    fread(&Image->Header.NumCluts,sizeof(Image->Header.NumCluts),1,TIMIMage);

    
    printf("ClutSize is %i\n",Image->Header.CLUTSize);
    printf("ClutLocation is %ux%u\n",Image->Header.CLUTOrgX,Image->Header.CLUTOrgY);
    printf("NumClutColors is %u\n",Image->Header.NumClutColors);
    printf("NumCluts is %u\n",Image->Header.NumCluts);
    Image->CLUT = malloc(Image->Header.NumClutColors * sizeof(CLUTColor_t));
    for( i = 0; i < Image->Header.NumClutColors; i++ ) {
        unsigned short Color;
        fread(&Color,sizeof(Color),1,TIMIMage);
        Image->CLUT[i].R = GetR(Color);
        Image->CLUT[i].G = GetG(Color);
        Image->CLUT[i].B = GetB(Color);
        Image->CLUT[i].STP = GetSTP(Color);
//         printf("Color is %u R G B A(%i;%i;%i;%i)\n",Color,Image->CLUT[i].R,Image->CLUT[i].G,Image->CLUT[i].B,Image->CLUT[i].STP);
    }
}


TIMImage_t *LoadTIMImage(FILE *TIMImage,int NumImages)
{
    TIMImage_t *ResultImage;
    float ImageSizeOffset;
    int Ret;
    int i;
    char OutName[256];
    
    ResultImage = malloc(sizeof(TIMImage_t));
    
    Ret = fread(&ResultImage->Header.Magic,sizeof(ResultImage->Header.Magic),1,TIMImage);
    if( Ret != 1 ) {
        printf("EOF reached.\n");
        free(ResultImage);
        return NULL;
    }
    if( ResultImage->Header.Magic != 0x10 ) {
        printf("Wrong signature detected (%i (%#08x)).\n",ResultImage->Header.Magic,ResultImage->Header.Magic);
        printf("Current offset in file is %li\n",ftell(TIMImage));
        free(ResultImage);
        return NULL;
    }
    printf("Found image at offset %li\n",ftell(TIMImage) - sizeof(ResultImage->Header.Magic));
    fread(&ResultImage->Header.BPP,sizeof(ResultImage->Header.BPP),1,TIMImage);
    printf("IMAGE %i\n",NumImages);
    printf("Magic is %i\n",ResultImage->Header.Magic);
    printf("Flags are %i\n",ResultImage->Header.BPP);

    switch(ResultImage->Header.BPP) {
        case BPP_4:
            printf("Found image with 4 BPP and CLUT!\n");
            ImageSizeOffset = 4;
            break;
        case BPP_4_NO_CLUT:
            printf("Found image with 4 BPP without CLUT!\n");
            break;
        case BPP_8:
            printf("Found image with 8 BPP and CLUT!\n");
            ImageSizeOffset = 2;
            break;
        case BPP_8_NO_CLUT:
            printf("Found image with 4 BPP without CLUT!\n");
            break;
        case BPP_16:
            printf("Found image with 16 BPP and CLUT!\n");
            ImageSizeOffset = 1;
            break;
        case BPP_24:
            printf("Found image with 24 BPP and CLUT!\n");
            ImageSizeOffset = 0.5f; //Or 2/3?
            break;
        default:
            printf("Unknown BPP %i found in tim file.\n",ResultImage->Header.BPP);
            return NULL;
    }
    
    if( ResultImage->Header.BPP == BPP_4_NO_CLUT || ResultImage->Header.BPP == BPP_8_NO_CLUT ) {
        printf("Unsupported BPP %i\n",ResultImage->Header.BPP);
        return NULL;
    }
    
    if( ResultImage->Header.BPP == BPP_24 ) {
        printf("Warning: BPP 24 was not tested.\n");
    }
    
    if( ResultImage->Header.BPP == BPP_4 || ResultImage->Header.BPP == BPP_8 ) {
        GetPalette(TIMImage,ResultImage);
    }
    fread(&ResultImage->NumPixels,sizeof(ResultImage->NumPixels),1,TIMImage);
    fread(&ResultImage->FrameBufferX,sizeof(ResultImage->FrameBufferX),1,TIMImage);
    fread(&ResultImage->FrameBufferY,sizeof(ResultImage->FrameBufferY),1,TIMImage);
    fread(&ResultImage->Width,sizeof(ResultImage->Width),1,TIMImage);
    fread(&ResultImage->Height,sizeof(ResultImage->Height),1,TIMImage);
    ResultImage->RowCount = ResultImage->Width;
    //Width is stored in 16-pixels unit so we need to offset it based on the current BPP.
    ResultImage->Width *= ImageSizeOffset;
    printf("NumPixels is %i\n",ResultImage->NumPixels);
    printf("FrameBuffer Coordinates %ux%u\n",ResultImage->FrameBufferX,ResultImage->FrameBufferY);
    printf("Image is %ux%u RowCount is %u\n",ResultImage->Width,ResultImage->Height,ResultImage->RowCount);
    ResultImage->Data = malloc((ResultImage->RowCount * ResultImage->Height) * sizeof(unsigned short));
    for( i = 0; i < ResultImage->RowCount * ResultImage->Height; i++ ) {
        int Ret;
        Ret = fread(&ResultImage->Data[i],sizeof(ResultImage->Data[i]),1,TIMImage);
        assert(Ret == 1);
    }
    return ResultImage;
}

int main(int argc,char **argv)
{
    FILE *TIMFile;
    TIMImage_t *ImageList;
    TIMImage_t *Iterator;
    TIMImage_t *Temp;
    int NumImages;
    int i;
    
    if( argc == 1 ) {
        printf("%s <Input.tim> will extract the content of Input.tim in the current folder if " 
                    "only one element is found otherwise a new Directory is created.\n",argv[0]);
        return -1;
    }

    printf("Loading file %s...\n",argv[1]);
    TIMFile = fopen(argv[1],"rb");
    
    if( !TIMFile ) {
        printf("Error opening tim file!\n");
        return -1;
    }

    ImageList = NULL;
    NumImages = 0;
    while( 1 ) {
        TIMImage_t *Image = LoadTIMImage(TIMFile,NumImages);
        if( Image == NULL ) {
            break;
        }
        Image->Next = ImageList;
        ImageList = Image;
        NumImages++;
    }
    if( NumImages == 1 ) {
        WritePNGImage(SwitchExt(argv[1],".png"),ImageList);
    } else {
        char *DirName;
        char FileName[256];
        DirName = SwitchExt(argv[1],"");
        CreateDirIfNotExists(DirName);
        for( Iterator = ImageList,i = 0; Iterator; Iterator = Iterator->Next,i++ ) {
            sprintf(FileName,"%s/%i.png\n",DirName,i);
            WritePNGImage(FileName,Iterator);
        }
        Iterator = ImageList;
        while( Iterator ) {
            Temp = Iterator;
            Iterator = Iterator->Next;
            free(Temp);
        }
    }
    printf("Found %i images in tim file.\n",NumImages);

    return 0;
}
