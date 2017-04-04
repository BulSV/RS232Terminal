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
#include "MacrosWidget.h"
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

MainWindow::MainWindow(QString title, QWidget *parent)
    : QMainWindow(parent)
    , toolBar(new QToolBar(this))
    , actionPortConfigure(new QAction(QIcon(":/Resources/ComPort.png"), tr("Port configure"), this))
    , actionStartStop(new QAction(QIcon(":/Resources/Play.png"), tr("Start"), this))
    , actionMacroses(new QAction(QIcon(":/Resources/Macros.png"), tr("Macroses"), this))
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
    , macroses(new Macroses)
    , macrosesDockWidget(new QDockWidget(tr("Macroses"), this))
    , m_bPause(new QPushButton(this))
    , m_bWriteLogClear(new QPushButton(QIcon(":/Resources/Clear.png"), tr("Clear"), this))
    , m_bReadLogClear(new QPushButton(QIcon(":/Resources/Clear.png"), tr("Clear"), this))
    , m_bSaveWriteLog(new QPushButton(QIcon(":/Resources/Save.png"), tr("Save"), this))
    , m_bSaveReadLog(new QPushButton(QIcon(":/Resources/Save.png"), tr("Save"), this))
    , m_bHiddenGroup(new QPushButton(">", this))
    , m_bDeleteAllMacroses(new QPushButton(this))
    , m_bNewMacros(new QPushButton(this))
    , m_bLoadMacroses(new QPushButton(this))
    , m_bRecordWriteLog(new QPushButton(QIcon(":/Resources/Rec.png"), tr("Rec"), this))
    , m_bRecordReadLog(new QPushButton(QIcon(":/Resources/Rec.png"), tr("Rec"), this))
    , m_bSendPackage(new QPushButton(QIcon(":/Resources/Send.png"), tr("Send packet"), this))
    , m_lTxCount(new QLabel("Tx: 0", this))
    , m_lRxCount(new QLabel("Rx: 0", this))
    , m_eLogRead(new LimitedTextEdit(this))
    , m_eLogWrite(new LimitedTextEdit(this))
    , m_sbRepeatSendInterval(new QSpinBox(this))
    , m_sbDelayBetweenPackets(new QSpinBox(this))
    , m_sbAllDelays(new QSpinBox(this))
    , m_leSendPackage(new QLineEdit(this))
    , m_cbReadScroll(new QCheckBox(tr("Scrolling"), this))
    , m_cbWriteScroll(new QCheckBox(tr("Scrolling"), this))
    , m_cbAllIntervals(new QCheckBox(tr("Interval"), this))
    , m_cbAllPeriods(new QCheckBox(tr("Period"), this))
    , m_cbDisplayWrite(new QCheckBox(tr("Display"), this))
    , m_cbDisplayRead(new QCheckBox(tr("Display"), this))
    , m_chbCR(new QCheckBox("CR", this))
    , m_chbLF(new QCheckBox("LF", this))
    , m_port(new QSerialPort(this))
    , comPortConfigure(new ComPortConfigure(m_port, this))
    , settings(new QSettings("settings.ini", QSettings::IniFormat))
    , fileDialog(new QFileDialog(this))
    , m_gbHiddenGroup(new QGroupBox(this))
    , scrollAreaLayout(new QVBoxLayout)
    , scrollArea(new QScrollArea(m_gbHiddenGroup))
    , scrollWidget(new QWidget(scrollArea))
    , hiddenLayout(new QVBoxLayout(scrollWidget))
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

    m_eLogRead->displayTime("hh:mm:ss.zzz");
    m_eLogRead->setReadOnly(true);
    m_eLogWrite->displayTime("hh:mm:ss.zzz");
    m_eLogWrite->setReadOnly(true);

    comPortConfigure->setWindowTitle(tr("Port configure"));
    comPortConfigure->setModal(true);

    macrosesDockWidget->setWidget(macroses);
    macrosesDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, macrosesDockWidget);

    m_bNewMacros->setIcon(QIcon(":/Resources/Add.png"));
    m_bLoadMacroses->setIcon(QIcon(":/Resources/Open.png"));
    m_bNewMacros->setFixedSize(20, 20);
    m_bLoadMacroses->setFixedSize(20, 20);
    m_bRecordWriteLog->setCheckable(true);
    m_bRecordReadLog->setCheckable(true);
    m_bSendPackage->setCheckable(true);
    m_bPause->setCheckable(true);
    m_bPause->setEnabled(false);
    m_sbRepeatSendInterval->setRange(0, 100000);
    m_sbDelayBetweenPackets->setRange(0, 10);
    m_sbDelayBetweenPackets->setValue(10);
    m_sbAllDelays->setRange(0, 999999);

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
    buffer << "HEX" << "ASCII" << "DEC";
    m_cbSendMode->addItems(buffer);
    m_cbReadMode->addItems(buffer);
    m_cbWriteMode->addItems(buffer);

    QDir dir;
    if(!dir.exists(dir.currentPath() + "/Macroses")) {
        dir.mkpath(dir.currentPath() + "/Macroses");
    }
    fileDialog->setDirectory(dir.currentPath() + "/Macros");
    fileDialog->setFileMode(QFileDialog::ExistingFiles);

    loadSession();
}

