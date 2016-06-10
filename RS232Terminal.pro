QT       += widgets serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RS232Terminal
TEMPLATE = app
win32:RC_FILE = RS232Terminal.rc

SOURCES += main.cpp\
    rs232terminalprotocol.cpp \
    ComPort.cpp \
    Macros.cpp \
    MacroWindow.cpp \
    MainWindow.cpp \
    MacrosEditing.cpp

HEADERS  += \
    rs232terminalprotocol.h \
    ComPort.h \
    IProtocol.h \
    Macros.h \
    MacroWindow.h \
    MainWindow.h \
    MiniMacros.h \
    MacrosEditing.h \
    MyPushButton.h

RESOURCES += \
    RS232Terminal.qrc
