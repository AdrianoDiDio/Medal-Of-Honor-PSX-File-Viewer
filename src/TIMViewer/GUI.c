// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2023 Adriano Di Dio.
    
    TIMViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TIMViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TIMViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#include "GUI.h"
#include "../Common/VRAM.h"
#include "TIMViewer.h"

void GUIFree(GUI_t *GUI)
{
    GUIReleaseContext(GUI->DefaultContext);
    ProgressBarDestroy(GUI->ProgressBar);
    FileDialogListFree();
    ErrorMessageDialogFree(GUI->ErrorMessageDialog);
    free(GUI->ConfigFilePath);
    free(GUI);
}

void GUIProcessEvent(GUI_t *GUI,SDL_Event *Event)
{
    ImGui_ImplSDL2_ProcessEvent(Event);
}

void GUIDrawMenuBar(Application_t *Application)
{
    if( !igBeginMainMenuBar() ) {
        return;
    }
    if (igBeginMenu("File",true)) {
        if( igMenuItem_Bool("Open",NULL,false,true) ) {
            ImageManagerOpenFileDialog(Application->ImageManager,Application->GUI,Application->Engine->VideoSystem);
        }
        if( igMenuItem_Bool("Exit",NULL,false,true) ) {
            Quit(Application);
        }
        igEndMenu();
    }
    if (igBeginMenu("Settings",true)) {
        if( igMenuItem_Bool("Video",NULL,Application->GUI->VideoSettingsWindowHandle,true) ) {
            Application->GUI->VideoSettingsWindowHandle = !Application->GUI->VideoSettingsWindowHandle;
        }
        igEndMenu();
    }
    igEndMainMenuBar();
}
void GUIDrawTextureWindow(ImageManager_t *ImageManager,float ColumnWidth)
{
    ImVec2 UV0;
    ImVec2 UV1;
    ImVec2 Size;
    ImVec4 TintColor;
    ImVec4 BorderColor;
    int VRAMPage;
    int DestX;
    int DestY;
    float BeginX;
    float BeginY;
    float EndX;
    float EndY;
    
    UV0.x = 0;
    UV0.y = 0;
    Size.x = 256;
    Size.y = 256;
    UV1.x = 1;
    UV1.y = 1;
    TintColor.x = 1;
    TintColor.y = 1;
    TintColor.z = 1;
    TintColor.w = 1;
    BorderColor.x = 0;
    BorderColor.y = 0;
    BorderColor.z = 0;
    BorderColor.w = 0;
    ImVec2 CursorPosition;
//     igBeginChild_Str("Current Texture",ZeroSize,false,0);
    igGetCursorScreenPos(&CursorPosition);
    if( !ImageManager->SelectedImage ) {
        return;
    }
    igBeginGroup();
    igText("Current Image:");
//     GUIDrawTitleBar("Current Texture",50);
    VRAMPage = ImageManager->SelectedImage->TexturePage;
    VRAMGetTIMImageCoordinates(ImageManager->SelectedImage,&DestX,&DestY);
    BeginX = VRAMGetTexturePageX(VRAMPage) + DestX;
    BeginY = VRAMGetTexturePageY(VRAMPage,ImageManager->SelectedImage->Header.BPP) + DestY;
    EndX = BeginX + ImageManager->SelectedImage->Width;
    EndY = BeginY + ImageManager->SelectedImage->Height;
    UV0.x = BeginX / ImageManager->VRAM->Page.Width;
    UV0.y = BeginY / ImageManager->VRAM->Page.Height;
    UV1.x = EndX / ImageManager->VRAM->Page.Width;
    UV1.y = EndY / ImageManager->VRAM->Page.Height;
    Size.x = 128;
    Size.y = 128;
    igBeginGroup();
    igText("FrameBuffer Position:%ix%i",ImageManager->SelectedImage->FrameBufferX,ImageManager->SelectedImage->FrameBufferY);
    if( ImageManager->SelectedImage->Header.BPP == TIM_IMAGE_BPP_4 || ImageManager->SelectedImage->Header.BPP == TIM_IMAGE_BPP_8 ) {
        igText("CLUT Position:%ix%i",ImageManager->SelectedImage->Header.CLUTOrgX,ImageManager->SelectedImage->Header.CLUTOrgY);
        igText("CLUT Size:%i",ImageManager->SelectedImage->Header.CLUTSize);
        igText("Number of CLUT tables:%i",ImageManager->SelectedImage->Header.NumCLUTs);
        igText("Number of Colors inside CLUT:%i",ImageManager->SelectedImage->Header.NumCLUTColors);
    } else {
        igText("Image has no CLUT data\n");
    }
    igText("Image Size:%ix%i",ImageManager->SelectedImage->Width,ImageManager->SelectedImage->Height);
    igText("BPP:%s",TIMGetBPPFromImage(ImageManager->SelectedImage));
    igEndGroup();
    igSameLine(CursorPosition.x,ColumnWidth - Size.x - 20.f);
    igBeginGroup();
    igImage((void*)(intptr_t)ImageManager->VRAM->Page.TextureId, Size,UV0,UV1,TintColor,BorderColor);
    igEndGroup();
    igEndGroup();
}

