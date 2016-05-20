#include <QDebug>
#include "Dialog.h"
#include <QGridLayout>
#include <QString>
#include <QApplication>
#include <QLineEdit>
#include <QSerialPortInfo>
#include "rs232terminalprotocol.h"
#include <QCloseEvent>
#include <QMouseEvent>

#define BLINKTIMETX 200
#define BLINKTIMERX 500

#define MAXLOGROWSCOUNT 1000

#define SEPARATOR "$"

Dialog::Dialog(QString title, QWidget *parent)
    : QMainWindow(parent, Qt::WindowCloseButtonHint)
    , widget(new QWidget(this))
    , m_cbReadDisp(new QCheckBox("Display log", this))
    , m_cbWriteDisp(new QCheckBox("Display log", this))
    , m_cbPort(new QComboBox(this))
    , m_cbBaud(new QComboBox(this))
    , m_cbBits(new QComboBox(this))
    , m_cbParity(new QComboBox(this))
    , m_cbStopBits(new QComboBox(this))
    , m_bStart(new QPushButton("Start", this))
    , m_bStop(new QPushButton("Stop", this))
    , m_bWriteLogClear(new QPushButton("Clear", this))
    , m_bReadLogClear(new QPushButton("Clear", this))
    , m_bOffsetLeft(new QPushButton("<---", this))
    , m_bOffsetRight(new QPushButton("--->", this))
    , m_bShowMacroForm(new QPushButton("Macro", this))
    , m_lTx(new QLabel("        Tx", this))
    , m_lRx(new QLabel("        Rx", this))
    , m_sbBytesCount(new QSpinBox(this))
    , m_eLogRead(new MyPlainTextEdit())
    , m_eLogWrite(new MyPlainTextEdit())
    , m_sbRepeatSendInterval(new QSpinBox(this))
    , m_leSendPackage(new QLineEdit(this))
    , m_abSendPackage(new QPushButton("Send", this))
    , m_cbEchoMode(new QCheckBox("Echo mode", this))
    , m_sbEchoInterval(new QSpinBox(this))
    , m_cbReadScroll(new QCheckBox("Auto scrolling", this))
    , m_cbWriteScroll(new QCheckBox("Auto scrolling", this))
    , m_BlinkTimeTxNone(new QTimer(this))
    , m_BlinkTimeRxNone(new QTimer(this))
    , m_BlinkTimeTxColor(new QTimer(this))
    , m_BlinkTimeRxColor(new QTimer(this))
    , m_tSend(new QTimer(this))
    , m_tEcho(new QTimer(this))
    , m_Port(new QSerialPort(this))
    , m_ComPort(new ComPort(m_Port))
    , m_Protocol(new RS232TerminalProtocol(m_ComPort, this))
    , settings(new QSettings("settings.ini", QSettings::IniFormat))
    , macroWindow(new MacroWindow(QString::fromUtf8("RS232 Terminal - Macro")))

{
    setWindowTitle(title);    
    resize(settings->value("config/width", 750).toInt(), settings->value("config/height", 300).toInt());
    view();
    connections();

    Offset = 0;
    logReadRowsCount = 0;
    logWriteRowsCount = 0;

    m_abSendPackage->setCheckable(true);
    m_abSendPackage->setEnabled(false);
    m_sbRepeatSendInterval->setEnabled(false);
    m_eLogRead->setReadOnly(true);
    m_eLogWrite->setReadOnly(true);
    m_bStop->setEnabled(false);
    m_cbPort->setEditable(true);
    m_sbRepeatSendInterval->setRange(0, 100000);
    m_sbEchoInterval->setRange(0, 100000);
    m_sbBytesCount->setRange(0, 64);

    m_lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_BlinkTimeTxNone->setInterval(BLINKTIMETX);
    m_BlinkTimeRxNone->setInterval(BLINKTIMERX);
    m_BlinkTimeTxColor->setInterval(BLINKTIMETX);
    m_BlinkTimeRxColor->setInterval(BLINKTIMERX);

    m_eLogRead->setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");
    m_eLogWrite->setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");

    QStringList portsNames;
    foreach(QSerialPortInfo portsAvailable, QSerialPortInfo::availablePorts())
    {
        portsNames << portsAvailable.portName();
    }
    m_cbPort->addItems(portsNames);

    QStringList buffer;
    buffer << "921600" << "115200" << "57600" << "38400" << "19200" << "9600" << "4800" << "2400" << "1200";
    m_cbBaud->addItems(buffer);
    buffer.clear();
    buffer << "5" << "6" << "7" << "8";
    m_cbBits->addItems(buffer);
    buffer.clear();
    buffer << "None" << "Odd" << "Even" << "Mark" << "Space";
    m_cbParity->addItems(buffer);
    buffer.clear();
    buffer << "1" << "1.5" << "2";
    m_cbStopBits->addItems(buffer);
    loadSession();
}