void MainWindow::view()
{
    QList<QAction*> actions;
    actions << actionPortConfigure << actionStartStop << actionMacroses;
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
    sendPackageLayout->addWidget(new QLabel(tr("Delay between\nread packets, ms:"), this));
    sendPackageLayout->addWidget(m_sbDelayBetweenPackets);
    sendPackageLayout->addWidget(new QLabel(tr("Mode:"), this));
    sendPackageLayout->addWidget(m_cbSendMode);
    sendPackageLayout->addWidget(m_leSendPackage);
    sendPackageLayout->addWidget(m_chbCR);
    sendPackageLayout->addWidget(m_chbLF);
    sendPackageLayout->addWidget(m_sbRepeatSendInterval);
    sendPackageLayout->addWidget(m_bSendPackage);
    sendPackageLayout->setSpacing(5);

    QGridLayout *writeLayout = new QGridLayout;
    writeLayout->addWidget(m_cbWriteMode, 0, 0);
    writeLayout->addWidget(m_cbWriteScroll, 0, 1);
    writeLayout->addWidget(m_cbDisplayWrite, 0, 2);
    writeLayout->addWidget(m_bRecordWriteLog, 1, 0);
    writeLayout->addWidget(m_bSaveWriteLog, 1, 1);
    writeLayout->addWidget(m_bWriteLogClear, 1, 2);
    writeLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 3, 2);
    writeLayout->addWidget(m_eLogWrite, 2, 0, 1, 4);
    writeLayout->setSpacing(5);
    writeLayout->setContentsMargins(5, 5, 5, 5);

    QGroupBox *gbWrite = new QGroupBox(tr("Write"), this);
    gbWrite->setLayout(writeLayout);

    QGridLayout *readLayout = new QGridLayout;
    readLayout->addWidget(m_cbReadMode, 0, 0);
    readLayout->addWidget(m_cbReadScroll, 0, 1);
    readLayout->addWidget(m_cbDisplayRead, 0, 2);
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

    m_bDeleteAllMacroses->setFixedSize(15, 15);
    m_bDeleteAllMacroses->setIcon(QIcon(":/Resources/Delete.png"));
    m_cbAllIntervals->setFixedWidth(58);
    m_cbAllPeriods->setFixedWidth(50);
    m_bNewMacros->setToolTip(tr("New Macros"));
    m_bLoadMacroses->setToolTip(tr("Load Macroses"));
    m_bPause->setFixedWidth(38);
    m_bPause->setIcon(QIcon(":/Resources/Pause.png"));

    QHBoxLayout *hiddenAllCheck = new QHBoxLayout;
    hiddenAllCheck->addWidget(m_bDeleteAllMacroses);
    hiddenAllCheck->addWidget(m_cbAllIntervals);
    hiddenAllCheck->addWidget(m_cbAllPeriods);
    hiddenAllCheck->addWidget(m_sbAllDelays);
    hiddenAllCheck->addWidget(m_bNewMacros);
    hiddenAllCheck->addWidget(m_bLoadMacroses);
    hiddenAllCheck->addWidget(m_bPause);
    hiddenAllCheck->setSpacing(5);
    hiddenAllCheck->setContentsMargins(0, 0, 0, 0);

    scrollAreaLayout->addLayout(hiddenAllCheck);
    scrollAreaLayout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(scrollWidget);
    scrollArea->show();
    scrollArea->setVisible(true);
    scrollArea->setVerticalScrollBar(new QScrollBar(Qt::Vertical, scrollArea));
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
    scrollAreaLayout->addWidget(scrollArea);

    hiddenLayout->setSpacing(0);
    hiddenLayout->setContentsMargins(0, 0, 0, 0);
    hiddenLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    m_gbHiddenGroup->setLayout(scrollAreaLayout);
    m_gbHiddenGroup->setFixedWidth(300);

    QGridLayout *allLayouts = new QGridLayout;
    allLayouts->setSpacing(5);
    allLayouts->setContentsMargins(5, 5, 5, 5);
    allLayouts->addLayout(dataLayout, 0, 0);
    m_bHiddenGroup->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_bHiddenGroup->setFixedWidth(15);
    allLayouts->addWidget(m_bHiddenGroup, 0, 1);
    allLayouts->addWidget(m_gbHiddenGroup, 0, 2);
    widget = new QWidget(this);
    widget->setLayout(allLayouts);
    setCentralWidget(widget);
}

