@echo off
echo Cleaning VampireSurvivors...

:: 删除构建目录
if exist build rmdir /S /Q build

:: 删除临时文件
del /Q /F *.o 2>nul
del /Q /F *.obj 2>nul
del /Q /F *.exe 2>nul
del /Q /F *.dll 2>nul
del /Q /F *.lib 2>nul
del /Q /F *.a 2>nul
del /Q /F *.so 2>nul
del /Q /F *.dylib 2>nul

echo Clean complete!
pause 