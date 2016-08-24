#-------------------------------------------------
#
# Project created by QtCreator 2014-07-29T02:19:25
#
#-------------------------------------------------

QT       -= core gui

TARGET = thread_pool
TEMPLATE = lib
CONFIG += staticlib

SOURCES += thread_pool.cpp

HEADERS += thread_pool.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
CONFIG +=c++11
