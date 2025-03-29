@echo off
setlocal

:: 设置项目路径和构建目录
set PROJECT_DIR=%~dp0NightSurvivors
set BUILD_DIR=%PROJECT_DIR%\build
set BIN_DIR=%PROJECT_DIR%\bin

:: 创建构建目录
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

:: 进入构建目录
cd /d "%BUILD_DIR%"

:: 询问是否需要完全清理重建
echo 是否需要完全清理并重新构建项目? (Y/N)
set /p clean_choice=
if /i "%clean_choice%"=="Y" (
    :: 删除旧的 CMake 配置文件和自动生成的文件
    if exist "CMakeCache.txt" (
        del /f /q "CMakeCache.txt"
    )
    if exist "CMakeFiles" (
        rmdir /s /q "CMakeFiles"
    )
    if exist "NightSurvivors_autogen" (
        rmdir /s /q "NightSurvivors_autogen"
    )
    
    :: 配置项目
    echo 配置项目中...
    cmake .. -G "MinGW Makefiles"
    if errorlevel 1 (
        echo 配置失败，请检查CMake配置。
        exit /b 1
    )
    
    :: 清理项目
    echo 清理项目中...
    mingw32-make clean
    if errorlevel 1 (
        echo 清理失败，继续编译...
    )
) else (
    :: 检查是否需要重新配置
    if not exist "Makefile" (
        echo 未找到Makefile，重新配置项目...
        cmake .. -G "MinGW Makefiles"
        if errorlevel 1 (
            echo 配置失败，请检查CMake配置。
            exit /b 1
        )
    )
)

:: 编译项目
echo 编译项目中...
mingw32-make
if errorlevel 1 (
    echo 编译失败，请检查代码或依赖项。
    exit /b 1
)

:: 运行生成的可执行文件
echo 运行项目中...
if exist "%BIN_DIR%\NightSurvivors.exe" (
    "%BIN_DIR%\NightSurvivors.exe"
) else (
    echo 可执行文件未找到，请检查构建输出。
    exit /b 1
)

endlocal