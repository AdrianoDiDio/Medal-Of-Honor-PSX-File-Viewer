cmake_minimum_required(VERSION 3.16)
project(Common)

set(COMMON_SOURCE_FILES Common.c Config.c Video.c Sound.c Engine.c
                    ShaderManager.c VAO.c IMGUIUtils.c 
                    TIM.c VRAM.c
)

add_library(${PROJECT_NAME} STATIC ${COMMON_SOURCE_FILES})

if( ENABLE_PVS_STUDIO_ANALYZER )
    pvs_studio_add_target(TARGET ${PROJECT_NAME}.Analyze
                        ANALYZE ${PROJECT_NAME}
                        SUPPRESS_BASE suppress_base.json
                        LOG FORMAT fullhtml
                        LOG ${PROJECT_NAME}-Report
                        ARGS -e *libs*
                        MODE GA:1,2
                        )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/cimgui/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/cimgui/generator)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/cimgui/generator/output)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/ImGuiFileDialog)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/glew-cmake/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/SDL)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/libsamplerate)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/zlib)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/libpng)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../../libs/libpng)
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW CIMGUI_USE_OPENGL3 CIMGUI_USE_SDL2)
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    $<$<CONFIG:Debug>:_DEBUG>
)
target_link_libraries(${PROJECT_NAME} SDL2 glew_s png m cimgui cglm_headers samplerate)


