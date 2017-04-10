#include <QGridLayout>
#include <QSerialPortInfo>
#include <QCloseEvent>
#include <QSplitter>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>
#include <QScrollBar>
#include <QDir>
#include <QListIterator>
#include <algorithm>
#include <QGroupBox>
#include <QWidgetAction>

#include "MainWindow.h"
#include "Macro.h"
#include "HexEncoder.h"
#include "AsciiEncoder.h"
#include "DecEncoder.h"

#include <QDebug>

const int BLINK_TIME_TX = 200;
const int BLINK_TIME_RX = 200;

#define CR 0x0D
#define LF 0x0A

QString PORT = QObject::tr("Port: ");
QString BAUD = QObject::tr("Baud rate: ");
QString BITS = QObject::tr("Data bits: ");
QString PARITY = QObject::tr("Parity: ");
QString STOP_BITS = QObject::tr("Stop bits: ");

const int DEFAULT_LOG_ROWS = 1000;
const int DEFAULT_MODE = 0; // ASCII
const int DEFAULT_LOG_TIMEOUT = 600000; // ms
const int DEFAULT_SEND_TIME = 0; // ms
const bool DEFAULT_DISPLAYING = true;
const bool DEFAULT_CR_LF = false;
const Qt::DockWidgetArea DEFAULT_MACROS_AREA = Qt::RightDockWidgetArea;
const bool DEFAULT_MACROS_HIDDEN = true;

