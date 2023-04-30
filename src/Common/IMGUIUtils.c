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
#include "IMGUIUtils.h"

Config_t *GUIFont;
Config_t *GUIFontSize;
Config_t *GUIShowFPS;

FileDialog_t *FileDialogList = NULL;
int NumRegisteredFileDialog = 0;

const VSyncSettings_t VSyncOptions[] = { 
    
    {
        "Disable",
        0
    },
    {
        "Standard",
        1
    },
    {
        "Adaptive",
        -1
    }
};

int NumVSyncOptions = sizeof(VSyncOptions) / sizeof(VSyncOptions[0]);


void ErrorMessageDialogFree(ErrorMessageDialog_t *ErrorMessageDialog)
{
    if( !ErrorMessageDialog ) {
        return;
    }
    
    if( ErrorMessageDialog->Message ) {
        free(ErrorMessageDialog->Message);
        ErrorMessageDialog->Message = NULL;
    }
    free(ErrorMessageDialog);
}

void ErrorMessageDialogSet(ErrorMessageDialog_t *ErrorMessageDialog,const char *ErrorMessage)
{
    if( !ErrorMessageDialog ) {
        DPrintf("ErrorMessageDialogSet:Invalid Dialog\n");
        return;
    }
    if( !ErrorMessage ) {
        DPrintf("ErrorMessageDialogSet:Invalid Error Message\n");
        return;
    }
    
    if( ErrorMessageDialog->Message ) {
        free(ErrorMessageDialog->Message);
    }
    
    ErrorMessageDialog->Message = StringCopy(ErrorMessage);
}

void ErrorMessageDialogDraw(ErrorMessageDialog_t *ErrorMessageDialog)
{
    ImVec2 ButtonSize;
    
    if( !ErrorMessageDialog ) {
        return;
    }
    
    if( !ErrorMessageDialog->Message ) {
        return;
    }
    
    if( !ErrorMessageDialog->Handle ) {
        igOpenPopup_Str("Error",0);
        ErrorMessageDialog->Handle = 1;
    }
    ButtonSize.x = 120;
    ButtonSize.y = 0;
    GUIPrepareModalWindow();
    if( igBeginPopupModal("Error",NULL,ImGuiWindowFlags_AlwaysAutoResize) ) {
        igText(ErrorMessageDialog->Message);
        if (igButton("OK", ButtonSize) ) {
            igCloseCurrentPopup();
            free(ErrorMessageDialog->Message);
            ErrorMessageDialog->Message = NULL;
            ErrorMessageDialog->Handle = 0;
            return;
        }
        igEndPopup();
    }
    return;
}
ErrorMessageDialog_t *ErrorMessageDialogInit()
{
    ErrorMessageDialog_t *Dialog;
    
    Dialog = malloc(sizeof(ErrorMessageDialog_t));
    
    if( !Dialog ) {
        DPrintf("ErrorMessageDialogInit:Failed to allocate memory\n");
        return NULL;
    }
    Dialog->Message = NULL;
    Dialog->Handle = 0;
    return Dialog;
}

int GUIDrawTitleBar(const char *Title,float ContentWidth)
{
    char *Buffer;
    ImVec2 TextSize;
    int Result;
    igPushStyleVar_Float(ImGuiStyleVar_DisabledAlpha,1.f);
    igBeginDisabled(1);
    asprintf(&Buffer,"##%s\n",Title);
    Result = igCollapsingHeader_TreeNodeFlags(Buffer, ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf );
    igCalcTextSize(&TextSize,Title,NULL,false,-1);
    igSameLine( (ContentWidth - TextSize.x ) / 2,-1);
    igText(Title);
    igEndDisabled();
    igPopStyleVar(1);
    free(Buffer);
    return Result;
}

void GUIPrepareModalWindow()
{
    ImGuiIO *IO;
    ImVec2 Pivot; 
    ImVec2 ModalPosition;
    
    IO = igGetIO();
    Pivot.x = 0.5f;
    Pivot.y = 0.5f;
    ModalPosition.x = IO->DisplaySize.x * 0.5;
    ModalPosition.y = IO->DisplaySize.y * 0.5;
    igSetNextWindowPos(ModalPosition, ImGuiCond_Always, Pivot);
}

