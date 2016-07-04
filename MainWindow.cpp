#include <QDebug>
#include "MainWindow.h"
#include "rs232terminalprotocol.h"
#include "MiniMacros.h"
#include <QGridLayout>
#include <QString>
#include <QApplication>
#include <QLineEdit>
#include <QSerialPortInfo>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QSplitter>
#include <QSpacerItem>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>
#include <QScrollBar>
#include <QDir>

#define BLINKTIMETX 100
#define BLINKTIMERX 100

#define SEPARATOR " "

MainWindow::MainWindow(QString title, QWidget *parent)
    : QMainWindow(parent)
    , widget(new QWidget(this))    
    , m_cbPort(new QComboBox(this))
    , m_cbBaud(new QComboBox(this))
    , m_cbBits(new QComboBox(this))
    , m_cbParity(new QComboBox(this))
    , m_cbStopBits(new QComboBox(this))
    , m_cbMode(new QComboBox(this))
    , m_bStart(new QPushButton("Start", this))
    , m_bStop(new QPushButton("Stop", this))
    , m_bWriteLogClear(new QPushButton("Clear", this))
    , m_bReadLogClear(new QPushButton("Clear", this))
    , m_bSaveWriteLog(new QPushButton("Save", this))
    , m_bSaveReadLog(new QPushButton("Save", this))
    , m_lTx(new QLabel("        Tx", this))
    , m_lRx(new QLabel("        Rx", this))
    , m_eLogRead(new QListWidget(this))
    , m_eLogWrite(new QListWidget(this))
    , m_sbRepeatSendInterval(new QSpinBox(this))
    , m_leSendPackage(new QLineEdit(this))
    , m_abSendPackage(new QPushButton("Send", this))
    , m_cbEchoMode(new QCheckBox("Echo mode", this))
    , m_sbEchoInterval(new QSpinBox(this))
    , m_cbReadScroll(new QCheckBox("Scrolling", this))
    , m_cbWriteScroll(new QCheckBox("Scrolling", this))
    , m_abSaveWriteLog(new QPushButton(this))
    , m_abSaveReadLog(new QPushButton(this))
    , m_BlinkTimeTxNone(new QTimer(this))
    , m_BlinkTimeRxNone(new QTimer(this))
    , m_BlinkTimeTxColor(new QTimer(this))
    , m_BlinkTimeRxColor(new QTimer(this))
    , m_tSend(new QTimer(this))
    , m_tEcho(new QTimer(this))
    , m_tWriteLog(new QTimer(this))
    , m_tReadLog(new QTimer(this))
    , m_Port(new QSerialPort(this))
    , m_ComPort(new ComPort(m_Port))
    , m_Protocol(new RS232TerminalProtocol(m_ComPort, this))
    , settings(new QSettings("settings.ini", QSettings::IniFormat))
    , fileDialog(new QFileDialog(this))
    , m_bHiddenGroup(new QPushButton(">", this))
    , m_gbHiddenGroup(new QGroupBox(this))
    , spacer(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding))
    , scrollAreaLayout(new QVBoxLayout)
    , scrollArea(new QScrollArea(m_gbHiddenGroup))
    , widgetScroll(new QWidget(scrollArea))
    , HiddenLayout(new QVBoxLayout(widgetScroll))
    , toolBar(new QToolBar(this))
{
    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    setWindowTitle(title);    
    resize(settings->value("config/width").toInt(), settings->value("config/height").toInt());
    view();
    connections();

    toolBar->addAction(QIcon(":/Resources/add.png"), "Add Macros", this, SLOT(addMacros()));
    toolBar->addAction(QIcon(":/Resources/open.png"), "Load Macroses");
    toolBar->setMovable(false);
    addToolBar(Qt::TopToolBarArea, toolBar);

    logReadRowsCount = 0;
    logWriteRowsCount = 0;
    readBytesDisplayed = 0;
    logWrite = false;
    logRead = false;
    index = 0;

    m_abSaveReadLog->setIcon(QIcon(":/Resources/startRecToFile.png"));
    m_abSaveWriteLog->setIcon(QIcon(":/Resources/startRecToFile.png"));
    m_abSaveWriteLog->setCheckable(true);
    m_abSaveReadLog->setCheckable(true);
    m_abSendPackage->setCheckable(true);
    m_abSendPackage->setEnabled(false);
    m_sbRepeatSendInterval->setEnabled(false);
    m_bStop->setEnabled(false);
    m_cbPort->setEditable(true);
    m_sbRepeatSendInterval->setRange(0, 100000);
    m_sbEchoInterval->setRange(0, 100000);

    m_lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_BlinkTimeTxNone->setInterval(BLINKTIMETX);
    m_BlinkTimeRxNone->setInterval(BLINKTIMERX);
    m_BlinkTimeTxColor->setInterval(BLINKTIMETX);
    m_BlinkTimeRxColor->setInterval(BLINKTIMERX);

    m_eLogRead->setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");
    m_eLogWrite->setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");

    QStringList buffer;
    foreach(QSerialPortInfo portsAvailable, QSerialPortInfo::availablePorts())
    {
        buffer << portsAvailable.portName();
    }
    m_cbPort->addItems(buffer);
    m_cbPort->setEditable(true);

    buffer.clear();
    buffer << "921600" << "115200" << "57600" << "38400" << "19200" << "9600" << "4800" << "2400" << "1200";
    m_cbBaud->addItems(buffer);
    buffer.clear();
    buffer << "8" << "7" << "6" << "5";
    m_cbBits->addItems(buffer);
    buffer.clear();
    buffer << "None" << "Odd" << "Even" << "Mark" << "Space";
    m_cbParity->addItems(buffer);
    buffer.clear();
    buffer << "1" << "1.5" << "2";
    m_cbStopBits->addItems(buffer);
    buffer.clear();
    buffer << "HEX" << "ASCII" << "DEC";
    m_cbMode->addItems(buffer);

    QDir dir;
    if (!dir.exists(dir.currentPath()+"/Macros"))
        dir.mkpath(dir.currentPath()+"/Macros");

    loadSession();
}

