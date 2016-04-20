#include "Dialog.h"
#include <QGridLayout>
#include <QString>
#include <QApplication>
#include <QSerialPortInfo>
#include "rs232terminalprotocol.h"

#define BLINKTIMETX 200
#define BLINKTIMERX 500

#define SEPARATOR "$"

Dialog::Dialog(QString title, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , m_cbPort(new QComboBox(this))
    , m_cbBaud(new QComboBox(this))
    , m_bStart(new QPushButton("Start", this))
    , m_bStop(new QPushButton("Stop", this))
    , m_bWriteLogClear(new QPushButton("Clear", this))
    , m_bReadLogClear(new QPushButton("Clear", this))
    , m_bOffsetLeft(new QPushButton("<--", this))
    , m_bOffsetRight(new QPushButton("-->", this))
    , m_lTx(new QLabel("        Tx", this))
    , m_lRx(new QLabel("        Rx", this))
    , m_sbBytesCount(new QSpinBox(this))
    , m_eLogRead(new QTextEdit(this))
    , m_eLogWrite(new QTextEdit(this))
    , m_sbRepeatSendCount(new QSpinBox(this))
    , m_leSendPackage(new QLineEdit(this))
    , m_bSendPackage(new QPushButton("Send", this))
    , m_BlinkTimeTxNone(new QTimer(this))
    , m_BlinkTimeRxNone(new QTimer(this))
    , m_BlinkTimeTxColor(new QTimer(this))
    , m_BlinkTimeRxColor(new QTimer(this))
    , m_Port(new QSerialPort(this))
    , m_ComPort(new ComPort(m_Port))
    , m_Protocol(new RS232TerminalProtocol(m_ComPort, this))

{
    setWindowTitle(title);
    view();
    connections();

    m_eLogRead->setReadOnly(true);
    m_eLogWrite->setReadOnly(true);

    m_bStop->setEnabled(false);

    m_sbRepeatSendCount->setRange(1, 100000);

    m_sbBytesCount->setRange(0, 32);
    m_sbBytesCount->setValue(6);
    DisplayByteIndex = 0;

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
}

void Dialog::view()
{
    QGridLayout *configLayout = new QGridLayout;
    configLayout->addWidget(new QLabel("<img src=':/Resources/elisat.png' height='40' width='150'/>", this), 0, 0, 2, 2, Qt::AlignCenter);
    configLayout->addWidget(new QLabel("Port:", this), 2, 0);
    configLayout->addWidget(m_cbPort, 2, 1);
    configLayout->addWidget(new QLabel("Baud:", this), 3, 0);
    configLayout->addWidget(m_cbBaud, 3, 1);
    configLayout->addWidget(m_bStart, 4, 0);
    configLayout->addWidget(m_bStop, 4, 1);
    configLayout->addWidget(m_lTx, 5, 0);
    configLayout->addWidget(m_lRx, 5, 1);
    configLayout->addWidget(new QLabel("Bytes count:", this), 6, 0);
    configLayout->addWidget(m_sbBytesCount, 6, 1);
    configLayout->addWidget(m_bOffsetLeft, 7, 0);
    configLayout->addWidget(m_bOffsetRight, 7, 1);
    configLayout->setSpacing(5);

    QGridLayout *sendPackageLayout = new QGridLayout;
    sendPackageLayout->addWidget(m_leSendPackage, 0, 0);
    sendPackageLayout->addWidget(m_sbRepeatSendCount, 0, 1);
    sendPackageLayout->addWidget(m_bSendPackage, 0, 2);

    QGridLayout *labelWriteLayout = new QGridLayout;
    labelWriteLayout->addWidget(new QLabel("Write:", this), 0, 0);
    labelWriteLayout->addWidget(m_bWriteLogClear, 0, 1);

    QGridLayout *labelReadLayout = new QGridLayout;
    labelReadLayout->addWidget(new QLabel("Read:", this), 0, 0);
    labelReadLayout->addWidget(m_bReadLogClear, 0, 1);

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

    connect(m_bStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(m_bStop, SIGNAL(clicked()), this, SLOT(stop()));

    connect(m_bSendPackage, SIGNAL(clicked()), this, SLOT(sendPackage()));

    connect(m_Protocol, SIGNAL(DataIsReaded(bool)), this, SLOT(received(bool)));

    connect(m_BlinkTimeTxColor, SIGNAL(timeout()), this, SLOT(colorIsTx()));
    connect(m_BlinkTimeRxColor, SIGNAL(timeout()), this, SLOT(colorIsRx()));
    connect(m_BlinkTimeTxNone, SIGNAL(timeout()), this, SLOT(colorTxNone()));
    connect(m_BlinkTimeRxNone, SIGNAL(timeout()), this, SLOT(colorRxNone()));
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
        m_lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
        m_lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
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
    //m_Protocol->resetProtocol();
}

void Dialog::received(bool isReceived)
{
    if(isReceived) {
        if(!m_BlinkTimeRxColor->isActive() && !m_BlinkTimeRxNone->isActive()) {
            m_BlinkTimeRxColor->start();
            m_lRx->setStyleSheet("background: green; font: bold; font-size: 10pt");
        }
        setDisplayData(m_Protocol->getReadedData(), m_eLogRead);
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

void Dialog::colorIsTx()
{
    m_lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_BlinkTimeTxColor->stop();
    m_BlinkTimeTxNone->start();
}

void Dialog::sendPackage()
{
    if (m_Port->isOpen())
    {   QString out;
        QStringList byteList = m_leSendPackage->text().split(SEPARATOR, QString::SkipEmptyParts);
        for (int i = 0; i < m_sbRepeatSendCount->value(); i++)
        {
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
            setDisplayData(out, m_eLogWrite);
        }
    }
}

void Dialog::setDisplayData(QString string, QTextEdit *edit)
{
    for (int i = 2; !(i >= string.length()); i += 3)
    {
        string.insert(i, SEPARATOR);
    }

    QStringList listOfBytes = string.split(SEPARATOR);
    for (int i = 0; i < listOfBytes.count(); i++)
    {
        DisplayBuffer += listOfBytes[i];
        DisplayByteIndex++;

        if (DisplayByteIndex == m_sbBytesCount->value())
        {
            displayData(edit);
        }
        else
            DisplayBuffer += " ";
    }
    if (DisplayByteIndex < m_sbBytesCount->value() && !DisplayBuffer.isEmpty())
    {
        displayData(edit);
    }
}

void Dialog::displayData(QTextEdit *edit)
{
    edit->insertPlainText(DisplayBuffer.toUpper() + "\n");
    DisplayBuffer.clear();
    DisplayByteIndex = 0;

    QTextCursor cursor =  edit->textCursor();
    cursor.movePosition(QTextCursor::End);
    edit->setTextCursor(cursor);
}

void Dialog::clearReadLog()
{
    m_eLogRead->clear();
}

void Dialog::clearWriteLog()
{
    m_eLogWrite->clear();
}

void Dialog::colorTxNone()
{
    m_BlinkTimeTxNone->stop();
}

Dialog::~Dialog()
{

}
