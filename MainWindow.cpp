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

MainWindow::MainWindow(QString title, QWidget *parent)
    : QMainWindow(parent)
    , toolBar(new QToolBar(this))
    , actionPortConfigure(new QAction(tr("Port configure"), this))
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
    , m_bStart(new QPushButton(tr("Start"), this))
    , m_bStop(new QPushButton(tr("Stop"), this))
    , m_bPause(new QPushButton(tr("Pause"), this))
    , m_bWriteLogClear(new QPushButton(tr("Clear"), this))
    , m_bReadLogClear(new QPushButton(tr("Clear"), this))
    , m_bSaveWriteLog(new QPushButton(tr("Save"), this))
    , m_bSaveReadLog(new QPushButton(tr("Save"), this))
    , m_bHiddenGroup(new QPushButton(">", this))
    , m_bDeleteAllMacroses(new QPushButton(this))
    , m_bNewMacros(new QPushButton(this))
    , m_bLoadMacroses(new QPushButton(this))
    , m_bRecordWriteLog(new QPushButton(this))
    , m_bRecordReadLog(new QPushButton(this))
    , m_bSendPackage(new QPushButton(tr("Send"), this))
    , m_lTx(new QLabel("Tx", this))
    , m_lRx(new QLabel("Rx", this))
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

    setMinimumWidth(665);

    m_port->setReadBufferSize(1);

    txCount = 0;
    rxCount = 0;
    logWrite = false;
    logRead = false;
    sendCount = 0;
    currentIntervalIndex = -1;

    m_eLogRead->displayTime("mm:ss.zzz");
    m_eLogRead->setReadOnly(true);
    m_eLogWrite->displayTime("mm:ss.zzz");
    m_eLogWrite->setReadOnly(true);

    comPortConfigure->setWindowTitle(tr("Port configure"));
    comPortConfigure->setModal(true);

    m_bNewMacros->setStyleSheet("border-image: url(:/Resources/add.png) stretch;");
    m_bLoadMacroses->setStyleSheet("border-image: url(:/Resources/open.png) stretch;");
    m_bNewMacros->setFixedSize(20, 20);
    m_bLoadMacroses->setFixedSize(20, 20);
    m_bRecordReadLog->setIcon(QIcon(":/Resources/startRecToFile.png"));
    m_bRecordWriteLog->setIcon(QIcon(":/Resources/startRecToFile.png"));
    m_bRecordWriteLog->setCheckable(true);
    m_bRecordReadLog->setCheckable(true);
    m_bSendPackage->setCheckable(true);
    m_bPause->setCheckable(true);
    m_bStop->setEnabled(false);
    m_bPause->setEnabled(false);
    m_sbRepeatSendInterval->setRange(0, 100000);
    m_sbDelayBetweenPackets->setRange(0, 10);
    m_sbDelayBetweenPackets->setValue(10);
    m_sbAllDelays->setRange(0, 999999);

    m_lTxCount->setStyleSheet("border-style: outset; border-width: 1px; border-color: black;");
    m_lRxCount->setStyleSheet("border-style: outset; border-width: 1px; border-color: black;");

    m_lTx->setStyleSheet("font: bold; font-size: 10pt; qproperty-alignment: AlignCenter");
    m_lRx->setStyleSheet("font: bold; font-size: 10pt; qproperty-alignment: AlignCenter");

    QStringList buffer;
    buffer << "HEX" << "ASCII" << "DEC";
    m_cbSendMode->addItems(buffer);
    m_cbReadMode->addItems(buffer);
    m_cbWriteMode->addItems(buffer);

    QDir dir;
    if(!dir.exists(dir.currentPath() + "/Macros")) {
        dir.mkpath(dir.currentPath() + "/Macros");
    }
    fileDialog->setDirectory(dir.currentPath() + "/Macros");
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setNameFilter(tr("Terminal Macros File (*.tmf)"));

    loadSession();
}

