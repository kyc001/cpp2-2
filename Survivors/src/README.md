# 游戏代码结构

本项目代码已按功能模块进行重构，更加模块化和可维护。以下是主要目录结构：

## 目录结构

### core/ - 核心模块
- `common.h` - 通用数据结构和函数
- `config.h` - 游戏配置参数
- `defines.h` - 宏定义和常量
- `assets.h/cpp` - 资源管理
- `game.h/cpp` - 游戏主逻辑
- `gameData.h` - 游戏数据结构
- `platform.h` - 平台抽象接口
- `win32_platform.cpp` - Windows平台实现
- `input.h` - 输入处理
- `shared.h` - 共享全局变量和状态

### entities/ - 实体模块
- `entity.h` - 基础实体定义
- `player.h` - 玩家实体
- `enemy.h` - 敌人实体
- `pickup.h` - 拾取物品
- `obstacle.h` - 障碍物

### weapons/ - 武器模块
- `weapon.h` - 武器系统

### rendering/ - 渲染模块
- `render_interface.h/cpp` - 渲染接口
- `gl_renderer.cpp` - OpenGL渲染器实现
- `custom_gl.h` - 自定义OpenGL功能
- `shader_header.h` - 着色器头文件

### utils/ - 工具模块
- `colors.h` - 颜色定义
- `my_math.h` - 数学工具函数
- `logger.h` - 日志系统
- `sound.h` - 音频系统
- `easing_functions.h` - 缓动函数

### ui/ - 用户界面模块
- `texts.h` - 文本和界面元素

### game_states/ - 游戏状态模块
- （预留目录，用于后续实现不同游戏状态）

## 主入口文件
- `game_module.h` - 统一包含所有模块的头文件，方便其他地方引用

## 使用方法
在需要使用游戏功能的源文件中，只需包含：
```cpp
#include "game_module.h"
```

## 编译说明
使用项目根目录下的`build.bat`来编译项目。 