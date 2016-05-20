#ifndef DIALOG_H
#define DIALOG_H

#include <QMainWindow>
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
public:
    void delLine(int lineNumber)
    {
        QStringList list = this->toPlainText().split("\n");
        list.removeAt(lineNumber);
        this->clear();
        this->insertPlainText(list.join("\n"));
    }
};

//class DispString : public QString {
//public:
//    QStringList bytes;
//    QStringList rest;
//
//    void appendBytes(QStringList slist)
//    {
//        foreach (QString, slist)
//            bytes += slist.takeFirst();
//    }
//
//    QString getBytes(int count)
//    {
//        if (bytes.isEmpty())
//            return "";
//
//        QString out;
//        for (int i = 0; i < count; i++)
//        {
//            out += bytes.takeFirst() + " ";
//        }
//    }
//};

class Dialog : public QMainWindow
{
    Q_OBJECT
    QWidget *widget;
    QCheckBox *m_cbReadDisp;
    QCheckBox *m_cbWriteDisp;
    QComboBox *m_cbPort;
    QComboBox *m_cbBaud;
    QComboBox *m_cbBits;
    QComboBox *m_cbParity;
    QComboBox *m_cbStopBits;
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

    int maxWriteLogRows;
    int maxReadLogRows;
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
    void textChanged(QString text);
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
