@echo off
REM ===============================================
REM CI Build Script (used by Jenkins, runnable locally)
REM Usage: tools\ci\build.bat [Debug|Release|Package]
REM ===============================================
setlocal

set CONFIG=%~1
if "%CONFIG%"=="" set CONFIG=Release

set ROOT=%~dp0..\..
set BUILD_DIR=%ROOT%\apps\desktop\build\ci-%CONFIG%

echo ===============================================
echo   Atelier3D CI build - %CONFIG%
echo ===============================================
cmake --version || exit /b 1

REM Always start from a clean build directory so every run is repeatable
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"

echo [1/2] Configuring...
REM No -G: CMake picks the newest installed Visual Studio generator
cmake -S "%ROOT%\apps\desktop" -B "%BUILD_DIR%" -A x64
if errorlevel 1 (
    echo ERROR: CMake configuration failed
    exit /b 1
)

echo [2/2] Building...
cmake --build "%BUILD_DIR%" --config %CONFIG% --parallel -- /nologo /verbosity:minimal
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

echo Build succeeded: %BUILD_DIR%
exit /b 0