void MainWindow::view()
{
    QList<QAction*> actions;
    actions << actionPortConfigure;
    addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->setMovable(false);
    toolBar->addActions(actions);

    QGridLayout *configLayout = new QGridLayout;
    configLayout->addWidget(new QLabel("<img src=':/Resources/elisat.png' height='40' width='160'/>", this), 0, 0, 2, 2, Qt::AlignCenter);
    configLayout->addWidget(m_lTx, 2, 0);
    configLayout->addWidget(m_lRx, 2, 1);
    configLayout->addWidget(new QLabel(tr("Delay between\npackets, ms:"), this), 3, 0);
    configLayout->addWidget(m_sbDelayBetweenPackets, 3, 1);
    configLayout->addWidget(m_bStart, 4, 0);
    configLayout->addWidget(m_bStop, 4, 1);
    configLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 5, 0);
    configLayout->addWidget(m_lTxCount, 14, 0);
    configLayout->addWidget(m_lRxCount, 14, 1);
    configLayout->setSpacing(5);

    QHBoxLayout *sendPackageLayout = new QHBoxLayout;
    sendPackageLayout->addWidget(new QLabel(tr("Mode:"), this));
    sendPackageLayout->addWidget(m_cbSendMode);
    sendPackageLayout->addWidget(m_leSendPackage);
    sendPackageLayout->addWidget(m_chbCR);
    sendPackageLayout->addWidget(m_chbLF);
    sendPackageLayout->addWidget(m_sbRepeatSendInterval);
    sendPackageLayout->addWidget(m_bSendPackage);

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
    m_bDeleteAllMacroses->setStyleSheet("border-image: url(:/Resources/del.png) stretch;");
    m_cbAllIntervals->setFixedWidth(58);
    m_cbAllPeriods->setFixedWidth(50);
    m_bNewMacros->setToolTip(tr("New Macros"));
    m_bLoadMacroses->setToolTip(tr("Load Macroses"));
    m_bPause->setFixedWidth(38);

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
    allLayouts->addLayout(configLayout, 0, 0);
    allLayouts->addLayout(dataLayout, 0, 1);
    m_bHiddenGroup->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_bHiddenGroup->setFixedWidth(15);
    allLayouts->addWidget(m_bHiddenGroup, 0, 2);
    allLayouts->addWidget(m_gbHiddenGroup, 0, 3);
    widget = new QWidget(this);
    widget->setLayout(allLayouts);
    setCentralWidget(widget);
}

