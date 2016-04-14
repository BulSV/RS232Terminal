#ifdef DEBUG
#include <QDebug>
#endif

#include "ComPort.h"
#include <QApplication>
#include <QTime>
#include <QDebug>

ComPort::ComPort(QSerialPort *port, QObject *parent)
    : QObject(parent),
      itsPort(port)
{
    itsReadData.clear();

    connect(itsPort, SIGNAL(readyRead()), this, SLOT(readData()));
}

void ComPort::readData()
{
    if(itsPort->openMode() != QSerialPort::WriteOnly) {
        itsReadData = itsPort->readAll();
        emit DataIsReaded(true);
        emit ReadedData(itsReadData);
    }
}

QByteArray ComPort::getReadData() const
{
    return itsReadData;
}

void ComPort::setWriteData(const QByteArray &data)
{
    itsWriteData = data;
    m_isDataWritten = false;
}

QByteArray ComPort::getWriteData() const
{
    return itsWriteData;
}

void ComPort::privateWriteData()
{
    if( itsPort->openMode() != QSerialPort::ReadOnly && itsPort->isOpen() ) {
        itsPort->write(itsWriteData);
        emit DataIsWrited(true);
        emit WritedData(itsWriteData);
        m_isDataWritten = true;
    }
}

void ComPort::writeData()
{
        privateWriteData();
}
