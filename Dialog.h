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
    QSpinBox *m_sbRepeatSendCount;
    QLineEdit *m_leSendPackage;
    QPushButton *m_bSendPackage;

    QTimer *m_BlinkTimeTxNone;
    QTimer *m_BlinkTimeRxNone;
    QTimer *m_BlinkTimeTxColor;
    QTimer *m_BlinkTimeRxColor;

    QSerialPort *m_Port;
    ComPort *m_ComPort;
    RS232TerminalProtocol *m_Protocol;

    int DisplayByteIndex;
    QString DisplayBuffer;

    void view();
    void connections();

private slots:
    void start();
    void stop();
    void clearReadLog();
    void clearWriteLog();
    void received(bool isReceived);
    void sendPackage();
    void setDisplayData(QString string, QTextEdit *edit);
    void displayData(QTextEdit *edit);
    void colorIsRx();
    void colorRxNone();
    void colorIsTx();
    void colorTxNone();

public:
    explicit Dialog(QString title, QWidget *parent = 0);
    ~Dialog();
};

#endif // DIALOG_H
