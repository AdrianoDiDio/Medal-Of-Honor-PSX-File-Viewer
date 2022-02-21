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
    int i;
    for( i = 0; i < VRAM->Size; i++ ) {
        CreateDirIfNotExists("VRAM");
        
        CreateDirIfNotExists("VRAM/VRAM4");
        sprintf(OutName,"VRAM/VRAM4/VRAM%i.png",i);
        VRAMWritePNG(VRAM->Page4Bit[i].Surface,OutName);
        
        CreateDirIfNotExists("VRAM/VRAM8");
        sprintf(OutName,"VRAM/VRAM8/VRAM%i.png",i);
        VRAMWritePNG(VRAM->Page8Bit[i].Surface,OutName);
    }
    sprintf(OutName,"VRAM/VRAM.png");
    VRAMWritePNG(VRAM->Page.Surface,OutName);
}

void VRAMDumpDataToFile(VRAM_t *VRAM,char *OutBaseDir)
{
    
    char OutName[1024];
    char Buffer[256];
    FILE *MaterialFile;
    int i;
    
    sprintf(OutName,"%svram_4_8_bit.mtl",OutBaseDir);
    MaterialFile = fopen(OutName,"w");
    for( i = 0; i < VRAM->Size; i++ ) {
        sprintf(OutName,"%svram_4_bit_page_%i.png",OutBaseDir,i);
        VRAMWritePNG(VRAM->Page4Bit[i].Surface,OutName);
        sprintf(Buffer,"newmtl vram_4_page_%i\nKa 1.000 1.000 1.000\nKd 1.000 1.000 1.000\nKs 1.000 1.000 1.000\nmap_Kd vram_4_bit_page_%i.png\n", i,i);
        fwrite(Buffer,strlen(Buffer),1,MaterialFile);
        
        sprintf(OutName,"%svram_8_bit_page_%i.png",OutBaseDir,i);
        VRAMWritePNG(VRAM->Page8Bit[i].Surface,OutName);
        sprintf(Buffer,"newmtl vram_8_page_%i\nKa 1.000 1.000 1.000\nKd 1.000 1.000 1.000\nKs 1.000 1.000 1.000\nmap_Kd vram_8_bit_page_%i.png\n", i,i );
        fwrite(Buffer,strlen(Buffer),1,MaterialFile);
    }
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
    
    VRAMPage = Image->TexturePage;

    if( Image->Header.BPP == BPP_4 ) {
        if( Image->FrameBufferY >= 256 ) {
            DestX = (Image->FrameBufferX - ((Image->TexturePage - 16) * 64)) * 4;
            DestY = Image->FrameBufferY - 256;
            
        } else {
            DestX = (Image->FrameBufferX - (Image->TexturePage * 64)) * 4;
            DestY = Image->FrameBufferY;
        }
        
        SrcRect.x = DestX;
        SrcRect.y = DestY;
        SrcRect.w = Image->Width;
        SrcRect.h = Image->Height;
        Byte *Data = TimToOpenGL32(Image);
        Src = SDL_CreateRGBSurfaceFrom(Data,Image->Width,Image->Height,32,4 * Image->Width,0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);
        SDL_BlitScaled(Src,NULL,VRAM->Page4Bit[VRAMPage].Surface,&SrcRect);
    } else if( Image->Header.BPP == BPP_8 ) {        
        if( Image->FrameBufferY >= 256 ) {
            DestX = (Image->FrameBufferX - ((Image->TexturePage - 16) * 64)) * 2;
            DestY = Image->FrameBufferY - 256;
        } else {
            DestX = (Image->FrameBufferX - (Image->TexturePage * 64 )) * 2;
            DestY = Image->FrameBufferY;
        }
        
        SrcRect.x = DestX;
        SrcRect.y = DestY;
        SrcRect.w = Image->Width;
        SrcRect.h = Image->Height;
        Byte *Data = TimToOpenGL32(Image);
        Src = SDL_CreateRGBSurfaceFrom(Data,Image->Width,Image->Height,32,4 * Image->Width,0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);
        SDL_BlitScaled(Src,NULL,VRAM->Page8Bit[VRAMPage].Surface,&SrcRect);
    }
    
    if( Image->Header.BPP == BPP_4 ) {
        if( Image->FrameBufferY >= 256 ) {
            DestX = (Image->FrameBufferX - ((Image->TexturePage - 16) * 64)) * 4;
            DestY = Image->FrameBufferY - 256;
            
        } else {
            DestX = (Image->FrameBufferX - (Image->TexturePage * 64)) * 4;
            DestY = Image->FrameBufferY;
        }

        SrcRect.x = VRAMGetTexturePageX(VRAMPage) + DestX;
        SrcRect.y = VRAMGetTexturePageY(VRAMPage,Image->Header.BPP) + DestY;
        SrcRect.w = Image->Width;
        SrcRect.h = Image->Height;
        Byte *Data = TimToOpenGL32(Image);
        Src = SDL_CreateRGBSurfaceFrom(Data,Image->Width,Image->Height,32,4 * Image->Width,0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);
        SDL_BlitScaled(Src,NULL,VRAM->Page.Surface,&SrcRect);
    } else if( Image->Header.BPP == BPP_8 ) {       
        if( Image->FrameBufferY >= 256 ) {
            DestX = (Image->FrameBufferX - ((Image->TexturePage - 16) * 64)) * 2;
            DestY = Image->FrameBufferY - 256;
        } else {
            DestX = (Image->FrameBufferX - (Image->TexturePage * 64 )) * 2;
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

}

VRAM_t *VRAMInit(TIMImage_t *ImageList)
{
    VRAM_t *VRAM;
    TIMImage_t *Iterator;
    int i;

    
    VRAM = malloc(sizeof(VRAM_t));
    VRAM->Size = sizeof(VRAM->Page4Bit) / sizeof(VRAM->Page4Bit[0]);
    
    //Set 32 256x256 texture pages and fill them with the image.
    //To debug dump the vram texture to file.
    //3 since it is an RGB image.
    for( i = 0; i < VRAM->Size; i++ ) {
        VRAM->Page4Bit[i].Surface = SDL_CreateRGBSurface(0,256,256,32, 0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);
        VRAM->Page4Bit[i].ID = i + 1;
        VRAM->Page8Bit[i].Surface = SDL_CreateRGBSurface(0,256,256,32, 0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);
        VRAM->Page8Bit[i].ID = i + 1;
    }
    
    VRAM->Page.Width = 4096.f;
    VRAM->Page.Height = 1024.f;
    
    VRAM->Page.Surface = SDL_CreateRGBSurface(0,VRAM->Page.Width,VRAM->Page.Height,32, 0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);


    for( Iterator = ImageList; Iterator; Iterator = Iterator->Next ) {
        VRAMPutTexture(VRAM,Iterator);
    }
#ifdef _DEBUG
    VRAMDump(VRAM);
#endif
    for( i = 0; i < VRAM->Size; i++ ) {
        glGenTextures(1,&VRAM->Page4Bit[i].TextureID);
        glBindTexture(GL_TEXTURE_2D,VRAM->Page4Bit[i].TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,GL_UNSIGNED_BYTE, VRAM->Page4Bit[i].Surface->pixels);
        glBindTexture(GL_TEXTURE_2D,0);
        
        glGenTextures(1,&VRAM->Page8Bit[i].TextureID);
        glBindTexture(GL_TEXTURE_2D,VRAM->Page8Bit[i].TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,GL_UNSIGNED_BYTE, VRAM->Page8Bit[i].Surface->pixels);
        glBindTexture(GL_TEXTURE_2D,0);
    }

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

