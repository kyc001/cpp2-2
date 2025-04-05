@ECHO OFF
SETLOCAL

REM Setup VS environment
CALL "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

REM Compiler settings
SET warnings=/W4 /wd4505 /wd4100
SET includes=/I"third_party" /I"third_party/Include"
SET defines=/D DEBUG /D GAME /D _CRT_SECURE_NO_WARNINGS
SET links=/link /LIBPATH:"third_party/Lib" user32.lib Shell32.lib opengl32.lib gdi32.lib freetype.lib ole32.lib

REM Clean old files
IF EXIST *.pdb DEL /Q *.pdb

REM Build game.dll
ECHO Building game.dll...
cl /nologo /std:c++20 %warnings% /Zc:strictStrings- /LD /FC src/game/game.cpp /Z7 %defines% /link /PDB:game.pdb /INCREMENTAL:NO

REM Build Survivors.exe
ECHO Building Survivors.exe...
cl /nologo /std:c++20 %warnings% /Zc:strictStrings- /FC src/platform/win32_platform.cpp /Z7 /FeSurvivors.exe %defines% %includes% %links% /INCREMENTAL:NO

ECHO Build complete!

