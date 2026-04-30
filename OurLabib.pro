QT += core gui widgets sql network charts quick quickwidgets serialport printsupport

CONFIG += c++17

SOURCES += \
    main.cpp \
    ourlabib.cpp \
    gestion_equipe.cpp \
    zones.cpp \
    chatbotequie.cpp \
    zonechatbot.cpp \
    twillio.cpp \
    mapzones.cpp \
    connection.cpp \
    arduino.cpp \
    employes.cpp \
    login.cpp \
    missions.cpp \
    poubelles.cpp

HEADERS += \
    ourlabib.h \
    gestion_equipe.h \
    zones.h \
    chatbotequie.h \
    zonechatbot.h \
    twillio.h \
    mapzones.h \
    connection.h \
    arduino.h \
    employes.h \
    missions.h \
    poubelles.h \
    qrender.h

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
