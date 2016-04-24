#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QSerialPort>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QSignalMapper>
#include "rs232terminalprotocol.h"
#include "ComPort.h"

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
    QLabel *m_lTx;
    QLabel *m_lRx;
    QSpinBox *m_sbBytesCount;
    QTextEdit *m_eLogRead;
    QTextEdit *m_eLogWrite;
    QSpinBox *m_sbRepeatSendInterval;
    QLineEdit *m_leSendPackage;
    QCheckBox *m_cbSendPackage;
    QCheckBox *m_cbEchoMode;
    QSpinBox *m_sbEchoInterval;

    QTimer *m_BlinkTimeTxNone;
    QTimer *m_BlinkTimeRxNone;
    QTimer *m_BlinkTimeTxColor;
    QTimer *m_BlinkTimeRxColor;

    QTimer *m_tSend;
    QTimer *m_tEcho;

    QSerialPort *m_Port;
    ComPort *m_ComPort;
    RS232TerminalProtocol *m_Protocol;

    int Offset;
    int DisplayByteIndex;
    int logReadRowsCount;
    int logWriteRowsCount;
    QStringList listOfBytes;
    QStringList restBytes;
    QString DisplayReadBuffer;
    QString echoData;

    void view();
    void connections();
    void sendPackage(QString string);
    QStringList doOffset(QStringList list);
    void displayReadData(QString string);
    void displayWriteData(QString string);

private slots:
    void start();
    void stop();
    void echo();
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
    void startSending();

public:
    explicit Dialog(QString title, QWidget *parent = 0);
    ~Dialog();
};

#endif // DIALOG_H
