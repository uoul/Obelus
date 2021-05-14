QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    eventwindow.cpp \
    global.cpp \
    main.cpp \
    mainwindow.cpp \
    mytemplate.cpp \
    slidesystem.cpp

HEADERS += \
    eventwindow.h \
    global.h \
    mainwindow.h \
    mytemplate.h \
    slidesystem.h

FORMS += \
    eventwindow.ui \
    mainwindow.ui

TRANSLATIONS += \
    Obelus_de_AT.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressourcen.qrc
