cmake_minimum_required(VERSION 3.16)

project(VABExtractor)

set(SOURCE_FILES VABExtractor.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} )

set_target_properties(${PROJECT_NAME} PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../bin/${PROJECT_NAME})   

if( BUILD_ENABLE_PVS_STUDIO_ANALYZER )
    pvs_studio_add_target(TARGET VABExtractor.Analyze
                        LOG FORMAT fullhtml
                        SUPPRESS_BASE suppress_base.json
                        ANALYZE ${PROJECT_NAME}
                        ARGS -e ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/*
                        MODE GA:1,2)
endif()
                    
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -Debug)
set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX -Release)

target_compile_options(${PROJECT_NAME} PRIVATE "-fdiagnostics-color=always")
if(WIN32)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
  target_link_libraries(${PROJECT_NAME} -static-libgcc mingw32)
  target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:-Wall;-fsanitize=address>")
  target_link_options(${PROJECT_NAME} PRIVATE  "$<$<CONFIG:DEBUG>:-fsanitize=address>")
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
)

target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:-g>")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:-O3>")

find_library(sndfile libsndfile)

target_link_libraries(${PROJECT_NAME} sndfile)