MainWindow::MainWindow(QString title, QWidget *parent)
    : QMainWindow(parent)
    , actionPortConfigure(new QAction(QIcon(":/Resources/ComPort.png"), tr("COM-port configure"), this))
    , actionStartStop(new QAction(QIcon(":/Resources/Play.png"), tr("Open COM-port"), this))
    , actionMacros(new QAction(QIcon(":/Resources/Macros.png"), tr("Macros"), this))
    , portName(new QLabel(PORT + tr("None"), this))
    , baud(new QLabel(BAUD + tr("None"), this))
    , bits(new QLabel(BITS + tr("None"), this))
    , parity(new QLabel(PARITY + tr("None"), this))
    , stopBits(new QLabel(STOP_BITS + tr("None"), this))
    , manualSendMode(new QComboBox(this))
    , m_cbReadMode(new QComboBox(this))
    , m_cbWriteMode(new QComboBox(this))
    , m_tSend(new QTimer(this))
    , m_tWriteLog(new QTimer(this))
    , m_tReadLog(new QTimer(this))
    , m_tIntervalSending(new QTimer(this))
    , m_timerDelayBetweenPackets(new QTimer(this))
    , m_tTx(new QTimer(this))
    , m_tRx(new QTimer(this))
    , macros(new Macros)
    , macrosDockWidget(new QDockWidget(tr("Macros"), this))
    , clearWriteLog(new QAction(QIcon(":/Resources/Clear.png"), tr("Clear displayed write data"), this))
    , clearReadLog(new QAction(QIcon(":/Resources/Clear.png"), tr("Clear displayed read data"), this))
    , saveWriteLog(new QAction(QIcon(":/Resources/Save.png"), tr("Save write log"), this))
    , saveReadLog(new QAction(QIcon(":/Resources/Save.png"), tr("Save read log"), this))
    , recordWriteLog(new QAction(QIcon(":/Resources/Record.png"), tr("Record write log"), this))
    , recordReadLog(new QAction(QIcon(":/Resources/Record.png"), tr("Record read log"), this))
    , manualSendPacket(new QAction(QIcon(":/Resources/Send.png"), tr("Send packet"), this))
    , m_lTxCount(new QLabel("Tx: 0", this))
    , m_lRxCount(new QLabel("Rx: 0", this))
    , m_eLogRead(new LimitedTextEdit(this))
    , m_eLogWrite(new LimitedTextEdit(this))
    , manualRepeatSendTime(new QSpinBox(this))
    , readDelayBetweenPackets(new QSpinBox(this))
    , manualPacketEdit(new QLineEdit(this))
    , displayWrite(new QAction(QIcon(":/Resources/Display.png"), tr("Hide write data"), this))
    , displayRead(new QAction(QIcon(":/Resources/Display.png"), tr("Hide read data"), this))
    , manualCR(new QAction(QIcon(":/Resources/CR.png"), "Add CR", this))
    , manualLF(new QAction(QIcon(":/Resources/LF.png"), "Add LF", this))
    , m_port(new QSerialPort(this))
    , comPortConfigure(new ComPortConfigure(m_port, this))
    , settings(new QSettings("settings.ini", QSettings::IniFormat))
    , fileDialog(new QFileDialog(this))
    , hexEncoder(new HexEncoder)
    , decEncoder(new DecEncoder)
    , asciiEncoder(new AsciiEncoder)
{
    setWindowTitle(title);

    view();
    connections();

    m_port->setReadBufferSize(1);

    txCount = 0;
    rxCount = 0;
    logWrite = false;
    logRead = false;
    sendCount = 0;
    currentIntervalIndex = -1;

    displayWrite->setCheckable(true);
    displayRead->setCheckable(true);
    manualCR->setCheckable(true);
    manualLF->setCheckable(true);

    m_eLogRead->displayTime("hh:mm:ss.zzz");
    m_eLogRead->setReadOnly(true);
    m_eLogWrite->displayTime("hh:mm:ss.zzz");
    m_eLogWrite->setReadOnly(true);

    comPortConfigure->setWindowTitle(tr("Port configure"));
    comPortConfigure->setModal(true);

    macrosDockWidget->setWidget(macros);
    macrosDockWidget->setFixedWidth(310);
    macrosDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    recordWriteLog->setCheckable(true);
    recordReadLog->setCheckable(true);
    manualSendPacket->setCheckable(true);
    manualRepeatSendTime->setRange(0, 100000);
    readDelayBetweenPackets->setRange(0, 10);
    readDelayBetweenPackets->setValue(10);

    portName->setFrameStyle(QFrame::Sunken);
    portName->setFrameShape(QFrame::Box);
    baud->setFrameStyle(QFrame::Sunken);
    baud->setFrameShape(QFrame::Box);
    bits->setFrameStyle(QFrame::Sunken);
    bits->setFrameShape(QFrame::Box);
    parity->setFrameStyle(QFrame::Sunken);
    parity->setFrameShape(QFrame::Box);
    stopBits->setFrameStyle(QFrame::Sunken);
    stopBits->setFrameShape(QFrame::Box);
    m_lTxCount->setFrameStyle(QFrame::Sunken);
    m_lTxCount->setFrameShape(QFrame::Box);
    m_lRxCount->setFrameStyle(QFrame::Sunken);
    m_lRxCount->setFrameShape(QFrame::Box);

    QStringList buffer;
    buffer << "ASCII" << "HEX" << "DEC";
    manualSendMode->addItems(buffer);
    m_cbReadMode->addItems(buffer);
    m_cbWriteMode->addItems(buffer);

    QDir dir;
    if(!dir.exists(dir.currentPath() + "/Macros")) {
        dir.mkpath(dir.currentPath() + "/Macros");
    }
    if(!dir.exists(dir.currentPath() + "/WriteLogs")) {
        dir.mkpath(dir.currentPath() + "/WriteLogs");
    }
    if(!dir.exists(dir.currentPath() + "/ReadLogs")) {
        dir.mkpath(dir.currentPath() + "/ReadLogs");
    }
    fileDialog->setDirectory(dir.currentPath() + "/Macros");
    fileDialog->setFileMode(QFileDialog::ExistingFiles);

    loadSession();
}

