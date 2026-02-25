QT += core gui sql widgets printsupport charts
CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connection.cpp \
    gestion_equipe.cpp \
    main.cpp \
    ourlabib.cpp

HEADERS += \
    connection.h \
    gestion_equipe.h \
    ourlabib.h

FORMS += \
    ourlabib.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    style.qss

RESOURCES += \
    src.qrc