void MainWindow::connections()
{
    connect(actionPortConfigure, &QAction::triggered, comPortConfigure, &ComPortConfigure::show);
    connect(m_bReadLogClear, SIGNAL(clicked()), m_eLogRead, SLOT(clear()));
    connect(m_bWriteLogClear, SIGNAL(clicked()), m_eLogWrite, SLOT(clear()));
    connect(actionStartStop, &QAction::triggered, this, &MainWindow::startStop);
    connect(actionMacroses, &QAction::triggered, macrosesDockWidget, &QDockWidget::show);
    connect(m_bPause, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));
    connect(m_bSaveWriteLog, SIGNAL(clicked()), this, SLOT(saveWrite()));
    connect(m_bSaveReadLog, SIGNAL(clicked()), this, SLOT(saveRead()));
    connect(m_bHiddenGroup, SIGNAL(clicked()), this, SLOT(hiddenClicked()));
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
    connect(macroses, &Macroses::packageSended, this, static_cast<void (MainWindow::*)(const QByteArray &)>(&MainWindow::sendPackage));
}

void MainWindow::hiddenClicked()
{
    if(m_gbHiddenGroup->isHidden()) {
        m_gbHiddenGroup->show();
        m_bHiddenGroup->setText("<");

        if(!isMaximized()) {
            resize(width() + m_gbHiddenGroup->width() + 5, height());
        }
        setMinimumWidth(665 + m_gbHiddenGroup->width() + 5);

        return;
    }

    setMinimumWidth(665);
    m_gbHiddenGroup->hide();
    m_bHiddenGroup->setText(">");

    if(!isMaximized()) {
        resize(width() - m_gbHiddenGroup->width() - 5, height());
    }
}

void MainWindow::startStop()
{
    if(actionStartStop->text() == tr("Start")) {
        actionStartStop->setIcon(QIcon(":/Resources/Stop.png"));
        actionStartStop->setText(tr("Stop"));
        start();
    } else {
        actionStartStop->setIcon(QIcon(":/Resources/Play.png"));
        actionStartStop->setText(tr("Start"));
        stop();
    }
}

void MainWindow::sendPackage(const QByteArray &data)
{
    Macroses *m = qobject_cast<Macroses*>(sender());
    QPushButton *b = qobject_cast<QPushButton*>(sender());
    if(m == 0 && b == 0) {
        return;
    }
    bool macros = b == 0 ? true : false;
    sendPackage(data, macros);
}

void MainWindow::writeLogTimeout()
{
    writeLogFile.close();
    m_bRecordWriteLog->setChecked(false);
    logWrite = false;
    m_tWriteLog->stop();
}

void MainWindow::readLogTimeout()
{
    readLogFile.close();
    m_bRecordReadLog->setChecked(false);
    logRead = false;
    m_tReadLog->stop();
}

void MainWindow::startWriteLog(bool check)
{
    if(check) {
        m_tWriteLog->stop();
        writeLogFile.close();
        logWrite = false;
        m_bRecordWriteLog->setIcon(QIcon(":/Resources/Rec.png"));

        return;
    }
    QString path = fileDialog->getSaveFileName(this,
                                               tr("Save File"),
                                               QDir::currentPath() + "/(WRITE)" + QDateTime::currentDateTime().toString("yyyy.MM.dd_HH.mm.ss") + ".txt",
                                               tr("Log Files (*.txt)"));
    if(path.isEmpty()) {
        m_bRecordWriteLog->setChecked(false);

        return;
    }
    writeLogFile.setFileName(path);
    writeLogFile.open(QIODevice::WriteOnly);
    m_tWriteLog->start();
    logWrite = true;
    m_bRecordWriteLog->setIcon(QIcon(":/Resources/RecBlink.png"));
}

