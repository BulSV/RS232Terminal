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
#include <QSettings>
#include "rs232terminalprotocol.h"
#include "ComPort.h"
#include "Macro.h"
#include <QAbstractButton>

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
    QTextEdit *m_eLogRead;
    QTextEdit *m_eLogWrite;
    QSpinBox *m_sbRepeatSendInterval;
    QLineEdit *m_leSendPackage;
    QAbstractButton *m_abSendPackage;
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
    Macro *macroWindow;
    QSettings *settings;

    int Offset;
    int DisplayByteIndex;
    int logReadRowsCount;
    int logWriteRowsCount;
    QStringList listOfBytes;
    QStringList restBytes;
    QString DisplayReadBuffer;
    QStringList echoData;

    void view();
    void connections();
    QStringList doOffset(QStringList list);
    void displayReadData(QString string);
    void displayWriteData(QString string);

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
    void start();
    void stop();
    void echo();
    void cleanEchoBuffer(bool check);
    void macrosRecieved(bool isRecieved);
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
    void sendPackage(QString string);
    explicit Dialog(QString title, QWidget *parent = 0);
    ~Dialog();
};

#endif // DIALOG_H
