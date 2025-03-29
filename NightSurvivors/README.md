# 暗夜幸存者 (NightSurvivors)

暗夜幸存者是一款Roguelike生存动作游戏，玩家需要在有限的时间内生存并击败尽可能多的敌人。

## 系统要求

- Windows 10/11 64位
- Qt 6.8.1或更高版本
- CMake 3.16或更高版本
- 支持C++17的编译器(如MinGW-w64)

## 构建项目

### 使用VSCode

1. 安装VSCode
2. 安装以下扩展：
   - C/C++
   - CMake
   - CMake Tools
3. 打开项目文件夹
4. 配置CMake
   - 按下F1，输入"CMake: Configure"
   - 选择"windows-default"预设
5. 构建项目
   - 按下F7，或选择"CMake: Build"
   - 或者运行任务"CMake Build"

### 使用命令行

```bash
# 配置项目
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=D:/Qt20250127/6.8.1/mingw_64

# 构建项目
cmake --build build

# 运行项目
./bin/NightSurvivors.exe
```

## 项目结构

- `include/` - 头文件
  - `core/` - 游戏核心系统
  - `entities/` - 游戏实体类
  - `ui/` - 用户界面类
  - `utils/` - 工具类
- `src/` - 源代码文件
- `forms/` - Qt Designer界面文件
- `resources/` - 游戏资源文件
- `bin/` - 编译后的可执行文件

## 游戏特性

- 多种角色选择(战士、法师、弓箭手、盗贼)
- 自动攻击系统
- 随机升级选择
- 多种武器和升级系统
- 各种敌人类型和BOSS战
- 金币和商店系统
- 永久升级系统

## 游戏操作

- 使用WASD键或方向键移动角色
- 空格键暂停游戏
- ESC键打开游戏菜单
- 数字键1-5快速选择升级选项

## 模块说明

### 核心模块 (Core)
- `gamestate.h/cpp`: 游戏状态管理
- `gamemap.h/cpp`: 游戏地图管理
- `weapon.h/cpp`: 武器系统
- `enemycontroller.h/cpp`: 敌人控制器
- `obstacle.h/cpp`: 障碍物系统

### 实体模块 (Entities)
- `hero.h/cpp`: 玩家角色
- `enemy.h/cpp`: 敌人基类
- `bullet.h/cpp`: 子弹系统
- `drop.h/cpp`: 掉落物系统

### 界面模块 (UI)
- `mainscene.h/cpp`: 主场景
- `shopui.h/cpp`: 商店界面
- `upgradeui.h/cpp`: 升级界面
- `saveui.h/cpp`: 存档界面

### 工具模块 (Utils)
- `savefile.h/cpp`: 存档系统

## 开发规范

1. 代码风格
   - 使用驼峰命名法
   - 类名首字母大写
   - 变量名首字母小写
   - 常量全大写

2. 注释规范
   - 类注释：说明类的功能和用途
   - 方法注释：说明参数和返回值
   - 关键代码注释：说明实现逻辑

3. 文件组织
   - 头文件和源文件分离
   - 相关功能放在同一目录
   - 避免循环依赖

## 版本控制

- 使用 Git 进行版本控制
- 主分支：main
- 开发分支：dev
- 功能分支：feature/*
- 修复分支：hotfix/*

## 贡献指南

1. Fork 项目
2. 创建功能分支
3. 提交更改
4. 发起 Pull Request

## 许可证

MIT License 