void GUIDrawVRAMWindow(GUI_t *GUI,ImageManager_t *ImageManager)
{
    ImGuiIO *IO;
    
    ImVec2 UV0;
    ImVec2 UV1;
    ImVec2 Size;
    ImVec4 TintColor;
    ImVec4 BorderColor;
    ImVec2 ScaleFactor;
    int VRAMPage;
    int DestX;
    int DestY;
    ImVec2 Min;
    ImVec2 Max;
    ImVec2 CursorPosition;
    ImVec2 TextPosition;
    ImVec2 MousePosition;
    ImVec2 ZeroSize;
    TIMImage_t *Image;
    ImDrawList *DrawList;
    float WheelFactor;
    static float Zoom = 1.f;
    
    UV0.x = 0;
    UV0.y = 0;
    Size.x = 256;
    Size.y = 256;
    UV1.x = 1;
    UV1.y = 1;
    TintColor.x = 1;
    TintColor.y = 1;
    TintColor.z = 1;
    TintColor.w = 1;
    BorderColor.x = 0;
    BorderColor.y = 0;
    BorderColor.z = 0;
    BorderColor.w = 0;
    ZeroSize.x = 0;
    ZeroSize.y = 0;
    IO = igGetIO();
    
    if( !ImageManager->SelectedImage || !ImageManager->VRAM ) {
        return;
    }

    igBeginChild_Str("VRAM Viewer",ZeroSize,false, ImGuiWindowFlags_HorizontalScrollbar);
    
    igSetItemKeyOwner(ImGuiKey_MouseWheelY,0);
    if (igIsWindowHovered(0) && ( igIsKeyDown_Nil(ImGuiKey_LeftCtrl) || igIsKeyDown_Nil(ImGuiKey_RightCtrl) ) ) {
        WheelFactor = IO->MouseWheel;
        if( WheelFactor ) {
            Zoom += WheelFactor;
            if( Zoom < 1 ) {
                Zoom = 1;
            }
        }
    }
    igText("VRAM Viewer");
    igText("Press and Hold Ctrl key while scrolling the mouse wheel to zoom in or out");
    igSeparator();
    VRAMPage = ImageManager->SelectedImage->TexturePage;
    VRAMGetTIMImageCoordinates(ImageManager->SelectedImage,&DestX,&DestY);
    igGetCursorScreenPos(&CursorPosition);
    Size.x = 512 * Zoom;
    Size.y = 512 * Zoom;
    ScaleFactor.x = ImageManager->VRAM->Page.Width / Size.x;
    ScaleFactor.y = ImageManager->VRAM->Page.Height / Size.y;

    Min.x = CursorPosition.x + ( (VRAMGetTexturePageX(VRAMPage) + DestX) / ScaleFactor.x );
    Min.y = CursorPosition.y + ( (VRAMGetTexturePageY(VRAMPage,ImageManager->SelectedImage->Header.BPP) + DestY) / ScaleFactor.y );
    Max.x = Min.x + (ImageManager->SelectedImage->Width / ScaleFactor.x );
    Max.y = Min.y + (ImageManager->SelectedImage->Height / ScaleFactor.y );
    UV0.x = 0;
    UV0.y = 0;
    UV1.x = 1;
    UV1.y = 1;

    DrawList = igGetWindowDrawList();
    igImage((void*)(intptr_t)ImageManager->VRAM->Page.TextureId, Size,UV0,UV1,TintColor,BorderColor);
    ImDrawList_AddRect(DrawList,Min,Max,0xFF0000FF,0.f,0,1.f);
    TextPosition.x = CursorPosition.x;
    TextPosition.y = CursorPosition.y;
    ImDrawList_AddText_Vec2(DrawList,TextPosition,0xFFFFFFFF,"4-BPP",NULL);
    TextPosition.x = CursorPosition.x;
    TextPosition.y = CursorPosition.y + (Size.y / 2);
    ImDrawList_AddText_Vec2(DrawList,TextPosition,0xFFFFFFFF,"8-BPP And No CLUT textures",NULL);
    if( igIsItemHovered(0) && igIsMouseClicked_Bool(ImGuiMouseButton_Left,false) ) {
        igGetMousePos(&MousePosition);
        for( Image = ImageManager->ImageList; Image; Image = Image->Next ) {
            VRAMPage = Image->TexturePage;
            VRAMGetTIMImageCoordinates(Image,&DestX,&DestY);
            Min.x = CursorPosition.x + ( (VRAMGetTexturePageX(VRAMPage) + DestX) / ScaleFactor.x );
            Min.y = CursorPosition.y + ( (VRAMGetTexturePageY(VRAMPage,Image->Header.BPP) + DestY) / ScaleFactor.y );
            Max.x = Min.x + (Image->Width / ScaleFactor.x );
            Max.y = Min.y + (Image->Height / ScaleFactor.y );
            if( MousePosition.x >= Min.x && MousePosition.y >= Min.y && 
                MousePosition.x <= Max.x && MousePosition.y <= Max.y ) {
                ImageManager->SelectedImage = Image;
                GUI->ImageSelectionChanged = 1;
                break;
            }
        }
    }
    igEnd();
}

