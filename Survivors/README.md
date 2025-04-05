# Vampire Survivors Clone
A small and simple Vampire Survivors clone made with OpenGL

# Build Guide

1. To build the game you need Visual Sudio 2022, get it here:
https://visualstudio.microsoft.com/vs/

2. Once installed, check `build.bat` and make sure the path:
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"`

3. If it's not corret, then update the path to match your system!

4. Call `build.bat` and the program should build.

Enjoy

# 项目名称 (请替换)

这是一个简单的 C++ 项目骨架。

## 模块结构

源代码位于 `src` 目录下，并按功能模块划分为以下子目录：

*   `src/core/`: 包含核心的、通用的代码，如数学库、日志、配置文件、通用数据结构等。
*   `src/platform/`: 包含平台相关的代码，如窗口创建、输入处理、文件 IO、音频接口等（目前仅支持 Win32）。
*   `src/graphics/`: 包含图形渲染相关的代码，如渲染接口、OpenGL 实现、着色器定义、颜色定义等。
*   `src/audio/`: 包含音频处理相关的代码（目前主要是声音定义）。
*   `src/assets/`: 包含资源管理相关的代码，如资源加载、精灵定义等。
*   `src/input/`: 包含输入抽象和处理代码。
*   `src/game/`: 包含游戏核心逻辑代码，如游戏状态、实体、游戏循环等。

## 构建

(请在此处添加构建说明，例如所需的编译器、依赖项和构建命令)

## 运行

(请在此处添加运行说明)

## 开发者注意

*   代码风格统一使用 `KYC_` 前缀代替之前的 `CAKEZ_`。
*   头文件引用已根据模块化结构进行了调整。
*   部分头文件（如 `common.h`, `my_math.h`）可能需要被更显式地包含在使用它们的文件中，以改善模块独立性。
*   部分 `.cpp` 文件直接被其他 `.cpp` 文件包含 (`#include "*.cpp"`)，这不是推荐的做法，未来可以考虑使用更标准的链接方式。
