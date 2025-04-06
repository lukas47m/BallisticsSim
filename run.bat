REM I did not test .bat, I tested only .sh

@echo off
setlocal enabledelayedexpansion

REM Create build directory if it doesn't exist
if not exist output mkdir output

REM Navigate to build directory
cd output

REM Configure with CMake
cmake ..
if %errorlevel% neq 0 (
    echo CMake configuration failed
    exit /b %errorlevel%
)

REM Build the project
cmake --build .
if %errorlevel% neq 0 (
    echo Build failed
    exit /b %errorlevel%
)

REM Run the executable
BallisticsSim.exe
if %errorlevel% neq 0 (
    echo Execution failed
    exit /b %errorlevel%
)

echo Program executed successfully! 