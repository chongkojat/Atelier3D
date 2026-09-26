cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE ATELIER_EDITOR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_executable(AtelierEditor ${ATELIER_EDITOR_SOURCES})

target_include_directories(AtelierEditor
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(AtelierEditor PRIVATE AtelierEngine)

atelier_enable_warnings(AtelierEditor)
