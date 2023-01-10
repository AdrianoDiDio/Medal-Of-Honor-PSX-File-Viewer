// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2023 Adriano Di Dio.
    
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

#include "TIM.h"

void TIMImageListFree(TIMImage_t *ImageList)
{
    TIMImage_t *Temp;
    
    while( ImageList ) {
        Temp = ImageList;
        ImageList = ImageList->Next;
        if( Temp->CLUT ) {
            free(Temp->CLUT);
        }
        free(Temp->Data);
        free(Temp);
    }
}
const char *TIMGetBPPFromImage(TIMImage_t *Image)
{
    if( !Image ) {
        return "";
    }
    switch( Image->Header.BPP ) {
        case TIM_IMAGE_BPP_4:
            return "4-BPP image";
        case TIM_IMAGE_BPP_8:
            return "8-BPP image";
        case TIM_IMAGE_BPP_16:
            return "16-BPP image";
        case TIM_IMAGE_BPP_24:
            return "24-BPP image";
        default:
            return "Unknown BPP";
    }
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

bool IsClutColorZero(unsigned short Color)
{
    return (GetR(Color) == 0) &&
           (GetG(Color) == 0) && 
           (GetB(Color) == 0);
}

int GetAlphaValue(unsigned short Color)
{
    int STP;
    int Result;    
    STP = GetSTP(Color);
    if( STP ) {
        Result = 255;
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
            DPrintf("Image has %ix%i %i bpp (looking for %ix%i)\n",Iterator->Header.CLUTOrgX,Iterator->Header.CLUTOrgY,
                Iterator->Header.BPP,clutx,cluty
            );
            if( Iterator->Header.CLUTOrgX == clutx &&
                Iterator->Header.CLUTOrgY == cluty ) {
                DPrintf("Found %s in list\n",Iterator->Name);
                return Iterator;
            }
        }
    }
    return NULL;
}
/*
    Given a valid TIM Image it returns a byte array containing all the indices
    layed out as a regular image.
*/
Byte *TIMExpandCLUTImageData(TIMImage_t *Image)
{
    Byte *Data;
    int x;
    int y;
    int WritePointer;
    
    if( !Image ) {
        DPrintf("TIMExpandCLUTImageData:Invalid Image\n");
        return NULL;
    }
    if( Image->Header.BPP != TIM_IMAGE_BPP_4 && Image->Header.BPP != TIM_IMAGE_BPP_8 ) {
        DPrintf("TIMExpandCLUTImageData:Cannot expand CLUT data on a non-paletted image.\n");
        return NULL;
    }
    Data = malloc(Image->Width * Image->Height );
    if( !Data ) {
        DPrintf("TIMExpandCLUTImageData:Couldn't allocate memory for image.\n");
        return NULL;
    }
    WritePointer = 0;
    if( Image->Header.BPP == TIM_IMAGE_BPP_8  ) {
        for (y = 0; y < Image->Height; y++) {
            for (x = 0; x < Image->RowCount; x++) {
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xFF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] & 0xFF00) >> 8;
                Data[WritePointer++] = ClutIndex0;
                Data[WritePointer++] = ClutIndex1;
            }
        }
    } else if ( Image->Header.BPP == TIM_IMAGE_BPP_4 ) {
        for( y = 0; y < Image->Height; y++ ) {
            for( x = 0; x < Image->RowCount; x++ ) {
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] >> 4) & 0xF;
                Byte ClutIndex2 = (Image->Data[x+Image->RowCount*y] >> 8) & 0xF;
                Byte ClutIndex3 = (Image->Data[x+Image->RowCount*y] >> 12) & 0xF;
                Data[WritePointer++] = ClutIndex0;
                Data[WritePointer++] = ClutIndex1;
                Data[WritePointer++] = ClutIndex2;
                Data[WritePointer++] = ClutIndex3;

            }
        }
    }
    return Data;
}

