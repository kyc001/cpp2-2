QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    baseplayscene.cpp \
    button.cpp \
    choosemodelscene.cpp \
    main.cpp \
    mapdrawer.cpp \
    playscene1.cpp \
    playscene2.cpp \
    resourcemanager.cpp \
    tank.cpp \
    mainwindow.cpp

HEADERS += \
    baseplayscene.h \
    button.h \
    choosemodelscene.h \
    mainwindow.h \
    mapdrawer.h \
    playscene1.h \
    playscene2.h \
    resourcemanager.h \
    tank.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
