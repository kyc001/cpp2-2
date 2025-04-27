@echo off

REM Set build directory
set BUILD_DIR=build

REM Check for clean flag
if "%1"=="clean" (
    echo Cleaning build directory...
    if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
    echo Build directory cleaned.
    goto :EOF
)

REM Create build directory
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

REM Enter build directory
cd %BUILD_DIR%

REM Run CMake configuration
echo Configuring project...
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug

REM Build with Ninja
echo Building project...
ninja

REM Return to project root
cd ..

echo Build completed. Run the executable with: .\build\Survivors.exe