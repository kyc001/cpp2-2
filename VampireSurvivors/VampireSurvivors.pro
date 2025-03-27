QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bullet.cpp \
    enemy.cpp \
    enemycontroller.cpp \
    gamemap.cpp \
    gamestate.cpp \
    hero.cpp \
    main.cpp \
    mainscene.cpp \
    obstacle.cpp \
    drop.cpp \
    upgradeui.cpp \
    shopui.cpp \
    saveui.cpp \
    savefile.cpp \
    weapon.cpp

HEADERS += \
    bullet.h \
    enemy.h \
    enemycontroller.h \
    gamemap.h \
    gamestate.h \
    hero.h \
    mainscene.h \
    obstacle.h \
    drop.h \
    upgradeui.h \
    shopui.h \
    saveui.h \
    savefile.h \
    weapon.h

FORMS += \
    mainscene.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
