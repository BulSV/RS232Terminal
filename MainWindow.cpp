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

MainWindow::MainWindow(QString title, QWidget *parent)
    : QMainWindow(parent)
    , toolBar(new QToolBar(this))
    , actionPortConfigure(new QAction(QIcon(":/Resources/ComPort.png"), tr("COM-port configure"), this))
    , actionStartStop(new QAction(QIcon(":/Resources/Play.png"), tr("Open COM-port"), this))
    , actionMacros(new QAction(QIcon(":/Resources/Macros.png"), tr("Macros"), this))
    , statusBar(new QStatusBar(this))
    , portName(new QLabel(PORT + tr("None"), this))
    , baud(new QLabel(BAUD + tr("None"), this))
    , bits(new QLabel(BITS + tr("None"), this))
    , parity(new QLabel(PARITY + tr("None"), this))
    , stopBits(new QLabel(STOP_BITS + tr("None"), this))
    , m_cbSendMode(new QComboBox(this))
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
    , m_bWriteLogClear(new QPushButton(QIcon(":/Resources/Clear.png"), tr("Clear"), this))
    , m_bReadLogClear(new QPushButton(QIcon(":/Resources/Clear.png"), tr("Clear"), this))
    , m_bSaveWriteLog(new QPushButton(QIcon(":/Resources/Save.png"), tr("Save"), this))
    , m_bSaveReadLog(new QPushButton(QIcon(":/Resources/Save.png"), tr("Save"), this))
    , m_bRecordWriteLog(new QPushButton(QIcon(":/Resources/Rec.png"), tr("Rec"), this))
    , m_bRecordReadLog(new QPushButton(QIcon(":/Resources/Rec.png"), tr("Rec"), this))
    , m_bSendPackage(new QPushButton(QIcon(":/Resources/Send.png"), tr("Send packet"), this))
    , m_lTxCount(new QLabel("Tx: 0", this))
    , m_lRxCount(new QLabel("Rx: 0", this))
    , m_eLogRead(new LimitedTextEdit(this))
    , m_eLogWrite(new LimitedTextEdit(this))
    , m_sbRepeatSendInterval(new QSpinBox(this))
    , m_sbReadDelayBetweenPackets(new QSpinBox(this))
    , m_leSendPackage(new QLineEdit(this))
    , displayWrite(new ClickableLabel("<img src=':/Resources/Display.png' width='20' height='20'/>", this))
    , displayRead(new ClickableLabel("<img src=':/Resources/Display.png' width='20' height='20'/>", this))
    , m_chbCR(new QCheckBox("CR", this))
    , m_chbLF(new QCheckBox("LF", this))
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

    displayWrite->setToolTip(tr("Hide write data"));
    displayRead->setToolTip(tr("Hide read data"));

    m_eLogRead->displayTime("hh:mm:ss.zzz");
    m_eLogRead->setReadOnly(true);
    m_eLogWrite->displayTime("hh:mm:ss.zzz");
    m_eLogWrite->setReadOnly(true);

    comPortConfigure->setWindowTitle(tr("Port configure"));
    comPortConfigure->setModal(true);

    macrosDockWidget->setWidget(macros);
    macrosDockWidget->setFixedWidth(310);
    macrosDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, macrosDockWidget);

    m_bRecordWriteLog->setCheckable(true);
    m_bRecordReadLog->setCheckable(true);
    m_bSendPackage->setCheckable(true);
    m_sbRepeatSendInterval->setRange(0, 100000);
    m_sbReadDelayBetweenPackets->setRange(0, 10);
    m_sbReadDelayBetweenPackets->setValue(10);

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
    m_cbSendMode->addItems(buffer);
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
    addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->setMovable(false);
    toolBar->addActions(actions);

    statusBar->addWidget(portName);
    statusBar->addWidget(baud);
    statusBar->addWidget(bits);
    statusBar->addWidget(parity);
    statusBar->addWidget(stopBits);
    statusBar->addWidget(m_lTxCount);
    statusBar->addWidget(m_lRxCount);
    setStatusBar(statusBar);

    QHBoxLayout *sendPackageLayout = new QHBoxLayout;
    sendPackageLayout->addWidget(new QLabel(tr("Read delay\nbetween packets, ms:"), this));
    sendPackageLayout->addWidget(m_sbReadDelayBetweenPackets);
    sendPackageLayout->addWidget(new QLabel(tr("Mode:"), this));
    sendPackageLayout->addWidget(m_cbSendMode);
    sendPackageLayout->addWidget(m_leSendPackage);
    sendPackageLayout->addWidget(m_chbCR);
    sendPackageLayout->addWidget(m_chbLF);
    sendPackageLayout->addWidget(m_sbRepeatSendInterval);
    sendPackageLayout->addWidget(m_bSendPackage);
    sendPackageLayout->setSpacing(5);

    displayWrite->setAlignment(Qt::AlignCenter);

    QGridLayout *writeLayout = new QGridLayout;
    writeLayout->addWidget(m_cbWriteMode, 0, 0);
    writeLayout->addWidget(displayWrite, 0, 1);
    writeLayout->addWidget(m_bRecordWriteLog, 1, 0);
    writeLayout->addWidget(m_bSaveWriteLog, 1, 1);
    writeLayout->addWidget(m_bWriteLogClear, 1, 2);
    writeLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 3, 2);
    writeLayout->addWidget(m_eLogWrite, 2, 0, 1, 4);
    writeLayout->setSpacing(5);
    writeLayout->setContentsMargins(5, 5, 5, 5);

    QGroupBox *gbWrite = new QGroupBox(tr("Write"), this);
    gbWrite->setLayout(writeLayout);

    displayRead->setAlignment(Qt::AlignCenter);

    QGridLayout *readLayout = new QGridLayout;
    readLayout->addWidget(m_cbReadMode, 0, 0);
    readLayout->addWidget(displayRead, 0, 1);
    readLayout->addWidget(m_bRecordReadLog, 1, 0);
    readLayout->addWidget(m_bSaveReadLog, 1, 1);
    readLayout->addWidget(m_bReadLogClear, 1, 2);
    readLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 3, 2);
    readLayout->addWidget(m_eLogRead, 2, 0, 1, 4);
    readLayout->setSpacing(5);
    readLayout->setContentsMargins(5, 5, 5, 5);

    QGroupBox *gbRead = new QGroupBox(tr("Read"), this);
    gbRead->setLayout(readLayout);

    QSplitter *splitter = new QSplitter;
    splitter->addWidget(gbWrite);
    splitter->addWidget(gbRead);
    splitter->setHandleWidth(1);

    QGridLayout *dataLayout = new QGridLayout;
    dataLayout->addWidget(splitter, 0, 0);
    dataLayout->addLayout(sendPackageLayout, 1, 0);
    dataLayout->setSpacing(0);
    dataLayout->setContentsMargins(0, 0, 0, 0);

    QGridLayout *allLayouts = new QGridLayout;
    allLayouts->setSpacing(5);
    allLayouts->setContentsMargins(5, 5, 5, 5);
    allLayouts->addLayout(dataLayout, 0, 0);
    widget = new QWidget(this);
    widget->setLayout(allLayouts);
    setCentralWidget(widget);
}