Byte *TIMToOpenGL32(TIMImage_t *Image)
{
    Byte *Data;
    int x;
    int y;
    int xOffs;
    
    if( !Image ) {
        DPrintf("TIMToOpenGL32:Invalid Image\n");
        return NULL;
    }
    
    Data = malloc(Image->Width * Image->Height * 4);
    
    if( !Data ) {
        DPrintf("TIMToOpenGL32:Couldn't allocate memory for image.\n");
        return NULL;
    }

    if( Image->Header.BPP == TIM_IMAGE_BPP_8  ) {
        for (y = 0; y < Image->Height; y++) {
            xOffs = 0;
            for (x = 0; x < Image->RowCount; x++) {
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xFF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] & 0xFF00) >> 8;
                int BaseLocation0 = (xOffs + Image->Width * y) * 4;
                Data[BaseLocation0 + 0] = GetR(Image->CLUT[ClutIndex0]);
                Data[BaseLocation0 + 1] = GetG(Image->CLUT[ClutIndex0]);
                Data[BaseLocation0 + 2] = GetB(Image->CLUT[ClutIndex0]);
                Data[BaseLocation0 + 3] = GetAlphaValue(Image->CLUT[ClutIndex0]);
                xOffs++;
                int BaseLocation1 = (xOffs + Image->Width * y) * 4;
                Data[BaseLocation1 + 0] = GetR(Image->CLUT[ClutIndex1]);
                Data[BaseLocation1 + 1] = GetG(Image->CLUT[ClutIndex1]);
                Data[BaseLocation1 + 2] = GetB(Image->CLUT[ClutIndex1]);
                Data[BaseLocation1 + 3] = GetAlphaValue(Image->CLUT[ClutIndex1]);
                xOffs++;
            }
        }
    } else if ( Image->Header.BPP == TIM_IMAGE_BPP_4 ) {
        for( y = 0; y < Image->Height; y++ ) {
            xOffs = 0;
            for( x = 0; x < Image->RowCount; x++ ) {
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] >> 4) & 0xF;
                Byte ClutIndex2 = (Image->Data[x+Image->RowCount*y] >> 8) & 0xF;
                Byte ClutIndex3 = (Image->Data[x+Image->RowCount*y] >> 12) & 0xF;

                int BaseLocation0 = (xOffs + Image->Width * y)*4;
                Data[BaseLocation0 + 0] = GetR(Image->CLUT[ClutIndex0]);
                Data[BaseLocation0 + 1] = GetG(Image->CLUT[ClutIndex0]);
                Data[BaseLocation0 + 2] = GetB(Image->CLUT[ClutIndex0]);
                Data[BaseLocation0 + 3] = GetAlphaValue(Image->CLUT[ClutIndex0]);
                xOffs++;
                int BaseLocation1 = (xOffs + Image->Width * y) * 4;
                Data[BaseLocation1 + 0] = GetR(Image->CLUT[ClutIndex1]);
                Data[BaseLocation1 + 1] = GetG(Image->CLUT[ClutIndex1]);
                Data[BaseLocation1 + 2] = GetB(Image->CLUT[ClutIndex1]);
                Data[BaseLocation1 + 3] = GetAlphaValue(Image->CLUT[ClutIndex1]);
                xOffs++;
                int BaseLocation2 = (xOffs + Image->Width * y) * 4;
                Data[BaseLocation2 + 0] = GetR(Image->CLUT[ClutIndex2]);
                Data[BaseLocation2 + 1] = GetG(Image->CLUT[ClutIndex2]);
                Data[BaseLocation2 + 2] = GetB(Image->CLUT[ClutIndex2]);
                Data[BaseLocation2 + 3] = GetAlphaValue(Image->CLUT[ClutIndex2]);
                xOffs++;
                int BaseLocation3 = (xOffs + Image->Width * y) * 4;
                Data[BaseLocation3 + 0] = GetR(Image->CLUT[ClutIndex3]);
                Data[BaseLocation3 + 1] = GetG(Image->CLUT[ClutIndex3]);
                Data[BaseLocation3 + 2] = GetB(Image->CLUT[ClutIndex3]);
                Data[BaseLocation3 + 3] = GetAlphaValue(Image->CLUT[ClutIndex3]);
                xOffs++;
            }
        }
    } else if ( Image->Header.BPP == TIM_IMAGE_BPP_16 ) {
        for( y = 0; y < Image->Height; y++ ) {
            for( x = 0; x < Image->RowCount; x++ ) {
                int BaseLocation0 = (x + Image->Width * y)*4;
                Data[BaseLocation0 + 0] = GetR(Image->Data[x+Image->RowCount*y]);
                Data[BaseLocation0 + 1] = GetG(Image->Data[x+Image->RowCount*y]);
                Data[BaseLocation0 + 2] = GetB(Image->Data[x+Image->RowCount*y]);
                Data[BaseLocation0 + 3] = GetAlphaValue(Image->Data[x+Image->RowCount*y]);
            }
        }
    }
    return Data;
}
Byte *TIMToOpenGL24(TIMImage_t *Image)
{
    Byte *Data;
    int x;
    int y;
    int xOffs;
    
    if( !Image ) {
        DPrintf("TIMToOpenGL24:Invalid Image\n");
        return NULL;
    }
    
    Data = malloc(Image->Width * Image->Height * 3);
    
    if( !Data ) {
        DPrintf("TIMToOpenGL24:Couldn't allocate memory for image.\n");
        return NULL;
    }
    
    if( Image->Header.BPP == TIM_IMAGE_BPP_8  ) {
        for (y = 0; y < Image->Height; y++) {
            xOffs = 0;
            for (x = 0; x < Image->RowCount; x++) {
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xFF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] & 0xFF00) >> 8;
                int BaseLocation0 = (xOffs + Image->Width * y) * 3;
                Data[BaseLocation0 + 0] = GetR(Image->CLUT[ClutIndex0]);
                Data[BaseLocation0 + 1] = GetG(Image->CLUT[ClutIndex0]);
                Data[BaseLocation0 + 2] = GetB(Image->CLUT[ClutIndex0]);
                xOffs++;
                int BaseLocation1 = (xOffs + Image->Width * y) * 3;
                Data[BaseLocation1 + 0] = GetR(Image->CLUT[ClutIndex1]);
                Data[BaseLocation1 + 1] = GetG(Image->CLUT[ClutIndex1]);
                Data[BaseLocation1 + 2] = GetB(Image->CLUT[ClutIndex1]);
                xOffs++;
            }
        }
    } else if ( Image->Header.BPP == TIM_IMAGE_BPP_4 ) {
        for( y = 0; y < Image->Height; y++ ) {
            xOffs = 0;
            for( x = 0; x < Image->RowCount; x++ ) {
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] >> 4) & 0xF;
                Byte ClutIndex2 = (Image->Data[x+Image->RowCount*y] >> 8) & 0xF;
                Byte ClutIndex3 = (Image->Data[x+Image->RowCount*y] >> 12) & 0xF;

                int BaseLocation0 = (xOffs + Image->Width * y)*3;
                Data[BaseLocation0 + 0] = GetR(Image->CLUT[ClutIndex0]);
                Data[BaseLocation0 + 1] = GetG(Image->CLUT[ClutIndex0]);
                Data[BaseLocation0 + 2] = GetB(Image->CLUT[ClutIndex0]);
                xOffs++;
                int BaseLocation1 = (xOffs + Image->Width * y) * 3;
                Data[BaseLocation1 + 0] = GetR(Image->CLUT[ClutIndex1]);
                Data[BaseLocation1 + 1] = GetG(Image->CLUT[ClutIndex1]);
                Data[BaseLocation1 + 2] = GetB(Image->CLUT[ClutIndex1]);
                xOffs++;
                int BaseLocation2 = (xOffs + Image->Width * y) * 3;
                Data[BaseLocation2 + 0] = GetR(Image->CLUT[ClutIndex2]);
                Data[BaseLocation2 + 1] = GetG(Image->CLUT[ClutIndex2]);
                Data[BaseLocation2 + 2] = GetB(Image->CLUT[ClutIndex2]);
                xOffs++;
                int BaseLocation3 = (xOffs + Image->Width * y) * 3;
                Data[BaseLocation3 + 0] = GetR(Image->CLUT[ClutIndex3]);
                Data[BaseLocation3 + 1] = GetG(Image->CLUT[ClutIndex3]);
                Data[BaseLocation3 + 2] = GetB(Image->CLUT[ClutIndex3]);
                xOffs++;
            }
        }
    }
    return Data;
}

