set(CMAKE_C_COMPILER "D:/Qt20250127/Tools/mingw1310_64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "D:/Qt20250127/Tools/mingw1310_64/bin/g++.exe")


cmake_minimum_required(VERSION 3.16)

# 设置UTF-8编码支持
if(WIN32)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finput-charset=UTF-8 -fexec-charset=UTF-8")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8 -fexec-charset=UTF-8")
endif()

# 强制设置Windows代码页为UTF-8
if(WIN32)
  execute_process(COMMAND cmd /c chcp 65001)
endif()

project(NightSurvivors VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置Qt路径
set(CMAKE_PREFIX_PATH "D:/Qt20250127/6.8.1/mingw_64")

# UI文件输出目录
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)
set(CMAKE_AUTOUIC_OPTIONS -tr QObject::tr)

# 查找Qt包
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 设置源文件
set(SOURCE_FILES
    src/core/gamestate.cpp
    src/core/gamemap.cpp
    src/core/weapon.cpp
    src/core/enemycontroller.cpp
    src/core/obstacle.cpp
    src/entities/hero.cpp
    src/entities/enemy.cpp
    src/entities/bullet.cpp
    src/entities/drop.cpp
    src/ui/mainscene.cpp
    src/ui/upgradeui.cpp
    src/ui/shopui.cpp
    src/ui/saveui.cpp
    src/ui/settingsui.cpp
    src/ui/introductionui.cpp
    src/ui/gamemenuui.cpp
    src/utils/resourcemanager.cpp
    src/utils/savefile.cpp
    src/main.cpp
)

# 设置头文件
set(HEADER_FILES
    include/core/gamestate.h
    include/core/gamemap.h
    include/core/weapon.h
    include/core/enemycontroller.h
    include/core/obstacle.h
    include/entities/hero.h
    include/entities/enemy.h
    include/entities/bullet.h
    include/entities/drop.h
    include/ui/mainscene.h
    include/ui/upgradeui.h
    include/ui/shopui.h
    include/ui/saveui.h
    include/ui/settingsui.h
    include/ui/introductionui.h
    include/ui/gamemenuui.h
    include/utils/resourcemanager.h
    include/utils/savefile.h
)

# 设置UI文件
set(UI_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/forms/mainscene.ui
)

# 注释掉资源文件
# 解除注释，确保资源文件能够正确加载
set(RESOURCE_FILES
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/game_resources.qrc
)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/forms
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_BINARY_DIR}
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# 设置编译选项
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
#    -Werror
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

# Windows特定设置
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        UNICODE
        _UNICODE
        WIN32
        MINGW_HAS_SECURE_API=1
    )
    
    # 确保可执行文件能够找到 Qt DLL
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "正在复制 Qt 库到目标目录..."
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                $<TARGET_FILE:Qt6::Core>
                $<TARGET_FILE:Qt6::Gui>
                $<TARGET_FILE:Qt6::Widgets>
                ${CMAKE_CURRENT_SOURCE_DIR}/bin
        )
    endif()
endif()

# Install the executable
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
) 