void GUIDrawMainWindow(GUI_t *GUI,VideoSystem_t *VideoSystem,ImageManager_t *ImageManager)
{
    ImGuiViewport *Viewport;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    ImVec2 MaxSize;
    ImVec2 ZeroSize;
    ImVec2 TablePadding;
    ImGuiTreeNodeFlags TreeNodeFlags;
    ImGuiTableFlags TableFlags;
    TIMImage_t *ImageIterator;

    Viewport = igGetMainViewport();
    WindowPosition.x = Viewport->WorkPos.x;
    WindowPosition.y = Viewport->WorkPos.y;
    WindowPivot.x = 0.f;
    WindowPivot.y = 0.f;
    MaxSize.x = Viewport->WorkSize.x;
    MaxSize.y = Viewport->WorkSize.y;
    ZeroSize.x = 0.f;
    ZeroSize.y = 0.f;
    
    igSetNextWindowSize(MaxSize,0);
    igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);

    if( !igBegin("Main Window", NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | 
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus
    ) ) {
        return;
    }
    
    TableFlags = ImGuiTableFlags_BordersInnerV;
    TablePadding.x = 10.f;
    TablePadding.y = 0.f;
    igPushStyleVar_Vec2(ImGuiStyleVar_CellPadding, TablePadding);

    if( igBeginTable("##WindowLayout",2,TableFlags,ZeroSize,0) ) {
        igTableNextRow(0,MaxSize.y - 10.f);

        igTableSetColumnIndex(0);
        igBeginChild_Str("Settings",ZeroSize,false,0);
        GUIDrawTitleBar("Info",igGetColumnWidth(0));
        if( !ImageManager->ImageList ) {
            igBeginDisabled(1);
        }
        if( igButton("Export all",ZeroSize) ) {
            ImageManagerExport(ImageManager,ImageManager->ImageList,NULL,GUI,VideoSystem);
        }
        if( !ImageManager->ImageList ) {
            igEndDisabled();
        }
        igSeparator();
        if( !ImageManager->SelectedImage ) {
            igBeginDisabled(1);
        }
        if( igButton("Export current image",ZeroSize) ) {
            ImageManagerExport(ImageManager,ImageManager->SelectedImage,ImageManager->SelectedImage,GUI,VideoSystem);
        }
        if( !ImageManager->SelectedImage ) {
            igEndDisabled();
        }
        GUIDrawTextureWindow(ImageManager,igGetColumnWidth(0));
        igSeparator();
        GUIDrawVRAMWindow(GUI,ImageManager);
        igEnd();
        igTableSetColumnIndex(1);
        igBeginChild_Str("ImageList",ZeroSize,false,0);
        TreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;
        GUIDrawTitleBar("Image List",igGetColumnWidth(1));
        for(ImageIterator = ImageManager->ImageList; ImageIterator; ImageIterator = ImageIterator->Next) {
            TreeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
            if( ImageIterator == ImageManager->SelectedImage ) {
                TreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
                if( GUI->ImageSelectionChanged ) {
                    igSetScrollHereY(0.5f);
                    GUI->ImageSelectionChanged = 0;
                }
            }
            if( igTreeNodeEx_Str(ImageIterator->Name,TreeNodeFlags) ) {
                if (igIsMouseDoubleClicked(0) && igIsItemHovered(ImGuiHoveredFlags_None) ) {
                    ImageManager->SelectedImage = ImageIterator;
                }
            }
        }
        igEnd();
        igEndTable();
    }
    igPopStyleVar(1);
    igEnd();
}
void GUIDraw(Application_t *Application)
{
    
    GUIBeginFrame();
    GUIDrawMenuBar(Application);
    GUIDrawMainWindow(Application->GUI,Application->Engine->VideoSystem,Application->ImageManager);
//     GUIDrawTextureWindow(Application->ImageManager);
    GUIDrawVideoSettingsWindow(&Application->GUI->VideoSettingsWindowHandle,Application->Engine->VideoSystem);
    FileDialogRenderList();
    ErrorMessageDialogDraw(Application->GUI->ErrorMessageDialog);
//     igShowDemoWindow(NULL);
    GUIEndFrame();
}

