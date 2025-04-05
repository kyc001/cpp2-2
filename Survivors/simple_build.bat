@ECHO OFF

ECHO =============================================
ECHO Simple Build Script for Survivors Project
ECHO =============================================

REM Setup VS environment
CALL "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

REM Create response files for compiler options (alternative method)
ECHO /W4 > cl_options.rsp
ECHO /wd4505 >> cl_options.rsp
ECHO /wd4100 >> cl_options.rsp
ECHO /utf-8 >> cl_options.rsp
ECHO /Zc:strictStrings- >> cl_options.rsp
ECHO /Z7 >> cl_options.rsp
ECHO /D DEBUG >> cl_options.rsp
ECHO /D GAME >> cl_options.rsp
ECHO /D _CRT_SECURE_NO_WARNINGS >> cl_options.rsp
ECHO /I"third_party" >> cl_options.rsp
ECHO /I"third_party/Include" >> cl_options.rsp
ECHO /link >> cl_options.rsp
ECHO /LIBPATH:"third_party/Lib" >> cl_options.rsp
ECHO user32.lib >> cl_options.rsp
ECHO Shell32.lib >> cl_options.rsp
ECHO opengl32.lib >> cl_options.rsp
ECHO gdi32.lib >> cl_options.rsp
ECHO freetype.lib >> cl_options.rsp
ECHO ole32.lib >> cl_options.rsp
ECHO /INCREMENTAL:NO >> cl_options.rsp

ECHO /W4 > vsClone_options.rsp
ECHO /wd4505 >> vsClone_options.rsp
ECHO /wd4100 >> vsClone_options.rsp
ECHO /utf-8 >> vsClone_options.rsp
ECHO /Zc:strictStrings- >> vsClone_options.rsp
ECHO /Z7 >> vsClone_options.rsp
ECHO /D DEBUG >> vsClone_options.rsp
ECHO /D GAME >> vsClone_options.rsp
ECHO /D _CRT_SECURE_NO_WARNINGS >> vsClone_options.rsp
ECHO /I"third_party" >> vsClone_options.rsp
ECHO /I"third_party/Include" >> vsClone_options.rsp
ECHO /link >> vsClone_options.rsp
ECHO /LIBPATH:"third_party/Lib" >> vsClone_options.rsp
ECHO user32.lib >> vsClone_options.rsp
ECHO Shell32.lib >> vsClone_options.rsp
ECHO opengl32.lib >> vsClone_options.rsp
ECHO gdi32.lib >> vsClone_options.rsp
ECHO freetype.lib >> vsClone_options.rsp
ECHO ole32.lib >> vsClone_options.rsp
ECHO /INCREMENTAL:NO >> vsClone_options.rsp

ECHO --- Contents of cl_options.rsp ---
TYPE cl_options.rsp
ECHO -----------------------------------
ECHO --- Contents of vsClone_options.rsp ---
TYPE vsClone_options.rsp
ECHO -----------------------------------
PAUSE

REM Clean old files
IF EXIST *.pdb DEL /Q *.pdb

REM Build game.dll
ECHO Building game.dll...
cl /nologo /std:c++20 /LD /FC src/core/game.cpp @cl_options.rsp /link /PDB:game.pdb /LIBPATH:"third_party/Lib"
IF ERRORLEVEL 1 GOTO error

REM Build vsClone.exe
ECHO Building vsClone.exe...
cl /nologo /std:c++20 /FC src/core/win32_platform.cpp /FevsClone.exe @vsClone_options.rsp /link /LIBPATH:"third_party/Lib"
IF ERRORLEVEL 1 GOTO error

REM Clean up response files
DEL /Q cl_options.rsp
DEL /Q vsClone_options.rsp

ECHO Build completed successfully!
EXIT /B 0

:error
ECHO Build failed! See errors above.
IF EXIST cl_options.rsp DEL /Q cl_options.rsp
IF EXIST vsClone_options.rsp DEL /Q vsClone_options.rsp
EXIT /B 1 