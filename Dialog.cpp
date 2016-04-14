#include "Dialog.h"
#include <QGridLayout>
#include <QApplication>
#include <QSerialPortInfo>
#include "rs232terminalprotocol.h"

#define BLINKTIMETX 200 // ms
#define BLINKTIMERX 500 // ms

Dialog::Dialog(QString title, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , m_cbPort(new QComboBox(this))
    , m_cbBaud(new QComboBox(this))
    , m_bStart(new QPushButton("Start", this))
    , m_bStop(new QPushButton("Stop", this))
    , m_lTx(new QLabel("        Tx", this))
    , m_lRx(new QLabel("        Rx", this))
    , m_sbBytesCount(new QSpinBox(this))
    , m_eLogRead(new QTextEdit(this))
    , m_eLogWrite(new QTextEdit(this))
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

    m_bStop->setEnabled(false);

    m_sbBytesCount->setRange(0, 32);
    m_sbBytesCount->setSingleStep(2);
    m_sbBytesCount->setValue(6);
    BytesCount = 6;

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
    configLayout->setSpacing(5);

    QGridLayout *logLayout = new QGridLayout;
    logLayout->addWidget(new QLabel("Read:", this), 0, 1);
    logLayout->addWidget(new QLabel("Write:", this), 0, 0);
    logLayout->addWidget(m_eLogRead, 1, 1);
    logLayout->addWidget(m_eLogWrite, 1, 0);

    QGridLayout *allLayouts = new QGridLayout;
    allLayouts->addLayout(configLayout, 0, 0);
    allLayouts->addLayout(logLayout, 0, 1);
    setLayout(allLayouts);
}

void Dialog::connections()
{
    connect(m_bStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(m_bStop, SIGNAL(clicked()), this, SLOT(stop()));

    connect(m_Protocol, SIGNAL(DataIsReaded(bool)), this, SLOT(received(bool)));

    connect(m_BlinkTimeTxColor, SIGNAL(timeout()), this, SLOT(colorIsTx()));
    connect(m_BlinkTimeRxColor, SIGNAL(timeout()), this, SLOT(colorIsRx()));
    connect(m_BlinkTimeTxNone, SIGNAL(timeout()), this, SLOT(colorTxNone()));
    connect(m_BlinkTimeRxNone, SIGNAL(timeout()), this, SLOT(colorRxNone()));

    //connect(m_sbBytesCount, SIGNAL(valueChanged(int)), this, SLOT(changeBytesCount()));
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
        displayData(m_Protocol->getReadedData());
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

//void Dialog::changeBytesCount()
//{
//
//}

void Dialog::displayData(QString string)
{
    for (int i = 0; i < string.length(); i++)
    {
        m_eLogRead->append(string.mid(0,2));
        string.remove(0, 2);
    }
}

void Dialog::colorTxNone()
{
    m_BlinkTimeTxNone->stop();
}

Dialog::~Dialog()
{

}
