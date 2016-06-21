#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "macrosediting.h"
#include "rs232terminalprotocol.h"
#include "ComPort.h"
#include "MacroWindow.h"
#include "Macros.h"
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
    MyPushButton *m_bStart;
    MyPushButton *m_bStop;
    MyPushButton *m_bWriteLogClear;
    MyPushButton *m_bReadLogClear;
    MyPushButton *m_bOffsetLeft;
    MyPushButton *m_bShowMacroForm;
    MyPushButton *m_bSaveWriteLog;
    MyPushButton *m_bSaveReadLog;
    QLabel *m_lTx;
    QLabel *m_lRx;
    QSpinBox *m_sbBytesCount;
    QListWidget *m_eLogRead;
    QListWidget *m_eLogWrite;
    QSpinBox *m_sbRepeatSendInterval;
    QLineEdit *m_leSendPackage;
    QAbstractButton *m_abSendPackage;
    QCheckBox *m_cbEchoMode;
    QCheckBox *m_cbSelectIntervals;
    QCheckBox *m_cbSelectPeriods;
    QSpinBox *m_sbEchoInterval;
    QCheckBox *m_cbReadScroll;
    QCheckBox *m_cbWriteScroll;
    QCheckBox *m_cbSaveWriteLog;
    QCheckBox *m_cbSaveReadLog;

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
    MacroWindow *macroWindow;
    QFileDialog *fileDialog;
    QPushButton *m_bHiddenGroup;
    QGroupBox *m_gbHiddenGroup;
    QVBoxLayout *hiddenLayout;
    QSpacerItem *spacer;

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
    QList<MacrosEditing *> editingList;

    void view();
    void saveSession();
    void loadSession();
    void connections();

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
    void editMacrosWindowShow(int index);
    void setAllMiniIntervals(bool check);
    void setAllMiniPeriods(bool check);
    void hiddenClick();
    void start();
    void stop();
    void echo();
    void saveWrite();
    void saveRead();
    void doOffset();
    void writeLogTimeout();
    void readLogTimeout();
    void startWriteLog(bool check);
    void startReadLog(bool check);
    void textChanged(QString text);
    void cleanEchoBuffer(bool check);
    void macrosRecieved(const QString &str);
    void showMacroWindow();
    void clearReadLog();
    void clearWriteLog();
    void received(bool isReceived);
    void sendSingle();
    void colorIsRx();
    void colorRxNone();
    void colorIsTx();
    void colorTxNone();
    void startSending(bool checked);
    void addToHidden(int index, const QString &str);
    void delFromHidden(int index);
    void miniMacrosTextChanged(QString str, int index) { MiniMacrosList[index]->bMiniMacros->setText(str); }
    void miniMacrosChecked(bool check, int index) {MiniMacrosList[index]->cbMiniMacrosInterval->setChecked(check);}
    void miniMacrosCheckedPeriod(bool check, int index) {MiniMacrosList[index]->cbMiniMacrosPeriod->setChecked(check);}

public:
    QMap<int, MiniMacros *> MiniMacrosList;
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
