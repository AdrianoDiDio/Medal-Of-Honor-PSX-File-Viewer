set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)


set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED) 

add_subdirectory(cglm ${CMAKE_CURRENT_BINARY_DIR}/cglm EXCLUDE_FROM_ALL)

add_subdirectory(libsamplerate ${CMAKE_CURRENT_BINARY_DIR}/libsamplerate EXCLUDE_FROM_ALL)

set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "" FORCE)
add_subdirectory(SDL ${CMAKE_CURRENT_BINARY_DIR}/SDL EXCLUDE_FROM_ALL)

set(GLEW_CUSTOM_OUTPUT_DIRS ON CACHE BOOL "" FORCE)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
add_definitions(-DGLEW_STATIC)
add_subdirectory(glew-cmake/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/glew-cmake EXCLUDE_FROM_ALL)

add_subdirectory(zlib ${CMAKE_CURRENT_BINARY_DIR}/zlib EXCLUDE_FROM_ALL)

set(ZLIB_ROOT zlib)

set(ZLIB_LIBRARY zlib)
set(PNG_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
set(PNG_SHARED ON CACHE BOOL "" FORCE)
set(PNG_STATIC OFF CACHE BOOL "" FORCE)
set(PNG_EXECUTABLES OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE) 
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)

add_subdirectory(libpng ${CMAKE_CURRENT_BINARY_DIR}/libpng EXCLUDE_FROM_ALL)

set(IMGUI_SOURCE_FILES cimgui/cimgui.cpp
                       cimgui/imgui/imgui.cpp 
                       cimgui/imgui/imgui_draw.cpp 
                       cimgui/imgui/imgui_demo.cpp 
                       cimgui/imgui/imgui_widgets.cpp
                       cimgui/imgui/imgui_tables.cpp
                       cimgui/imgui/backends/imgui_impl_opengl3.cpp
                       cimgui/imgui/backends/imgui_impl_sdl2.cpp
                       ImGuiFileDialog/ImGuiFileDialog.cpp
)

add_library(cimgui STATIC ${IMGUI_SOURCE_FILES})
target_compile_definitions(cimgui PRIVATE "IMGUI_USER_CONFIG=\"${PROJECT_SOURCE_DIR}/src/Common/IMGUIConfig.h\"")

target_include_directories(cimgui PRIVATE cimgui/imgui)
target_link_libraries(cimgui SDL2)

if( WIN32 ) 
    target_link_libraries(cimgui -static-libstdc++ -static-libgcc -static)
    target_compile_definitions(cimgui PRIVATE "-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else()
    target_link_libraries(cimgui dl)
    target_compile_definitions(cimgui PRIVATE "-DIMGUI_IMPL_API=extern \"C\" ")
endif()
