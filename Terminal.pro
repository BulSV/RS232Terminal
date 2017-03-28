QT += widgets serialport
CONFIG += C++11

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
    MacrosEditWidget.cpp \
    MacrosWidget.cpp \
    MacrosRawEditWidget.cpp \
    OpenMacros.cpp \
    SaveMacros.cpp \
    DataTable.cpp

HEADERS  += \
    MainWindow.h \
    DataEncoder.h \
    HexEncoder.h \
    AsciiEncoder.h \
    DecEncoder.h \
    RightClickedButton.h \
    LimitedItemsListWidget.h \
    MacrosEditWidget.h \
    MacrosWidget.h \
    MacrosRawEditWidget.h \
    OpenMacros.h \
    SaveMacros.h \
    DataTable.h

RESOURCES += \
    Terminal.qrc