void MainWindow::startReadLog(bool check)
{
    if(!check) {
        m_tReadLog->stop();
        readLogFile.close();
        logRead = false;
        m_bRecordReadLog->setIcon(QIcon(":/Resources/Rec.png"));

        return;
    }

    QString path = fileDialog->getSaveFileName(this,
                                               tr("Save File"),
                                               QDir::currentPath() + "/(READ)_" + QDateTime::currentDateTime().toString("yyyy.MM.dd_HH.mm.ss") + ".txt",
                                               tr("Log Files (*.txt)"));
    if(path.isEmpty()) {
        m_bRecordReadLog->setChecked(false);

        return;
    }
    readLogFile.setFileName(path);
    readLogFile.open(QIODevice::WriteOnly);
    m_tReadLog->start();
    logRead = true;
    m_bRecordReadLog->setIcon(QIcon(":/Resources/RecBlink.png"));
}

void MainWindow::updateIntervalsList(bool add)
{
    MacrosWidget *m = qobject_cast<MacrosWidget*>(sender());
    if(m == 0) {
        return;
    }
    int index = macrosWidgets.indexOf(m);
    if(add) {
        indexesOfIntervals.append(index);
        std::sort(indexesOfIntervals.begin(), indexesOfIntervals.end(), qLess<int>());

        return;
    }

    indexesOfIntervals.removeOne(index);
}