GUI_t *GUIInit(VideoSystem_t *VideoSystem)
{
    GUI_t *GUI;
    char *ConfigPath;

    GUI = malloc(sizeof(GUI_t));
    
    if( !GUI ) {
        DPrintf("GUIInit:Failed to allocate memory for struct\n");
        return NULL;
    }
    
    memset(GUI,0,sizeof(GUI_t));
    GUI->ErrorMessageDialog = ErrorMessageDialogInit();
    if( !GUI->ErrorMessageDialog ) {
        DPrintf("GUIInit:Failed to initialize error message dialog\n");
        return NULL;
    }
    GUI->ImageSelectionChanged = 0;
    
    ConfigPath = AppGetConfigPath();
    asprintf(&GUI->ConfigFilePath,"%simgui.ini",ConfigPath);
    free(ConfigPath);

    GUILoadCommonSettings();
    
    GUI->DefaultContext = igCreateContext(NULL);
    GUI->ProgressBar = ProgressBarInitialize(VideoSystem);
    
    if( !GUI->ProgressBar ) {
        DPrintf("GUIInit:Failed to initialize ProgressBar\n");
        free(GUI);
        return NULL;
    }
    GUIContextInit(GUI->DefaultContext,VideoSystem,GUI->ConfigFilePath);

    return GUI;
} 