void Dialog::view()
{
    QGridLayout *configLayout = new QGridLayout;
    configLayout->addWidget(new QLabel("<img src=':/Resources/elisat.png' height='40' width='150'/>", this), 0, 0, 2, 2, Qt::AlignCenter);
    configLayout->addWidget(m_bShowMacroForm, 2, 0, 1, 2);
    configLayout->addWidget(new QLabel("Port:", this), 3, 0);
    configLayout->addWidget(m_cbPort, 3, 1);
    configLayout->addWidget(new QLabel("Baud:", this), 4, 0);
    configLayout->addWidget(m_cbBaud, 4, 1);
    configLayout->addWidget(new QLabel("Data bits:", this), 5, 0);
    configLayout->addWidget(m_cbBits, 5, 1);
    configLayout->addWidget(new QLabel("Paruty:", this), 6, 0);
    configLayout->addWidget(m_cbParity, 6, 1);
    configLayout->addWidget(new QLabel("Stop bits:", this), 7, 0);
    configLayout->addWidget(m_cbStopBits, 7, 1);
    configLayout->addWidget(m_bStart, 8, 0);
    configLayout->addWidget(m_bStop, 8, 1);
    configLayout->addWidget(m_lTx, 9, 0);
    configLayout->addWidget(m_lRx, 9, 1);
    configLayout->addWidget(m_cbEchoMode, 10, 0);
    configLayout->addWidget(m_sbEchoInterval, 10, 1);
    configLayout->addWidget(new QLabel("Bytes count:", this), 11, 0);
    configLayout->addWidget(m_sbBytesCount, 11, 1);
    configLayout->addWidget(m_bOffsetLeft, 12, 0);
    configLayout->addWidget(m_bOffsetRight, 12, 1);
    configLayout->setSpacing(5);

    QGridLayout *sendPackageLayout = new QGridLayout;
    sendPackageLayout->addWidget(m_leSendPackage, 0, 0);
    sendPackageLayout->addWidget(m_sbRepeatSendInterval, 0, 1);
    sendPackageLayout->addWidget(m_abSendPackage, 0, 2);

    QGridLayout *labelWriteLayout = new QGridLayout;
    labelWriteLayout->addWidget(new QLabel("Write:", this), 0, 0);
    labelWriteLayout->addWidget(m_cbWriteDisp, 0, 1);
    labelWriteLayout->addWidget(m_cbWriteScroll, 0, 2);
    labelWriteLayout->addWidget(m_bWriteLogClear, 0, 3);

    QGridLayout *labelReadLayout = new QGridLayout;
    labelReadLayout->addWidget(new QLabel("Read:", this), 0, 0);
    labelReadLayout->addWidget(m_cbReadDisp, 0, 1);
    labelReadLayout->addWidget(m_cbReadScroll, 0, 2);
    labelReadLayout->addWidget(m_bReadLogClear, 0, 3);

    QGridLayout *dataLayout = new QGridLayout;
    dataLayout->addLayout(labelWriteLayout, 0, 0);
    dataLayout->addLayout(labelReadLayout, 0, 1);
    dataLayout->addWidget(m_eLogRead, 1, 1);
    dataLayout->addWidget(m_eLogWrite, 1, 0);
    dataLayout->addLayout(sendPackageLayout, 2, 0, 2, 0);

    QGridLayout *allLayouts = new QGridLayout;
    allLayouts->addLayout(configLayout, 0, 0);
    allLayouts->addLayout(dataLayout, 0, 1);
    widget->setLayout(allLayouts);
    setCentralWidget(widget);
}

