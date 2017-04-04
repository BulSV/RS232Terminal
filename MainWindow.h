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
#include <QFileDialog>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QList>
#include <QAction>
#include <QStatusBar>
#include <QDockWidget>

#include "RightClickedButton.h"
#include "LimitedTextEdit.h"
#include "MacrosWidget.h"
#include "DataEncoder.h"
#include "HexEncoder.h"
#include "DecEncoder.h"
#include "AsciiEncoder.h"
#include "ComPortConfigure.h"
#include "Macroses.h"

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
public:
    explicit MainWindow(QString title, QWidget *parent = 0);
protected:
    virtual void closeEvent(QCloseEvent *e);
private slots:
    void rxNone();
    void txNone();
    void rxHold();
    void txHold();
    void delayBetweenPacketsEnded();
    void hiddenClicked();
    void startStop();
    void start();
    void stop();
    void pause(bool check);
    void saveReadWriteLog(bool writeLog);
    void saveWrite();
    void saveRead();
    void writeLogTimeout();
    void readLogTimeout();
    void startWriteLog(bool check);
    void startReadLog(bool check);
    void received();
    void singleSend();
    void startSending(bool checked = true);
    void sendPackage(const QByteArray &data);
    void sendPackage(const QByteArray &data, bool macros);
private:
    QWidget *widget;
    QToolBar *toolBar;
    QAction *actionPortConfigure;
    QAction *actionStartStop;
    QAction *actionMacroses;
    QStatusBar *statusBar;
    QLabel *portName;
    QLabel *baud;
    QLabel *bits;
    QLabel *parity;
    QLabel *stopBits;
    QComboBox *m_cbSendMode;
    QComboBox *m_cbReadMode;
    QComboBox *m_cbWriteMode;
    QTimer *m_tSend;
    QTimer *m_tWriteLog;
    QTimer *m_tReadLog;
    QTimer *m_tIntervalSending;
    QTimer *m_timerDelayBetweenPackets;
    QTimer *m_tTx;
    QTimer *m_tRx;
    Macroses *macroses;
    QDockWidget *macrosesDockWidget;
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
    QLabel *m_lTxCount;
    QLabel *m_lRxCount;
    LimitedTextEdit *m_eLogRead;
    LimitedTextEdit *m_eLogWrite;
    QSpinBox *m_sbRepeatSendInterval;
    QSpinBox *m_sbDelayBetweenPackets;
    QSpinBox *m_sbAllDelays;
    QLineEdit *m_leSendPackage;
    QCheckBox *m_cbReadScroll;
    QCheckBox *m_cbWriteScroll;
    QCheckBox *m_cbAllIntervals;
    QCheckBox *m_cbAllPeriods;
    QCheckBox *m_cbDisplayWrite;
    QCheckBox *m_cbDisplayRead;
    QCheckBox *m_chbCR;
    QCheckBox *m_chbLF;

    QSerialPort *m_port;
    ComPortConfigure *comPortConfigure;
    QSettings *settings;
    QFileDialog *fileDialog;
    QGroupBox *m_gbHiddenGroup;
    QVBoxLayout *scrollAreaLayout;
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QVBoxLayout *hiddenLayout;

    QFile writeLogFile;
    QFile readLogFile;
    int txCount;
    int rxCount;
    bool logWrite;
    bool logRead;

    QByteArray readBuffer;
    QList<MacrosWidget*> macrosWidgets;
    QList<int> indexesOfIntervals;
    int sendCount;
    int currentIntervalIndex;

    HexEncoder *hexEncoder;
    DecEncoder *decEncoder;
    AsciiEncoder *asciiEncoder;

    void view();
    void saveSession();
    void loadSession();
    void connections();

    void updateIntervalsList(bool add);
    void sendNextMacros();
    void displayWrittenData(const QByteArray &writeData);
    DataEncoder *getEncoder(int mode);
    QString baudToString(int baud);
    QString bitsToString(int bits);
    QString parityToString(int parity);
    QString stopBitsToString(int stopBits);
};

#endif // MAIN_WINDOW_H
