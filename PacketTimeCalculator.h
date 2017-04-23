#ifndef PACKET_TIME_CALCULATOR_H
#define PACKET_TIME_CALCULATOR_H

#include <QSerialPort>

class PacketTimeCalculator
{
public:
    PacketTimeCalculator(const QSerialPort *port);
    bool isValid() const;
    double calculateTime(int bytesCount);
private:
    const QSerialPort *port;
};

#endif // PACKET_TIME_CALCULATOR_H