void MainWindow::view()
{
    QList<QAction*> actions;
    actions << actionPortConfigure << actionStartStop << actionMacros;
    QToolBar* toolBar = new QToolBar(this);
    addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->setMovable(false);
    toolBar->addActions(actions);

    QWidgetAction *actionReadDelayBetweenPackets = new QWidgetAction(this);
    actionReadDelayBetweenPackets->setDefaultWidget(readDelayBetweenPackets);
    readDelayBetweenPackets->setToolTip(tr("Read delay\nbetween packets, ms"));
    QWidgetAction *actionManualSendMode = new QWidgetAction(this);
    actionManualSendMode->setDefaultWidget(manualSendMode);
    manualSendMode->setToolTip(tr("Mode"));
    QWidgetAction *actionManualPacketEdit = new QWidgetAction(this);
    actionManualPacketEdit->setDefaultWidget(manualPacketEdit);
    QWidgetAction *actionManualRepeatSendTime = new QWidgetAction(this);
    actionManualRepeatSendTime->setDefaultWidget(manualRepeatSendTime);
    manualRepeatSendTime->setToolTip(tr("Repeat send time, ms"));
    actions.clear();
    actions << actionReadDelayBetweenPackets << actionManualSendMode << actionManualPacketEdit
            << actionManualPacketEdit << manualCR << manualLF << actionManualRepeatSendTime << manualSendPacket;
    QToolBar* manualSendToolBar = new QToolBar(this);
    addToolBar(Qt::BottomToolBarArea, manualSendToolBar);
    manualSendToolBar->setMovable(false);
    manualSendToolBar->addActions(actions);
    manualSendToolBar->setStyleSheet("spacing:2px");

    m_lTxCount->setToolTip(tr("Written bytes count"));
    m_lRxCount->setToolTip(tr("Read bytes count"));

    QStatusBar *statusBar = new QStatusBar(this);
    statusBar->addWidget(portName);
    statusBar->addWidget(baud);
    statusBar->addWidget(bits);
    statusBar->addWidget(parity);
    statusBar->addWidget(stopBits);
    statusBar->addWidget(m_lTxCount);
    statusBar->addWidget(m_lRxCount);
    setStatusBar(statusBar);

    QWidgetAction *actionWriteMode = new QWidgetAction(this);
    actionWriteMode->setDefaultWidget(m_cbWriteMode);
    actions.clear();
    actions << actionWriteMode << displayWrite << recordWriteLog
            << saveWriteLog << clearWriteLog;
    QToolBar *writeToolBar = new QToolBar(this);
    writeToolBar->setAllowedAreas(Qt::TopToolBarArea);
    writeToolBar->setMovable(false);
    writeToolBar->setStyleSheet("spacing:2px");
    writeToolBar->addActions(actions);
    QMainWindow *writeWindow = new QMainWindow(this);
    writeWindow->addToolBar(writeToolBar);
    writeWindow->setCentralWidget(m_eLogWrite);

    QGridLayout *writeLayout = new QGridLayout;
    writeLayout->addWidget(writeWindow, 0, 0);
    writeLayout->setContentsMargins(5, 5, 5, 5);

    QGroupBox *gbWrite = new QGroupBox(tr("Write"), this);
    gbWrite->setLayout(writeLayout);


    QWidgetAction *actionReadMode = new QWidgetAction(this);
    actionReadMode->setDefaultWidget(m_cbReadMode);
    actions.clear();
    actions << actionReadMode << displayRead << recordReadLog
            << saveReadLog << clearReadLog;
    QToolBar *readToolBar = new QToolBar(this);
    readToolBar->setAllowedAreas(Qt::TopToolBarArea);
    readToolBar->setMovable(false);
    readToolBar->setStyleSheet("spacing:2px");
    readToolBar->addActions(actions);
    QMainWindow *readWindow = new QMainWindow(this);
    readWindow->addToolBar(readToolBar);
    readWindow->setCentralWidget(m_eLogRead);

    QGridLayout *readLayout = new QGridLayout;
    readLayout->addWidget(readWindow, 0, 0);
    readLayout->setContentsMargins(5, 5, 5, 5);

    QGroupBox *gbRead = new QGroupBox(tr("Read"), this);
    gbRead->setLayout(readLayout);


    QSplitter *splitter = new QSplitter;
    splitter->addWidget(gbWrite);
    splitter->addWidget(gbRead);
    splitter->setHandleWidth(1);
    splitter->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    QGridLayout *dataLayout = new QGridLayout;
    dataLayout->addWidget(splitter, 0, 0);
    dataLayout->setSpacing(0);
    dataLayout->setContentsMargins(0, 0, 0, 0);

    QGridLayout *mainLayouts = new QGridLayout;
    mainLayouts->setSpacing(5);
    mainLayouts->setContentsMargins(5, 5, 5, 5);
    mainLayouts->addLayout(dataLayout, 0, 0);
    QWidget *widget = new QWidget(this);
    widget->setLayout(mainLayouts);
    setCentralWidget(widget);
}

