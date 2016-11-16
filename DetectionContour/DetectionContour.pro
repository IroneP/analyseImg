QT += core
QT += gui
QT += widgets

TARGET = DetectionContour
CONFIG += console
CONFIG -= app_bundle
CONFIG += link_pkgconfig

unix {
    PKGCONFIG += opencv
}

TEMPLATE = app

win32 {
    INCLUDEPATH += D:/Tools/OpenCV/opencv/Generated/install/include

    CONFIG( debug, debug|release ) {
        LIBS += -LD:/Tools/OpenCV/opencv/Generated/install/x64/vc10/lib\
                -lopencv_core300d\
                -lopencv_highgui300d\
                -lopencv_imgproc300d\
                -lopencv_features2d300d\
                -lopencv_imgcodecs300d\
    }
    else {
        LIBS += -LD:/Tools/OpenCV/opencv/Generated/install/x64/vc10/lib\
                -lopencv_core300\
                -lopencv_highgui300\
                -lopencv_imgproc300\
                -lopencv_features2d300\
                -lopencv_imgcodecs300\
    }
}

SOURCES += main.cpp \
    MainWindow.cpp \
    Filter.cpp \
    Pipeline.cpp \
    Histogram.cpp \
    Image.cpp \
    PerformanceTimer.cpp \
    PerformanceTimer.inl \
    Hough.cpp \
    Algorithm.cpp

HEADERS += \
    MainWindow.h \
    Filter.h \
    Pipeline.h \
    Histogram.h \
    Image.h \
    PerformanceTimer.h \
    Hough.h \
    Algorithm.h

FORMS += \
    MainWindow.ui

