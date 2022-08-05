cmake_minimum_required(VERSION 3.16)

include(FetchContent)

project(MOHModelViewer)

set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "$\{ORIGIN\}")

set(SOURCE_FILES MOHModelViewer.c
)
                 
# set(IMGUI_SOURCE_FILES Libs/cimgui/cimgui.cpp
#                        Libs/cimgui/imgui/imgui.cpp 
#                        Libs/cimgui/imgui/imgui_draw.cpp 
#                        Libs/cimgui/imgui/imgui_demo.cpp 
#                        Libs/cimgui/imgui/imgui_widgets.cpp
#                        Libs/cimgui/imgui/imgui_tables.cpp
#                        Libs/cimgui/imgui/backends/imgui_impl_opengl3.cpp
#                        Libs/cimgui/imgui/backends/imgui_impl_sdl.cpp
#                        Libs/ImGuiFileDialog/ImGuiFileDialog.cpp
# )
add_executable(${PROJECT_NAME} ${SOURCE_FILES} )

FetchContent_Declare(
    PVS_CMakeModule
    GIT_REPOSITORY "https://github.com/viva64/pvs-studio-cmake-module.git"
    GIT_TAG        "master" 
)
FetchContent_MakeAvailable(PVS_CMakeModule)
include("${pvs_cmakemodule_SOURCE_DIR}/PVS-Studio.cmake")
pvs_studio_add_target(TARGET MOHModelViewer.Analyze ALL
                      LOG FORMAT fullhtml
                      SUPPRESS_BASE suppress_base.json
                      ANALYZE ${PROJECT_NAME}
                      ARGS -e ${CMAKE_SOURCE_DIR}/Libs/*
                      MODE GA:1,2)
                    
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -Debug)
set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX -Release)

# add_subdirectory(Libs/libsamplerate)
# include_directories(Libs/libsamplerate/include/)
# 
# include_directories(Libs/cimgui/imgui)
# set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
# set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "" FORCE)
# add_subdirectory(Libs/SDL)
# include_directories(Libs/SDL/include)
# 
# set(GLEW_CUSTOM_OUTPUT_DIRS ON CACHE BOOL "" FORCE)
# 
# add_subdirectory(Libs/glew-cmake/build/cmake)
# 
# add_subdirectory(Libs/zlib ${CMAKE_CURRENT_BINARY_DIR}/Libs/zlib)
# 
# set(ZLIB_ROOT Libs/zlib)
# 
# set(ZLIB_LIBRARY zlib)
# set(PNG_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
# set(PNG_SHARED ON CACHE BOOL "" FORCE)
# set(PNG_STATIC OFF CACHE BOOL "" FORCE)
# set(PNG_EXECUTABLES OFF CACHE BOOL "" FORCE)
# set(PNG_TESTS OFF CACHE BOOL "" FORCE) 
# set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
# 
# add_subdirectory(Libs/libpng)
# 
# set(OpenGL_GL_PREFERENCE LEGACY)
# find_package(OpenGL REQUIRED)
# 
# add_library(cimgui STATIC ${IMGUI_SOURCE_FILES})
# target_compile_definitions(cimgui PRIVATE "IMGUI_USER_CONFIG=\"${CMAKE_SOURCE_DIR}/IMGUIConfig.h\"")
# target_link_libraries(cimgui SDL2)

target_compile_options(${PROJECT_NAME} PRIVATE "-fdiagnostics-color=always")
if(WIN32)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
  add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
  target_compile_definitions(${PROJECT_NAME} PRIVATE "-DSDL_MAIN_HANDLED")
  target_link_libraries(${PROJECT_NAME} -static-libgcc mingw32)
  target_link_libraries(cimgui -static-libstdc++ -static-libgcc -static)
  target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:-Wall;-fsanitize=address>")
  target_link_options(${PROJECT_NAME} PRIVATE  "$<$<CONFIG:DEBUG>:-fsanitize=address>")
#   target_link_libraries(cimgui dl)
#   add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif()

# add_compile_definitions("IMGUI_IMPL_OPENGL_LOADER_GLEW")

# target_compile_definitions(${PROJECT_NAME} PRIVATE -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_SDL)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
)
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:-g>")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:-O3>")

# include_directories(${OPENGL_INCLUDE_DIRS})
# include_directories(Libs/cimgui/generator/output/)
# include_directories(Libs/cimgui/)
# include_directories(Libs/ImGuiFileDialog/)
# target_link_libraries(${PROJECT_NAME} SDL2 glew_s png ${OPENGL_LIBRARIES} m cimgui samplerate)

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Fonts/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/Fonts/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Shaders/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders/
)
