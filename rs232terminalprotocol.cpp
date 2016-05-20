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

QString RS232TerminalProtocol::getReadedData() const
{
    return itsReadData;
}

void RS232TerminalProtocol::readData(bool isReaded)
{
    itsReadData.clear();

    if(isReaded) {
        QByteArray ba;

        ba = itsComPort->getReadData();

        itsReadData = QString(ba.toHex());

        emit DataIsReaded(true);
    } else {
        emit DataIsReaded(false);
    }
}

void RS232TerminalProtocol::writeData()
{
    QByteArray ba;

    ba.append(itsWriteData.toInt());

    itsComPort->setWriteData(ba);
    itsComPort->writeData();
}


int RS232TerminalProtocol::wordToInt(QByteArray ba)
{
    int temp = ba[0];
    if(temp < 0)
    {
        temp += 0x100;
        temp *= 0x100;
    }
    else
        temp = ba[0]*0x100;

    int i = ba[1];
    if(i < 0)
    {
        i += 0x100;
        temp += i;
    }
    else
        temp += ba[1];

    return temp;
}

QByteArray RS232TerminalProtocol::intToByteArray(const int &value, const int &numBytes)
{
    QByteArray ba;

    for(int i = numBytes - 1; i > - 1; --i) {
        ba.append((value >> 8*i) & 0xFF);
    }

    return ba;
}
