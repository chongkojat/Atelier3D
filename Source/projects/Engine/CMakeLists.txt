cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE GAM300_ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# Only compile the window backend for the current platform
if (WIN32)
    list(FILTER GAM300_ENGINE_SOURCES EXCLUDE REGEX "/Platform/GlfwWindow\\.(cpp|hpp)$")
else ()
    list(FILTER GAM300_ENGINE_SOURCES EXCLUDE REGEX "/Platform/WindowsWindow\\.(cpp|hpp)$")
endif ()

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
else ()
    find_package(OpenGL REQUIRED)

    # GLFW 3.4 is built from source to match the bundled 3.4 headers
    # (Ubuntu's libglfw3-dev is 3.3, which would fail to link)
    include(FetchContent)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(glfw
        URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
        URL_HASH SHA256=b5ec004b2712fd08e8861dc271428f048775200a2df719ccf575143ba749a3e9
    )
    FetchContent_MakeAvailable(glfw)

    target_sources(GAM300Engine PRIVATE "${GAM300_LIBRARIES_DIR}/include/imgui/backends/imgui_impl_glfw.cpp")
    # imgui backends include "imgui.h" without the imgui/ prefix
    target_include_directories(GAM300Engine PRIVATE "${GAM300_LIBRARIES_DIR}/include/imgui")
    target_link_libraries(GAM300Engine PUBLIC OpenGL::GL glfw ${CMAKE_DL_LIBS})

    # imgui_impl_glfw calls Xlib directly on Linux/BSD
    if (NOT APPLE)
        find_package(X11 REQUIRED)
        target_link_libraries(GAM300Engine PUBLIC X11::X11)
    endif ()
endif ()

target_compile_definitions(GAM300Engine
    PUBLIC
        GAM300_ROOT_DIR="${GAM300_ROOT_DIR}"
)
