QT       += widgets serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RS232Terminal
TEMPLATE = app


SOURCES += main.cpp\
        Dialog.cpp \
    rs232terminalprotocol.cpp \
    ComPort.cpp

HEADERS  += Dialog.h \
    rs232terminalprotocol.h \
    ComPort.h \
    IProtocol.h

RESOURCES += \
    RS232Terminal.qrc