void MainWindow::connections()
{
    connect(actionPortConfigure, &QAction::triggered, comPortConfigure, &ComPortConfigure::show);
    connect(displayWrite, &ClickableLabel::clicked, this, &MainWindow::toggleWriteDisplay);
    connect(displayRead, &ClickableLabel::clicked, this, &MainWindow::toggleReadDisplay);
    connect(m_bReadLogClear, SIGNAL(clicked()), m_eLogRead, SLOT(clear()));
    connect(m_bWriteLogClear, SIGNAL(clicked()), m_eLogWrite, SLOT(clear()));
    connect(actionStartStop, &QAction::triggered, this, &MainWindow::startStop);
    connect(actionMacros, &QAction::triggered, this, &MainWindow::toggleMacrosView);
    connect(m_bSaveWriteLog, SIGNAL(clicked()), this, SLOT(saveWrite()));
    connect(m_bSaveReadLog, SIGNAL(clicked()), this, SLOT(saveRead()));
    connect(m_bRecordWriteLog, SIGNAL(toggled(bool)), this, SLOT(startWriteLog(bool)));
    connect(m_bRecordReadLog, SIGNAL(toggled(bool)), this, SLOT(startReadLog(bool)));
    connect(m_bSendPackage, SIGNAL(toggled(bool)), this, SLOT(startSending(bool)));
    connect(m_leSendPackage, &QLineEdit::returnPressed, [this](){startSending();});
    connect(m_tIntervalSending, SIGNAL(timeout()), this, SLOT(sendInterval()));
    connect(m_tSend, SIGNAL(timeout()), this, SLOT(singleSend()));
    connect(m_timerDelayBetweenPackets, &QTimer::timeout, this, &MainWindow::delayBetweenPacketsEnded);
    connect(m_tWriteLog, SIGNAL(timeout()), this, SLOT(writeLogTimeout()));
    connect(m_tReadLog, SIGNAL(timeout()), this, SLOT(readLogTimeout()));
    connect(m_port, SIGNAL(readyRead()), this, SLOT(received()));
    connect(macros, &Macros::packageSended, this, static_cast<void (MainWindow::*)(const QByteArray &)>(&MainWindow::sendPackage));
    connect(macrosDockWidget, &QDockWidget::topLevelChanged, this, &MainWindow::setMacrosMinimizeFeature);
}

