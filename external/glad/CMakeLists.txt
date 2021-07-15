cmake_minimum_required(VERSION 3.8)

project(GLAD)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_definitions(-DENABLE_OPENGL)

add_library(glad 
    src/glad.c
    include/glad/glad.h
    include/KHR/khrplatform.h
)

set_target_properties(glad PROPERTIES FOLDER "External/glad")