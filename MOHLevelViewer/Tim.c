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

void TimImageListFree(TIMImage_t *ImageList)
{
    TIMImage_t *Temp;
    
    while( ImageList ) {
        Temp = ImageList;
        ImageList = ImageList->Next;
        free(Temp->CLUT);
        free(Temp->Data);
        free(Temp);
    }
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

bool IsClutColorZero(CLUTColor_t Color)
{
    return (Color.R == 0) &&
           (Color.G == 0) && 
           (Color.B == 0);
}

int GetAlphaValue(CLUTColor_t Color)
{
    int Result;    
    
    if( Color.STP ) {
        if( IsClutColorZero(Color) ) {
            Result = 255;
        } else {
            Result = 200;
        }
    } else {
        if( IsClutColorZero(Color) ) {
            Result = 0;
        } else {
            Result = 255;
        }
    }
    //255 Opaque 0 transparent
    return Result;
}

TIMImage_t *GetTIMByClutPage(TIMImage_t *List,int clutx,int cluty,int texpage)
{
    for( TIMImage_t *Iterator = List; Iterator; Iterator = Iterator->Next ) {
        if( Iterator->TexturePage == texpage ) {
            printf("Image has %ix%i %i bpp (looking for %ix%i)\n",Iterator->Header.CLUTOrgX,Iterator->Header.CLUTOrgY,
                Iterator->Header.BPP,clutx,cluty
            );
            if( Iterator->Header.CLUTOrgX == clutx &&
                Iterator->Header.CLUTOrgY == cluty ) {
                printf("Found %s in list\n",Iterator->Name);
                return Iterator;
            }
        }
    }
    return NULL;
}

/*
    Going RGBA now....
*/
Byte *TimToOpenGL32(TIMImage_t *Image)
{
    Byte *Data;
    int x;
    int y;
    int xOffs;
    
    Data = malloc(Image->Width * Image->Height * 4);

    if( Image->Header.BPP == BPP_8  ) {
        for (y = 0; y < Image->Height; y++) {
            xOffs = 0;
            for (x = 0; x < Image->RowCount; x++) {
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xFF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] & 0xFF00) >> 8;
                int BaseLocation0 = (xOffs + Image->Width * y) * 4;
                Data[BaseLocation0 + 0] = Image->CLUT[ClutIndex0].R;
                Data[BaseLocation0 + 1] = Image->CLUT[ClutIndex0].G;
                Data[BaseLocation0 + 2] = Image->CLUT[ClutIndex0].B;
                Data[BaseLocation0 + 3] = GetAlphaValue(Image->CLUT[ClutIndex0]);
                xOffs++;
                int BaseLocation1 = (xOffs + Image->Width * y) * 4;
                Data[BaseLocation1 + 0] = Image->CLUT[ClutIndex1].R;
                Data[BaseLocation1 + 1] = Image->CLUT[ClutIndex1].G;
                Data[BaseLocation1 + 2] = Image->CLUT[ClutIndex1].B;
                Data[BaseLocation1 + 3] = GetAlphaValue(Image->CLUT[ClutIndex1]);
                xOffs++;
            }
        }
    } else if ( Image->Header.BPP == BPP_4 ) {
        for( y = 0; y < Image->Height; y++ ) {
            xOffs = 0;
            for( x = 0; x < Image->RowCount; x++ ) {
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] >> 4) & 0xF;
                Byte ClutIndex2 = (Image->Data[x+Image->RowCount*y] >> 8) & 0xF;
                Byte ClutIndex3 = (Image->Data[x+Image->RowCount*y] >> 12) & 0xF;

                int BaseLocation0 = (xOffs + Image->Width * y)*4;
                Data[BaseLocation0 + 0] = Image->CLUT[ClutIndex0].R;
                Data[BaseLocation0 + 1] = Image->CLUT[ClutIndex0].G;
                Data[BaseLocation0 + 2] = Image->CLUT[ClutIndex0].B;
                Data[BaseLocation0 + 3] = GetAlphaValue(Image->CLUT[ClutIndex0]);
                xOffs++;
                int BaseLocation1 = (xOffs + Image->Width * y) * 4;
                Data[BaseLocation1 + 0] = Image->CLUT[ClutIndex1].R;
                Data[BaseLocation1 + 1] = Image->CLUT[ClutIndex1].G;
                Data[BaseLocation1 + 2] = Image->CLUT[ClutIndex1].B;
                Data[BaseLocation1 + 3] = GetAlphaValue(Image->CLUT[ClutIndex1]);
                xOffs++;
                int BaseLocation2 = (xOffs + Image->Width * y) * 4;
                Data[BaseLocation2 + 0] = Image->CLUT[ClutIndex2].R;
                Data[BaseLocation2 + 1] = Image->CLUT[ClutIndex2].G;
                Data[BaseLocation2 + 2] = Image->CLUT[ClutIndex2].B;
                Data[BaseLocation2 + 3] = GetAlphaValue(Image->CLUT[ClutIndex2]);
                xOffs++;
                int BaseLocation3 = (xOffs + Image->Width * y) * 4;
                Data[BaseLocation3 + 0] = Image->CLUT[ClutIndex3].R;
                Data[BaseLocation3 + 1] = Image->CLUT[ClutIndex3].G;
                Data[BaseLocation3 + 2] = Image->CLUT[ClutIndex3].B;
                Data[BaseLocation3 + 3] = GetAlphaValue(Image->CLUT[ClutIndex3]);
                xOffs++;
            }
        }
    }
    return Data;
}
Byte *TimToOpenGL24(TIMImage_t *Image)
{
    Byte *Data;
    int x;
    int y;
    int xOffs;
    
    Data = malloc(Image->Width * Image->Height * 3);

    if( Image->Header.BPP == BPP_8  ) {
        for (y = 0; y < Image->Height; y++) {
            xOffs = 0;
            for (x = 0; x < Image->RowCount; x++) {
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xFF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] & 0xFF00) >> 8;
                int BaseLocation0 = (xOffs + Image->Width * y) * 3;
                Data[BaseLocation0 + 0] = Image->CLUT[ClutIndex0].R;
                Data[BaseLocation0 + 1] = Image->CLUT[ClutIndex0].G;
                Data[BaseLocation0 + 2] = Image->CLUT[ClutIndex0].B;
                xOffs++;
                int BaseLocation1 = (xOffs + Image->Width * y) * 3;
                Data[BaseLocation1 + 0] = Image->CLUT[ClutIndex1].R;
                Data[BaseLocation1 + 1] = Image->CLUT[ClutIndex1].G;
                Data[BaseLocation1 + 2] = Image->CLUT[ClutIndex1].B;
                xOffs++;
            }
        }
    } else if ( Image->Header.BPP == BPP_4 ) {
        for( y = 0; y < Image->Height; y++ ) {
            xOffs = 0;
            for( x = 0; x < Image->RowCount; x++ ) {
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] >> 4) & 0xF;
                Byte ClutIndex2 = (Image->Data[x+Image->RowCount*y] >> 8) & 0xF;
                Byte ClutIndex3 = (Image->Data[x+Image->RowCount*y] >> 12) & 0xF;

                int BaseLocation0 = (xOffs + Image->Width * y)*3;
                Data[BaseLocation0 + 0] = Image->CLUT[ClutIndex0].R;
                Data[BaseLocation0 + 1] = Image->CLUT[ClutIndex0].G;
                Data[BaseLocation0 + 2] = Image->CLUT[ClutIndex0].B;
                xOffs++;
                int BaseLocation1 = (xOffs + Image->Width * y) * 3;
                Data[BaseLocation1 + 0] = Image->CLUT[ClutIndex1].R;
                Data[BaseLocation1 + 1] = Image->CLUT[ClutIndex1].G;
                Data[BaseLocation1 + 2] = Image->CLUT[ClutIndex1].B;
                xOffs++;
                int BaseLocation2 = (xOffs + Image->Width * y) * 3;
                Data[BaseLocation2 + 0] = Image->CLUT[ClutIndex2].R;
                Data[BaseLocation2 + 1] = Image->CLUT[ClutIndex2].G;
                Data[BaseLocation2 + 2] = Image->CLUT[ClutIndex2].B;
                xOffs++;
                int BaseLocation3 = (xOffs + Image->Width * y) * 3;
                Data[BaseLocation3 + 0] = Image->CLUT[ClutIndex3].R;
                Data[BaseLocation3 + 1] = Image->CLUT[ClutIndex3].G;
                Data[BaseLocation3 + 2] = Image->CLUT[ClutIndex3].B;
                xOffs++;
            }
        }
    }
    return Data;
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
    sprintf(ResultImage->Name,"IMAGE %i",NumImages);
    printf("-- %s --\n",ResultImage->Name);
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
    //TODO:Store this info together with the image.
    switch( ResultImage->Header.BPP ) {
//         case BPP_4:
//             ResultImage->TexturePage = 1 + ((ResultImage->FrameBufferX - 1) / 64);
//             break;
//         case BPP_8:
//             ResultImage->TexturePage = 1 + ((ResultImage->FrameBufferX - 1) / 128);
//             break;
        default:
            ResultImage->TexturePage = (ResultImage->FrameBufferX / 64) /*+ 1*/;
            break;
    }
    printf("FrameBuffer Coordinates %ux%u page %i\n",ResultImage->FrameBufferX,ResultImage->FrameBufferY,ResultImage->TexturePage);
    printf("Image has calculated texture page as %i\n",ResultImage->TexturePage);
    //Texture Page is Zero based.
    //Next row.
    if( ResultImage->FrameBufferY >= 256 ) {
        ResultImage->TexturePage += 16;
    }
    //Width is stored in 16-pixels unit so we need to offset it based on the current BPP.
    ResultImage->Width *= ImageSizeOffset;
    printf("NumPixels is %i\n",ResultImage->NumPixels);
    printf("FrameBuffer Coordinates %ux%u page %i\n",ResultImage->FrameBufferX,ResultImage->FrameBufferY,ResultImage->TexturePage);
    printf("Image is %ux%u RowCount is %u\n",ResultImage->Width,ResultImage->Height,ResultImage->RowCount);
//     if( ResultImage->FrameBufferX == 512 && ResultImage->FrameBufferY == 0 ) {
//         exit(0);
//     }
    ResultImage->Data = malloc((ResultImage->RowCount * ResultImage->Height) * sizeof(unsigned short));
    for( i = 0; i < ResultImage->RowCount * ResultImage->Height; i++ ) {
        int Ret;
        Ret = fread(&ResultImage->Data[i],sizeof(ResultImage->Data[i]),1,TIMImage);
        assert(Ret == 1);
    }
    return ResultImage;
}

TIMImage_t *GetAllTimImages(char *File)
{
    TIMImage_t *List;
    FILE *TIMFile;
    int NumImages;
    
    List = NULL;
    
    TIMFile = fopen(File,"rb");
    
    if( !TIMFile ) {
        printf("Error opening file %s!\n",File);
        return NULL;
    }
    List = NULL;
    NumImages = 0;
    while( 1 ) {
        TIMImage_t *Image = LoadTIMImage(TIMFile,NumImages);
        if( Image == NULL ) {
            break;
        }
        Image->Next = List;
        List = Image;
        NumImages++;
    }
    fclose(TIMFile);
    return List;
}