void MainWindow::connections()
{
    connect(actionPortConfigure, &QAction::triggered, comPortConfigure, &ComPortConfigure::show);
    connect(displayWrite, &QAction::triggered, this, &MainWindow::toggleWriteDisplay);
    connect(displayRead, &QAction::triggered, this, &MainWindow::toggleReadDisplay);
    connect(clearReadLog, &QAction::triggered, m_eLogRead, &LimitedTextEdit::clear);
    connect(clearWriteLog, &QAction::triggered, m_eLogWrite, &LimitedTextEdit::clear);
    connect(actionStartStop, &QAction::triggered, this, &MainWindow::startStop);
    connect(actionMacros, &QAction::triggered, this, &MainWindow::toggleMacrosView);
    connect(saveWriteLog, &QAction::triggered, this, &MainWindow::saveWrite);
    connect(saveReadLog, &QAction::triggered, this, &MainWindow::saveRead);
    connect(recordWriteLog, &QAction::triggered, this, &MainWindow::startWriteLog);
    connect(recordReadLog, &QAction::triggered, this, &MainWindow::startReadLog);
    connect(manualSendPacket, &QAction::triggered, this, &MainWindow::startSending);
    connect(manualPacketEdit, &QLineEdit::returnPressed, [this](){startSending();});
    connect(m_tIntervalSending, SIGNAL(timeout()), this, SLOT(sendInterval()));
    connect(m_tSend, SIGNAL(timeout()), this, SLOT(singleSend()));
    connect(m_timerDelayBetweenPackets, &QTimer::timeout, this, &MainWindow::delayBetweenPacketsEnded);
    connect(m_tWriteLog, SIGNAL(timeout()), this, SLOT(writeLogTimeout()));
    connect(m_tReadLog, SIGNAL(timeout()), this, SLOT(readLogTimeout()));
    connect(m_port, SIGNAL(readyRead()), this, SLOT(received()));
    connect(macros, &Macros::packetSended, this, static_cast<void (MainWindow::*)(const QByteArray &)>(&MainWindow::sendPackage));
    connect(macrosDockWidget, &QDockWidget::topLevelChanged, this, &MainWindow::setMacrosMinimizeFeature);
    connect(macrosDockWidget, &QDockWidget::dockLocationChanged, this, &MainWindow::saveCurrentMacrosArea);
}

void MainWindow::startStop()
{
    if(actionStartStop->text() == tr("Open COM-port")) {
        start();
        if(!m_port->isOpen()) {
            return;
        }
        actionStartStop->setIcon(QIcon(":/Resources/Stop.png"));
        actionStartStop->setText(tr("Close COM-port"));
    } else {
        stop();
        actionStartStop->setIcon(QIcon(":/Resources/Play.png"));
        actionStartStop->setText(tr("Open COM-port"));
    }
}

void MainWindow::sendPackage(const QByteArray &data)
{
    Macros *macros = qobject_cast<Macros*>(sender());
    QPushButton *b = qobject_cast<QPushButton*>(sender());
    if(macros == 0 && b == 0) {
        return;
    }
    bool macro = b == 0 ? true : false;
    sendPackage(data, macro);
}

void MainWindow::writeLogTimeout()
{
    m_tWriteLog->stop();
    writeLogFile.close();
    logWrite = false;
}

void MainWindow::readLogTimeout()
{
    m_tReadLog->stop();
    readLogFile.close();
    logRead = false;
}

void MainWindow::startWriteLog(bool check)
{
    if(!check) {
        writeLogTimeout();

        return;
    }
    QString path = QDir::currentPath() + "/WriteLogs" + "/(WRITE)_" + QDateTime::currentDateTime().toString("yyyy.MM.dd_HH.mm.ss") + ".txt";
    writeLogFile.setFileName(path);
    if(!writeLogFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not save file"));

        return;
    }
    m_tWriteLog->start();
    logWrite = true;
}

