#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T14:10:49
#
#-------------------------------------------------

QT       -= core gui

TARGET = ObjectCore
TEMPLATE = lib
CONFIG += staticlib
DEFINES += OBJECTCORE_LIBRARY

SOURCES += \
    objectcore.cpp \
    actionentrance.cpp \
    managerentrance.cpp \
    queryentrance.cpp

HEADERS += \
    objectcore.h \
    actionentrance.h \
    managerentrance.h \
    IObject.h \
    IMObject.h \
    Common.h \
    queryentrance.h \
    extern.h

CONFIG +=c++11

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

LIBS += -L$$PWD/../build-thread_pool-Desktop_Qt_5_3_MSVC2013_64bit-Debug/debug/ -lthread_pool

INCLUDEPATH += $$PWD/../thread_pool
DEPENDPATH += $$PWD/../thread_pool

PRE_TARGETDEPS += $$PWD/../build-thread_pool-Desktop_Qt_5_3_MSVC2013_64bit-Debug/debug/thread_pool.lib
