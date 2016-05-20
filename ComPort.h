#ifndef ONEPACKET_H
#define ONEPACKET_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>

class ComPort: public QObject
{
    Q_OBJECT
public:
    ComPort(QSerialPort *port, QObject *parent = 0);
    QByteArray getReadData() const;
    void setWriteData(const QByteArray &data);
    QByteArray getWriteData() const;

public slots:
    void writeData();

signals:
    void DataIsReaded(bool);
    void ReadedData(QByteArray);
    void DataIsWrited(bool);
    void WritedData(QByteArray);

private slots:
    void readData();

private:
    QSerialPort *itsPort;
    QByteArray itsReadData;
    QByteArray itsWriteData;
    bool m_isDataWritten;
    void privateWriteData();
};

#endif // ONEPACKET_H