void MainWindow::startReadLog(bool check)
{
    if(!check) {
        readLogTimeout();

        return;
    }

    QString path = QDir::currentPath() + "/ReadLogs" + "/(READ)_" + QDateTime::currentDateTime().toString("yyyy.MM.dd_HH.mm.ss") + ".txt";
    readLogFile.setFileName(path);
    if(!readLogFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not save file"));

        return;
    }
    m_tReadLog->start();
    logRead = true;
}

void MainWindow::updateIntervalsList(bool add)
{
//    MacroWidget *macro = qobject_cast<MacroWidget*>(sender());
//    if(macro == 0) {
//        return;
//    }
//    int index = macrosWidgets.indexOf(macro);
//    if(add) {
//        indexesOfIntervals.append(index);
//        std::sort(indexesOfIntervals.begin(), indexesOfIntervals.end(), qLess<int>());

//        return;
//    }

//    indexesOfIntervals.removeOne(index);
}

void MainWindow::sendNextMacro()
{
//    if(m_port->isOpen()) {
//        MacroWidget *macro = macrosWidgets.at(indexesOfIntervals.at(currentIntervalIndex++));
//        if(currentIntervalIndex >= indexesOfIntervals.size()) {
//            currentIntervalIndex = 0;
//        }
//        sendPackage(macro->getPackage(), true);
//        m_tIntervalSending->start(macro->getTime());
//    }
}

void MainWindow::start()
{
    m_port->close();

    if(!m_port->open(QSerialPort::ReadWrite)) {
        m_lTxCount->setStyleSheet("background: none");
        m_lRxCount->setStyleSheet("background: none");

        return;
    }

    m_port->setFlowControl(QSerialPort::NoFlowControl);

    actionPortConfigure->setEnabled(false);

    m_lTxCount->setStyleSheet("background: yellow");
    m_lRxCount->setStyleSheet("background: yellow");

    txCount = 0;
    m_lTxCount->setText("Tx: 0");
    rxCount = 0;
    m_lRxCount->setText("Rx: 0");
    portName->setText(PORT + m_port->portName());
    baud->setText(BAUD + baudToString(m_port->baudRate()));
    bits->setText(BITS + bitsToString(m_port->dataBits()));
    parity->setText(PARITY + parityToString(m_port->parity()));
    stopBits->setText(STOP_BITS + stopBitsToString(m_port->stopBits()));

//    sendNextMacros(); // FIXME
}

void MainWindow::stop()
{
    m_port->close();
    m_lTxCount->setStyleSheet("background: none");
    m_lRxCount->setStyleSheet("background: none");

    actionPortConfigure->setEnabled(true);
    manualSendPacket->setChecked(false);
    m_tSend->stop();
    m_timerDelayBetweenPackets->stop();
    m_tIntervalSending->stop();
    portName->setText(PORT + tr("None"));
    baud->setText(BAUD + tr("None"));
    bits->setText(BITS + tr("None"));
    parity->setText(PARITY + tr("None"));
    stopBits->setText(STOP_BITS + tr("None"));
}

void MainWindow::received()
{
    readBuffer += m_port->readAll();
    int delay = readDelayBetweenPackets->value();
    if(delay == 0) {
        delayBetweenPacketsEnded();

        return;
    }
    m_timerDelayBetweenPackets->start(delay);
}

void MainWindow::singleSend()
{
    DataEncoder *dataEncoder = getEncoder(manualSendMode->currentIndex());
    dataEncoder->setData(manualPacketEdit->text(), " ");
    sendPackage(dataEncoder->encodedByteArray(), false);
}

