#-------------------------------------------------
#
# Project created by QtCreator 2016-12-20T16:24:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = histogramm
TEMPLATE = app

CONFIG += c++11

win32 {
  QMAKE_CXXFLAGS+= -openmp
}
!win32{
  QMAKE_CXXFLAGS+= -fopenmp
  QMAKE_LFLAGS += -fopenmp
}

SOURCES += main.cpp\
        mainwindow.cpp \
    transformcurveview.cpp \
    transformthread.cpp \
    liveimageview.cpp

HEADERS  += mainwindow.h \
    transformcurveview.h \
    transformthread.h \
    liveimageview.h

FORMS    += mainwindow.ui
