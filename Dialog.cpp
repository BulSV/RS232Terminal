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

#define MAXLOGROWSCOUNT 10000

#define SEPARATOR "$"

Dialog::Dialog(QString title, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , m_cbPort(new QComboBox(this))
    , m_cbBaud(new QComboBox(this))
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
    m_cbEchoMode->setEnabled(false);
    m_abSendPackage->setEnabled(false);
    m_eLogRead->setReadOnly(true);
    m_eLogWrite->setReadOnly(true);

    m_bStop->setEnabled(false);

    m_sbRepeatSendInterval->setRange(0, 100000);
    m_sbEchoInterval->setRange(0, 100000);

    m_sbBytesCount->setRange(0, 64);
    m_sbBytesCount->setValue(0);

    m_lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_BlinkTimeTxNone->setInterval(BLINKTIMETX);
    m_BlinkTimeRxNone->setInterval(BLINKTIMERX);
    m_BlinkTimeTxColor->setInterval(BLINKTIMETX);
    m_BlinkTimeRxColor->setInterval(BLINKTIMERX);

    QStringList portsNames;

    foreach(QSerialPortInfo portsAvailable, QSerialPortInfo::availablePorts())
    {
        portsNames << portsAvailable.portName();
    }
    m_cbPort->addItems(portsNames);

    QStringList bauds;
    bauds << "921600" << "115200" << "57600" << "38400" << "19200" << "9600" << "4800" << "2400" << "1200";
    m_cbBaud->addItems(bauds);

    m_cbPort->setCurrentIndex(settings->value("config/port").toInt());
    m_cbBaud->setCurrentIndex(settings->value("config/baud").toInt());

    m_eLogRead->setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");
    m_eLogWrite->setStyleSheet("background: black; color: lightgreen; font-family: \"Lucida Console\"; font-size: 10pt");
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
    configLayout->addWidget(m_bStart, 5, 0);
    configLayout->addWidget(m_bStop, 5, 1);
    configLayout->addWidget(m_lTx, 6, 0);
    configLayout->addWidget(m_lRx, 6, 1);
    configLayout->addWidget(m_cbEchoMode, 7, 0);
    configLayout->addWidget(m_sbEchoInterval, 7, 1);
    configLayout->addWidget(new QLabel("Bytes count:", this), 8, 0);
    configLayout->addWidget(m_sbBytesCount, 8, 1);
    configLayout->addWidget(m_bOffsetLeft, 9, 0);
    configLayout->addWidget(m_bOffsetRight, 9, 1);
    configLayout->setSpacing(5);

    QGridLayout *sendPackageLayout = new QGridLayout;
    sendPackageLayout->addWidget(m_leSendPackage, 0, 0);
    sendPackageLayout->addWidget(m_sbRepeatSendInterval, 0, 1);
    sendPackageLayout->addWidget(m_abSendPackage, 0, 2);

    QGridLayout *labelWriteLayout = new QGridLayout;
    labelWriteLayout->addWidget(new QLabel("Write:", this), 0, 0);
    labelWriteLayout->addWidget(m_cbWriteScroll, 0, 1);
    labelWriteLayout->addWidget(m_bWriteLogClear, 0, 2);

    QGridLayout *labelReadLayout = new QGridLayout;
    labelReadLayout->addWidget(new QLabel("Read:", this), 0, 0);
    labelReadLayout->addWidget(m_cbReadScroll, 0, 1);
    labelReadLayout->addWidget(m_bReadLogClear, 0, 2);

    QGridLayout *dataLayout = new QGridLayout;
    dataLayout->addLayout(labelWriteLayout, 0, 0);
    dataLayout->addLayout(labelReadLayout, 0, 1);
    dataLayout->addWidget(m_eLogRead, 1, 1);
    dataLayout->addWidget(m_eLogWrite, 1, 0);
    dataLayout->addLayout(sendPackageLayout, 2, 0, 2, 0);

    QGridLayout *allLayouts = new QGridLayout;
    allLayouts->addLayout(configLayout, 0, 0);
    allLayouts->addLayout(dataLayout, 0, 1);
    setLayout(allLayouts);
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

    connect(m_Protocol, SIGNAL(DataIsReaded(bool)), this, SLOT(received(bool)));

    connect(m_tSend, SIGNAL(timeout()), this, SLOT(sendSingle()));
    connect(m_tEcho, SIGNAL(timeout()), this, SLOT(echo()));

    connect(macroWindow, SIGNAL(WriteMacros(const QString)), this, SLOT(macrosRecieved(const QString)));

    connect(m_BlinkTimeTxColor, SIGNAL(timeout()), this, SLOT(colorIsTx()));
    connect(m_BlinkTimeRxColor, SIGNAL(timeout()), this, SLOT(colorIsRx()));
    connect(m_BlinkTimeTxNone, SIGNAL(timeout()), this, SLOT(colorTxNone()));
    connect(m_BlinkTimeRxNone, SIGNAL(timeout()), this, SLOT(colorRxNone()));
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
    settings->setValue("config/port", m_cbPort->currentIndex());

    if(m_Port->open(QSerialPort::ReadWrite))
    {
        settings->setValue("config/baud", m_cbBaud->currentIndex());
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
        default:
            m_Port->setBaudRate(QSerialPort::Baud921600);
            break;
        }

        m_Port->setDataBits(QSerialPort::Data8);
        m_Port->setParity(QSerialPort::NoParity);
        m_Port->setFlowControl(QSerialPort::NoFlowControl);

        m_bStart->setEnabled(false);
        m_bStop->setEnabled(true);
        m_cbPort->setEnabled(false);
        m_cbBaud->setEnabled(false);
        m_abSendPackage->setEnabled(true);
        m_cbEchoMode->setEnabled(true);
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
    m_abSendPackage->setEnabled(false);
    m_abSendPackage->setChecked(false);
    m_tSend->stop();
    m_tEcho->stop();
    m_cbEchoMode->setEnabled(false);
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
                m_lTx->setStyleSheet("background: green; font: bold; font-size: 10pt");
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

void Dialog::displayReadData(QString string)
{
    logReadRowsCount++;
    for (int i = 2; !(i >= string.length()); i += 3)
    {
        string.insert(i, SEPARATOR);
    }

    if (m_cbEchoMode->isChecked())
    {
        echoData.append(" " + string);
        if (!m_tEcho->isActive())
        {
            m_tEcho->setInterval(m_sbEchoInterval->value());
            m_tEcho->start();
        }
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
        m_eLogRead->insertPlainText(out.toUpper() + "\n");
        listOfBytes.clear();
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
            }
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
               m_eLogRead->insertPlainText(restDisplay.toUpper() + "\n");
               restBytes.clear();
            }
            DisplayReadBuffer.clear();
            listOfBytes.clear();
        }
    }
    if (m_cbReadScroll->checkState())
    {
        QTextCursor cursor =  m_eLogRead->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_eLogRead->setTextCursor(cursor);
    }
    if (logReadRowsCount >= MAXLOGROWSCOUNT)
    {
        m_eLogRead->clear();
        logReadRowsCount = 0;
    }
}

void Dialog::displayWriteData(QString string)
{
    logWriteRowsCount++;
    m_eLogWrite->insertPlainText(string.replace("$", " ").toUpper() + "\n");

    if (m_cbWriteScroll->checkState())
    {
        QTextCursor cursor =  m_eLogWrite->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_eLogWrite->setTextCursor(cursor);
    }
    if (logWriteRowsCount >= MAXLOGROWSCOUNT)
    {
        m_eLogWrite->clear();
        logWriteRowsCount = 0;
}
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
}

void Dialog::clearWriteLog()
{
    m_eLogWrite->clear();
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

void Dialog::closeEvent(QCloseEvent *e)
{
    settings->setValue("config/height", this->height());
    settings->setValue("config/width", this->width());
    settings->setValue("config/m_height", macroWindow->height());
    settings->setValue("config/m_width", macroWindow->width());

    macroWindow->saveSession();
    macroWindow->close();
    e->accept();
}
