cmake_minimum_required(VERSION 3.16)
include(FetchContent)

project(Medal-Of-Honor-PSX-File-Viewer)


set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/SharedLibraries )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ )

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "$\{ORIGIN\}")

# Common dependencies for every project that requires it...
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED) 

# Make sure that PVS studio is available for every subdirectory...
FetchContent_Declare(
    PVS_CMakeModule
    GIT_REPOSITORY "https://github.com/viva64/pvs-studio-cmake-module.git"
    GIT_TAG        "master" 
)
FetchContent_MakeAvailable(PVS_CMakeModule)
include("${pvs_cmakemodule_SOURCE_DIR}/PVS-Studio.cmake")

add_subdirectory(libs)
add_subdirectory(src)
