TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

INCLUDEPATH += E:\opencv\build\include

LIBS += E:\opencv-build\bin\libopencv_core320.dll
LIBS += E:\opencv-build\bin\libopencv_highgui320.dll
LIBS += E:\opencv-build\bin\libopencv_imgcodecs320.dll
LIBS += E:\opencv-build\bin\libopencv_imgproc320.dll
LIBS += E:\opencv-build\bin\libopencv_features2d320.dll
LIBS += E:\opencv-build\bin\libopencv_calib3d320.dll
LIBS += E:\opencv-build\bin\libopencv_objdetect320.dll

# more correct variant, how set includepath and libs for mingw
# add system variable: OPENCV_SDK_DIR=D:/opencv/opencv-build/install
# read http://doc.qt.io/qt-5/qmake-variable-reference.html#libs

#INCLUDEPATH += $$(OPENCV_SDK_DIR)/include

#LIBS += -L$$(OPENCV_SDK_DIR)/x86/mingw/lib \
#        -lopencv_core320        \
#        -lopencv_highgui320     \
#        -lopencv_imgcodecs320   \
#        -lopencv_imgproc320     \
#        -lopencv_features2d320  \
#        -lopencv_calib3d320