void MainWindow::saveReadWriteLog(bool writeLog)
{
    QString defaultFileName = (writeLog ? "(WRITE)_" : "(READ)_")
            + QDateTime::currentDateTime().toString("yyyy.MM.dd_HH.mm.ss")
            + ".txt";
    QString fileName = fileDialog->getSaveFileName(this,
                                                   tr("Save File"),
                                                   QDir::currentPath()
                                                   + (writeLog ? "/WriteLogs/" : "/ReadLogs/")
                                                   + defaultFileName,
                                                   tr("Log Files (*.txt)"));
    if(fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not save file"));

        return;
    }
    QTextStream stream(&file);
    if(writeLog) {
        stream << m_eLogWrite->toPlainText();
    } else {
        stream << m_eLogRead->toPlainText();
    }
    file.close();
}

void MainWindow::saveWrite()
{
    saveReadWriteLog(true);
}

void MainWindow::saveRead()
{
    saveReadWriteLog(false);
}

void MainWindow::startSending(bool checked)
{
    if(!checked) {
        m_tSend->stop();

        return;
    }

    if(m_port->isOpen()) {
        if(manualRepeatSendTime->value() != 0) {
            m_tSend->setInterval(manualRepeatSendTime->value());
            m_tSend->start();

            return;
        }
        singleSend();
    }
    manualSendPacket->setChecked(false);
}

void MainWindow::sendPackage(const QByteArray &writeData, bool macro)
{
    if(!m_port->isOpen() || writeData.isEmpty()) {
        return;
    }

    QByteArray modifiedData = writeData;
    if(!macro) {
        m_tSend->setInterval(manualRepeatSendTime->value());

        if(manualCR->isChecked()) {
            modifiedData.append(CR);
        }
        if(manualLF->isChecked()) {
            modifiedData.append(LF);
        }
    }

    displayWrittenData(modifiedData);
    txCount += m_port->write(modifiedData);
    m_lTxCount->setText("Tx: " + QString::number(txCount));

    if(!m_tTx->isSingleShot()) {
        m_lTxCount->setStyleSheet("background: green");
        m_tTx->singleShot(BLINK_TIME_TX, this, &MainWindow::txNone);
        m_tTx->setSingleShot(true);
    }
}

void MainWindow::displayWrittenData(const QByteArray &writeData)
{
    if(!displayWrite->isChecked()) {
        return;
    }

    DataEncoder *dataEncoder = getEncoder(m_cbWriteMode->currentIndex());
    dataEncoder->setData(writeData);
    QString displayString = dataEncoder->encodedStringList().join(" ");
    m_eLogWrite->addLine(displayString);
    QScrollBar *sb = m_eLogWrite->verticalScrollBar();
    sb->setValue(sb->maximum());

    if(logWrite) {
        QTextStream writeStream (&writeLogFile);
        writeStream << displayString + "\n";
    }
}

DataEncoder *MainWindow::getEncoder(int mode)
{
    DataEncoder *dataEncoder = 0;
    switch(mode) {
    case ASCII:
        dataEncoder = asciiEncoder;
        break;
    case HEX:
        dataEncoder = hexEncoder;
        break;
    default:
        dataEncoder = decEncoder;
    }

    return dataEncoder;
}

QString MainWindow::baudToString(int baud)
{
    QString baudString;
    switch(baud) {
    case QSerialPort::Baud1200:
        baudString = "1200";
        break;
    case QSerialPort::Baud2400:
        baudString = "2400";
        break;
    case QSerialPort::Baud4800:
        baudString = "4800";
        break;
    case QSerialPort::Baud9600:
        baudString = "9600";
        break;
    case QSerialPort::Baud19200:
        baudString = "19200";
        break;
    case QSerialPort::Baud38400:
        baudString = "38400";
        break;
    case QSerialPort::Baud57600:
        baudString = "57600";
        break;
    case QSerialPort::Baud115200:
        baudString = "115200";
        break;
    case QSerialPort::Baud230400:
        baudString = "230400";
        break;
    case QSerialPort::Baud460800:
        baudString = "460800";
        break;
    default:
        baudString = "921600";
        break;
    }

    return baudString;
}

QString MainWindow::bitsToString(int bits)
{
    QString bitsString;
    switch(bits) {
    case QSerialPort::Data5:
       bitsString = "5";
        break;
    case QSerialPort::Data6:
        bitsString = "6";
        break;
    case QSerialPort::Data7:
        bitsString = "7";
        break;
    default:
        bitsString = "8";
        break;
    }

    return bitsString;
}