void MainWindow::view()
{   
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *configLayout = new QGridLayout;
    configLayout->addWidget(new QLabel("<img src=':/Resources/elisat.png' height='40' width='160'/>", this), 0, 0, 2, 2, Qt::AlignCenter);
    configLayout->addWidget(m_lTx, 2, 0);
    configLayout->addWidget(m_lRx, 2, 1);
    configLayout->addWidget(new QLabel("Port:", this), 3, 0);
    configLayout->addWidget(m_cbPort, 3, 1);
    configLayout->addWidget(new QLabel("Baud:", this), 4, 0);
    configLayout->addWidget(m_cbBaud, 4, 1);
    configLayout->addWidget(new QLabel("Data bits:", this), 5, 0);
    configLayout->addWidget(m_cbBits, 5, 1);
    configLayout->addWidget(new QLabel("Parity:", this), 6, 0);
    configLayout->addWidget(m_cbParity, 6, 1);
    configLayout->addWidget(new QLabel("Stop bits:", this), 7, 0);
    configLayout->addWidget(m_cbStopBits, 7, 1);
    configLayout->addWidget(m_cbEchoMode, 8, 0);
    configLayout->addWidget(m_sbEchoInterval, 8, 1);
    configLayout->addWidget(new QLabel("Mode:", this), 9, 0);
    configLayout->addWidget(m_cbMode, 9, 1);
    configLayout->addWidget(m_bStart, 10, 0);
    configLayout->addWidget(m_bStop, 10, 1);
    configLayout->addItem(spacer, 11, 0);
    configLayout->setSpacing(5);

    QGridLayout *sendPackageLayout = new QGridLayout;
    sendPackageLayout->addWidget(m_leSendPackage, 0, 0);
    sendPackageLayout->addWidget(m_sbRepeatSendInterval, 0, 1);
    sendPackageLayout->addWidget(m_abSendPackage, 0, 2);

    QGridLayout *WriteLayout = new QGridLayout;
    WriteLayout->addWidget(new QLabel("Write:", this), 0, 0);
    m_cbWriteScroll->setFixedWidth(65);
    WriteLayout->addWidget(m_cbWriteScroll, 0, 1);
    m_abSaveWriteLog->setFixedWidth(35);
    WriteLayout->addWidget(m_abSaveWriteLog, 0, 2);
    m_bSaveWriteLog->setFixedWidth(50);
    WriteLayout->addWidget(m_bSaveWriteLog, 0, 3);
    m_bWriteLogClear->setFixedWidth(50);
    WriteLayout->addWidget(m_bWriteLogClear, 0, 4);
    WriteLayout->addWidget(m_eLogWrite, 1, 0, 1, 5);
    WriteLayout->setSpacing(5);
    WriteLayout->setMargin(5);

    QGridLayout *ReadLayout = new QGridLayout;
    ReadLayout->addWidget(new QLabel("Read:", this), 0, 0);
    m_cbReadScroll->setFixedWidth(65);
    ReadLayout->addWidget(m_cbReadScroll, 0, 1);
    m_abSaveReadLog->setFixedWidth(35);
    ReadLayout->addWidget(m_abSaveReadLog, 0, 2);
    m_bSaveReadLog->setFixedWidth(50);
    ReadLayout->addWidget(m_bSaveReadLog, 0, 3);
    m_bReadLogClear->setFixedWidth(50);
    ReadLayout->addWidget(m_bReadLogClear, 0, 4);
    ReadLayout->addWidget(m_eLogRead, 1, 0, 1, 5);
    ReadLayout->setSpacing(5);
    ReadLayout->setMargin(5);

    QWidget *wWrite = new QWidget;
    wWrite->setLayout(WriteLayout);
    QWidget *wRead = new QWidget;
    wRead->setLayout(ReadLayout);

    QSplitter *splitter = new QSplitter;
    splitter->addWidget(wWrite);
    splitter->addWidget(wRead);
    splitter->setHandleWidth(1);

    QGridLayout *dataLayout = new QGridLayout;
    dataLayout->addWidget(splitter, 0, 0);
    dataLayout->addLayout(sendPackageLayout, 1, 0);
    dataLayout->setSpacing(0);
    dataLayout->setMargin(0);

    scrollArea->setWidget(widgetScroll);
    scrollArea->show();
    scrollArea->setVisible(true);
    scrollArea->setVerticalScrollBar(new QScrollBar(Qt::Vertical, scrollArea));
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
    scrollAreaLayout->addWidget(scrollArea);
    HiddenLayout->setSpacing(0);
    HiddenLayout->setMargin(2);
    m_gbHiddenGroup->setLayout(scrollAreaLayout);
    m_gbHiddenGroup->setFixedWidth(300);

    QGridLayout *allLayouts = new QGridLayout;
    allLayouts->setSpacing(5);
    allLayouts->setMargin(5);
    allLayouts->addLayout(configLayout, 0, 0);
    allLayouts->addLayout(dataLayout, 0, 1);
    m_bHiddenGroup->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_bHiddenGroup->setFixedWidth(15);
    allLayouts->addWidget(m_bHiddenGroup, 0, 2);
    allLayouts->addWidget(m_gbHiddenGroup, 0, 3);
    widget->setLayout(allLayouts);
    setCentralWidget(widget);    
}

