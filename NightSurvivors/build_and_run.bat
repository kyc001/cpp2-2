@echo off
echo 正在配置暗夜幸存者项目...
echo ==============================

:: 设置环境变量
set PATH=D:\Qt20250127\Tools\mingw1310_64\bin;D:\Qt20250127\6.8.1\mingw_64\bin;%PATH%

:: 创建构建目录
if not exist build (
    mkdir build
    echo 创建了构建目录
)

:: 配置项目
echo 配置项目...
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug

:: 构建项目
echo 构建项目...
cmake --build .

:: 检查构建是否成功
if %ERRORLEVEL% NEQ 0 (
    echo 构建失败！请检查错误信息。
    exit /b %ERRORLEVEL%
)

:: 返回上级目录并运行程序
cd ..
echo 启动游戏...
bin\NightSurvivors.exe

echo 已完成。 