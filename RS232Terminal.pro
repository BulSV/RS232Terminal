QT       += widgets serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RS232Terminal
TEMPLATE = app
win32:RC_FILE = RS232Terminal.rc

SOURCES += main.cpp\
        Dialog.cpp \
    rs232terminalprotocol.cpp \
    ComPort.cpp \
    Macro.cpp

HEADERS  += Dialog.h \
    rs232terminalprotocol.h \
    ComPort.h \
    IProtocol.h \
    Macro.h

RESOURCES += \
    RS232Terminal.qrc
