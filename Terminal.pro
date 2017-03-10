QT += widgets serialport

TARGET = Terminal
TEMPLATE = app
win32:RC_FILE = Terminal.rc

SOURCES += main.cpp\
    MainWindow.cpp \
    HexEncoder.cpp \
    DataEncoder.cpp \
    AsciiEncoder.cpp \
    DecEncoder.cpp \
    RightClickedButton.cpp \
    LimitedItemsListWidget.cpp \
    MacrosWidget.cpp \
    MacrosItemWidget.cpp

HEADERS  += \
    MainWindow.h \
    DataEncoder.h \
    HexEncoder.h \
    AsciiEncoder.h \
    DecEncoder.h \
    RightClickedButton.h \
    LimitedItemsListWidget.h \
    MacrosWidget.h \
    MacrosItemWidget.h

RESOURCES += \
    Terminal.qrc
