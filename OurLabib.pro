QT += sql printsupport charts network
QT += core gui  serialport
CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    arduino.cpp \
    connection.cpp \
    employes.cpp \
    equipes.cpp \
    login.cpp \
    main.cpp \
    missions.cpp \
    ourlabib.cpp \
    poubelles.cpp \
    zones.cpp

HEADERS += \
    arduino.h \
    connection.h \
    employes.h \
    equipes.h \
    missions.h \
    ourlabib.h \
    poubelles.h \
    zones.h

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

# --- OpenCV Configuration ---
DEFINES += USE_OPENCV

OPENCV_ROOT = "C:/Users/LENOVO/Desktop/qt/ourlabib/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/OpenCV-MinGW-Build-OpenCV-4.5.5-x64"

INCLUDEPATH += $$OPENCV_ROOT/include
LIBS += -L$$OPENCV_ROOT/x64/mingw/lib \
        -lopencv_core455 \
        -lopencv_imgproc455 \
        -lopencv_imgcodecs455 \
        -lopencv_highgui455 \
        -lopencv_videoio455 \
        -lopencv_objdetect455

