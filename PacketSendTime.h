#ifndef PACKET_SEND_TIME_H
#define PACKET_SEND_TIME_H

#include <QSerialPort>

class PacketSendTime
{
public:
    PacketSendTime(const QSerialPort *port);
    double calculateTime(int bytesCount);
private:
    const QSerialPort *port;
};

#endif // PACKET_SEND_TIME_H
