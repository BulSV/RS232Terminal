QT += widgets serialport

TARGET = Terminal
TEMPLATE = app
win32:RC_FILE = Terminal.rc

SOURCES += main.cpp\
    MainWindow.cpp \
    Macros.cpp \
    MiniMacros.cpp \
    HexEncoder.cpp \
    DataEncoder.cpp \
    AsciiEncoder.cpp \
    DecEncoder.cpp \
    RightClickedButton.cpp \
    LimitedItemsListWidget.cpp

HEADERS  += \
    MainWindow.h \
    Macros.h \
    MiniMacros.h \
    DataEncoder.h \
    HexEncoder.h \
    AsciiEncoder.h \
    DecEncoder.h \
    RightClickedButton.h \
    LimitedItemsListWidget.h

RESOURCES += \
    Terminal.qrc
