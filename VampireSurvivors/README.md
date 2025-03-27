# VampireSurvivors

一个基于 Qt 开发的 2D 动作游戏，玩家需要在一个充满敌人的地图中生存，通过击败敌人获得经验值和金币，提升角色能力。

## 项目结构

```
VampireSurvivors/
├── include/              # 头文件目录
│   ├── core/            # 核心游戏逻辑
│   ├── entities/        # 游戏实体
│   ├── ui/              # 用户界面
│   └── utils/           # 工具类
├── src/                 # 源文件目录
│   ├── core/            # 核心游戏逻辑实现
│   ├── entities/        # 游戏实体实现
│   ├── ui/              # 用户界面实现
│   └── utils/           # 工具类实现
├── resources/           # 资源文件目录
│   ├── images/          # 图片资源
│   ├── sounds/          # 音效资源
│   └── maps/            # 地图资源
├── forms/               # Qt UI 文件
└── build/               # 构建目录
```

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

## 构建说明

1. 确保安装了 Qt 开发环境（推荐 Qt 6.0 或更高版本）
2. 克隆项目到本地
3. 使用 Qt Creator 打开 `VampireSurvivors.pro` 文件
4. 配置项目并构建

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