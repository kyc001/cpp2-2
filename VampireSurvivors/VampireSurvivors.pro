QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 添加包含路径
INCLUDEPATH += include
INCLUDEPATH += $$[QT_INSTALL_HEADERS]
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtCore
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtGui
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtWidgets

# 源文件
SOURCES += \
    src/core/gamestate.cpp \
    src/core/gamemap.cpp \
    src/core/weapon.cpp \
    src/core/enemycontroller.cpp \
    src/core/obstacle.cpp \
    src/entities/hero.cpp \
    src/entities/enemy.cpp \
    src/entities/bullet.cpp \
    src/entities/drop.cpp \
    src/ui/mainscene.cpp \
    src/ui/shopui.cpp \
    src/ui/upgradeui.cpp \
    src/ui/saveui.cpp \
    src/ui/settingsui.cpp \
    src/ui/introductionui.cpp \
    src/ui/gamemenuui.cpp \
    src/utils/savefile.cpp \
    src/utils/resourcemanager.cpp \
    src/main.cpp

# 头文件
HEADERS += \
    include/core/gamestate.h \
    include/core/gamemap.h \
    include/core/weapon.h \
    include/core/enemycontroller.h \
    include/core/obstacle.h \
    include/entities/hero.h \
    include/entities/enemy.h \
    include/entities/bullet.h \
    include/entities/drop.h \
    include/ui/mainscene.h \
    include/ui/shopui.h \
    include/ui/upgradeui.h \
    include/ui/saveui.h \
    include/ui/settingsui.h \
    include/ui/introductionui.h \
    include/ui/gamemenuui.h \
    include/utils/savefile.h \
    include/utils/resourcemanager.h

# UI文件
FORMS += \
    forms/mainscene.ui

# 资源文件
RESOURCES += \
    resources/resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
