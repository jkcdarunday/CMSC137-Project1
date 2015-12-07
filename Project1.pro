QT += core network
QT -= gui

CONFIG += c++11

TARGET = Project1
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    kserver.cpp \
    kclient.cpp \
    kheader.cpp

HEADERS += \
    kserver.h \
    kclient.h \
    kheader.h
