QT       += core gui network

VERSION = 1.0.0

RC_ICONS = icon.ico

win32{ LIBS += -luser32 }

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TRANSLATIONS = translation/MTC_Now_zh_CN.ts\
               translation/MTC_Now_zh_TW.ts

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    languagedialog.cpp \
    main.cpp \
    mainwindow.cpp \
    ntpdialog.cpp \
    serverdialog.cpp

HEADERS += \
    languagedialog.h \
    mainwindow.h \
    ntpdialog.h \
    serverdialog.h

FORMS += \
    languagedialog.ui \
    mainwindow.ui \
    ntpdialog.ui \
    serverdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
