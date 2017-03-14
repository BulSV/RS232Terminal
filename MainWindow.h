#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <QTime>
#include <QSerialPort>
#include <QCheckBox>
#include <QSpinBox>
#include <QSettings>
#include <QAbstractButton>
#include <QFileDialog>
#include <QGroupBox>
#include <QSpacerItem>
#include <QScrollArea>

#include "RightClickedButton.h"
#include "LimitedItemsListWidget.h"
#include "MacrosItemWidget.h"
#include "DataEncoder.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum DataMode
    {
        HEX = 0,
        ASCII = 1,
        DEC = 2
    };
    enum MacrosMoveDirection
    {
        MoveUp = 0,
        MoveDown = 1
    };

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
    QPushButton *m_bPause;
    QPushButton *m_bWriteLogClear;
    QPushButton *m_bReadLogClear;
    QPushButton *m_bSaveWriteLog;
    QPushButton *m_bSaveReadLog;
    QPushButton *m_bHiddenGroup;
    QPushButton *m_bDeleteAllMacroses;
    QPushButton *m_bNewMacros;
    QPushButton *m_bLoadMacroses;
    QPushButton *m_bRecordWriteLog;
    QPushButton *m_bRecordReadLog;
    QPushButton *m_bSendPackage;
    QLabel *m_lTx;
    QLabel *m_lRx;
    QLabel *m_lTxCount;
    QLabel *m_lRxCount;
    LimitedItemsListWidget *m_eLogRead;
    LimitedItemsListWidget *m_eLogWrite;
    QSpinBox *m_sbRepeatSendInterval;
    QSpinBox *m_sbEchoInterval;
    QSpinBox *m_sbDelay;
    QSpinBox *m_sbAllDelays;
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
    QCheckBox *m_chbCR;
    QCheckBox *m_chbLF;

    QSerialPort *m_Port;
    QSettings *settings;
    QFileDialog *fileDialog;
    QGroupBox *m_gbHiddenGroup;
    QSpacerItem *spacer;
    QVBoxLayout *scrollAreaLayout;
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QVBoxLayout *HiddenLayout;

    QFile writeLogFile;
    QFile readLogFile;
    int txCount;
    int rxCount;
    bool logWrite;
    bool logRead;

    bool echoWaiting;
    QStringList echoBuffer;
    QByteArray readBuffer;
    unsigned int index;
    QMap<int, MacrosItemWidget *> macrosItemWidgets;
    unsigned int sendCount;
    unsigned int sendIndex;
    QStringList echoSlave;

    DataEncoder *dataEncoder;

    void view();
    void saveSession();
    void loadSession();
    void connections();
    int findIntervalItem(int start);

    void portBaudSetting();
    void portDataBitsSetting();
    void portParitySetting();
    void portStopBitsSetting();

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
    void checkAllMacroses();
    void changeAllDelays(int n);
    void moveMacUp(int index);
    void moveMacDown(int index);
    bool moveMacros(QWidget *widget, MacrosMoveDirection direction);
public:
    void displayWriteData(QStringList list);
    explicit MainWindow(QString title, QWidget *parent = 0);
};

#endif // MAIN_WINDOW_H
