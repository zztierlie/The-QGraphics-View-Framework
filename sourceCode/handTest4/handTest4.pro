#-------------------------------------------------
#
# Project created by QtCreator 2017-02-08T10:06:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES +=HAVE_CONFIG_H
TARGET = handTest4
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imageitem.cpp \
    textitem.cpp \
    qrcode/bitstream.c \
    qrcode/mask.c \
    qrcode/mmask.c \
    qrcode/mqrspec.c \
    qrcode/qrencode.c \
    qrcode/qrinput.c \
    qrcode/qrspec.c \
    qrcode/rscode.c \
    qrcode/split.c \
    dialog.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    imageitem.h \
    textitem.h \
    qrcode/bitstream.h \
    qrcode/config.h \
    qrcode/mask.h \
    qrcode/mmask.h \
    qrcode/mqrspec.h \
    qrcode/qrencode.h \
    qrcode/qrencode_inner.h \
    qrcode/qrinput.h \
    qrcode/qrspec.h \
    qrcode/rscode.h \
    qrcode/split.h \
    dialog.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    dialog.ui \
    aboutdialog.ui

RESOURCES += \
    stylesheet.qrc