void MainWindow::connections()
{
    connect(m_bReadLogClear, SIGNAL(clicked()), this, SLOT(clearReadLog()));
    connect(m_bWriteLogClear, SIGNAL(clicked()), this, SLOT(clearWriteLog()));
    connect(m_bStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(m_bStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(m_bSaveWriteLog, SIGNAL(clicked()), this, SLOT(saveWrite()));
    connect(m_bSaveReadLog, SIGNAL(clicked()), this, SLOT(saveRead()));
    connect(m_bHiddenGroup, SIGNAL(clicked()), this, SLOT(hiddenClick()));
    connect(m_abSaveWriteLog, SIGNAL(toggled(bool)), this, SLOT(startWriteLog(bool)));
    connect(m_abSaveReadLog, SIGNAL(toggled(bool)), this, SLOT(startReadLog(bool)));

    connect(m_abSendPackage, SIGNAL(toggled(bool)), this, SLOT(startSending(bool)));
    connect(m_cbEchoMode, SIGNAL(toggled(bool)), this, SLOT(cleanEchoBuffer(bool)));
    connect(m_leSendPackage, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(m_leSendPackage, SIGNAL(returnPressed()), m_abSendPackage, SLOT(click()));
    connect(m_Protocol, SIGNAL(DataIsReaded(bool)), this, SLOT(received(bool)));

    connect(m_tSend, SIGNAL(timeout()), this, SLOT(sendSingle()));
    connect(m_tEcho, SIGNAL(timeout()), this, SLOT(echo()));

    connect(m_tWriteLog, SIGNAL(timeout()), this, SLOT(writeLogTimeout()));
    connect(m_tReadLog, SIGNAL(timeout()), this, SLOT(readLogTimeout()));

    connect(m_BlinkTimeTxColor, SIGNAL(timeout()), this, SLOT(colorIsTx()));
    connect(m_BlinkTimeRxColor, SIGNAL(timeout()), this, SLOT(colorIsRx()));
    connect(m_BlinkTimeTxNone, SIGNAL(timeout()), this, SLOT(colorTxNone()));
    connect(m_BlinkTimeRxNone, SIGNAL(timeout()), this, SLOT(colorRxNone()));
}

void MainWindow::hiddenClick()
{
    if (m_gbHiddenGroup->isHidden())
    {
        m_gbHiddenGroup->show();
        m_bHiddenGroup->setText("<");
        resize(width() + m_gbHiddenGroup->width() + 5, height());
    }
    else
    {
        m_gbHiddenGroup->hide();
        m_bHiddenGroup->setText(">");
        resize(width() - m_gbHiddenGroup->width() - 5, height());
    }
}

void MainWindow::addMacros()
{    
    HiddenLayout->removeItem(spacer);
    MiniMacrosList.insert(index, new MiniMacros(index, this));
    HiddenLayout->addWidget(MiniMacrosList[index]);
    HiddenLayout->addSpacerItem(spacer);

    connect(MiniMacrosList[index], SIGNAL(deleteSignal(int)), this, SLOT(delMacros(int)));
    index++;
}

void MainWindow::delMacros(int index)
{
    delete MiniMacrosList[index];
    MiniMacrosList.remove(index);
}

void MainWindow::writeLogTimeout()
{
    writeLog.close();
    m_abSaveWriteLog->setChecked(false);
    logWrite = false;
    m_tWriteLog->stop();
}

void MainWindow::readLogTimeout()
{
    readLog.close();
    m_abSaveReadLog->setChecked(false);
    logRead = false;
    m_tReadLog->stop();
}

void MainWindow::startWriteLog(bool check)
{
    if (check)
    {
        QString path = fileDialog->getSaveFileName(this,
                                                   tr("Save File"),
                                                   QDir::currentPath() + "/(WRITE)" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".txt",
                                                   tr("Log Files (*.txt)"));
        if (path.isEmpty())
        {
            m_abSaveWriteLog->setChecked(false);
            return;
        }
        writeLog.setFileName(path);
        writeLog.open(QIODevice::WriteOnly);
        m_tWriteLog->start();
        logWrite = true;
        m_abSaveWriteLog->setIcon(QIcon(":/Resources/startRecToFileBlink.png"));
    } else
    {
        m_tWriteLog->stop();
        writeLog.close();
        logWrite = false;
        m_abSaveWriteLog->setIcon(QIcon(":/Resources/startRecToFile.png"));
    }
}

void MainWindow::startReadLog(bool check)
{
    if (check)
    {
        QString path = fileDialog->getSaveFileName(this,
                                                   tr("Save File"),
                                                   QDir::currentPath() + "/(READ)" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".txt",
                                                   tr("Log Files (*.txt)"));
        if (path.isEmpty())
        {
            m_abSaveReadLog->setChecked(false);
            return;
        }
        readLog.setFileName(path);
        readLog.open(QIODevice::WriteOnly);
        m_tReadLog->start();
        logRead = true;
        m_abSaveReadLog->setIcon(QIcon(":/Resources/startRecToFileBlink.png"));
    } else
    {
        m_tReadLog->stop();
        readLog.close();
        logRead = false;
        m_abSaveReadLog->setIcon(QIcon(":/Resources/startRecToFile.png"));
    }
}

void MainWindow::saveWrite()
{
    QString fileName = fileDialog->getSaveFileName(this,
                                                   tr("Save File"),
                                                   QDir::currentPath() + "/(WRITE)" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".txt",
                                                   tr("Log Files (*.txt)"));

        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not save file"));
                return;
            } else {
                QTextStream stream(&file);
                for(int i = 0; i < m_eLogWrite->count(); ++i)
                {
                    stream << m_eLogWrite->item(i)->text() + "\n";
                }
                file.close();
            }
        }
}

void MainWindow::saveRead()
{
    QString fileName = fileDialog->getSaveFileName(this,
                                                   tr("Save File"),
                                                   QDir::currentPath() + "/(READ)" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".txt",
                                                   tr("Log Files (*.txt)"));

        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not save file"));
                return;
            } else {
                QTextStream stream(&file);
                for(int i = 0; i < m_eLogRead->count(); ++i)
                {
                    stream << m_eLogRead->item(i)->text() + "\n";
                }
                file.close();
            }
        }
}

