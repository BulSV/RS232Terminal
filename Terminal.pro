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
    DataTable.cpp \
    LimitedTextEdit.cpp \
    ComPortConfigure.cpp \
    ClickableLabel.cpp \
    Macros.cpp \
    SaveMacro.cpp \
    OpenMacro.cpp \
    Macro.cpp \
    MacroEdit.cpp \
    MacroRawEdit.cpp

HEADERS  += \
    MainWindow.h \
    DataEncoder.h \
    HexEncoder.h \
    AsciiEncoder.h \
    DecEncoder.h \
    DataTable.h \
    LimitedTextEdit.h \
    ComPortConfigure.h \
    ClickableLabel.h \
    Macros.h \
    SaveMacro.h \
    OpenMacro.h \
    Macro.h \
    MacroEdit.h \
    MacroRawEdit.h

RESOURCES += \
    Terminal.qrc