void Dialog::connections()
{
    connect(m_bReadLogClear, SIGNAL(clicked()), this, SLOT(clearReadLog()));
    connect(m_bWriteLogClear, SIGNAL(clicked()), this, SLOT(clearWriteLog()));
    connect(m_bShowMacroForm, SIGNAL(clicked()), this, SLOT(showMacroWindow()));
    connect(m_bOffsetLeft, SIGNAL(clicked()), this, SLOT(offsetDec()));
    connect(m_bOffsetRight, SIGNAL(clicked()), this, SLOT(offsetInc()));
    connect(m_bStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(m_bStop, SIGNAL(clicked()), this, SLOT(stop()));

    connect(m_abSendPackage, SIGNAL(toggled(bool)), this, SLOT(startSending(bool)));
    connect(m_cbEchoMode, SIGNAL(toggled(bool)), this, SLOT(cleanEchoBuffer(bool)));
    connect(m_leSendPackage, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(m_Protocol, SIGNAL(DataIsReaded(bool)), this, SLOT(received(bool)));

    connect(m_tSend, SIGNAL(timeout()), this, SLOT(sendSingle()));
    connect(m_tEcho, SIGNAL(timeout()), this, SLOT(echo()));

    connect(macroWindow, SIGNAL(WriteMacros(const QString)), this, SLOT(macrosRecieved(const QString)));

    connect(m_BlinkTimeTxColor, SIGNAL(timeout()), this, SLOT(colorIsTx()));
    connect(m_BlinkTimeRxColor, SIGNAL(timeout()), this, SLOT(colorIsRx()));
    connect(m_BlinkTimeTxNone, SIGNAL(timeout()), this, SLOT(colorTxNone()));
    connect(m_BlinkTimeRxNone, SIGNAL(timeout()), this, SLOT(colorRxNone()));
}

void Dialog::textChanged(QString text)
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

void Dialog::cleanEchoBuffer(bool check)
{
    m_tEcho->stop();
    if (!check)
        echoData.clear();
}

void Dialog::start()
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
        macroWindow->start();
    }
    else
    {
        m_lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
        m_lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    }
}

void Dialog::stop()
{
    m_Port->close();
    macroWindow->stop();
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
    Offset = 0;
    m_Protocol->resetProtocol();
}

void Dialog::macrosRecieved(const QString &str)
{
    sendPackage(str);
}

void Dialog::received(bool isReceived)
{
    if(isReceived) {
        if(!m_BlinkTimeRxColor->isActive() && !m_BlinkTimeRxNone->isActive()) {
            m_BlinkTimeRxColor->start();
            m_lRx->setStyleSheet("background: green; font: bold; font-size: 10pt");
        }
        displayReadData(m_Protocol->getReadedData());
    }
}

void Dialog::colorIsRx()
{
    m_lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_BlinkTimeRxColor->stop();
    m_BlinkTimeRxNone->start();
}

void Dialog::colorRxNone()
{
    m_BlinkTimeRxNone->stop();
}

void Dialog::sendSingle()
{
    sendPackage(m_leSendPackage->text());
}

void Dialog::echo()
{
    sendPackage(echoData.takeFirst());
    if (echoData.isEmpty())
    {
        m_tEcho->stop();
    }
}

void Dialog::startSending(bool checked)
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

