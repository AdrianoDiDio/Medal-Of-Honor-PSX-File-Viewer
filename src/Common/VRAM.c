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

#include "VRAM.h"

void VRAMFree(VRAM_t *VRAM)
{
    glDeleteTextures(1,&VRAM->Page.TextureId);
    glDeleteTextures(1,&VRAM->TextureIndexPage.TextureId);
    glDeleteTextures(1,&VRAM->PalettePage.TextureId);

    SDL_FreeSurface(VRAM->Page.Surface);
    free(VRAM);
}
void VRAMWritePNG(SDL_Surface *ImageSurface,const char *OutName)
{
    FILE *PNGImage;
    png_structp PNGPtr;
    png_infop PNGInfoPtr;
    Byte **RowPointer;
    int y;

    if( ImageSurface == NULL ) {
        printf("Couldn't dump %s\n",OutName);
        return;
    }
    
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
    png_destroy_write_struct(&PNGPtr,&PNGInfoPtr);
    fclose(PNGImage);
}
void VRAMSave(VRAM_t *VRAM,const char *File)
{
    VRAMWritePNG(VRAM->Page.Surface,File);
}
void VRAMDump(VRAM_t *VRAM)
{
    char OutName[256];
    CreateDirIfNotExists("VRAM");
    sprintf(OutName,"VRAM%cVRAM.png",PATH_SEPARATOR);
    VRAMSave(VRAM,OutName);
}

void VRAMDumpDataToFile(VRAM_t *VRAM,const char *OutBaseDir)
{
    
    char OutName[1024];
    char Buffer[256];
    FILE *MaterialFile;
    
    sprintf(OutName,"%s%cvram.mtl",OutBaseDir,PATH_SEPARATOR);
    MaterialFile = fopen(OutName,"w");

    sprintf(OutName,"%s%cvram.png",OutBaseDir,PATH_SEPARATOR);
    VRAMSave(VRAM,OutName);
    sprintf(Buffer,"newmtl vram\nKa 1.000 1.000 1.000\nKd 1.000 1.000 1.000\nKs 1.000 1.000 1.000\nmap_Kd vram.png\n");
    fwrite(Buffer,strlen(Buffer),1,MaterialFile);
    fclose(MaterialFile);
}

int VRAMGetTexturePageX(int VRAMPage)
{
    return ((VRAMPage % 16) * 256);
}

int VRAMGetTexturePageY(int VRAMPage,int ColorMode)
{
    float ModeOffset;
    float PageY;
    if( ColorMode == TIM_IMAGE_BPP_4 || ColorMode == 0) {
        ModeOffset = 0;
    } else {
        ModeOffset = 512;
    }
    PageY = ModeOffset;
    if( VRAMPage >= 16 ) {
        PageY += 256;
    }
    return PageY;
}

int VRAMGetCLUTPage(int CLUTPosX,int CLUTPosY)
{
    int CLUTPage;
    CLUTPage = CLUTPosX / 64;
    if( CLUTPosY >= 256 ) {
        CLUTPage += 16;
    }
    return CLUTPage;
}
int VRAMGetCLUTOffsetY(int ColorMode)
{
    if( ColorMode == TIM_IMAGE_BPP_8 || ColorMode == 1 ) {
        return 512;
    }
    
    return 0;
}

int VRAMGetCLUTPositionX(int CLUTX,int CLUTY,int CLUTPage)
{
    int Offset;
    if( CLUTY >= 256 ) {
        Offset = (CLUTX - ((CLUTPage - 16) * 64));
    } else {
        Offset = (CLUTX - (CLUTPage * 64));
    }
    return Offset;
}