Byte **TIMSetRowPointer(png_structp PNGPtr,TIMImage_t *Image)
{
    Byte **RowPointer;
    int x;
    int y;
    int NumComponent;
    //Used in 24BPP mode
    Byte R1;
    Byte G1;
    Byte B1;
    Byte R2;
    Byte G2;
    Byte B2;

    RowPointer = png_malloc  (PNGPtr, Image->Height * sizeof (Byte *));
    if( Image->Header.BPP == TIM_IMAGE_BPP_4  ) {
        for (y = 0; y < Image->Height; y++) {
            Byte *Row = png_malloc (PNGPtr, sizeof (Byte) * Image->Width * 3);
            RowPointer[y] = Row;
            for (x = 0; x < Image->RowCount; x++) {
//                 DPrintf("Pixel is %u 0x%08x\n",Image->Data[x+Image->RowCount*y],Image->Data[x+Image->RowCount*y]);
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] >> 4) & 0xF;
                Byte ClutIndex2 = (Image->Data[x+Image->RowCount*y] >> 8) & 0xF;
                Byte ClutIndex3 = (Image->Data[x+Image->RowCount*y] >> 12) & 0xF;
                *Row++ = GetR(Image->CLUT[ClutIndex0]);
                *Row++ = GetG(Image->CLUT[ClutIndex0]);
                *Row++ = GetB(Image->CLUT[ClutIndex0]);
                *Row++ = GetR(Image->CLUT[ClutIndex1]);
                *Row++ = GetG(Image->CLUT[ClutIndex1]);
                *Row++ = GetB(Image->CLUT[ClutIndex1]);
                *Row++ = GetR(Image->CLUT[ClutIndex2]);
                *Row++ = GetG(Image->CLUT[ClutIndex2]);
                *Row++ = GetB(Image->CLUT[ClutIndex2]);
                *Row++ = GetR(Image->CLUT[ClutIndex3]);
                *Row++ = GetG(Image->CLUT[ClutIndex3]);
                *Row++ = GetB(Image->CLUT[ClutIndex3]);
            }
        }
    }
    if( Image->Header.BPP == TIM_IMAGE_BPP_8  ) {
        for (y = 0; y < Image->Height; y++) {
            Byte *Row = png_malloc (PNGPtr, sizeof (Byte) * Image->Width * 3);
            RowPointer[y] = Row;
            for (x = 0; x < Image->RowCount; x++) {
//             DPrintf("Pixel is %u 0x%08x\n",Image->Data[x+Image->RowCount*y],Image->Data[x+Image->RowCount*y]);
                Byte ClutIndex0 = Image->Data[x+Image->RowCount*y] & 0xFF;
                Byte ClutIndex1 = (Image->Data[x+Image->RowCount*y] & 0xFF00) >> 8;
                *Row++ = GetR(Image->CLUT[ClutIndex0]);
                *Row++ = GetG(Image->CLUT[ClutIndex0]);
                *Row++ = GetB(Image->CLUT[ClutIndex0]);
                *Row++ = GetR(Image->CLUT[ClutIndex1]);
                *Row++ = GetG(Image->CLUT[ClutIndex1]);
                *Row++ = GetB(Image->CLUT[ClutIndex1]);
            }
        }
    }
    
    if( Image->Header.BPP == TIM_IMAGE_BPP_16  ) {
        for (y = 0; y < Image->Height; y++) {
            Byte *Row = png_malloc (PNGPtr, sizeof (Byte) * Image->Width * 3);
            RowPointer[y] = Row;
            for (x = 0; x < Image->RowCount; x++) {
//             DPrintf("Pixel is %u 0x%08x\n",Image->Data[x+Image->RowCount*y],Image->Data[x+Image->RowCount*y]);
                Byte R = GetR(Image->Data[x+Image->RowCount*y]);
                Byte G = GetG(Image->Data[x+Image->RowCount*y]);
                Byte B = GetB(Image->Data[x+Image->RowCount*y]);
                *Row++ = R;
                *Row++ = G;
                *Row++ = B;
            }
        }
    }
    if( Image->Header.BPP == TIM_IMAGE_BPP_24 ) {
        NumComponent = 0;
        for (y = 0; y < Image->Height; y++) {
            Byte *Row = png_malloc (PNGPtr, sizeof (Byte) * Image->Width * 3);
            RowPointer[y] = Row;
            for (x = 0; x < Image->RowCount; x++) {
                switch( NumComponent ) {
                    case 0:
                        R1 = Image->Data[x+Image->RowCount*y] & 0xFF;
                        G1 = (Image->Data[x+Image->RowCount*y] >> 8) & 0xFF;
                        NumComponent++;
                        break;
                    case 1:
                        B1 = Image->Data[x+Image->RowCount*y] & 0xFF;
                        R2 = (Image->Data[x+Image->RowCount*y] >> 8) & 0xFF;
                        NumComponent++;
                        break;
                    case 2:
                        G2 = Image->Data[x+Image->RowCount*y] & 0xFF;
                        B2 = (Image->Data[x+Image->RowCount*y] >> 8) & 0xFF;
                        NumComponent = 0;
                        *Row++ = R1;
                        *Row++ = G1;
                        *Row++ = B1;
                        *Row++ = R2;
                        *Row++ = G2;
                        *Row++ = B2;
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

void TIMWritePNGImage(TIMImage_t *Image,char *OutName)
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
        free(PNGImage);
        return;
    }
    
    PNGInfoPtr = png_create_info_struct (PNGPtr);
    if (PNGInfoPtr == NULL) {
        printf("PNG: Couldn't create info struct!\n");
        png_destroy_write_struct (&PNGPtr, NULL);
        free(PNGImage);
        return;
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
    RowPointer = TIMSetRowPointer(PNGPtr,Image);
    
    png_init_io (PNGPtr, PNGImage);
    png_set_rows (PNGPtr, PNGInfoPtr, RowPointer);
    png_write_png (PNGPtr, PNGInfoPtr, PNG_TRANSFORM_IDENTITY, NULL);
    
    for (y = 0; y < Image->Height; y++) {
        png_free (PNGPtr, RowPointer[y]);
    }
    png_free (PNGPtr, RowPointer);
    png_destroy_write_struct(&PNGPtr,&PNGInfoPtr);
    fclose(PNGImage);
}

void TIMGetPalette(FILE *TIMIMage,TIMImage_t *Image)
{
    unsigned short i;
    
    fread(&Image->Header.CLUTSize,sizeof(Image->Header.CLUTSize),1,TIMIMage);
    fread(&Image->Header.CLUTOrgX,sizeof(Image->Header.CLUTOrgX),1,TIMIMage);
    fread(&Image->Header.CLUTOrgY,sizeof(Image->Header.CLUTOrgY),1,TIMIMage);
    fread(&Image->Header.NumCLUTColors,sizeof(Image->Header.NumCLUTColors),1,TIMIMage);
    fread(&Image->Header.NumCLUTs,sizeof(Image->Header.NumCLUTs),1,TIMIMage);

    DPrintf("CLUTSize is %i\n",Image->Header.CLUTSize);
    DPrintf("CLUTLocation is %ux%u\n",Image->Header.CLUTOrgX,Image->Header.CLUTOrgY);
    DPrintf("NumCLUTColors is %u\n",Image->Header.NumCLUTColors);
    DPrintf("NumCLUTs is %u\n",Image->Header.NumCLUTs);
    Image->CLUT = malloc(Image->Header.NumCLUTColors * sizeof(unsigned short));
    if( !Image->CLUT ) {
        DPrintf("TIMGetPalette:Failed to allocate memory for CLUT data\n");
        return;
    }
    for( i = 0; i < Image->Header.NumCLUTColors; i++ ) {
        fread(&Image->CLUT[i],sizeof(Image->CLUT[i]),1,TIMIMage);
    }
}


TIMImage_t *TIMLoadImage(FILE *TIMImage,const char *FileName,int NumImages)
{
    TIMImage_t *ResultImage;
    char *BaseName;
    char *FinalName;
    float ImageSizeOffset;
    int Ret;
    int i;

    ResultImage = malloc(sizeof(TIMImage_t));
    
    if( !ResultImage ) {
        DPrintf("TIMLoadImage:Failed to allocate memory for image\n");
        return NULL;
    }
    ResultImage->Next = NULL;
    Ret = fread(&ResultImage->Header.Magic,sizeof(ResultImage->Header.Magic),1,TIMImage);
    if( Ret != 1 ) {
        DPrintf("EOF reached.\n");
        free(ResultImage);
        return NULL;
    }
    if( ResultImage->Header.Magic != 0x10 ) {
        DPrintf("Wrong signature detected (%i (%#08x)).\n",ResultImage->Header.Magic,ResultImage->Header.Magic);
        DPrintf("Current offset in file is %li\n",ftell(TIMImage));
        //NOTE(Adriano):Rewind the stream if the Magic number doesn't match.
        fseek(TIMImage,-sizeof(ResultImage->Header.Magic),SEEK_CUR);
        free(ResultImage);
        return NULL;
    }
    DPrintf("Found image at offset %li\n",ftell(TIMImage) - sizeof(ResultImage->Header.Magic));
    fread(&ResultImage->Header.BPP,sizeof(ResultImage->Header.BPP),1,TIMImage);
    if( FileName ) {
        BaseName = GetBaseName(FileName);
        FinalName = SwitchExt(BaseName,"");
        sprintf(ResultImage->Name,"%s-Image-%i",FinalName,NumImages);
        free(BaseName);
        free(FinalName);
    } else {
        sprintf(ResultImage->Name,"Image-%i",NumImages);
    }
    DPrintf("-- %s --\n",ResultImage->Name);
    DPrintf("Magic is %i\n",ResultImage->Header.Magic);
    DPrintf("Flags are %i\n",ResultImage->Header.BPP);
    ImageSizeOffset = 1;
    switch(ResultImage->Header.BPP) {
        case TIM_IMAGE_BPP_4:
            DPrintf("Found image with 4 BPP and CLUT!\n");
            ImageSizeOffset = 4;
            break;
        case TIM_IMAGE_BPP_4_NO_CLUT:
            DPrintf("Found image with 4 BPP without CLUT!\n");
            break;
        case TIM_IMAGE_BPP_8:
            DPrintf("Found image with 8 BPP and CLUT!\n");
            ImageSizeOffset = 2;
            break;
        case TIM_IMAGE_BPP_8_NO_CLUT:
            DPrintf("Found image with 8 BPP without CLUT!\n");
            break;
        case TIM_IMAGE_BPP_16:
            DPrintf("Found image with 16 BPP!\n");
            ImageSizeOffset = 1;
            break;
        case TIM_IMAGE_BPP_24:
            DPrintf("Found image with 24 BPP!\n");
            ImageSizeOffset = 0.5f; //Or 2/3?
            break;
        default:
            DPrintf("Unknown BPP %i found in tim file.\n",ResultImage->Header.BPP);
            free(ResultImage);
            return NULL;
    }
    
    if( ResultImage->Header.BPP == TIM_IMAGE_BPP_4_NO_CLUT || ResultImage->Header.BPP == TIM_IMAGE_BPP_8_NO_CLUT ) {
        DPrintf("Unsupported BPP %i\n",ResultImage->Header.BPP);
        free(ResultImage);
        return NULL;
    }
    
    if( ResultImage->Header.BPP == TIM_IMAGE_BPP_24 ) {
        DPrintf("Warning: BPP 24 was not tested.\n");
    }
    
    if( ResultImage->Header.BPP == TIM_IMAGE_BPP_4 || ResultImage->Header.BPP == TIM_IMAGE_BPP_8 ) {
        TIMGetPalette(TIMImage,ResultImage);
    } else {
        ResultImage->CLUT = NULL;
    }
    fread(&ResultImage->NumPixels,sizeof(ResultImage->NumPixels),1,TIMImage);
    fread(&ResultImage->FrameBufferX,sizeof(ResultImage->FrameBufferX),1,TIMImage);
    fread(&ResultImage->FrameBufferY,sizeof(ResultImage->FrameBufferY),1,TIMImage);
    fread(&ResultImage->Width,sizeof(ResultImage->Width),1,TIMImage);
    fread(&ResultImage->Height,sizeof(ResultImage->Height),1,TIMImage);
    ResultImage->RowCount = ResultImage->Width;


    ResultImage->TexturePage = (ResultImage->FrameBufferX / 64);
    ResultImage->CLUTTexturePage = (ResultImage->Header.CLUTOrgX / 64);

    DPrintf("FrameBuffer Coordinates %ux%u page %i\n",ResultImage->FrameBufferX,ResultImage->FrameBufferY,ResultImage->TexturePage);
    DPrintf("Image has calculated texture page as %i\n",ResultImage->TexturePage);
    //Texture Page is Zero based.
    //Next row.
    if( ResultImage->FrameBufferY >= 256 ) {
        ResultImage->TexturePage += 16;
    }
    if( ResultImage->Header.CLUTOrgY >= 256 ) {
        ResultImage->CLUTTexturePage += 16;
    }
    //Width is stored in 16-pixels unit so we need to offset it based on the current BPP.
    ResultImage->Width *= ImageSizeOffset;
    DPrintf("NumPixels is %i\n",ResultImage->NumPixels);
    DPrintf("FrameBuffer Coordinates %ux%u page %i\n",ResultImage->FrameBufferX,ResultImage->FrameBufferY,ResultImage->TexturePage);
    DPrintf("Image is %ux%u RowCount is %u\n",ResultImage->Width,ResultImage->Height,ResultImage->RowCount);

    ResultImage->Data = malloc((ResultImage->RowCount * ResultImage->Height) * sizeof(unsigned short));
    if( !ResultImage->Data ) {
        DPrintf("TIMLoadImage:Failed to allocate memory for image data\n");
        free(ResultImage);
        return NULL;
    }
    for( i = 0; i < ResultImage->RowCount * ResultImage->Height; i++ ) {
        Ret = fread(&ResultImage->Data[i],sizeof(ResultImage->Data[i]),1,TIMImage);
        assert(Ret == 1);
    }
    return ResultImage;
}

void TIMAppendImageToList(TIMImage_t **ImageList,TIMImage_t *Image)
{
    TIMImage_t *LastNode;
    if( !*ImageList ) {
        *ImageList = Image;
    } else {
        LastNode = *ImageList;
        while( LastNode->Next ) {
            LastNode = LastNode->Next;
        }
        LastNode->Next = Image;
    }
}
TIMImage_t *TIMLoadAllImages(const char *File,int *NumImages)
{
    TIMImage_t *List;
    FILE *TIMFile;
    int LocalNumImages;
    
    List = NULL;
    
    TIMFile = fopen(File,"rb");
    
    if( !TIMFile ) {
        printf("TIMLoadAllImages:Error opening file %s!\n",File);
        return NULL;
    }
    List = NULL;
    LocalNumImages = 0;
    while( 1 ) {
        TIMImage_t *Image = TIMLoadImage(TIMFile,File,LocalNumImages);
        if( Image == NULL ) {
            break;
        }
        TIMAppendImageToList(&List,Image);
        LocalNumImages++;
    }
    DPrintf("TIMLoadAllImages:Loaded %i images\n",LocalNumImages);
    if( NumImages ) {
        *NumImages = LocalNumImages;
    }
    fclose(TIMFile);
    return List;
}
