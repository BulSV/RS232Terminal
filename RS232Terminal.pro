QT       += widgets serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RS232Terminal
TEMPLATE = app
win32:RC_FILE = RS232Terminal.rc

SOURCES += main.cpp\
    MainWindow.cpp \
    Macros.cpp \
    MiniMacros.cpp \
    mylistwidget.cpp \
    HexEncoder.cpp \
    DataEncoder.cpp \
    AsciiEncoder.cpp \
    DecEncoder.cpp

HEADERS  += \
    MainWindow.h \
    MyPushButton.h \
    Macros.h \
    MiniMacros.h \
    mylistwidget.h \
    DataEncoder.h \
    HexEncoder.h \
    AsciiEncoder.h \
    DecEncoder.h

RESOURCES += \
    RS232Terminal.qrc
