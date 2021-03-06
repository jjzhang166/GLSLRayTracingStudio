﻿cmake_minimum_required(VERSION 3.12.0)

set(ProjectName GLSLRayTracingStudio)
project(${ProjectName})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DNOMINMAX=1)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DPLATFORM_WINDOWS=1)
add_definitions(-DAPP_VERSION="1.0.0")
add_definitions(-DASSETS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/assets/\")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

if (MSVC)
    add_link_options("/DEBUG")
endif()

find_package(OpenGL REQUIRED)

include_directories(
    src/
    external/glfw/include/
    external/glad/include/
    external/imgui/
    external/imguizmo/
)

add_subdirectory(src)
add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/imgui)
add_subdirectory(external/imguizmo)

set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw
    glad
    imgui
    imguizmo
    engine
)

# assets
file(GLOB_RECURSE files "${CMAKE_CURRENT_SOURCE_DIR}/assets/*.*" "${CMAKE_CURRENT_SOURCE_DIR}/assets/*/*.*")
foreach(file ${files})
    SET(ASSETS
        ${ASSETS}
        ${file}
    )
endforeach()
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Resources" FILES ${ASSETS})

# exe
add_executable(${ProjectName}
    src/Main.cpp
    ${ASSETS}
)
target_link_libraries(${ProjectName} ${ALL_LIBS})

# copy assets
add_custom_command(TARGET ${ProjectName} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${ProjectName}>/assets
)