void MainWindow::textChanged(QString text)
{
    if (!text.isEmpty() && m_bStop->isEnabled())
    {
        m_sbRepeatSendInterval->setEnabled(true);
        m_abSendPackage->setEnabled(true);
        m_abSendPackage->setCheckable(true);
    }
    else
    {
        m_sbRepeatSendInterval->setEnabled(false);
        m_abSendPackage->setEnabled(false);
        m_abSendPackage->setCheckable(false);
    }
}

void MainWindow::cleanEchoBuffer(bool check)
{
    m_tEcho->stop();
    if (!check)
        echoData.clear();
}

void MainWindow::start()
{
    m_Port->close();
    m_Port->setPortName(m_cbPort->currentText());

    if(m_Port->open(QSerialPort::ReadWrite))
    {       
        switch (m_cbBaud->currentIndex()) {
        case 0:
            m_Port->setBaudRate(QSerialPort::Baud921600);
            break;
        case 1:
            m_Port->setBaudRate(QSerialPort::Baud115200);
            break;
        case 2:
            m_Port->setBaudRate(QSerialPort::Baud57600);
            break;
        case 3:
            m_Port->setBaudRate(QSerialPort::Baud38400);
            break;
        case 4:
            m_Port->setBaudRate(QSerialPort::Baud19200);
            break;
        case 5:
            m_Port->setBaudRate(QSerialPort::Baud9600);
            break;
        case 6:
            m_Port->setBaudRate(QSerialPort::Baud4800);
            break;
        case 7:
            m_Port->setBaudRate(QSerialPort::Baud2400);
            break;
        case 8:
            m_Port->setBaudRate(QSerialPort::Baud1200);
            break;
        }

        switch (m_cbBits->currentIndex()) {
        case 0:
            m_Port->setDataBits(QSerialPort::Data5);
            break;
        case 1:
            m_Port->setDataBits(QSerialPort::Data6);
            break;
        case 2:
            m_Port->setDataBits(QSerialPort::Data7);
            break;
        case 3:
            m_Port->setDataBits(QSerialPort::Data8);
            break;
        }

        switch (m_cbParity->currentIndex()) {
        case 0:
            m_Port->setParity(QSerialPort::NoParity);
            break;
        case 1:
            m_Port->setParity(QSerialPort::OddParity);
            break;
        case 2:
            m_Port->setParity(QSerialPort::EvenParity);
            break;
        case 3:
            m_Port->setParity(QSerialPort::MarkParity);
            break;
        case 4:
            m_Port->setParity(QSerialPort::SpaceParity);
            break;
        }

        switch (m_cbStopBits->currentIndex()) {
        case 0:
            m_Port->setStopBits(QSerialPort::OneStop);
            break;
        case 1:
            m_Port->setStopBits(QSerialPort::OneAndHalfStop);
            break;
        case 2:
            m_Port->setStopBits(QSerialPort::TwoStop);
            break;
        }


        m_Port->setDataBits(QSerialPort::Data8);
        m_Port->setParity(QSerialPort::NoParity);
        m_Port->setFlowControl(QSerialPort::NoFlowControl);

        m_bStart->setEnabled(false);
        m_bStop->setEnabled(true);
        m_cbPort->setEnabled(false);
        m_cbBaud->setEnabled(false);
        m_cbBits->setEnabled(false);
        m_cbParity->setEnabled(false);
        m_cbStopBits->setEnabled(false);
        m_lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
        m_lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
        if (!m_leSendPackage->text().isEmpty())
            m_abSendPackage->setEnabled(true);
    }
    else
    {
        m_lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
        m_lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    }
}