void VRAMGetTIMImageCoordinates(TIMImage_t *Image,int *ImageX,int *ImageY)
{
    int ColorOffsetMultiplier;
    int DestX;
    int DestY;
    
    if( Image->Header.BPP == TIM_IMAGE_BPP_4 ) {
        ColorOffsetMultiplier = 4;
    } else if( Image->Header.BPP == TIM_IMAGE_BPP_8) {
        ColorOffsetMultiplier = 2;
    } else {
        ColorOffsetMultiplier = 1;
    }
    
    if( Image->FrameBufferY >= 256 ) {
        DestX = (Image->FrameBufferX - ((Image->TexturePage - 16) * 64)) * ColorOffsetMultiplier;
        DestY = Image->FrameBufferY - 256;
    } else {
        DestX = (Image->FrameBufferX - (Image->TexturePage * 64)) * ColorOffsetMultiplier;
        DestY = Image->FrameBufferY;
    }
    if( ImageX ) {
        *ImageX = DestX;
    }
    if( ImageY ) {
        *ImageY = DestY;
    }
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
    VRAMGetTIMImageCoordinates(Image,&DestX,&DestY);
    SrcRect.x = VRAMGetTexturePageX(VRAMPage) + DestX;
    SrcRect.y = VRAMGetTexturePageY(VRAMPage,Image->Header.BPP) + DestY;
    SrcRect.w = Image->Width;
    SrcRect.h = Image->Height;
    Byte *Data = TIMToOpenGL32(Image);
    Src = SDL_CreateRGBSurfaceFrom(Data,Image->Width,Image->Height,32,4 * Image->Width,0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);
    SDL_BlitScaled(Src,NULL,VRAM->Page.Surface,&SrcRect);
    SDL_FreeSurface(Src);
    free(Data);
}
void VRAMPutRawTexture(VRAM_t *VRAM,TIMImage_t *Image)
{
    int VRAMPage;
    SDL_Rect SrcRect;
    int DestX;
    int DestY;
    float ColorOffsetMultiplier;
    Byte *ImageData;
    
    VRAMPage = Image->TexturePage;
    VRAMGetTIMImageCoordinates(Image,&DestX,&DestY);
    SrcRect.x = VRAMGetTexturePageX(VRAMPage) + DestX;
    SrcRect.y = VRAMGetTexturePageY(VRAMPage,Image->Header.BPP) + DestY;
    SrcRect.w = Image->Width;
    SrcRect.h = Image->Height;    

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    ImageData = TIMExpandCLUTImageData(Image);
    if( ImageData == NULL ) {
        DPrintf("VRAMPutRAWTexture:Failed to expand image %s\n",Image->Name);
        return;
    }
    glBindTexture(GL_TEXTURE_2D,VRAM->TextureIndexPage.TextureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, SrcRect.x, SrcRect.y, SrcRect.w, SrcRect.h, GL_RED_INTEGER, GL_UNSIGNED_BYTE, ImageData);
    glBindTexture(GL_TEXTURE_2D,0);
    free(ImageData);
}
void VRAMPutCLUT(VRAM_t *VRAM,TIMImage_t *Image)
{
    SDL_Rect SrcRect;
    int DestX;
    int DestY;

    //TODO:Special case decide whether we want to blit image into CLUT or
    //     create a new storage for direct textures to be bind and passed to shaders.
    //     Either way we need to pass ColorMode to shader in order to fetch the correct data!
    //     (NOTE):An example of 16-bit texture can be found in MOH:Mission 7 Level 2!
    if( Image->Header.BPP == TIM_IMAGE_BPP_16 ) {
        return;
    }

    DestX = VRAMGetCLUTPositionX(Image->Header.CLUTOrgX,Image->Header.CLUTOrgY,Image->CLUTTexturePage);
    DestY = Image->Header.CLUTOrgY + VRAMGetCLUTOffsetY(Image->Header.BPP);
    DestX += VRAMGetTexturePageX(Image->CLUTTexturePage);

    SrcRect.x = DestX;
    SrcRect.y = DestY;
    SrcRect.w = Image->Header.NumCLUTColors;
    SrcRect.h = Image->Header.NumCLUTs;
    if( Image->Header.BPP == TIM_IMAGE_BPP_4 ) {
        if( SrcRect.w > 16 ) {
            SrcRect.w = 16;
        }
    } else {
        if( SrcRect.w > 256 ) {
            SrcRect.w = 256;
        }
    }
    glBindTexture(GL_TEXTURE_2D,VRAM->PalettePage.TextureId);
    glTexSubImage2D(GL_TEXTURE_2D,0, SrcRect.x, SrcRect.y, SrcRect.w, SrcRect.h, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, Image->CLUT);
    glBindTexture(GL_TEXTURE_2D,0);
}