QString MainWindow::parityToString(int parity)
{
    QString parityString;
    switch(parity) {
    case QSerialPort::NoParity:
        parityString = "No";
        break;
    case QSerialPort::OddParity:
        parityString = "Odd";
        break;
    case QSerialPort::EvenParity:
        parityString = "Even";
        break;
    case QSerialPort::MarkParity:
        parityString = "Mark";
        break;
    default:
        parityString = "Space";
        break;
    }

    return parityString;
}

QString MainWindow::stopBitsToString(int stopBits)
{
    QString stopBitsString;
    switch(stopBits) {
    case QSerialPort::OneStop:
        stopBitsString = "1";
        break;
    case QSerialPort::OneAndHalfStop:
        stopBitsString = "1.5";
        break;
    default:
        stopBitsString = "2";
        break;
    }

    return stopBitsString;
}

void MainWindow::setMacrosMinimizeFeature(bool floating)
{
    if(floating) {
        macrosDockWidget->setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
        macrosDockWidget->show();
    }
}

void MainWindow::toggleMacrosView()
{
    if(macrosDockWidget->toggleViewAction()->isChecked()) {
        macrosDockWidget->close();

        return;
    }

    macrosDockWidget->show();
}

void MainWindow::toggleWriteDisplay(bool toggled)
{
    if(toggled) {
        displayWrite->setIcon(QIcon(":/Resources/Display.png"));
        displayWrite->setToolTip(tr("Hide write data"));
    } else {
        displayWrite->setIcon(QIcon(":/Resources/Hide.png"));
        displayWrite->setToolTip(tr("Display write data"));
    }
}

void MainWindow::toggleReadDisplay(bool toggled)
{
    if(toggled) {
        displayRead->setIcon(QIcon(":/Resources/Display.png"));
        displayRead->setToolTip(tr("Hide read data"));
    } else {
        displayRead->setIcon(QIcon(":/Resources/Hide.png"));
        displayRead->setToolTip(tr("Display read data"));
    }
}

void MainWindow::saveCurrentMacrosArea(Qt::DockWidgetArea area)
{
    macrosDockWidgetArea = area;
}

// Перевод строки при приеме данных
// Срабатывает по таймеру m_timerDelayBetweenPackets
// Определяет отображаемую длину принятого пакета
void MainWindow::delayBetweenPacketsEnded()
{
    m_timerDelayBetweenPackets->stop();

    if(!m_tRx->isSingleShot()) {
        m_lRxCount->setStyleSheet("background: red");
        m_tRx->singleShot(BLINK_TIME_RX, this, &MainWindow::rxNone);
        m_tRx->setSingleShot(true);
    }
    rxCount+=readBuffer.size();
    m_lRxCount->setText("Rx: " + QString::number(rxCount));

    if(displayRead->isChecked() || logRead) {
        DataEncoder *dataEncoder = getEncoder(m_cbReadMode->currentIndex());
        dataEncoder->setData(readBuffer);

        if(displayRead->isChecked()) {
            m_eLogRead->addLine(dataEncoder->encodedStringList().join(" "));
            QScrollBar *sb = m_eLogRead->verticalScrollBar();
            sb->setValue(sb->maximum());
        }

        if(logRead) {
            QTextStream readStream(&readLogFile);
            readStream << dataEncoder->encodedStringList().join(" ") + "\n";
        }
    }
    readBuffer.clear();
}

void MainWindow::rxNone()
{
    m_lRxCount->setStyleSheet("background: yellow");
    m_tRx->singleShot(BLINK_TIME_RX, this, SLOT(rxHold()));
}

void MainWindow::txNone()
{
    m_lTxCount->setStyleSheet("background: yellow");
    m_tTx->singleShot(BLINK_TIME_TX, this, SLOT(txHold()));
}

void MainWindow::rxHold()
{
    m_tRx->setSingleShot(false);
}

