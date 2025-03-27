@echo off
echo Building VampireSurvivors...

:: 创建构建目录
if not exist build mkdir build
cd build

:: 运行 qmake
qmake ../VampireSurvivors.pro

:: 构建项目
nmake

:: 复制资源文件
if not exist ..\build\release\resources mkdir ..\build\release\resources
xcopy /E /I /Y ..\resources\* ..\build\release\resources\

echo Build complete!
pause 