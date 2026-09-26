cmake_minimum_required(VERSION 3.20)

# Engine library: public headers in include/engine/, sources in src/ (same folder layout)
file(GLOB_RECURSE ATELIER_ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# Only compile the window backend for the current platform
if (WIN32)
    list(FILTER ATELIER_ENGINE_SOURCES EXCLUDE REGEX "/platform/GlfwWindow\\.(cpp|hpp)$")
else ()
    list(FILTER ATELIER_ENGINE_SOURCES EXCLUDE REGEX "/platform/WindowsWindow\\.(cpp|hpp)$")
endif ()

add_library(AtelierEngine STATIC ${ATELIER_ENGINE_SOURCES})

set(ATELIER_IMGUI_SOURCES
    "${ATELIER_LIBRARIES_DIR}/include/imgui/imgui.cpp"
    "${ATELIER_LIBRARIES_DIR}/include/imgui/imgui_draw.cpp"
    "${ATELIER_LIBRARIES_DIR}/include/imgui/imgui_tables.cpp"
    "${ATELIER_LIBRARIES_DIR}/include/imgui/imgui_widgets.cpp"
)

target_sources(AtelierEngine PRIVATE ${ATELIER_IMGUI_SOURCES})

target_include_directories(AtelierEngine
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${ATELIER_LIBRARIES_DIR}/include"
)

if (WIN32)
    target_compile_definitions(AtelierEngine PUBLIC UNICODE _UNICODE)
    target_link_libraries(AtelierEngine PUBLIC opengl32 gdi32 user32)
else ()
    find_package(OpenGL REQUIRED)

    # GLFW 3.4 is built from source to match the bundled 3.4 headers
    # (Ubuntu's libglfw3-dev is 3.3, which would fail to link)
    include(FetchContent)
    # Use extraction time as file timestamps for downloaded archives
    if (POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif ()
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

    target_sources(AtelierEngine PRIVATE "${ATELIER_LIBRARIES_DIR}/include/imgui/backends/imgui_impl_glfw.cpp")
    # imgui backends include "imgui.h" without the imgui/ prefix
    target_include_directories(AtelierEngine PRIVATE "${ATELIER_LIBRARIES_DIR}/include/imgui")
    target_link_libraries(AtelierEngine PUBLIC OpenGL::GL glfw ${CMAKE_DL_LIBS})

    # imgui_impl_glfw calls Xlib directly on Linux/BSD
    if (NOT APPLE)
        find_package(X11 REQUIRED)
        target_link_libraries(AtelierEngine PUBLIC X11::X11)
    endif ()
endif ()

atelier_enable_warnings(AtelierEngine)

# Third-party sources compiled into the engine: silence their warnings so reports only show our code
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties(
        ${ATELIER_IMGUI_SOURCES}
        "${ATELIER_LIBRARIES_DIR}/include/imgui/backends/imgui_impl_glfw.cpp"
        PROPERTIES COMPILE_OPTIONS "-w"
    )
endif ()
