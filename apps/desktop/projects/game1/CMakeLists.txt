cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE GAM300_GAME_PYRAMID_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_executable(GAM300GamePyramid ${GAM300_GAME_PYRAMID_SOURCES})

target_include_directories(GAM300GamePyramid
    PRIVATE
        "${GAM300_PROJECTS_DIR}"
        "${GAM300_LIBRARIES_DIR}/include"
)

target_link_libraries(GAM300GamePyramid PRIVATE GAM300Engine)

