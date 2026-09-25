# Cross-compile Windows x64 .exe files from Linux with MinGW-w64
# Usage: cmake -S Source -B build -DCMAKE_TOOLCHAIN_FILE=Source/cmake/mingw-w64-x86_64.cmake
# Ubuntu: sudo apt install mingw-w64

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(GAM300_MINGW_PREFIX x86_64-w64-mingw32)
set(CMAKE_C_COMPILER   ${GAM300_MINGW_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${GAM300_MINGW_PREFIX}-g++)
set(CMAKE_RC_COMPILER  ${GAM300_MINGW_PREFIX}-windres)

set(CMAKE_FIND_ROOT_PATH /usr/${GAM300_MINGW_PREFIX})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Link the GCC/C++ runtimes statically so the .exe runs without extra MinGW DLLs
set(CMAKE_EXE_LINKER_FLAGS_INIT "-static -static-libgcc -static-libstdc++")
