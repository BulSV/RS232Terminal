#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "rs232terminalprotocol.h"
#include "ComPort.h"
#include "MiniMacros.h"
#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <MyPushButton.h>
#include <QPlainTextEdit>
#include <QTimer>
#include <QSerialPort>
#include <QCheckBox>
#include <QSpinBox>
#include <QSignalMapper>
#include <QSettings>
#include <QAbstractButton>
#include <QFileDialog>
#include <QGroupBox>
#include <QSpacerItem>
#include <QListWidget>
#include <QScrollArea>
#include <QToolBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QWidget *widget;
    QComboBox *m_cbPort;
    QComboBox *m_cbBaud;
    QComboBox *m_cbBits;
    QComboBox *m_cbParity;
    QComboBox *m_cbStopBits;
    QComboBox *m_cbMode;
    QPushButton *m_bStart;
    QPushButton *m_bStop;
    QPushButton *m_bWriteLogClear;
    QPushButton *m_bReadLogClear;
    QPushButton *m_bSaveWriteLog;
    QPushButton *m_bSaveReadLog;
    QLabel *m_lTx;
    QLabel *m_lRx;
    QListWidget *m_eLogRead;
    QListWidget *m_eLogWrite;
    QSpinBox *m_sbRepeatSendInterval;
    QLineEdit *m_leSendPackage;
    QAbstractButton *m_abSendPackage;
    QCheckBox *m_cbEchoMode;
    QSpinBox *m_sbEchoInterval;
    QCheckBox *m_cbReadScroll;
    QCheckBox *m_cbWriteScroll;
    QAbstractButton *m_abSaveWriteLog;
    QAbstractButton *m_abSaveReadLog;

    QTimer *m_BlinkTimeTxNone;
    QTimer *m_BlinkTimeRxNone;
    QTimer *m_BlinkTimeTxColor;
    QTimer *m_BlinkTimeRxColor;
    QTimer *m_tSend;
    QTimer *m_tEcho;
    QTimer *m_tWriteLog;
    QTimer *m_tReadLog;

    QSerialPort *m_Port;
    ComPort *m_ComPort;
    RS232TerminalProtocol *m_Protocol;
    QSettings *settings;
    QFileDialog *fileDialog;
    QPushButton *m_bHiddenGroup;
    QGroupBox *m_gbHiddenGroup;
    QSpacerItem *spacer;
    QVBoxLayout *scrollAreaLayout;
    QScrollArea *scrollArea;
    QWidget *widgetScroll;
    QVBoxLayout *HiddenLayout;
    QToolBar *toolBar;

    QFile writeLog;
    QFile readLog;
    int maxWriteLogRows;
    int maxReadLogRows;
    int logReadRowsCount;
    int logWriteRowsCount;
    QStringList listOfBytes;
    int readBytesDisplayed;
    QStringList echoData;
    bool logWrite;
    bool logRead;
    QString writeLogBuffer;
    QString readLogBuffer;
    QString buffer;

    int index;
    QMap<int, MiniMacros *> MiniMacrosList;

    void view();
    void saveSession();
    void loadSession();
    void connections();

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
    void hiddenClick();
    void start();
    void stop();
    void echo();
    void saveWrite();
    void saveRead();
    void writeLogTimeout();
    void readLogTimeout();
    void startWriteLog(bool check);
    void startReadLog(bool check);
    void textChanged(QString text);
    void cleanEchoBuffer(bool check);
    void clearReadLog();
    void clearWriteLog();
    void received(bool isReceived);
    void sendSingle();
    void colorIsRx();
    void colorRxNone();
    void colorIsTx();
    void colorTxNone();
    void startSending(bool checked);
    void addMacros();
    void openDialog();
    void delMacros(int index);

public:
    void displayReadDataHEX(QString string);
    void displayReadDataASCII(QString string);
    void displayReadDataDEC(QString string);
    void displayWriteData(QString string);
    void sendPackage(QString string);
    void sendPackageHEX(QString string);
    void sendPackageASCII(QString string);
    void sendPackageDEC(QString string);
    explicit MainWindow(QString title, QWidget *parent = 0);
};

#endif