void MainWindow::txHold()
{
    m_tTx->setSingleShot(false);
}

void MainWindow::saveSession()
{
    settings->remove("main");
    settings->setValue("main/size", currentWindowSize);
    settings->setValue("main/position", currentWindowPos);

    settings->setValue("main/write_mode", m_cbWriteMode->currentIndex());
    settings->setValue("main/read_mode", m_cbReadMode->currentIndex());
    settings->setValue("main/max_write_log_rows", m_eLogWrite->linesLimit());
    settings->setValue("main/max_read_log_rows", m_eLogRead->linesLimit());
    settings->setValue("main/write_display", displayWrite->isChecked());
    settings->setValue("main/read_display", displayRead->isChecked());
    settings->setValue("main/write_log_timeout", m_tWriteLog->interval());
    settings->setValue("main/read_log_timeout", m_tReadLog->interval());

    comPortConfigure->saveSettings(settings);
    macros->saveSettings(settings);

    settings->setValue("main/single_send_interval", manualRepeatSendTime->value());
    settings->setValue("main/mode", manualSendMode->currentIndex());
    settings->setValue("main/CR", manualCR->isChecked());
    settings->setValue("main/LF", manualLF->isChecked());

    settings->setValue("main/macros_dock_widget_area", macrosDockWidgetArea);
    settings->setValue("main/macros_dock_widget_hidden", macrosDockWidget->isHidden());
}

void MainWindow::loadSession()
{
    QSize size = settings->value("main/size").toSize();
    if(size.isValid()) {
        resize(size);
    }
    QPoint pos = settings->value("main/position").toPoint();
    if(!pos.isNull()) {
        move(pos);
    }

    m_eLogRead->setLinesLimit(settings->value("main/max_write_log_rows", DEFAULT_LOG_ROWS).toInt());
    m_eLogWrite->setLinesLimit(settings->value("main/max_read_log_rows", DEFAULT_LOG_ROWS).toInt());

    comPortConfigure->loadSettings(settings);
    macros->loadSettings(settings);

    m_cbWriteMode->setCurrentIndex(settings->value("main/write_mode", DEFAULT_MODE).toInt());
    m_cbReadMode->setCurrentIndex(settings->value("main/read_mode", DEFAULT_MODE).toInt());
    displayWrite->setChecked(settings->value("main/write_display", DEFAULT_DISPLAYING).toBool());
    displayRead->setChecked(settings->value("main/read_display", DEFAULT_DISPLAYING).toBool());
    m_tWriteLog->setInterval(settings->value("main/write_log_timeout", DEFAULT_LOG_TIMEOUT).toInt());
    m_tReadLog->setInterval(settings->value("main/read_log_timeout", DEFAULT_LOG_TIMEOUT).toInt());

    manualRepeatSendTime->setValue(settings->value("main/single_send_interval", DEFAULT_SEND_TIME).toInt());
    manualCR->setChecked(settings->value("main/CR", DEFAULT_CR_LF).toBool());
    manualLF->setChecked(settings->value("main/LF", DEFAULT_CR_LF).toBool());
    manualSendMode->setCurrentIndex(settings->value("main/mode", DEFAULT_MODE).toInt());

    addDockWidget(static_cast<Qt::DockWidgetArea>(settings->value("main/macros_dock_widget_area",
                                                                  DEFAULT_MACROS_AREA).toInt()),
                  macrosDockWidget);
    macrosDockWidget->setHidden(settings->value("main/macros_dock_widget_hidden", DEFAULT_MACROS_HIDDEN).toBool());
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveSession();

    QWidget::closeEvent(e);
}

void MainWindow::moveEvent(QMoveEvent *e)
{
    if(isMaximized()) {
        currentWindowPos = e->pos();
    } else {
        currentWindowPos = e->oldPos();
    }

    QWidget::moveEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    if(isMaximized()) {
        currentWindowSize = e->size();
    } else {
        currentWindowSize = e->oldSize();
    }

    QWidget::resizeEvent(e);
}

void MainWindow::showEvent(QShowEvent *e)
{
    currentWindowSize = size();

    QWidget::showEvent(e);
}
