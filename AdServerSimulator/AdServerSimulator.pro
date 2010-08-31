QT       += network

#LIBS += -L. -lAdServerCore

unix:{
  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/lib
  QMAKE_RPATH=
}

#CONFIG += static
unix:TARGET = AdServerSimulator

SOURCES += \
    adserversimulator.cpp \
    main.cpp \
    channellogview.cpp \
    sessionsmodel.cpp \
    logsmodel.cpp \
    channelconfigview.cpp \
    adblockitem.cpp \
    adblockedit.cpp \
    adconfigitem.cpp \
    esitem.cpp \
    descriptoritem.cpp \
    channelconfigitem.cpp \
    ../AdServerCore/helpers.cpp \
    ../AdServerCore/channellogger.cpp \
    ../AdServerCore/channelconfig.cpp \
    ../AdServerCore/adchannel.cpp \
    ../AdServerCore/splicestates.cpp \
    ../AdServerCore/splicesessions.cpp \
    ../AdServerCore/scte30messages.cpp

FORMS += \
    adserversimulator.ui \
    channellogview.ui \
    channelconfigview.ui \
    adblockitem.ui \
    adblockedit.ui \
    adconfigitem.ui \
    esitem.ui \
    descriptoritem.ui \
    channelconfigitem.ui

HEADERS += \
    adserversimulator.h \
    channellogview.h \
    sessionsmodel.h \
    logsmodel.h \
    channelconfigview.h \
    adblockitem.h \
    adblockedit.h \
    adconfigitem.h \
    esitem.h \
    descriptoritem.h \
    channelconfigitem.h \
    ../AdServerCore/splicestates.h \
    ../AdServerCore/splicesessions.h \
    ../AdServerCore/scte30messages.h \
    ../AdServerCore/helpers.h \
    ../AdServerCore/channellogger.h \
    ../AdServerCore/channelconfig.h \
    ../AdServerCore/adchannel.h
INCLUDEPATH += ../AdServerCore

RESOURCES += \
    resources.qrc

