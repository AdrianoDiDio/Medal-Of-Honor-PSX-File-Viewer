cmake_minimum_required(VERSION 3.16)

project(MOHModelViewer)

set(SOURCE_FILES    ../Common/Common.c ../Common/Config.c ../Common/Video.c ../Common/Sound.c ../Common/Engine.c
                    ../Common/ShaderManager.c ../Common/VAO.c ../Common/IMGUIUtils.c 
                    Camera.c GUI.c ../Common/TIM.c ../Common/VRAM.c BSD.c 
                    RenderObjectManager.c MOHModelViewer.c
)
                 

add_executable(${PROJECT_NAME} ${SOURCE_FILES} )
set_target_properties(${PROJECT_NAME} PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../${PROJECT_NAME})        

target_compile_definitions(${PROJECT_NAME} PRIVATE APP_NAME="${PROJECT_NAME}")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/cimgui/)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/cimgui/generator)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/cimgui/generator/output)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/ImGuiFileDialog)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/SDL)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/libsamplerate)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/zlib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/libpng)

pvs_studio_add_target(TARGET MOHModelViewer.Analyze
                      LOG FORMAT fullhtml
                      SUPPRESS_BASE suppress_base.json
                      ANALYZE ${PROJECT_NAME}
                      ARGS -e ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/*
                      MODE GA:1,2)
                    
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -Debug)
set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX -Release)

target_compile_options(${PROJECT_NAME} PRIVATE "-fdiagnostics-color=always")
if(WIN32)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
  add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
  target_compile_definitions(${PROJECT_NAME} PRIVATE "-DSDL_MAIN_HANDLED")
  target_link_libraries(${PROJECT_NAME} -static-libgcc mingw32)
  target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:-Wall;-fsanitize=address>")
  target_link_options(${PROJECT_NAME} PRIVATE  "$<$<CONFIG:DEBUG>:-fsanitize=address>")
  add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif()

add_compile_definitions("IMGUI_IMPL_OPENGL_LOADER_GLEW")

target_compile_definitions(${PROJECT_NAME} PRIVATE -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_SDL)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
)
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:-g>")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:-O3>")

target_link_libraries(${PROJECT_NAME} SDL2 glew_s png ${OPENGL_LIBRARIES} m cimgui cglm_headers samplerate )

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Fonts/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/Fonts/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders/
)
