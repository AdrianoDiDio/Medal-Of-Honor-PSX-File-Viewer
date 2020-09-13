/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
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

/* libpng callbacks */ 
static void png_error_SDL(png_structp ctx, png_const_charp str)
{
	SDL_SetError("libpng: %s\n", str);
}
static void png_write_SDL(png_structp png_ptr, png_bytep data, png_size_t length)
{
	SDL_RWops *rw = (SDL_RWops*)png_get_io_ptr(png_ptr);
	SDL_RWwrite(rw, data, sizeof(png_byte), length);
}


int SDL_SavePNG_RW(SDL_Surface *surface, SDL_RWops *dst, int freedst) 
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_colorp pal_ptr;
	SDL_Palette *pal;
	int i, colortype;
    int rmask = 0x000000ff;
    int gmask = 0x0000ff00;
    int bmask = 0x00ff0000;
	png_bytep *row_pointers;

    /* Initialize and do basic error checking */
	if (!dst)
	{
		SDL_SetError("Argument 2 to SDL_SavePNG_RW can't be NULL, expecting SDL_RWops*\n");
		return -1;
	}
	if (!surface)
	{
		SDL_SetError("Argument 1 to SDL_SavePNG_RW can't be NULL, expecting SDL_Surface*\n");
		if (freedst) SDL_RWclose(dst);
		return -1;
	}
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, png_error_SDL, NULL); /* err_ptr, err_fn, warn_fn */
	if (!png_ptr) 
	{
		SDL_SetError("Unable to png_create_write_struct on %s\n", PNG_LIBPNG_VER_STRING);
		if (freedst) SDL_RWclose(dst);
		return -1;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		SDL_SetError("Unable to png_create_info_struct\n");
		png_destroy_write_struct(&png_ptr, NULL);
		if (freedst) SDL_RWclose(dst);
		return -1;
	}
	if (setjmp(png_jmpbuf(png_ptr)))	/* All other errors, see also "png_error_SDL" */
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		if (freedst) SDL_RWclose(dst);
		return -1;
	}

	/* Setup our RWops writer */
	png_set_write_fn(png_ptr, dst, png_write_SDL, NULL); /* w_ptr, write_fn, flush_fn */

	/* Prepare chunks */
	colortype = PNG_COLOR_MASK_COLOR;
	if (surface->format->BytesPerPixel > 0
	&&  surface->format->BytesPerPixel <= 8
	&& (pal = surface->format->palette))
	{
		colortype |= PNG_COLOR_MASK_PALETTE;
		pal_ptr = (png_colorp)malloc(pal->ncolors * sizeof(png_color));
		for (i = 0; i < pal->ncolors; i++) {
			pal_ptr[i].red   = pal->colors[i].r;
			pal_ptr[i].green = pal->colors[i].g;
			pal_ptr[i].blue  = pal->colors[i].b;
		}
		png_set_PLTE(png_ptr, info_ptr, pal_ptr, pal->ncolors);
		free(pal_ptr);
	}
	else if (surface->format->BytesPerPixel > 3 || surface->format->Amask)
		colortype |= PNG_COLOR_MASK_ALPHA;

	png_set_IHDR(png_ptr, info_ptr, surface->w, surface->h, 8, colortype,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

//	png_set_packing(png_ptr);

	/* Allow BGR surfaces */
	if (surface->format->Rmask == bmask
	&& surface->format->Gmask == gmask
	&& surface->format->Bmask == rmask)
		png_set_bgr(png_ptr);

	/* Write everything */
	png_write_info(png_ptr, info_ptr);
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep)*surface->h);
	for (i = 0; i < surface->h; i++)
		row_pointers[i] = (png_bytep)(Uint8*)surface->pixels + i * surface->pitch;
	png_write_image(png_ptr, row_pointers);
	free(row_pointers);

	png_write_end(png_ptr, info_ptr);

	/* Done */
	png_destroy_write_struct(&png_ptr, &info_ptr);
	if (freedst) SDL_RWclose(dst);
	return 0;
}
#define SDL_SavePNG(surface, file) \
SDL_SavePNG_RW(surface, SDL_RWFromFile(file, "wb"), 1)

void PutTextureToVRam(VRam_t *VRam,TIMImage_t *Image)
{
    int VRAMPage;
    SDL_Surface *Src;
    SDL_Rect SrcRect;
    int DestX;
    int DestY;
    
    VRAMPage = Image->TexturePage;

    if( Image->Header.BPP == BPP_4 ) {
//         return;
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
        SDL_BlitScaled(Src,NULL,VRam->Page4Bit[VRAMPage].Surface,&SrcRect);
    } else if( Image->Header.BPP == BPP_8 ) {
//         return;
//         DPrintf("Image BPP_8 has texture page %i\n",Image->TexturePage);
//         DPrintf("Image BPP_8 has FrameBufferX %i\n",Image->FrameBufferX);
//         DPrintf("Image BPP_8 has FrameBufferY %i\n",Image->FrameBufferY);
        
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
        SDL_BlitScaled(Src,NULL,VRam->Page8Bit[VRAMPage].Surface,&SrcRect);
    }
}

VRam_t *VRamInit(TIMImage_t *ImageList)
{
    VRam_t *VRam;
    TIMImage_t *Iterator;
    int i;

    int VRAMSize;
    
    VRam = malloc(sizeof(VRam_t));
    VRAMSize = sizeof(VRam->Page4Bit) / sizeof(VRam->Page4Bit[0]);
    
    //Set 31 256x256 texture pages and fill them with the image.
    //To debug dump the vram texture to file.
    //3 since it is an RGB image.
    for( i = 0; i < VRAMSize; i++ ) {
        VRam->Page4Bit[i].Surface = SDL_CreateRGBSurface(0,256,256,32, 0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);
        VRam->Page4Bit[i].ID = i + 1;
        VRam->Page8Bit[i].Surface = SDL_CreateRGBSurface(0,256,256,32, 0x000000FF,0x0000FF00,0x00FF0000, 0xFF000000);
        VRam->Page8Bit[i].ID = i + 1;
    }

    for( Iterator = ImageList; Iterator; Iterator = Iterator->Next ) {
        PutTextureToVRam(VRam,Iterator);
    }
#ifdef _DEBUG
    for( i = 0; i < VRAMSize; i++ ) {
        char OutName[256];
        CreateDirIfNotExists("VRAM/VRAM4");
        sprintf(OutName,"VRAM/VRAM4/VRAM%i.png",i);
        SDL_SavePNG(VRam->Page4Bit[i].Surface, OutName);
        
        CreateDirIfNotExists("VRAM/VRAM8");
        sprintf(OutName,"VRAM/VRAM8/VRAM%i.png",i);
        SDL_SavePNG(VRam->Page8Bit[i].Surface, OutName);
    }
#endif
    for( i = 0; i < VRAMSize; i++ ) {
        glGenTextures(1,&VRam->Page4Bit[i].TextureID);
        glBindTexture(GL_TEXTURE_2D,VRam->Page4Bit[i].TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,GL_UNSIGNED_BYTE, VRam->Page4Bit[i].Surface->pixels);
        glBindTexture(GL_TEXTURE_2D,0);
        
        glGenTextures(1,&VRam->Page8Bit[i].TextureID);
        glBindTexture(GL_TEXTURE_2D,VRam->Page8Bit[i].TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,GL_UNSIGNED_BYTE, VRam->Page8Bit[i].Surface->pixels);
        glBindTexture(GL_TEXTURE_2D,0);
    }
    return VRam;
}

