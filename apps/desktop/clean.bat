@echo off
REM ===============================================
REM Removes all build output: build\ (run.bat, CI scripts and Visual Studio) and the old out\ folder
REM ===============================================
setlocal

set ROOT=%~dp0

for %%D in (build out) do (
    if exist "%ROOT%%%D" (
        echo Removing %%D\ ...
        rmdir /s /q "%ROOT%%%D"
    )
)

echo Clean completed.
