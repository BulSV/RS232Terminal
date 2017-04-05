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
    DataTable.cpp \
    LimitedTextEdit.cpp \
    ComPortConfigure.cpp \
    ClickableLabel.cpp \
    Macros.cpp \
    MacroEditWidget.cpp \
    MacroRawEditWidget.cpp \
    MacroWidget.cpp \
    SaveMacro.cpp \
    OpenMacro.cpp

HEADERS  += \
    MainWindow.h \
    DataEncoder.h \
    HexEncoder.h \
    AsciiEncoder.h \
    DecEncoder.h \
    RightClickedButton.h \
    DataTable.h \
    LimitedTextEdit.h \
    ComPortConfigure.h \
    ClickableLabel.h \
    Macros.h \
    MacroEditWidget.h \
    MacroRawEditWidget.h \
    MacroWidget.h \
    SaveMacro.h \
    OpenMacro.h

RESOURCES += \
    Terminal.qrc
