#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QSerialPort>
#include <QSpinBox>
#include "rs232terminalprotocol.h"
#include "ComPort.h"

class Dialog : public QWidget
{
    Q_OBJECT
    QComboBox *m_cbPort;
    QComboBox *m_cbBaud;
    QPushButton *m_bStart;
    QPushButton *m_bStop;
    QLabel *m_lTx;
    QLabel *m_lRx;
    QSpinBox *m_sbBytesCount;
    QTextEdit *m_eLogRead;
    QTextEdit *m_eLogWrite;

    QTimer *m_BlinkTimeTxNone;
    QTimer *m_BlinkTimeRxNone;
    QTimer *m_BlinkTimeTxColor;
    QTimer *m_BlinkTimeRxColor;

    QSerialPort *m_Port;
    ComPort *m_ComPort;
    RS232TerminalProtocol *m_Protocol;

    int BytesCount;

    void view();
    void connections();

private slots:
    void start();
    void stop();
    void received(bool isReceived);
    //void changeBytesCount();
    void displayData(QString string);
    void colorIsRx();
    void colorRxNone();
    void colorIsTx();
    void colorTxNone();

public:
    explicit Dialog(QString title, QWidget *parent = 0);
    ~Dialog();
};

#endif // DIALOG_H
