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
    MacrosEditWidget.cpp \
    MacrosWidget.cpp \
    MacrosRawEditWidget.cpp \
    OpenMacros.cpp \
    SaveMacros.cpp \
    DataTable.cpp \
    LimitedTextEdit.cpp \
    ComPortConfigure.cpp \
    Macroses.cpp

HEADERS  += \
    MainWindow.h \
    DataEncoder.h \
    HexEncoder.h \
    AsciiEncoder.h \
    DecEncoder.h \
    RightClickedButton.h \
    MacrosEditWidget.h \
    MacrosWidget.h \
    MacrosRawEditWidget.h \
    OpenMacros.h \
    SaveMacros.h \
    DataTable.h \
    LimitedTextEdit.h \
    ComPortConfigure.h \
    Macroses.h

RESOURCES += \
    Terminal.qrc
