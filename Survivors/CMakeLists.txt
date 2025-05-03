cmake_minimum_required(VERSION 3.25)
project(Survivors)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 添加UI文件目录作为包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# 使用Windows上自动检测Qt
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        MultimediaWidgets
        REQUIRED)

# 定义源文件分组
set(GAME_LOGIC_SRC
    GameLogic/GameState.cpp
    GameLogic/GameState.h
    GameLogic/config.h
    GameLogic/config.cpp
    GameLogic/PaintInfo.cpp
    GameLogic/PaintInfo.h
    GameLogic/GameMap.cpp
    GameLogic/GameMap.h
    GameLogic/Hero.cpp
    GameLogic/Hero.h
    GameLogic/Option.cpp
    GameLogic/Option.h
    GameLogic/Weapon.cpp
    GameLogic/Weapon.h
    GameLogic/Bullet.cpp
    GameLogic/Bullet.h
    GameLogic/Enemy.cpp
    GameLogic/Enemy.h
    GameLogic/EnemyController.cpp
    GameLogic/EnemyController.h
    GameLogic/ExpBall.cpp
    GameLogic/ExpBall.h
    GameLogic/FloatingOrb.cpp
    GameLogic/FloatingOrb.h
)

set(MAIN_SRC
    src/main.cpp
    src/gamemain.cpp
    src/gamemain.h
    src/gameres.qrc
)

set(UI_SRC
    src/ui/menu.cpp
    src/ui/menu.h
)

set(DIALOG_SRC
    src/dialogs/upgradedialog.cpp
    src/dialogs/upgradedialog.h
    src/dialogs/gameoverdialog.cpp
    src/dialogs/gameoverdialog.h
    src/dialogs/settingdialog.cpp
    src/dialogs/settingdialog.h
    src/dialogs/aboutdialog.cpp
    src/dialogs/aboutdialog.h
    src/dialogs/heroselectdialog.cpp
    src/dialogs/heroselectdialog.h
)

# 主可执行文件
add_executable(Survivors 
    ${MAIN_SRC}
    ${GAME_LOGIC_SRC}
    ${UI_SRC}
    ${DIALOG_SRC}
)

target_link_libraries(Survivors
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Multimedia
    Qt::MultimediaWidgets
    winmm  # 添加Windows多媒体库，用于mciSendStringW函数
)