void MainWindow::stop()
{
    m_Port->close();
    m_BlinkTimeTxNone->stop();
    m_BlinkTimeTxColor->stop();
    m_BlinkTimeRxNone->stop();
    m_BlinkTimeRxColor->stop();
    m_lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_bStop->setEnabled(false);
    m_bStart->setEnabled(true);
    m_cbPort->setEnabled(true);
    m_cbBaud->setEnabled(true);
    m_cbBits->setEnabled(true);
    m_cbParity->setEnabled(true);
    m_cbStopBits->setEnabled(true);
    m_abSendPackage->setEnabled(false);
    m_abSendPackage->setChecked(false);
    m_tSend->stop();
    m_tEcho->stop();
}

void MainWindow::received(bool isReceived)
{
    if(isReceived) {
        if(!m_BlinkTimeRxColor->isActive() && !m_BlinkTimeRxNone->isActive()) {
            m_BlinkTimeRxColor->start();
            m_lRx->setStyleSheet("background: green; font: bold; font-size: 10pt");
        }
        QByteArray out = m_Protocol->getReadedData();
        if (m_cbMode->currentText() == "HEX")
            displayReadDataHEX(QString(out.toHex()));
        if (m_cbMode->currentText() == "ASCII")
            displayReadDataASCII(QString(out));
        if (m_cbMode->currentText() == "DEC")
            displayReadDataDEC(QString(out));
    }
}

