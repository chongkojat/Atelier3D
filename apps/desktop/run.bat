@echo off
REM ===============================================
REM Atelier3D build and run helper (Windows)
REM Usage:
REM   run.bat                                   build Debug and open the editor
REM   run.bat new ^<name^> [empty^|cube^|pyramid]  build Debug and create a new game in projects\ next to the exe
REM   run.bat release ...                       same as above, with a Release build
REM Builds incrementally into build\dev (clean.bat removes it)
REM ===============================================
setlocal

set CONFIG=Debug
if /i "%~1"=="release" (
    set CONFIG=Release
    shift
)

set ROOT=%~dp0
set BUILD_DIR=%ROOT%build\dev
set BIN_DIR=%BUILD_DIR%\bin\%CONFIG%

cmake --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake is not in PATH. Run this from "Developer PowerShell for VS" or install CMake.
    exit /b 1
)

if not exist "%BUILD_DIR%\CMakeCache.txt" (
    echo [1/2] Configuring...
    cmake -S "%ROOT%." -B "%BUILD_DIR%" -A x64
    if errorlevel 1 exit /b 1
)

echo [2/2] Building %CONFIG%...
cmake --build "%BUILD_DIR%" --config %CONFIG% --parallel -- /nologo /verbosity:minimal
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

if /i "%~1"=="new" goto new

"%BIN_DIR%\AtelierEditor.exe"
exit /b %errorlevel%

:new
if "%~2"=="" (
    echo Usage: run.bat new ^<name^> [empty^|cube^|pyramid]
    exit /b 1
)
set TEMPLATE_ARGS=
if not "%~3"=="" set TEMPLATE_ARGS=--template %3
"%BIN_DIR%\AtelierEditor.exe" --new-game %2 %TEMPLATE_ARGS%
exit /b %errorlevel%
