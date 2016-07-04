QT       += widgets serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RS232Terminal
TEMPLATE = app
win32:RC_FILE = RS232Terminal.rc

SOURCES += main.cpp\
    rs232terminalprotocol.cpp \
    ComPort.cpp \
    MainWindow.cpp \
    Macros.cpp

HEADERS  += \
    rs232terminalprotocol.h \
    ComPort.h \
    IProtocol.h \
    MainWindow.h \
    MiniMacros.h \
    MyPushButton.h \
    Macros.h

RESOURCES += \
    RS232Terminal.qrc