void MainWindow::colorIsRx()
{
    m_lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_BlinkTimeRxColor->stop();
    m_BlinkTimeRxNone->start();
}

void MainWindow::colorRxNone()
{
    m_BlinkTimeRxNone->stop();
}

void MainWindow::sendSingle()
{
    sendPackage(m_leSendPackage->text());
}

void MainWindow::echo()
{
    sendPackage(echoData.takeFirst());
    if (echoData.isEmpty())
    {
        m_tEcho->stop();
    }
}

void MainWindow::startSending(bool checked)
{
    if (checked)
        {
            if (m_Port->isOpen())
            {
                if (m_sbRepeatSendInterval->value() == 0)
                {
                    sendPackage(m_leSendPackage->text());
                    m_abSendPackage->setChecked(false);
                } else
                {
                    m_tSend->setInterval(m_sbRepeatSendInterval->value());
                    m_tSend->start();
                }
            }
        } else
        {
            m_tSend->stop();
        }
}

void MainWindow::sendPackage(QString string)
{
    if (m_cbMode->currentText() == "HEX")
        sendPackageHEX(string);
    if (m_cbMode->currentText() == "ASCII")
        sendPackageASCII(string);
    if (m_cbMode->currentText() == "DEC")
        sendPackageDEC(string);

    if (m_Port->isOpen())
    {
        if(!m_BlinkTimeTxColor->isActive() && !m_BlinkTimeTxNone->isActive()) {
            m_BlinkTimeTxColor->start();
            m_lTx->setStyleSheet("background: red; font: bold; font-size: 10pt");
        }
    }
}

