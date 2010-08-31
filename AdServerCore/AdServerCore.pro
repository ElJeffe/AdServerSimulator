#-------------------------------------------------
#
# Project created by QtCreator 2010-08-20T13:28:02
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = AdServerCore
TEMPLATE = lib

CONFIG += staticlib

SOURCES += \
    splicestates.cpp \
    splicesessions.cpp \
    scte30messages.cpp \
    helpers.cpp \
    channellogger.cpp \
    channelconfig.cpp \
    adchannel.cpp

HEADERS +=\
    splicestates.h \
    splicesessions.h \
    scte30messages.h \
    helpers.h \
    channellogger.h \
    channelconfig.h \
    adchannel.h
