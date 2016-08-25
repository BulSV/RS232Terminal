#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <mylistwidget.h>
#include "MiniMacros.h"
#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <MyPushButton.h>
#include <QTimer>
#include <QTime>
#include <QSerialPort>
#include <QCheckBox>
#include <QSpinBox>
#include <QSignalMapper>
#include <QSettings>
#include <QAbstractButton>
#include <QFileDialog>
#include <QGroupBox>
#include <QSpacerItem>
#include <QScrollArea>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QWidget *widget;
    QComboBox *m_cbPort;
    QComboBox *m_cbBaud;
    QComboBox *m_cbBits;
    QComboBox *m_cbParity;
    QComboBox *m_cbStopBits;
    QComboBox *m_cbSendMode;
    QComboBox *m_cbReadMode;
    QComboBox *m_cbWriteMode;
    QTimer *m_tSend;
    QTimer *m_tEcho;
    QTimer *m_tWriteLog;
    QTimer *m_tReadLog;
    QTimer *m_tIntervalSending;
    QTimer *m_tDelay;
    QTimer *m_tTx;
    QTimer *m_tRx;
    QPushButton *m_bStart;
    QPushButton *m_bStop;
    QAbstractButton *m_abPause;
    QPushButton *m_bWriteLogClear;
    QPushButton *m_bReadLogClear;
    QPushButton *m_bSaveWriteLog;
    QPushButton *m_bSaveReadLog;
    QPushButton *m_bHiddenGroup;
    QPushButton *m_bDeleteAllMacroses;
    QPushButton *m_bAddMacros;
    QPushButton *m_bLoadMacroses;
    QAbstractButton *m_abSaveWriteLog;
    QAbstractButton *m_abSaveReadLog;
    QAbstractButton *m_abSendPackage;
    QLabel *m_lTx;
    QLabel *m_lRx;
    QLabel *m_lTxCount;
    QLabel *m_lRxCount;
    MyListWidget *m_eLogRead;
    MyListWidget *m_eLogWrite;
    QSpinBox *m_sbRepeatSendInterval;
    QSpinBox *m_sbEchoInterval;
    QSpinBox *m_sbDelay;
    QSpinBox *m_sbAllDelay;
    QLineEdit *m_leSendPackage;
    QCheckBox *m_cbEchoMaster;
    QCheckBox *m_cbEchoSlave;
    QCheckBox *m_cbReadScroll;
    QCheckBox *m_cbWriteScroll;
    QCheckBox *m_cbAllIntervals;
    QCheckBox *m_cbAllPeriods;
    QCheckBox *m_cbDisplayWrite;
    QCheckBox *m_cbDisplayRead;
    QCheckBox *m_cbUniformSizes;

    QSerialPort *m_Port;
    QSettings *settings;
    QFileDialog *fileDialog;
    QGroupBox *m_gbHiddenGroup;
    QSpacerItem *spacer;
    QVBoxLayout *scrollAreaLayout;
    QScrollArea *scrollArea;
    QWidget *widgetScroll;
    QVBoxLayout *HiddenLayout;

    QFile writeLog;
    QFile readLog;
    qint32 txCount;
    qint32 rxCount;
    bool logWrite;
    bool logRead;

    bool echoWaiting;
    QStringList echoBuffer;
    QByteArray readBuffer;
    unsigned short int index;
    QMap<int, MiniMacros *> MiniMacrosList;
    unsigned short int sendCount;
    unsigned short int sendIndex;
    QStringList echoSlave;

    void view();
    void saveSession();
    void loadSession();
    void connections();
    int findIntervalItem(unsigned short start);
    enum MoveDirection { MoveUp, MoveDown };

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
    void setUniformSizes(bool check);
    void rxNone();
    void txNone();
    void rxHold();
    void txHold();
    void breakLine();
    void sendInterval();
    void intervalSendAdded(int index, bool check);
    void hiddenClick();
    void start();
    void stop();
    void pause(bool check);
    void echo();
    void saveWrite();
    void saveRead();
    void writeLogTimeout();
    void readLogTimeout();
    void startWriteLog(bool check);
    void startReadLog(bool check);
    void textChanged(QString text);
    void echoCheckMaster(bool check);
    void echoCheckSlave(bool check);
    void received();
    void sendSingle();
    void startSending(bool checked);
    void addMacros();
    void openDialog();
    void delMacros(int index);
    void sendPackage(QString string, int mode);
    void deleteAllMacroses();
    void checkAllIntervals(bool check);
    void checkAllPeriods(bool check);
    void changeAllDelays(int n);
    void moveMacUp(int index);
    void moveMacDown(int index);
    bool moveMacros(QWidget *widget, MoveDirection direction);

public:
    void displayWriteData(QStringList list);
    explicit MainWindow(QString title, QWidget *parent = 0);
};

#endif