void Dialog::sendPackage(QString string)
{
    if (m_Port->isOpen())
    {
        QString out;
        QStringList byteList = string.split(SEPARATOR, QString::SkipEmptyParts);
        if (!byteList.isEmpty())
        {
            if(!m_BlinkTimeTxColor->isActive() && !m_BlinkTimeTxNone->isActive()) {
                m_BlinkTimeTxColor->start();
                m_lTx->setStyleSheet("background: red; font: bold; font-size: 10pt");
            }
            foreach (QString s, byteList)
            {
                bool ok;
                m_Protocol->setDataToWrite(QString::number(s.toInt(&ok, 16)));
                if (ok)
                {
                    m_Protocol->writeData();
                    out += s;
                }
            }
            displayWriteData(string);
            m_tSend->setInterval(m_sbRepeatSendInterval->value());
        }
    }
}

void Dialog::scrollToBot(QCheckBox *cb, MyPlainTextEdit *te)
{
    if (cb->checkState())
    {
        QTextCursor cursor =  te->textCursor();
        cursor.movePosition(QTextCursor::End);
        te->setTextCursor(cursor);
    }
}

void Dialog::displayReadData(QString string)
{
    if (m_cbReadDisp->isChecked())
        logReadRowsCount++;
    for (int i = 2; !(i >= string.length()); i += 3)
    {
        string.insert(i, SEPARATOR);
    }

    if (!m_sbBytesCount->value())
    {
        QString out;
        listOfBytes = string.split(SEPARATOR);
        for (int i = 0; i < listOfBytes.count(); i++)
        {
            out += listOfBytes[i];
            if (i != listOfBytes.count()-1)
                out += " ";
        }
        if (m_cbReadDisp->isChecked())
            m_eLogRead->insertPlainText(out.toUpper() + "\n");
        listOfBytes.clear();

        if (m_cbEchoMode->isChecked())
        {
            echoData.append(" " + string);
            if (!m_tEcho->isActive())
            {
                m_tEcho->setInterval(m_sbEchoInterval->value());
                m_tEcho->start();
            }
        }
    }
    else
    {        
        listOfBytes += string.split(SEPARATOR);

        if (listOfBytes.count() < m_sbBytesCount->value())
            return;
        else
        {
            listOfBytes = doOffset(listOfBytes);
            if (listOfBytes.count() > m_sbBytesCount->value())
            {
                for (int i = listOfBytes.count() - 1; i >= m_sbBytesCount->value(); i--)
                {
                    restBytes.prepend(listOfBytes.takeAt(i));
                }
            }
            for (int i = 0; i < listOfBytes.count(); i++)
            {
                DisplayReadBuffer += listOfBytes[i];
                if (i != listOfBytes.count()-1)
                    DisplayReadBuffer += " ";
                qApp->processEvents();
            }
            if (m_cbReadDisp->isChecked())
                m_eLogRead->insertPlainText(DisplayReadBuffer.toUpper() + "\n");
            if (restBytes.count())
            {
               QString restDisplay;
               for (int i = 0; i < restBytes.count(); i++)
               {
                   restDisplay += restBytes[i];
                   if (i != restBytes.count()-1)
                       restDisplay += " ";
               }
               if (m_cbReadDisp->isChecked())
                m_eLogRead->insertPlainText(restDisplay.toUpper() + "\n");
            }
            if (m_cbEchoMode->isChecked())
            {
                echoData.append(" " +listOfBytes.join(SEPARATOR)+SEPARATOR+restBytes.join(SEPARATOR));
                if (!m_tEcho->isActive())
                {
                    m_tEcho->setInterval(m_sbEchoInterval->value());
                    m_tEcho->start();
                }
            }
            restBytes.clear();
            DisplayReadBuffer.clear();
            listOfBytes.clear();
        }
    }
    if (logReadRowsCount >= MAXLOGROWSCOUNT)
    {
        m_eLogRead->delLine(0);
    }
    scrollToBot(m_cbReadScroll, m_eLogRead);
}

