#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <QTime>
#include <QSerialPort>
#include <QCheckBox>
#include <QSpinBox>
#include <QSettings>
#include <QFileDialog>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QList>
#include <QAction>
#include <QStatusBar>
#include <QDockWidget>

#include "LimitedTextEdit.h"
#include "Macro.h"
#include "DataEncoder.h"
#include "HexEncoder.h"
#include "DecEncoder.h"
#include "AsciiEncoder.h"
#include "ComPortConfigure.h"
#include "Macros.h"
#include "ClickableLabel.h"
#include "PacketTimeCalculator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum DataMode
    {
        ASCII = 0,
        HEX = 1,
        DEC = 2
    };
public:
    explicit MainWindow(QString title, QWidget *parent = 0);
protected:
    virtual void closeEvent(QCloseEvent *e);
    virtual void moveEvent(QMoveEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void showEvent(QShowEvent *e);
private slots:
    void rxNone();
    void txNone();
    void rxHold();
    void txHold();
    void delayBetweenPacketsEnded();
    void startStop();
    void start();
    void stop();
    void saveReadWriteLog(bool writeLog);
    void saveWrite();
    void saveRead();
    void writeLogTimeout();
    void readLogTimeout();
    void startWriteLog(bool check);
    void startReadLog(bool check);
    void received();
    void singleSend();
    void startSending(bool checked = true);
    void sendPacket(const QByteArray &data);
    void sendPacket(const QByteArray &data, bool macro);
private:
    QAction *actionPortConfigure;
    QAction *actionStartStop;
    QAction *actionMacros;
    QLabel *portName;
    QLabel *baud;
    QLabel *bits;
    QLabel *parity;
    QLabel *stopBits;
    QComboBox *manualSendEncodingMode;
    QComboBox *readEncodingMode;
    QComboBox *writeEncodingMode;
    QTimer *manualSendTimer;
    QTimer *writeLogTimer;
    QTimer *readLogTimer;
    QTimer *delayBetweenPacketsTimer;
    QTimer *txTimer;
    QTimer *rxTimer;
    Macros *macros;
    QDockWidget *macrosDockWidget;
    QAction *clearWriteLog;
    QAction *clearReadLog;
    QAction *saveWriteLog;
    QAction *saveReadLog;
    QAction *recordWriteLog;
    QAction *recordReadLog;
    QAction *manualSendPacket;
    QLabel *txLabel;
    QLabel *rxLabel;
    LimitedTextEdit *sheetRead;
    LimitedTextEdit *sheetWrite;
    QSpinBox *manualRepeatSendTime;
    QSpinBox *readDelayBetweenPackets;
    QLineEdit *manualPacketEdit;
    QLineEdit *manualSeparatorEdit;
    QAction *displayWrite;
    QAction *displayRead;
    QAction *manualCR;
    QAction *manualLF;
    QAction *manualSendMode;

    QSerialPort *port;
    ComPortConfigure *comPortConfigure;
    QSettings *settings;
    QFileDialog *fileDialog;
    PacketTimeCalculator *packetTimeCalculator;

    QFile writeLogFile;
    QFile readLogFile;
    int txCount;
    int rxCount;
    bool logWrite;
    bool logRead;

    QByteArray readBuffer;

    HexEncoder *hexEncoder;
    DecEncoder *decEncoder;
    AsciiEncoder *asciiEncoder;

    QSize currentWindowSize;
    QPoint currentWindowPos;

    int macrosDockWidgetArea;

    void view();
    void saveSession();
    void loadSession();
    void connections();

    void displayWrittenData(const QByteArray &writeData);
    DataEncoder *getEncoder(int mode);
    QString baudToString(int baud);
    QString bitsToString(int bits);
    QString parityToString(int parity);
    QString stopBitsToString(int stopBits);
    void setMacrosMinimizeFeature(bool floating);
    void toggleMacrosView();
    void toggleWriteDisplay();
    void toggleReadDisplay();
    void saveCurrentMacrosArea(Qt::DockWidgetArea area);
    void onManualModeSelect();
    void toggleManualSendMode();
    void manualPacketEdited();
};

#endif // MAIN_WINDOW_H