bool GUICheckBoxWithTooltip(char *Label,bool *Value,char *DescriptionFormat,...)
{
    va_list Arguments;
    int IsChecked;
    IsChecked = igCheckbox(Label,Value);
    if( DescriptionFormat != NULL && igIsItemHovered(ImGuiHoveredFlags_None) ) {
        igBeginTooltip();
        igPushTextWrapPos(igGetFontSize() * 40.0f);
        va_start(Arguments, DescriptionFormat);
        igTextV(DescriptionFormat,Arguments);
        va_end(Arguments);
        igPopTextWrapPos();
        igEndTooltip();
    }
    return IsChecked;
}

void GUIReleaseContext(ImGuiContext *Context)
{    
    igSetCurrentContext(Context);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    igDestroyContext(Context);
}

void GUIContextInit(ImGuiContext *Context,VideoSystem_t *VideoSystem,const char *ConfigFilePath)
{
    ImGuiIO *IO;
    ImGuiStyle *Style;
    ImFont *Font;
    ImFontConfig *FontConfig;
    
    IO = igGetIO();
    igSetCurrentContext(Context);
    ImGui_ImplSDL2_InitForOpenGL(VideoSystem->Window, &VideoSystem->GLContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    igStyleColorsDark(NULL);
    if( GUIFont->Value[0] ) {
        Font = ImFontAtlas_AddFontFromFileTTF(IO->Fonts,GUIFont->Value,floor(GUIFontSize->FValue * VideoSystem->DPIScale),NULL,NULL);
        if( !Font ) {
            DPrintf("GUIContextInit:Invalid font file...using default\n");
            ConfigSet("GUIFont","");
        }
    } else {
        FontConfig = ImFontConfig_ImFontConfig();
        FontConfig->OversampleH = 1;
        FontConfig->OversampleV = 1;
        FontConfig->PixelSnapH = true;
        FontConfig->SizePixels = floor(GUIFontSize->FValue * VideoSystem->DPIScale);
        ImFontAtlas_AddFontDefault(IO->Fonts,FontConfig);
        ImFontConfig_destroy(FontConfig);
    }
    Style = igGetStyle();
    Style->WindowTitleAlign.x = 0.5f;
    ImGuiStyle_ScaleAllSizes(Style,VideoSystem->DPIScale);
    IO->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    IO->IniFilename = ConfigFilePath;
}

void GUIBeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();   
}

