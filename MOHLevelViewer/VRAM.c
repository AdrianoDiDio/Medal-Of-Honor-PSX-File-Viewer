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

void VRAMWritePNG(SDL_Surface *ImageSurface,char *OutName)
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
    
    if( ImageSurface == NULL ) {
        printf("Couldn't dump %s\n",OutName);
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
                  ImageSurface->w,
                  ImageSurface->h,
                  8, // Depth
                  PNG_COLOR_TYPE_RGBA,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    png_init_io (PNGPtr, PNGImage);
    RowPointer = png_malloc  (PNGPtr, ImageSurface->h * sizeof (Byte *));
    for (y = 0; y < ImageSurface->h; y++) {
        RowPointer[y] = ImageSurface->pixels + y * ImageSurface->pitch;
    }
    png_set_rows (PNGPtr, PNGInfoPtr, RowPointer);
    png_write_png (PNGPtr, PNGInfoPtr, PNG_TRANSFORM_IDENTITY, NULL);
    png_free (PNGPtr, RowPointer);
    fclose(PNGImage);
}

void VRAMDump(VRAM_t *VRAM)
{
    char OutName[256];
    CreateDirIfNotExists("VRAM");
    sprintf(OutName,"VRAM/VRAM.png");
    VRAMWritePNG(VRAM->Page.Surface,OutName);
}

void VRAMDumpDataToFile(VRAM_t *VRAM,char *OutBaseDir)
{
    
    char OutName[1024];
    char Buffer[256];
    FILE *MaterialFile;
    
    sprintf(OutName,"%svram.mtl",OutBaseDir);
    MaterialFile = fopen(OutName,"w");

    sprintf(OutName,"%svram.png",OutBaseDir);
    VRAMWritePNG(VRAM->Page.Surface,OutName);
    sprintf(Buffer,"newmtl vram\nKa 1.000 1.000 1.000\nKd 1.000 1.000 1.000\nKs 1.000 1.000 1.000\nmap_Kd vram.png\n");
    fwrite(Buffer,strlen(Buffer),1,MaterialFile);
    fclose(MaterialFile);
}

float VRAMGetTexturePageX(int VRAMPage)
{
    return ((VRAMPage % 16) * 256.f);
}

float VRAMGetTexturePageY(int VRAMPage,int ColorMode)
{
    float ModeOffset;
    float PageY;
    if( ColorMode == BPP_4 || ColorMode == 0) {
        ModeOffset = 0.f;
    } else {
        ModeOffset = 512.f;
    }
    PageY = ModeOffset;
    if( VRAMPage > 16 ) {
        PageY += 256.f;
    }
    return PageY;
}

void VRAMPutTexture(VRAM_t *VRAM,TIMImage_t *Image)
{
    int VRAMPage;
    SDL_Surface *Src;
    SDL_Rect SrcRect;
    int DestX;
    int DestY;
    float ColorOffsetMultiplier;
    
    VRAMPage = Image->TexturePage;
    
    if( Image->Header.BPP == BPP_4 ) {
        ColorOffsetMultiplier = 4;
    } else {
        ColorOffsetMultiplier = 2;
    }
    
    if( Image->FrameBufferY >= 256 ) {
        DestX = (Image->FrameBufferX - ((Image->TexturePage - 16) * 64)) * ColorOffsetMultiplier;
        DestY = Image->FrameBufferY - 256;
    } else {
        DestX = (Image->FrameBufferX - (Image->TexturePage * 64)) * ColorOffsetMultiplier;
        DestY = Image->FrameBufferY;
    }
    SrcRect.x = VRAMGetTexturePageX(VRAMPage) + DestX;
    SrcRect.y = VRAMGetTexturePageY(VRAMPage,Image->Header.BPP) + DestY;
    SrcRect.w = Image->Width;
    SrcRect.h = Image->Height;
    Byte *Data = TimToOpenGL32(Image);
    Src = SDL_CreateRGBSurfaceFrom(Data,Image->Width,Image->Height,32,4 * Image->Width,0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);
    SDL_BlitScaled(Src,NULL,VRAM->Page.Surface,&SrcRect);
}

VRAM_t *VRAMInit(TIMImage_t *ImageList)
{
    VRAM_t *VRAM;
    TIMImage_t *Iterator;
    
    VRAM = malloc(sizeof(VRAM_t));    

    VRAM->Page.Width = 4096.f;
    VRAM->Page.Height = 1024.f;
    
    VRAM->Page.Surface = SDL_CreateRGBSurface(0,VRAM->Page.Width,VRAM->Page.Height,32, 0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);


    for( Iterator = ImageList; Iterator; Iterator = Iterator->Next ) {
        VRAMPutTexture(VRAM,Iterator);
    }
#ifdef _DEBUG
    VRAMDump(VRAM);
#endif
    glGenTextures(1,&VRAM->Page.TextureID);
    glBindTexture(GL_TEXTURE_2D,VRAM->Page.TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VRAM->Page.Width, VRAM->Page.Height, 0, GL_RGBA,GL_UNSIGNED_BYTE, VRAM->Page.Surface->pixels);
    glBindTexture(GL_TEXTURE_2D,0);
    return VRAM;
}

