cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE GAM300_GAME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_executable(GAM300Game ${GAM300_GAME_SOURCES})

target_include_directories(GAM300Game
    PRIVATE
        "${GAM300_PROJECTS_DIR}"
        "${GAM300_LIBRARIES_DIR}/include"
)

target_link_libraries(GAM300Game PRIVATE GAM300Engine)

gam300_enable_warnings(GAM300Game)