void VRAMPutDirectModeIntoCLUT(VRAM_t *VRAM,TIMImage_t *Image)
{
    int VRAMPage;
    SDL_Rect SrcRect;
    int DestX;
    int DestY;
    
    VRAMPage = Image->TexturePage;
    
    //NOTE:Guarding against 24-bpp images which are not tested yet.
    assert(Image->Header.BPP == TIM_IMAGE_BPP_16);
    VRAMGetTIMImageCoordinates(Image,&DestX,&DestY);
    SrcRect.x = VRAMGetTexturePageX(VRAMPage) + DestX;
    SrcRect.y = VRAMGetTexturePageY(VRAMPage,Image->Header.BPP) + DestY;
    SrcRect.w = Image->Width;
    SrcRect.h = Image->Height;
    glBindTexture(GL_TEXTURE_2D,VRAM->PalettePage.TextureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, SrcRect.x, SrcRect.y, SrcRect.w, SrcRect.h, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, 
                        Image->Data);
    glBindTexture(GL_TEXTURE_2D,0);
}

VRAM_t *VRAMInit(TIMImage_t *ImageList)
{
    VRAM_t *VRAM;
    TIMImage_t *Iterator;
    
    VRAM = malloc(sizeof(VRAM_t));
    
    if( !VRAM ) {
        DPrintf("VRAMInit:Failed to allocate memory for struct\n");
        return NULL;
    }

    VRAM->Page.Width = 4096.f;
    VRAM->Page.Height = 1024.f;
    
    VRAM->Page.Surface = SDL_CreateRGBSurface(0,VRAM->Page.Width,VRAM->Page.Height,32, 0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);

    glGenTextures(1,&VRAM->PalettePage.TextureId);
    glBindTexture(GL_TEXTURE_2D,VRAM->PalettePage.TextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexStorage2D(GL_TEXTURE_2D,1,GL_RGB5_A1,VRAM->Page.Width, VRAM->Page.Height);
    glBindTexture(GL_TEXTURE_2D,0);

    glGenTextures(1,&VRAM->TextureIndexPage.TextureId);
    glBindTexture(GL_TEXTURE_2D,VRAM->TextureIndexPage.TextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexStorage2D(GL_TEXTURE_2D,1,GL_R8UI,VRAM->Page.Width, VRAM->Page.Height);
    glBindTexture(GL_TEXTURE_2D,0);


    for( Iterator = ImageList; Iterator; Iterator = Iterator->Next ) {
        //NOTE(Adriano):This guard is used in case there are 24-bits textures that requires loading.
        //At the moment only 16-BPP are used in MOH:MSN7LVL2.
        assert(Iterator->Header.BPP != TIM_IMAGE_BPP_24);
        VRAMPutTexture(VRAM,Iterator);
        if( Iterator->Header.BPP == TIM_IMAGE_BPP_16 ) {
            VRAMPutDirectModeIntoCLUT(VRAM,Iterator);
        } else {
            VRAMPutRawTexture(VRAM,Iterator);
            VRAMPutCLUT(VRAM,Iterator);
        }
    }
#ifdef _DEBUG
    VRAMDump(VRAM);
#endif
    glGenTextures(1,&VRAM->Page.TextureId);
    glBindTexture(GL_TEXTURE_2D,VRAM->Page.TextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VRAM->Page.Width, VRAM->Page.Height, 0, GL_RGBA,GL_UNSIGNED_BYTE, VRAM->Page.Surface->pixels);
    glBindTexture(GL_TEXTURE_2D,0);
    return VRAM;
}

