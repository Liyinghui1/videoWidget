#-------------------------------------------------
#
# Project created by QtCreator 2019-12-10T14:45:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia multimediawidgets sql

TARGET = videoPlayer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    bottomwidget.cpp \
    capturedialog.cpp \
    incapturelabel.cpp \
    incapturewidget.cpp \
    filesystemdialog.cpp \
    filetreeview.cpp \
    listwidget.cpp \
    videowidget.cpp \
    topWidget.cpp \
    MySlider.cpp \
    MyToolTip.cpp \
    SplitLine.cpp \
    rightWidget.cpp \
    mainWidget.cpp

HEADERS += \
    bottomwidget.h \
    capturedialog.h \
    incapturelabel.h \
    incapturewidget.h \
    filesystemdialog.h \
    filetreeview.h \
    listwidget.h \
    videowidget.h \
    topWidget.h \
    MySlider.h \
    MyToolTip.h \
    SplitLine.h \
    rightWidget.h \
    mainWidget.h

FORMS += \
        widget.ui

RESOURCES += \
    res.qrc
