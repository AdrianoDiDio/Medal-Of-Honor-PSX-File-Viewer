cmake_minimum_required(VERSION 3.16)

project(RSCExtractor)

set(SOURCE_FILES RSCExtractor.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} )

set_target_properties(${PROJECT_NAME} PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../bin/${PROJECT_NAME})   

if( ENABLE_PVS_STUDIO_ANALYZER )
    pvs_studio_add_target(TARGET ${PROJECT_NAME}.Analyze
                        LOG FORMAT fullhtml
                        LOG ${PROJECT_NAME}-Report
                        SUPPRESS_BASE suppress_base.json
                        ANALYZE ${PROJECT_NAME}
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
