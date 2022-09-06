cmake_minimum_required(VERSION 3.16)

project(TIMExtractor)

set(SOURCE_FILES TIMExtractor.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} )

set_target_properties(${PROJECT_NAME} PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../${PROJECT_NAME})   

pvs_studio_add_target(TARGET TIMExtractor.Analyze
                      LOG FORMAT fullhtml
                      SUPPRESS_BASE suppress_base.json
                      ANALYZE ${PROJECT_NAME}
                      ARGS -e ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/*
                      MODE GA:1,2)
                    
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -Debug)
set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX -Release)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/libpng)

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

target_link_libraries(${PROJECT_NAME} png)

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_BINARY_DIR}/SharedLibraries/*png* $<TARGET_FILE_DIR:${PROJECT_NAME}>/
)
