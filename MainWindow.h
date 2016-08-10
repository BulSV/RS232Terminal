#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <QListWidget>
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
    QListWidget *m_eLogRead;
    QListWidget *m_eLogWrite;
    QSpinBox *m_sbRepeatSendInterval;
    QSpinBox *m_sbEchoInterval;
    QSpinBox *m_sbDelay;
    QSpinBox *m_sbAllDelay;
    QLineEdit *m_leSendPackage;
    QCheckBox *m_cbEchoMode;
    QCheckBox *m_cbReadScroll;
    QCheckBox *m_cbWriteScroll;
    QCheckBox *m_cbAllIntervals;
    QCheckBox *m_cbAllPeriods;
    QCheckBox *m_cbDisplayWrite;
    QCheckBox *m_cbDisplayRead;

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
    unsigned short int maxWriteLogRows;
    unsigned short int maxReadLogRows;
    unsigned short int logReadRowsCount;
    unsigned short int logWriteRowsCount;
    qint32 txCount;
    qint32 rxCount;
    bool logWrite;
    bool logRead;

    bool echoWaiting;
    QStringList echoBuffer;
    QByteArray readBuffer;
    unsigned short int index;
    QMap<int, MiniMacros *> MiniMacrosList;
    QList<int> intervalSendingIndexes;

    void view();
    void saveSession();
    void loadSession();
    void connections();

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
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
    void echo();
    void saveWrite();
    void saveRead();
    void writeLogTimeout();
    void readLogTimeout();
    void startWriteLog(bool check);
    void startReadLog(bool check);
    void textChanged(QString text);
    void echoCheck(bool check);
    void clearReadLog();
    void clearWriteLog();
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

public:
    void displayWriteData(QStringList list);
    explicit MainWindow(QString title, QWidget *parent = 0);
};

#endif
