cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE GAM300_ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(GAM300Engine STATIC ${GAM300_ENGINE_SOURCES})

set(GAM300_IMGUI_SOURCES
    "${GAM300_LIBRARIES_DIR}/include/imgui/imgui.cpp"
    "${GAM300_LIBRARIES_DIR}/include/imgui/imgui_draw.cpp"
    "${GAM300_LIBRARIES_DIR}/include/imgui/imgui_tables.cpp"
    "${GAM300_LIBRARIES_DIR}/include/imgui/imgui_widgets.cpp"
)

target_sources(GAM300Engine PRIVATE ${GAM300_IMGUI_SOURCES})

target_include_directories(GAM300Engine
    PUBLIC
        "${GAM300_PROJECTS_DIR}"
        "${GAM300_LIBRARIES_DIR}/include"
)

if (WIN32)
    target_compile_definitions(GAM300Engine PUBLIC UNICODE _UNICODE)
    target_link_libraries(GAM300Engine PUBLIC opengl32 gdi32 user32)
endif ()

target_compile_definitions(GAM300Engine
    PUBLIC
        GAM300_ROOT_DIR="${GAM300_ROOT_DIR}"
)