void MainWindow::connections()
{
    connect(actionPortConfigure, &QAction::triggered, comPortConfigure, &ComPortConfigure::show);
    connect(m_bReadLogClear, SIGNAL(clicked()), m_eLogRead, SLOT(clear()));
    connect(m_bWriteLogClear, SIGNAL(clicked()), m_eLogWrite, SLOT(clear()));
    connect(m_bStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(m_bStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(m_bPause, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));
    connect(m_bSaveWriteLog, SIGNAL(clicked()), this, SLOT(saveWrite()));
    connect(m_bSaveReadLog, SIGNAL(clicked()), this, SLOT(saveRead()));
    connect(m_bHiddenGroup, SIGNAL(clicked()), this, SLOT(hiddenClicked()));
    connect(m_bRecordWriteLog, SIGNAL(toggled(bool)), this, SLOT(startWriteLog(bool)));
    connect(m_bRecordReadLog, SIGNAL(toggled(bool)), this, SLOT(startReadLog(bool)));
    connect(m_bDeleteAllMacroses, SIGNAL(clicked()), this, SLOT(deleteAllMacroses()));
    connect(m_bSendPackage, SIGNAL(toggled(bool)), this, SLOT(startSending(bool)));
    connect(m_leSendPackage, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(m_leSendPackage, &QLineEdit::returnPressed, [this](){startSending();});
    connect(m_bNewMacros, SIGNAL(clicked()), this, SLOT(addMacros()));
    connect(m_bLoadMacroses, SIGNAL(clicked()), this, SLOT(openDialog()));
    connect(m_cbAllIntervals, SIGNAL(toggled(bool)), this, SLOT(checkAllMacroses()));
    connect(m_cbAllPeriods, SIGNAL(toggled(bool)), this, SLOT(checkAllMacroses()));
    connect(m_sbAllDelays, SIGNAL(valueChanged(int)), this, SLOT(changeAllDelays(int)));
    connect(m_tIntervalSending, SIGNAL(timeout()), this, SLOT(sendInterval()));
    connect(m_tSend, SIGNAL(timeout()), this, SLOT(singleSend()));
    connect(m_timerDelayBetweenPackets, &QTimer::timeout, this, &MainWindow::delayBetweenPacketsEnded);
    connect(m_tWriteLog, SIGNAL(timeout()), this, SLOT(writeLogTimeout()));
    connect(m_tReadLog, SIGNAL(timeout()), this, SLOT(readLogTimeout()));
    connect(m_port, SIGNAL(readyRead()), this, SLOT(received()));
}

void MainWindow::changeAllDelays(int time)
{
    QListIterator<MacrosWidget*> it(macrosWidgets);
    while(it.hasNext()) {
        it.next()->setTime(time);
    }
}

void MainWindow::checkAllMacroses()
{
    if(m_cbAllIntervals->isChecked() && m_cbAllPeriods->isChecked()) {
        QCheckBox *tempCheckBox = dynamic_cast<QCheckBox*>(sender());
        if(tempCheckBox) {
            tempCheckBox->setChecked(false);
        }

        return;
    }
    QListIterator<MacrosWidget*> it(macrosWidgets);
    MacrosWidget *m = 0;
    while(it.hasNext()) {
        m = it.next();
        if(m_cbAllIntervals->isChecked()) {
            m->setCheckedInterval(true);

            continue;
        }
        if(m_cbAllPeriods->isChecked()) {
            m->setCheckedPeriod(true);

            continue;
        }
        m->setCheckedInterval(false);
        m->setCheckedPeriod(false);
    }
}

void MainWindow::deleteAllMacroses()
{
    int button = QMessageBox::question(this, tr("Warning"),
                                       tr("Delete ALL macroses?"),
                                       QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes) {
        QListIterator<MacrosWidget*> it(macrosWidgets);
        MacrosWidget *m = 0;
        while(it.hasNext()) {
            m = it.next();
            m->setCheckedInterval(false);
            m->setCheckedPeriod(false);
            hiddenLayout->removeWidget(m);
            disconnect(m, &MacrosWidget::deleted, this, &MainWindow::deleteMacros);
            disconnect(m, &MacrosWidget::packageSended, this, static_cast<void (MainWindow::*)(const QByteArray &)>(&MainWindow::sendPackage));
            disconnect(m, &MacrosWidget::intervalChecked, this, &MainWindow::updateIntervalsList);
            disconnect(m, &MacrosWidget::movedUp, this, &MainWindow::moveMacrosUp);
            disconnect(m, &MacrosWidget::movedDown, this, &MainWindow::moveMacrosDown);
            delete m;
            m = 0;
        }
        macrosWidgets.clear();
    }
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

void MainWindow::openDialog()
{
    QStringList fileNames;
    if(fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }

    QListIterator<QString> it(fileNames);
    while(it.hasNext()) {
        addMacros();
        macrosWidgets.last()->openMacrosFile(it.next());
    }
}

void MainWindow::addMacros()
{
    MacrosWidget *macrosWidget = new MacrosWidget(this);
    macrosWidgets.append(macrosWidget);
    hiddenLayout->insertWidget(hiddenLayout->count() - 1, macrosWidget);

    connect(macrosWidget, &MacrosWidget::deleted, this, &MainWindow::deleteMacros);
    connect(macrosWidget, &MacrosWidget::packageSended, this, static_cast<void (MainWindow::*)(const QByteArray &)>(&MainWindow::sendPackage));
    connect(macrosWidget, &MacrosWidget::intervalChecked, this, &MainWindow::updateIntervalsList);
    connect(macrosWidget, &MacrosWidget::movedUp, this, &MainWindow::moveMacrosUp);
    connect(macrosWidget, &MacrosWidget::movedDown, this, &MainWindow::moveMacrosDown);
}

void MainWindow::moveMacros(MacrosWidget *macrosWidget, MacrosMoveDirection direction)
{
    if(!macrosWidget) {
        return;
    }

    int macrosIndex = macrosWidgets.indexOf(macrosWidget);

    QVBoxLayout* tempLayout = qobject_cast<QVBoxLayout*>(macrosWidget->parentWidget()->layout());
    int index = tempLayout->indexOf(macrosWidget);

    if(direction == MoveUp) {
        if(macrosIndex == 0) {
            return;
        }

        macrosWidgets.swap(macrosIndex, macrosIndex - 1);
        --index;
    } else {
        if(macrosIndex == macrosWidgets.size() - 1) {
            return;
        }

        macrosWidgets.swap(macrosIndex, macrosIndex + 1);
        ++index;
    }
    tempLayout->removeWidget(macrosWidget);
    tempLayout->insertWidget(index, macrosWidget);
}

void MainWindow::moveMacrosUp()
{
    moveMacros(qobject_cast<MacrosWidget*>(sender()), MoveUp);
}

void MainWindow::moveMacrosDown()
{
    moveMacros(qobject_cast<MacrosWidget*>(sender()), MoveDown);
}

void MainWindow::deleteMacros()
{
    MacrosWidget *m = qobject_cast<MacrosWidget*>(sender());
    if(m == 0) {
        return;
    }
    m->setCheckedInterval(false);
    m->setCheckedPeriod(false);
    macrosWidgets.removeOne(m);
    hiddenLayout->removeWidget(m);
    disconnect(m, &MacrosWidget::deleted, this, &MainWindow::deleteMacros);
    disconnect(m, &MacrosWidget::packageSended, this, static_cast<void (MainWindow::*)(const QByteArray &)>(&MainWindow::sendPackage));
    disconnect(m, &MacrosWidget::intervalChecked, this, &MainWindow::updateIntervalsList);
    disconnect(m, &MacrosWidget::movedUp, this, &MainWindow::moveMacrosUp);
    disconnect(m, &MacrosWidget::movedDown, this, &MainWindow::moveMacrosDown);
    delete m;
}

void MainWindow::sendPackage(const QByteArray &data)
{
    MacrosWidget *m = qobject_cast<MacrosWidget*>(sender());
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
        m_bRecordWriteLog->setIcon(QIcon(":/Resources/startRecToFile.png"));

        return;
    }
    QString path = fileDialog->getSaveFileName(this,
                                               tr("Save File"),
                                               QDir::currentPath() + "/(WRITE)" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".txt",
                                               tr("Log Files (*.txt)"));
    if(path.isEmpty()) {
        m_bRecordWriteLog->setChecked(false);

        return;
    }
    writeLogFile.setFileName(path);
    writeLogFile.open(QIODevice::WriteOnly);
    m_tWriteLog->start();
    logWrite = true;
    m_bRecordWriteLog->setIcon(QIcon(":/Resources/startRecToFileBlink.png"));
}

void MainWindow::startReadLog(bool check)
{
    if(!check) {
        m_tReadLog->stop();
        readLogFile.close();
        logRead = false;
        m_bRecordReadLog->setIcon(QIcon(":/Resources/startRecToFile.png"));

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
    m_bRecordReadLog->setIcon(QIcon(":/Resources/startRecToFileBlink.png"));
}

void MainWindow::textChanged(const QString &text)
{
    if(!text.isEmpty() && m_bStop->isEnabled()) {
        m_bSendPackage->setEnabled(true);
        m_bSendPackage->setCheckable(true);

        return;
    }
    m_bSendPackage->setEnabled(false);
    m_bSendPackage->setCheckable(false);
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
        m_lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
        m_lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");

        return;
    }

    m_port->setFlowControl(QSerialPort::NoFlowControl);

    m_bStart->setEnabled(false);
    m_bStop->setEnabled(true);
    m_bPause->setEnabled(true);
    m_lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");

//    sendNextMacros(); // FIXME
}

void MainWindow::stop()
{
    m_port->close();
    m_lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_bStop->setEnabled(false);
    m_bPause->setEnabled(false);
    m_bStart->setEnabled(true);
    m_bSendPackage->setChecked(false);
    m_tSend->stop();
    m_timerDelayBetweenPackets->stop();
    m_tIntervalSending->stop();
    txCount = 0;
    m_lTxCount->setText("Tx: 0");
    rxCount = 0;
    m_lRxCount->setText("Rx: 0");
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
        m_lTx->setStyleSheet("background: green; font: bold; font-size: 10pt");
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
    case DEC:
        dataEncoder = decEncoder;
    }

    return dataEncoder;
}

// Перевод строки при приеме данных
// Срабатывает по таймеру m_timerDelayBetweenPackets
// Определяет отображаемую длину принятого пакета
void MainWindow::delayBetweenPacketsEnded()
{
    m_timerDelayBetweenPackets->stop();

    if(!m_tRx->isSingleShot()) {
        m_lRx->setStyleSheet("background: red; font: bold; font-size: 10pt");
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
    m_lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_tRx->singleShot(BLINK_TIME_RX, this, SLOT(rxHold()));
}

void MainWindow::txNone()
{
    m_lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
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

    settings->setValue("config/hidden_group_isHidden", m_gbHiddenGroup->isHidden());
    settings->setValue("config/all_delays", m_sbAllDelays->value());

    settings->setValue("config/single_send_interval", m_sbRepeatSendInterval->value());
    settings->setValue("config/mode", m_cbSendMode->currentIndex());
    settings->setValue("config/CR", m_chbCR->isChecked());
    settings->setValue("config/LF", m_chbLF->isChecked());

    settings->remove("macroses");
    int macrosIndex = 1;
    QListIterator<MacrosWidget*> it(macrosWidgets);
    MacrosWidget *m = 0;
    while(it.hasNext()) {
        m = it.next();
        m->saveSettings(settings, macrosIndex);
        macrosIndex++;
    }
    settings->setValue("macroses/count", macrosIndex - 1);
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

    int MacrosesCount = settings->value("macroses/count", 0).toInt();
    for(int macrosIndex = 1; macrosIndex <= MacrosesCount; ++macrosIndex) {
        addMacros();
        macrosWidgets.last()->loadSettings(settings, macrosIndex);
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveSession();

    QWidget::closeEvent(e);
}
