cmake_minimum_required(VERSION 3.10)

project(MOHLevelViewer)

set(SOURCE_FILES ShaderManager.c VAO.c TSP.c 
                 TIM.c Font.c BSD.c BSDTwoPlayers.c 
                 VRAM.c MOHLevelViewer.c
)

set(IMGUI_SOURCE_FILES Libs/cimgui/cimgui.cpp 
                       Libs/cimgui/imgui/imgui.cpp 
                       Libs/cimgui/imgui/imgui_draw.cpp 
                       Libs/cimgui/imgui/imgui_demo.cpp 
                       Libs/cimgui/imgui/imgui_widgets.cpp
                       Libs/cimgui/imgui/imgui_tables.cpp
                       Libs/cimgui/imgui/backends/imgui_impl_opengl3.cpp
                       Libs/cimgui/imgui/backends/imgui_impl_sdl.cpp
                       Libs/ImGuiFileDialog/ImGuiFileDialog.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} )
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -Debug)


if(WIN32)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wl,-subsystem,windows)
  add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
  add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif()

add_compile_definitions("IMGUI_IMPL_OPENGL_LOADER_GLEW")

target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:-D_DEBUG -g>")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:-O3>")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

include_directories(Libs/cimgui/imgui)
add_library(cimgui SHARED ${IMGUI_SOURCE_FILES})
target_link_libraries(cimgui ${FREETYPE_LIBRARIES} ${SDL2_LIBRARIES})


include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(Libs/cimgui/generator/output/)
include_directories(Libs/cimgui/)
include_directories(Libs/ImGuiFileDialog/)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${GLEW_LIBRARIES} ${PNG_LIBRARIES} ${OPENGL_LIBRARIES} m cimgui)
