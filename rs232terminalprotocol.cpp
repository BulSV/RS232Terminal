#include "RS232TerminalProtocol.h"

RS232TerminalProtocol::RS232TerminalProtocol(ComPort *comPort, QObject *parent) :
    IProtocol(parent),
    itsComPort(comPort)
{
    connect(itsComPort, SIGNAL(DataIsReaded(bool)), this, SLOT(readData(bool)));
    connect(itsComPort, SIGNAL(DataIsWrited(bool)), this, SIGNAL(DataIsWrited(bool)));
}

void RS232TerminalProtocol::setDataToWrite(const QString &data)
{
    itsWriteData = data;
}

QByteArray RS232TerminalProtocol::getReadedData() const
{
    return itsReadData;
}

void RS232TerminalProtocol::readData(bool isReaded)
{
    itsReadData.clear();

    if(isReaded) {
        QByteArray ba;

        ba = itsComPort->getReadData();

        itsReadData = ba;

        emit DataIsReaded(true);
    } else {
        emit DataIsReaded(false);
    }
}

void RS232TerminalProtocol::writeDataNow(QByteArray ba)
{
    ba.append(itsWriteData.toInt());

    itsComPort->setWriteData(ba);
    itsComPort->writeData();
}

void RS232TerminalProtocol::writeData()
{
    QByteArray ba;

    ba.append(itsWriteData.toInt());

    itsComPort->setWriteData(ba);
    itsComPort->writeData();
}