void MainWindow::sendNextMacros()
{
    if(m_port->isOpen()) {
        MacrosWidget *m = macrosWidgets.at(indexesOfIntervals.at(currentIntervalIndex++));
        if(currentIntervalIndex >= indexesOfIntervals.size()) {
            currentIntervalIndex = 0;
        }
        sendPackage(m->getPackage(), true);
        m_tIntervalSending->start(m->getTime());
    }
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
    m_bPause->setEnabled(true);

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
    m_bPause->setEnabled(false);
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

void MainWindow::pause(bool check)
{
    if(check) {
        m_tIntervalSending->stop();
    } else if(sendCount != 0) {
        m_tIntervalSending->start();
    }

    QListIterator<MacrosWidget*> it(macrosWidgets);
    MacrosWidget* m = 0;
    while(it.hasNext()) {
        m = it.next();
        m->setEnabled(!check);
        m->setEnabled(!check);
    }
    m_cbAllIntervals->setEnabled(!check);
    m_cbAllPeriods->setEnabled(!check);
}

void MainWindow::received()
{
    readBuffer += m_port->readAll();
    int delayBetweenPackets = m_sbDelayBetweenPackets->value();
    if(delayBetweenPackets == 0) {
        delayBetweenPacketsEnded();

        return;
    }
    m_timerDelayBetweenPackets->start(delayBetweenPackets);
}

void MainWindow::singleSend()
{
    DataEncoder *dataEncoder = getEncoder(m_cbSendMode->currentIndex());
    dataEncoder->setData(m_leSendPackage->text(), " ");
    sendPackage(dataEncoder->encodedByteArray(), false);
}

void MainWindow::saveReadWriteLog(bool writeLog)
{
    QString defaultFileName = (writeLog ? "(WRITE)_" : "(READ)_") + QDateTime::currentDateTime().toString("yyyy.MM.dd_HH.mm.ss") + ".txt";
    QString fileName = fileDialog->getSaveFileName(this,
                                                   tr("Save File"),
                                                   QDir::currentPath() + "/" + defaultFileName,
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

void MainWindow::sendPackage(const QByteArray &writeData, bool macros)
{
    if(!m_port->isOpen() || writeData.isEmpty()) {
        return;
    }

    QByteArray modifiedData = writeData;
    if(!macros) {
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
        m_tTx->singleShot(BLINK_TIME_TX, this, SLOT(txNone()));
        m_tTx->setSingleShot(true);
    }
}

void MainWindow::displayWrittenData(const QByteArray &writeData)
{
    if (!m_cbDisplayWrite->isChecked()) {
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

    if(m_cbWriteScroll->isChecked()) {
        QScrollBar *sb = m_eLogWrite->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

DataEncoder *MainWindow::getEncoder(int mode)
{
    DataEncoder *dataEncoder = 0;
    switch(mode) {
    case HEX:
        dataEncoder = hexEncoder;
        break;
    case ASCII:
        dataEncoder = asciiEncoder;
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

// Перевод строки при приеме данных
// Срабатывает по таймеру m_timerDelayBetweenPackets
// Определяет отображаемую длину принятого пакета
void MainWindow::delayBetweenPacketsEnded()
{
    m_timerDelayBetweenPackets->stop();

    if(!m_tRx->isSingleShot()) {
        m_lRxCount->setStyleSheet("background: red");
        m_tRx->singleShot(BLINK_TIME_RX, this, SLOT(rxNone()));
        m_tRx->setSingleShot(true);
    }
    rxCount+=readBuffer.size();
    m_lRxCount->setText("Rx: " + QString::number(rxCount));

    if(m_cbDisplayRead->isChecked() || logRead) {
        DataEncoder *dataEncoder = getEncoder(m_cbReadMode->currentIndex());
        dataEncoder->setData(readBuffer);

        if(m_cbDisplayRead->isChecked()) {
            m_eLogRead->addLine(dataEncoder->encodedStringList().join(" "));
        }

        if(logRead) {
            QTextStream readStream(&readLogFile);
            readStream << dataEncoder->encodedStringList().join(" ") + "\n";
        }
    }
    readBuffer.clear();

    if(m_cbReadScroll->isChecked()) {
        QScrollBar *sb = m_eLogRead->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
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
    settings->remove("config");
    settings->setValue("config/size", size());
    settings->setValue("config/position", pos());
    settings->setValue("config/isMaximized", isMaximized());

    settings->setValue("config/write_mode", m_cbWriteMode->currentIndex());
    settings->setValue("config/read_mode", m_cbReadMode->currentIndex());
    settings->setValue("config/max_write_log_rows", m_eLogWrite->linesLimit());
    settings->setValue("config/max_read_log_rows", m_eLogRead->linesLimit());
    settings->setValue("config/write_display", m_cbDisplayWrite->isChecked());
    settings->setValue("config/read_display", m_cbDisplayRead->isChecked());
    settings->setValue("config/write_autoscroll", m_cbWriteScroll->isChecked());
    settings->setValue("config/read_autoscroll", m_cbReadScroll->isChecked());
    settings->setValue("config/write_log_timeout", m_tWriteLog->interval());
    settings->setValue("config/read_log_timeout", m_tReadLog->interval());

    comPortConfigure->saveSettings(settings);
    macroses->saveSettings(settings);

    settings->setValue("config/hidden_group_isHidden", m_gbHiddenGroup->isHidden());
    settings->setValue("config/all_delays", m_sbAllDelays->value());

    settings->setValue("config/single_send_interval", m_sbRepeatSendInterval->value());
    settings->setValue("config/mode", m_cbSendMode->currentIndex());
    settings->setValue("config/CR", m_chbCR->isChecked());
    settings->setValue("config/LF", m_chbLF->isChecked());
}

void MainWindow::loadSession()
{
    QSize size = settings->value("config/size").toSize();
    if(size.isValid()) {
        resize(size);
    }
    QPoint pos = settings->value("config/position").toPoint();
    if(!pos.isNull()) {
        move(pos);
    }
    if(settings->value("config/isMaximized").toBool()) {
        showMaximized();
    }

    m_eLogRead->setLinesLimit(settings->value("config/max_write_log_rows", 1000).toInt());
    m_eLogWrite->setLinesLimit(settings->value("config/max_read_log_rows", 1000).toInt());

    comPortConfigure->loadSettings(settings);
    macroses->loadSettings(settings);

    m_cbWriteMode->setCurrentIndex(settings->value("config/write_mode", 0).toInt());
    m_cbReadMode->setCurrentIndex(settings->value("config/read_mode", 0).toInt());
    m_cbWriteScroll->setChecked(settings->value("config/write_autoscroll", true).toBool());
    m_cbReadScroll->setChecked(settings->value("config/read_autoscroll", true).toBool());
    m_tWriteLog->setInterval(settings->value("config/write_log_timeout", 600000).toInt());
    m_tReadLog->setInterval(settings->value("config/read_log_timeout", 600000).toInt());

    m_gbHiddenGroup->setHidden(settings->value("config/hidden_group_isHidden", true).toBool());
    m_sbAllDelays->setValue(settings->value("config/all_delays", 50).toInt());

    m_sbRepeatSendInterval->setValue(settings->value("config/single_send_interval").toInt());
    m_chbCR->setChecked(settings->value("config/CR", false).toBool());
    m_chbLF->setChecked(settings->value("config/LF", false).toBool());
    if(!m_gbHiddenGroup->isHidden()) {
        m_bHiddenGroup->setText("<");
        setMinimumWidth(665 + m_gbHiddenGroup->width() + 5);
    }
    m_cbSendMode->setCurrentIndex(settings->value("config/mode", 0).toInt());
    m_cbDisplayWrite->setChecked(settings->value("config/write_display", true).toBool());
    m_cbDisplayRead->setChecked(settings->value("config/read_display", true).toBool());
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveSession();

    QWidget::closeEvent(e);
}