void MainWindow::sendPackageDEC(QString string)
{
    if (m_Port->isOpen())
    {
        m_tSend->setInterval(m_sbRepeatSendInterval->value());
        m_Protocol->setDataToWrite(string);
        m_Protocol->writeData();
        displayWriteData(string);
    }
}

void MainWindow::sendPackageASCII(QString string)
{
    if (m_Port->isOpen())
    {
        m_tSend->setInterval(m_sbRepeatSendInterval->value());
        m_Protocol->writeDataNow(string.toLatin1());
        displayWriteData(string);
    }
}

void MainWindow::sendPackageHEX(QString string)
{
    if (m_Port->isOpen())
    {
        QStringList byteList = string.split(SEPARATOR, QString::SkipEmptyParts);

        if (byteList.last().length() == 1)
            string.insert(string.length()-1, "0");

        m_tSend->setInterval(m_sbRepeatSendInterval->value());
        foreach (QString s, byteList)
        {
            bool ok;
            m_Protocol->setDataToWrite(QString::number(s.toInt(&ok, 16)));
            if (ok)
                m_Protocol->writeData();
        }
        displayWriteData(string.toUpper());
    }
}

void MainWindow::displayReadDataASCII(QString string)
{
    QTextStream readStream(&readLog);
    logReadRowsCount++;
    m_eLogRead->addItem(string);
    if (logRead)
        readStream << string + "\n";

    if (logReadRowsCount >= maxReadLogRows)
    {
        delete m_eLogRead->takeItem(0);
        logReadRowsCount--;
    }
    if (m_cbReadScroll->isChecked())
        m_eLogRead->scrollToBottom();
}

void MainWindow::displayReadDataHEX(QString string)
{    
    QTextStream readStream(&readLog);

    for (int i = 2; !(i >= string.length()); i += 3)
    {
        string.insert(i, SEPARATOR);
    }

    if (m_cbEchoMode->isChecked())
    {
        echoData.append(string);
        if (!m_tEcho->isActive())
        {
            m_tEcho->setInterval(m_sbEchoInterval->value());
            m_tEcho->start();
        }
    }

    m_eLogRead->addItem(string.toUpper());
    if (logRead)
        readStream << string.toUpper() + "\n";
    logReadRowsCount++;

    if (logReadRowsCount >= maxReadLogRows)
    {
        delete m_eLogRead->takeItem(0);
        logReadRowsCount--;
    }
    if (m_cbReadScroll->isChecked())
        m_eLogRead->scrollToBottom();
}

void MainWindow::displayReadDataDEC(QString string)
{
    m_eLogRead->addItem(string);
}

void MainWindow::displayWriteData(QString string)
{   
    logWriteRowsCount++;
    m_eLogWrite->addItem(string);
    if (logWrite)
    {
        QTextStream writeStream (&writeLog);
        writeStream << string + "\n";
    }

    if (logWriteRowsCount >= maxWriteLogRows)
    {
        delete m_eLogWrite->takeItem(0);
        logWriteRowsCount--;
    }

    if (m_cbWriteScroll->isChecked())
        m_eLogWrite->scrollToBottom();
}

