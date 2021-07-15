cmake_minimum_required(VERSION 3.8)

project(IMGUIZMO)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(imguizmo
    GraphEditor.h
    GraphEditor.cpp
    ImCurveEdit.h
    ImCurveEdit.cpp
    ImGradient.h
    ImGradient.cpp
    ImGuizmo.h
    ImGuizmo.cpp
    ImSequencer.h
    ImSequencer.cpp
    ImZoomSlider.h
)

set_target_properties(imguizmo PROPERTIES FOLDER "External/imguizmo")