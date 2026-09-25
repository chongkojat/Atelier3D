# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("GAM300" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GAM300_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(GAM300_PROJECTS_DIR "${GAM300_ROOT_DIR}/projects")
set(GAM300_LIBRARIES_DIR "${GAM300_ROOT_DIR}/libraries")

get_property(GAM300_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (GAM300_IS_MULTI_CONFIG)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Package" CACHE STRING "" FORCE)
endif ()

# Copy settings from Release to Package
set(CMAKE_C_FLAGS_PACKAGE     "${CMAKE_C_FLAGS_RELEASE}"     CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_PACKAGE   "${CMAKE_CXX_FLAGS_RELEASE}"   CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_PACKAGE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING "" FORCE)

# Compiler warnings for our own targets (reported by CI)
function(gam300_enable_warnings target)
  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${target} PRIVATE -Wall -Wextra)
  endif ()
endfunction()

option(GAM300_BUILD_TESTS "Build the unit tests in tests/" ON)

add_subdirectory ("projects/Engine")
add_subdirectory ("projects/Editor")
add_subdirectory ("projects/game")
add_subdirectory ("projects/game1")

# Unit tests run on the build machine, so skip them when cross-compiling
if (GAM300_BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
  enable_testing()
  add_subdirectory ("tests")
endif ()
