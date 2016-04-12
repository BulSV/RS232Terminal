QT       += core gui
QT += widgets serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RS232Terminal
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h
