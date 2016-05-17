#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTimer>
#include <QSerialPort>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QSignalMapper>
#include <QSettings>
#include "rs232terminalprotocol.h"
#include "ComPort.h"
#include "MacroWindow.h"
#include <QAbstractButton>

class MyPlainTextEdit : public QPlainTextEdit {
protected:
   void mousePressEvent(QMouseEvent *e)
   {
       e->ignore();
   }
};

class Dialog : public QWidget
{
    Q_OBJECT
    QComboBox *m_cbPort;
    QComboBox *m_cbBaud;
    QPushButton *m_bStart;
    QPushButton *m_bStop;
    QPushButton *m_bWriteLogClear;
    QPushButton *m_bReadLogClear;
    QPushButton *m_bOffsetLeft;
    QPushButton *m_bOffsetRight;
    QPushButton *m_bShowMacroForm;
    QLabel *m_lTx;
    QLabel *m_lRx;
    QSpinBox *m_sbBytesCount;
    MyPlainTextEdit *m_eLogRead;
    MyPlainTextEdit *m_eLogWrite;
    QSpinBox *m_sbRepeatSendInterval;
    QLineEdit *m_leSendPackage;
    QAbstractButton *m_abSendPackage;
    QCheckBox *m_cbEchoMode;
    QSpinBox *m_sbEchoInterval;
    QCheckBox *m_cbReadScroll;
    QCheckBox *m_cbWriteScroll;

    QTimer *m_BlinkTimeTxNone;
    QTimer *m_BlinkTimeRxNone;
    QTimer *m_BlinkTimeTxColor;
    QTimer *m_BlinkTimeRxColor;

    QTimer *m_tSend;
    QTimer *m_tEcho;

    QSerialPort *m_Port;
    ComPort *m_ComPort;
    RS232TerminalProtocol *m_Protocol;
    QSettings *settings;
    MacroWindow *macroWindow;

    int Offset;
    int logReadRowsCount;
    int logWriteRowsCount;
    QStringList listOfBytes;
    QStringList restBytes;
    QString DisplayReadBuffer;
    QStringList echoData;

    void view();
    void saveSession();
    void loadSession();
    void connections();
    void scrollToBot(QCheckBox *cb, MyPlainTextEdit *te);
    QStringList doOffset(QStringList list);

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
    void start();
    void stop();
    void echo();
    void cleanEchoBuffer(bool check);
    void macrosRecieved(const QString &str);
    void showMacroWindow();
    void offsetDec();
    void offsetInc();
    void clearReadLog();
    void clearWriteLog();
    void received(bool isReceived);
    void sendSingle();
    void colorIsRx();
    void colorRxNone();
    void colorIsTx();
    void colorTxNone();
    void startSending(bool checked);

public:
    void displayReadData(QString string);
    void displayWriteData(QString string);
    void sendPackage(QString string);
    explicit Dialog(QString title, QWidget *parent = 0);
};

#endif // DIALOG_H
