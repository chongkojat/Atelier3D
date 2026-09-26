# CMake build for the Atelier3D desktop app (apps/desktop)
#   projects/atelier_engine   engine library
#   projects/atelier_editor   editor (AtelierEditor): create, pick and play game projects
#                             (games are created in projects/ next to the exe, see GameProject::ProjectsRoot)
#   ../../tests/unit          GoogleTest unit tests (top-level tests/ folder)
cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Atelier3D" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# compile_commands.json in the build directory, used by clang-tidy (tools/ci/lint.sh)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# All executables go to <build>/bin (<build>/bin/<config> with Visual Studio)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(ATELIER_PROJECTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/projects")
set(ATELIER_LIBRARIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")

get_property(ATELIER_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (ATELIER_IS_MULTI_CONFIG)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Package" CACHE STRING "" FORCE)
endif ()

# Copy settings from Release to Package
set(CMAKE_C_FLAGS_PACKAGE     "${CMAKE_C_FLAGS_RELEASE}"     CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_PACKAGE   "${CMAKE_CXX_FLAGS_RELEASE}"   CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_PACKAGE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING "" FORCE)

# Compiler warnings for our own targets (reported by CI)
function(atelier_enable_warnings target)
  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${target} PRIVATE -Wall -Wextra)
  endif ()
endfunction()

option(ATELIER_BUILD_TESTS "Build the unit tests in tests/unit at the repository root" ON)

add_subdirectory ("projects/atelier_engine")
add_subdirectory ("projects/atelier_editor")

# Unit tests run on the build machine, so skip them when cross-compiling
if (ATELIER_BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
  enable_testing()
  add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit" "${CMAKE_BINARY_DIR}/tests")
endif ()
