@echo off
chcp 65001
echo Starting to build NightSurvivors project...

set QT_DIR=D:\Qt20250127\6.8.1\mingw_64
set PATH=%QT_DIR%\bin;%PATH%

echo Creating temp build directory...
set TEMP_DIR=C:\NightSurvivors_build
if exist %TEMP_DIR% rmdir /s /q %TEMP_DIR%
mkdir %TEMP_DIR%

echo Copying project files to temp directory...
xcopy /E /I /Y %~dp0\src %TEMP_DIR%\src
xcopy /E /I /Y %~dp0\include %TEMP_DIR%\include
xcopy /E /I /Y %~dp0\resources %TEMP_DIR%\resources
xcopy /E /I /Y %~dp0\forms %TEMP_DIR%\forms
xcopy /E /I /Y %~dp0\ui %TEMP_DIR%\ui
copy /Y %~dp0\CMakeLists.txt %TEMP_DIR%\
copy /Y %~dp0\CMakePresets.json %TEMP_DIR%\

cd %TEMP_DIR%
mkdir build
cd build

echo Configuring CMake project...
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=%QT_DIR%

echo Starting compilation...
cmake --build . --config Release

echo Copying executable back to original location...
if not exist %~dp0\bin mkdir %~dp0\bin
copy /Y .\NightSurvivors.exe %~dp0\bin\

echo Copying Qt dynamic libraries...
cd %~dp0\bin
windeployqt NightSurvivors.exe

echo Build completed!
cd %~dp0
pause 