void Dialog::displayWriteData(QString string)
{   
    if (m_cbWriteDisp->isChecked())
    {
        logWriteRowsCount++;
        m_eLogWrite->insertPlainText(string.replace("$", " ").toUpper() + "\n");
    }
    if (logWriteRowsCount >= MAXLOGROWSCOUNT)
    {
        m_eLogWrite->delLine(0);
    }
    scrollToBot(m_cbWriteScroll, m_eLogWrite);
}

QStringList Dialog::doOffset(QStringList list)
{
    if (Offset && abs(Offset) < list.count())
    {
        if (Offset > 0)
        {
            for (int i = 0; i < abs(Offset); i++)
                list.insert(0, list.takeLast());
        }
        if (Offset < 0)
        {
            for (int i = 0; i < abs(Offset); i++)
                list.append(list.takeFirst());
        }
    }
    else
        Offset = 0;
    return list;
}

void Dialog::clearReadLog()
{
    m_eLogRead->clear();
    logReadRowsCount = 0;
}

void Dialog::clearWriteLog()
{
    m_eLogWrite->clear();
    logWriteRowsCount = 0;
}

void Dialog::colorIsTx()
{
    m_lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_BlinkTimeTxColor->stop();
    m_BlinkTimeTxNone->start();
}

void Dialog::colorTxNone()
{
    m_BlinkTimeTxNone->stop();
}

void Dialog::offsetDec()
{
    Offset--;
}

void Dialog::offsetInc()
{
    Offset++;
}

void Dialog::showMacroWindow()
{
    macroWindow->show();
}

void Dialog::saveSession()
{
    settings->setValue("config/height", height());
    settings->setValue("config/width", width());
    settings->setValue("config/position", pos());

    settings->setValue("config/port", m_cbPort->currentText());
    settings->setValue("config/baud", m_cbBaud->currentIndex());
    settings->setValue("config/data_bits", m_cbBits->currentIndex());
    settings->setValue("config/parity", m_cbParity->currentIndex());
    settings->setValue("config/stop_bits", m_cbStopBits->currentIndex());
    settings->setValue("config/bytes_count", m_sbBytesCount->value());
    settings->setValue("config/echo", m_cbEchoMode->checkState());
    settings->setValue("config/echo_interval", m_sbEchoInterval->value());
    settings->setValue("config/single_send_interval", m_sbRepeatSendInterval->value());
    settings->setValue("config/write_autoscroll", m_cbWriteScroll->isChecked());
    settings->setValue("config/read_autoscroll", m_cbReadScroll->isChecked());
    settings->setValue("config/write_disp", m_cbWriteDisp->isChecked());
    settings->setValue("config/read_disp", m_cbReadDisp->isChecked());
}

void Dialog::loadSession()
{
    const QPoint pos = settings->value ("config/position").toPoint();
        if (!pos.isNull())
            move (pos);

    m_cbPort->setCurrentText(settings->value("config/port").toString());
    m_cbBaud->setCurrentIndex(settings->value("config/baud").toInt());
    m_cbBits->setCurrentIndex(settings->value("config/data_bits").toInt());
    m_cbParity->setCurrentIndex(settings->value("config/parity").toInt());
    m_cbStopBits->setCurrentIndex(settings->value("config/stop_bits").toInt());
    m_sbBytesCount->setValue(settings->value("config/bytes_count").toInt());
    m_cbEchoMode->setChecked(settings->value("config/echo").toBool());
    m_sbEchoInterval->setValue(settings->value("config/echo_interval").toInt());
    m_sbRepeatSendInterval->setValue(settings->value("config/single_send_interval").toInt());
    m_cbWriteScroll->setChecked(settings->value("config/write_autoscroll", true).toBool());
    m_cbReadScroll->setChecked(settings->value("config/read_autoscroll", true).toBool());
    m_cbWriteDisp->setChecked(settings->value("config/write_disp", true).toBool());
    m_cbReadDisp->setChecked(settings->value("config/read_disp", true).toBool());
}

void Dialog::closeEvent(QCloseEvent *e)
{
    saveSession();
    macroWindow->saveSession();
    macroWindow->close();
    e->accept();
}