void MainWindow::startStop()
{
    if(actionStartStop->text() == tr("Open COM-port")) {
        actionStartStop->setIcon(QIcon(":/Resources/Stop.png"));
        actionStartStop->setText(tr("Close COM-port"));
        start();
    } else {
        actionStartStop->setIcon(QIcon(":/Resources/Play.png"));
        actionStartStop->setText(tr("Open COM-port"));
        stop();
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
    m_bSendPackage->setChecked(false);
    m_tSend->stop();
    m_timerDelayBetweenPackets->stop();
    m_tIntervalSending->stop();
    portName->setText(PORT + m_port->portName());
    baud->setText(BAUD + baudToString(m_port->baudRate()));
    bits->setText(BITS + bitsToString(m_port->dataBits()));
    parity->setText(PARITY + parityToString(m_port->parity()));
    stopBits->setText(STOP_BITS + stopBitsToString(m_port->stopBits()));
}

void MainWindow::received()
{
    readBuffer += m_port->readAll();
    int delay = m_sbReadDelayBetweenPackets->value();
    if(delay == 0) {
        delayBetweenPacketsEnded();

        return;
    }
    m_timerDelayBetweenPackets->start(delay);
}

void MainWindow::singleSend()
{
    DataEncoder *dataEncoder = getEncoder(m_cbSendMode->currentIndex());
    dataEncoder->setData(m_leSendPackage->text(), " ");
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
        if(m_sbRepeatSendInterval->value() != 0) {
            m_tSend->setInterval(m_sbRepeatSendInterval->value());
            m_tSend->start();

            return;
        }
        m_bSendPackage->setChecked(false);
        singleSend();
    }
}

void MainWindow::sendPackage(const QByteArray &writeData, bool macro)
{
    if(!m_port->isOpen() || writeData.isEmpty()) {
        return;
    }

    QByteArray modifiedData = writeData;
    if(!macro) {
        m_tSend->setInterval(m_sbRepeatSendInterval->value());

        if(m_chbCR->isChecked()) {
            modifiedData.append(CR);
        }
        if(m_chbLF->isChecked()) {
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

    if(logWrite) {
        QTextStream writeStream (&writeLogFile);
        writeStream << displayString + "\n";
    }

    QScrollBar *sb = m_eLogWrite->verticalScrollBar();
    sb->setValue(sb->maximum());
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
        displayWrite->setText("<img src=':/Resources/Display.png' width='20' height='20'/>");
        displayWrite->setToolTip(tr("Hide write data"));
    } else {
        displayWrite->setText("<img src=':/Resources/Hide.png' width='20' height='20'/>");
        displayWrite->setToolTip(tr("Display write data"));
    }
}

void MainWindow::toggleReadDisplay(bool toggled)
{
    if(toggled) {
        displayRead->setText("<img src=':/Resources/Display.png' width='20' height='20'/>");
        displayRead->setToolTip(tr("Hide read data"));
    } else {
        displayRead->setText("<img src=':/Resources/Hide.png' width='20' height='20'/>");
        displayRead->setToolTip(tr("Display read data"));
    }
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
        }

        if(logRead) {
            QTextStream readStream(&readLogFile);
            readStream << dataEncoder->encodedStringList().join(" ") + "\n";
        }
    }
    readBuffer.clear();

    QScrollBar *sb = m_eLogRead->verticalScrollBar();
    sb->setValue(sb->maximum());
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
    settings->setValue("main/size", size());
    settings->setValue("main/position", pos());
    settings->setValue("main/isMaximized", isMaximized());

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

    settings->setValue("main/single_send_interval", m_sbRepeatSendInterval->value());
    settings->setValue("main/mode", m_cbSendMode->currentIndex());
    settings->setValue("main/CR", m_chbCR->isChecked());
    settings->setValue("main/LF", m_chbLF->isChecked());
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
    if(settings->value("main/isMaximized").toBool()) {
        showMaximized();
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

    m_sbRepeatSendInterval->setValue(settings->value("main/single_send_interval", DEFAULT_SEND_TIME).toInt());
    m_chbCR->setChecked(settings->value("main/CR", DEFAULT_CR_LF).toBool());
    m_chbLF->setChecked(settings->value("main/LF", DEFAULT_CR_LF).toBool());
    m_cbSendMode->setCurrentIndex(settings->value("main/mode", DEFAULT_MODE).toInt());
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveSession();

    QWidget::closeEvent(e);
}
