QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TRANSLATIONS  =MTC_Now_en.ts\
              MTC_Now_zh-CN.ts

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    tlcdcurrentdate.cpp \
    tlcdcurrenttime.cpp \
    widget.cpp

HEADERS += \
    tlcdcurrentdate.h \
    tlcdcurrenttime.h \
    widget.h

FORMS += \
    tlcdcurrentdate.ui \
    tlcdcurrenttime.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
