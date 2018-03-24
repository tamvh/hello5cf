QT -= gui
QT += concurrent network websockets printsupport core xml
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    http/httpbase.cpp \
    http/httpbase2.cpp \
    http/httpparams.cpp \
    http/httptool.cpp \
    http/json.cpp \
    ../common/prntdataformat.cpp \
    ../common/configsetting.cpp \
    ../common/deviceclient.cpp \
    ../common/deviceclientservice.cpp \
    ../common/udpbrdcastclient.cpp \
    ../common/wscssclient.cpp \
    ../common/utility.cpp \
    ../common/logging.cpp \
    ../common/cmn.cpp \
    billdetail.cpp \
    commonfunction.cpp \
    wsclient.cpp \
    maincontroller.cpp

HEADERS += \
    http/httpbase.h \
    http/httpbase2.h \
    http/httpparams.h \
    http/httptool.h \
    http/json.h \
    ../common/prntdataformat.h \
    ../common/configsetting.h \
    ../common/deviceclient.h \
    ../common/deviceclientservice.h \
    ../common/udpbrdcastclient.h \
    ../common/wscssclient.h \
    ../common/utility.h \
    ../common/logging.h \
    ../common/cmn.h \
    billdetail.h \
    commonfunction.h \
    wsclient.h \
    maincontroller.h

DISTFILES += config