void MainWindow::clearReadLog()
{
    m_eLogRead->clear();
    logReadRowsCount = 0;
    readBytesDisplayed = 0;
}

void MainWindow::clearWriteLog()
{
    m_eLogWrite->clear();
    logWriteRowsCount = 0;
}

void MainWindow::colorIsTx()
{
    m_lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_BlinkTimeTxColor->stop();
    m_BlinkTimeTxNone->start();
}

void MainWindow::colorTxNone()
{
    m_BlinkTimeTxNone->stop();
}

void MainWindow::saveSession()
{
    settings->setValue("config/height", height());
    settings->setValue("config/width", width());
    settings->setValue("config/position", pos());

    settings->setValue("config/max_write_log_rows", maxWriteLogRows);
    settings->setValue("config/max_read_log_rows", maxReadLogRows);
    settings->setValue("config/port", m_cbPort->currentText());
    settings->setValue("config/baud", m_cbBaud->currentIndex());
    settings->setValue("config/data_bits", m_cbBits->currentIndex());
    settings->setValue("config/parity", m_cbParity->currentIndex());
    settings->setValue("config/stop_bits", m_cbStopBits->currentIndex());
    settings->setValue("config/echo", m_cbEchoMode->checkState());
    settings->setValue("config/echo_interval", m_sbEchoInterval->value());
    settings->setValue("config/single_send_interval", m_sbRepeatSendInterval->value());
    settings->setValue("config/write_autoscroll", m_cbWriteScroll->isChecked());
    settings->setValue("config/read_autoscroll", m_cbReadScroll->isChecked());
    settings->setValue("config/write_log_timeout", m_tWriteLog->interval());
    settings->setValue("config/read_log_timeout", m_tReadLog->interval());
    settings->setValue("config/hidden_group_isHidden", m_gbHiddenGroup->isHidden());
    settings->setValue("config/mode", m_cbMode->currentIndex());
}

void MainWindow::loadSession()
{
    const QPoint pos = settings->value ("config/position").toPoint();
        if (!pos.isNull())
            move (pos);

    maxWriteLogRows = settings->value("config/max_write_log_rows", 1000).toInt();
    maxReadLogRows = settings->value("config/max_read_log_rows", 1000).toInt();
    m_cbPort->setCurrentText(settings->value("config/port").toString());
    m_cbBaud->setCurrentIndex(settings->value("config/baud").toInt());
    m_cbBits->setCurrentIndex(settings->value("config/data_bits").toInt());
    m_cbParity->setCurrentIndex(settings->value("config/parity").toInt());
    m_cbStopBits->setCurrentIndex(settings->value("config/stop_bits").toInt());
    m_cbEchoMode->setChecked(settings->value("config/echo").toBool());
    m_sbEchoInterval->setValue(settings->value("config/echo_interval").toInt());
    m_sbRepeatSendInterval->setValue(settings->value("config/single_send_interval").toInt());
    m_cbWriteScroll->setChecked(settings->value("config/write_autoscroll", true).toBool());
    m_cbReadScroll->setChecked(settings->value("config/read_autoscroll", true).toBool());
    m_tWriteLog->setInterval(settings->value("config/write_log_timeout", 600000).toInt());
    m_tReadLog->setInterval(settings->value("config/read_log_timeout", 600000).toInt());
    m_gbHiddenGroup->setHidden(settings->value("config/hidden_group_isHidden", true).toBool());
    m_cbMode->setCurrentIndex(settings->value("config/mode", 0).toInt());
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    foreach (MiniMacros *m, MiniMacrosList.values()) {
        m->editing->close();
    }
    saveSession();
    e->accept();
}