void GUIEndFrame()
{
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

void ProgressBarDestroy(ProgressBar_t *ProgressBar)
{
    if( !ProgressBar ) {
        return;
    }
    GUIReleaseContext(ProgressBar->Context);
    if( ProgressBar->DialogTitle ) {
        free(ProgressBar->DialogTitle);
    }
    free(ProgressBar);
}
void ProgressBarEnd(ProgressBar_t *ProgressBar,VideoSystem_t *VideoSystem)
{
    int Width;
    int Height;
    
    igSetCurrentContext(ProgressBar->OldContext);
    ProgressBar->IsOpen = 0;
    ProgressBar->CurrentPercentage = 0.f;
    //NOTE(Adriano):Make sure to update the current window size.
    VideoSystemGetCurrentWindowSize(VideoSystem,&Width,&Height);
    if( Width != VidConfigWidth->IValue ) {
        ConfigSetNumber("VideoWidth",Width);
    }
    if( Height != VidConfigHeight->IValue ) {
        ConfigSetNumber("VideoHeight",Height);
    }
}
void ProgressBarBegin(ProgressBar_t *ProgressBar,const char *Title)
{
    ProgressBar->OldContext = igGetCurrentContext();
    igSetCurrentContext(ProgressBar->Context);
    ProgressBar->IsOpen = 0;
    ProgressBar->CurrentPercentage = 0.f;
    ProgressBarSetDialogTitle(ProgressBar,Title);
}
void ProgressBarReset(ProgressBar_t *ProgressBar)
{
    if(!ProgressBar) {
        return;
    }
    ProgressBar->CurrentPercentage = 0;
}
void ProgressBarSetDialogTitle(ProgressBar_t *ProgressBar,const char *Title)
{
    if( ProgressBar->DialogTitle ) {
        free(ProgressBar->DialogTitle);
    }
    ProgressBar->DialogTitle = (Title != NULL) ? StringCopy(Title) : "Loading...";
    //NOTE(Adriano):Forces a refresh...since changing the title disrupts the rendering process.
    ProgressBar->IsOpen = 0;
}
/*
    This function can be seen as a complete rendering loop.
    Each time we increment the progress bar, we check for any pending event that the GUI
    can handle and then we clear the display, show the current progress and swap buffers.
 */
void ProgressBarIncrement(ProgressBar_t *ProgressBar,VideoSystem_t *VideoSystem,float Increment,const char *Message)
{
    SDL_Event Event;
    ImGuiViewport *Viewport;
    ImVec2 ScreenCenter;
    ImVec2 Pivot;
    ImVec2 Size;
    
    if( !ProgressBar ) {
        return;
    }
    
   SDL_PumpEvents();
    //NOTE(Adriano):
    //Process any window event that could be generated while showing the progress bar.
    //We need to make sure not to process any mouse/keyboad event otherwise when the progress bar ends the
    //queue may be empty and the GUI won't respond to events properly...
    while( SDL_PeepEvents(&Event, 1, SDL_GETEVENT, SDL_WINDOWEVENT, SDL_WINDOWEVENT) > 0 ) {
        ImGui_ImplSDL2_ProcessEvent(&Event);
    }
    
    //NOTE(Adriano):Since we are checking for events these function have now an updated view of the current window size.
    Viewport = igGetMainViewport();
    
    ImGuiViewport_GetCenter(&ScreenCenter,Viewport);

    Pivot.x = 0.5f;
    Pivot.y = 0.5f;

    glClear(GL_COLOR_BUFFER_BIT );
    

    GUIBeginFrame();
    
    if( !ProgressBar->IsOpen ) {
        igOpenPopup_Str(ProgressBar->DialogTitle,0);
        ProgressBar->IsOpen = 1;
    }
    
    Size.x = 0.f;
    Size.y = 0.f;
    igSetNextWindowPos(ScreenCenter, ImGuiCond_Always, Pivot);
    ProgressBar->CurrentPercentage += Increment;
    if (igBeginPopupModal(ProgressBar->DialogTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        igProgressBar((ProgressBar->CurrentPercentage / 100.f),Size,Message);
        igEnd();
    }
    GUIEndFrame();
    VideoSystemSwapBuffers(VideoSystem);
}

ProgressBar_t *ProgressBarInitialize(VideoSystem_t *VideoSystem)
{
    ProgressBar_t *ProgressBar;
    
    ProgressBar = malloc(sizeof(ProgressBar_t));
    if( !ProgressBar ) {
        return NULL;
    }
    ProgressBar->Context = igCreateContext(NULL);
    ProgressBar->DialogTitle = NULL;
    //NOTE(Adriano):Progress bar doesn't need a Config File since it's position
    //is fixed and doesn't depend from the user.
    GUIContextInit(ProgressBar->Context,VideoSystem,NULL);
    return ProgressBar;
}

void FileDialogListFree()
{
    FileDialog_t *Temp;
    
    while( FileDialogList ) {
        free(FileDialogList->WindowTitle);
        free(FileDialogList->Key);
        if( FileDialogList->Filters ) {
            free(FileDialogList->Filters);
        }
        IGFD_Destroy(FileDialogList->Window);
        Temp = FileDialogList;
        FileDialogList = FileDialogList->Next;
        free(Temp);
    }
}

void FileDialogRender(FileDialog_t *FileDialog)
{
    ImVec2 MaxSize;
    ImVec2 MinSize;
    ImGuiViewport *Viewport;
    ImVec2 WindowPosition;
    ImVec2 WindowPivot;
    char *DirectoryPath;
    char *FileName;
    void *UserData;
    
    if( !FileDialog ) {
        return;
    }
    if( !IGFD_IsOpened(FileDialog->Window) ) {
        return;
    }
    Viewport = igGetMainViewport();
    WindowPosition.x = Viewport->WorkPos.x;
    WindowPosition.y = Viewport->WorkPos.y;
    WindowPivot.x = 0.f;
    WindowPivot.y = 0.f;
    MaxSize.x = Viewport->WorkSize.x;
    MaxSize.y = Viewport->WorkSize.y;
    MinSize.x = -1;
    MinSize.y = -1;

    igSetNextWindowSize(MaxSize,0);
    igSetNextWindowPos(WindowPosition, ImGuiCond_Always, WindowPivot);
    if (IGFD_DisplayDialog(FileDialog->Window, FileDialog->Key, 
        ImGuiWindowFlags_NoCollapse  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings, MinSize, MaxSize)) {
        if (IGFD_IsOk(FileDialog->Window)) {
                FileName = IGFD_GetFilePathName(FileDialog->Window);
                DirectoryPath = IGFD_GetCurrentPath(FileDialog->Window);
                UserData = IGFD_GetUserDatas(FileDialog->Window);
                if( FileDialog->OnElementSelected ) {
                    FileDialog->OnElementSelected(FileDialog,DirectoryPath,FileName,UserData);
                }
                if( DirectoryPath ) {
                    free(DirectoryPath);
                }
                if( FileName ) {
                    free(FileName);
                }
        } else {
            if( FileDialog->OnDialogCancelled ) {
                FileDialog->OnDialogCancelled(FileDialog);
            }
        }
    }

}
void FileDialogRenderList()
{
    FileDialog_t *Iterator;
    
    for( Iterator = FileDialogList; Iterator; Iterator = Iterator->Next ) {
        FileDialogRender(Iterator);
    }
}
int FileDialogIsOpen(FileDialog_t *FileDialog)
{
    if( !FileDialog ) {
        DPrintf("FileDialogIsOpen:Invalid dialog data\n");
        return 0;
    }
    
    return IGFD_IsOpened(FileDialog->Window);
}

void *FileDialogGetUserData(FileDialog_t *FileDialog)
{
    if( !FileDialog ) {
        DPrintf("FileDialogGetUserData:Invalid dialog data\n");
        return 0;
    } 
    return IGFD_GetUserDatas(FileDialog->Window);
}
void FileDialogOpenWithUserData(FileDialog_t *FileDialog,const char *Path,void *UserData)
{
    if( !FileDialog ) {
        DPrintf("FileDialogOpenWithUserData:Invalid dialog data\n");
        return;
    }
    
    if( IGFD_IsOpened(FileDialog->Window) ) {
        return;
    }
    IGFD_OpenDialog(FileDialog->Window,FileDialog->Key,FileDialog->WindowTitle,FileDialog->Filters,
                    Path == NULL ? FILE_DIALOG_DEFAULT_OPEN_DIR : Path,"",1,
                    UserData,ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_CaseInsensitiveExtention);
}
void FileDialogOpen(FileDialog_t *FileDialog)
{
    FileDialogOpenWithUserData(FileDialog,".",NULL);
}

void FileDialogOpenAtPath(FileDialog_t *FileDialog,const char *Path)
{
    FileDialogOpenWithUserData(FileDialog,Path,NULL);
}

void FileDialogClose(FileDialog_t *FileDialog)
{
    if( !FileDialog ) {
        DPrintf("FileDialogClose:Invalid dialog data\n");
        return;
    }
    
    if( !IGFD_IsOpened(FileDialog->Window) ) {
        return;
    }
    IGFD_CloseDialog(FileDialog->Window);
}
/*
 Register a new file dialog.
 Filters can be NULL if we want a dir selection dialog or have a value based on ImGuiFileDialog documentation if we want to
 select a certain type of file.
 OnElementSelected and OnDialogCancelled are two callback that can be set to NULL if we are not interested in the result.
 NOTE that setting them to NULL or the cancel callback to NULL doesn't close the dialog.
 */
FileDialog_t *FileDialogRegister(const char *WindowTitle,const char *Filters,FileDialogSelectCallback_t OnElementSelected,
                                       FileDialogCancelCallback_t OnDialogCancelled)
{
    FileDialog_t *FileDialog;
    
    if( !WindowTitle) {
        DPrintf("FileDialogRegister:Invalid Window Title\n");
        return NULL;
    }

    FileDialog = malloc(sizeof(FileDialog_t));
    
    if( !FileDialog ) {
        DPrintf("FileDialogRegister:Couldn't allocate struct data.\n");
        return NULL;
    }
    asprintf(&FileDialog->Key,"FileDialog%i",NumRegisteredFileDialog);
    FileDialog->WindowTitle = StringCopy(WindowTitle);
    if( Filters ) {
        FileDialog->Filters = StringCopy(Filters);
    } else {
        FileDialog->Filters = NULL;
    }
    FileDialog->Window = IGFD_Create();
    FileDialog->OnElementSelected = OnElementSelected;
    FileDialog->OnDialogCancelled = OnDialogCancelled;
    FileDialog->Next = FileDialogList;
    FileDialogList = FileDialog;
    NumRegisteredFileDialog++;
    
    return FileDialog;
}
void FileDialogSetTitle(FileDialog_t *FileDialog,const char *Title)
{
    if(!FileDialog) {
        DPrintf("FileDialogSetTitle:Invalid dialog\n");
        return;
    }
    if( !Title ) {
        DPrintf("FileDialogSetTitle:Invalid title\n");
        return;
    }
    if( FileDialog->WindowTitle ) {
        free(FileDialog->WindowTitle);
    }
    FileDialog->WindowTitle = StringCopy(Title);
}

void FileDialogSetOnElementSelectedCallback(FileDialog_t *FileDialog,FileDialogSelectCallback_t OnElementSelected)
{
    if(!FileDialog) {
        DPrintf("FileDialogSetOnElementSelectedCallback:Invalid dialog\n");
        return;
    }
    FileDialog->OnElementSelected = OnElementSelected;
}
void FileDialogSetOnDialogCancelledCallback(FileDialog_t *FileDialog,FileDialogCancelCallback_t OnDialogCancelled)
{
    if(!FileDialog) {
        DPrintf("FileDialogSetOnDialogCancelledCallback:Invalid dialog\n");
        return;
    }
    FileDialog->OnDialogCancelled = OnDialogCancelled;
}
int GUIGetVSyncOptionValue()
{
    int i;
    for (i = 0; i < NumVSyncOptions; i++) {
        if( VSyncOptions[i].Value == VidConfigVSync->IValue ) {
            return i;
        }
    }
    return 0;
}
void GUIDrawVideoSettingsWindow(bool *WindowHandle,VideoSystem_t *VideoSystem)
{
    int OldValue;
    int IsSelected;
    int i;
    int CurrentVSyncOption;

    if( !WindowHandle || !(*WindowHandle) ) {
        return;
    }
    ImVec2 ZeroSize;
    ZeroSize.x = ZeroSize.y = 0.f;
    int PreviewIndex = VideoSystem->CurrentVideoMode != -1 ? VideoSystem->CurrentVideoMode : 0;
    if( igBegin("Video Settings",WindowHandle,ImGuiWindowFlags_AlwaysAutoResize) ) {
        CurrentVSyncOption = GUIGetVSyncOptionValue();
        if( igBeginCombo("VSync Options",VSyncOptions[CurrentVSyncOption].DisplayValue,0) ) {
            for (i = 0; i < NumVSyncOptions; i++) {
                IsSelected = (CurrentVSyncOption == i);
                if (igSelectable_Bool(VSyncOptions[i].DisplayValue, IsSelected,0,ZeroSize)) {
                    if( CurrentVSyncOption != i ) {
                        OldValue = VidConfigVSync->IValue;
                        if( VideoSystemSetSwapInterval(VSyncOptions[i].Value) < 0 ) {
                            VideoSystemSetSwapInterval(OldValue);
                        }
                    }
                }
                if( IsSelected ) {
                    igSetItemDefaultFocus();
                }
          
            }
            igEndCombo();
        }
        igSeparator();
        //NOTE(Adriano):Only in Fullscreen mode we can select the video mode we want.
        if( VidConfigFullScreen->IValue ) {
            igText("Video Mode");
            if( igBeginCombo("##Resolution", VideoSystem->VideoModeList[PreviewIndex].Description, 0) ) {
                for( i = 0; i < VideoSystem->NumVideoModes; i++ ) {
                    int IsSelected = ((VideoSystem->VideoModeList[i].Width == VidConfigWidth->IValue) && 
                        (VideoSystem->VideoModeList[i].Height == VidConfigHeight->IValue)) ? 1 : 0;
                    if( igSelectable_Bool(VideoSystem->VideoModeList[i].Description,IsSelected,0,ZeroSize ) ) {
                        VideoSystemSetVideoSettings(VideoSystem,i);
                    }
                    if( IsSelected ) {
                        igSetItemDefaultFocus();
                    }
                }
                igEndCombo();
            }
            igSeparator();
        }
        if( igCheckbox("Fullscreen Mode",(bool *) &VidConfigFullScreen->IValue) ) {
            DPrintf("VidConfigFullScreen:%i\n",VidConfigFullScreen->IValue);
            VideoSystemSetVideoSettings(VideoSystem,-1);
        }
    }
    igEnd();
}

void GUILoadCommonSettings()
{
    GUIFont = ConfigGet("GUIFont");
    GUIFontSize = ConfigGet("GUIFontSize");
    GUIShowFPS = ConfigGet("GUIShowFPS");
}
