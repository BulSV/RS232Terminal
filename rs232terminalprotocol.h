#ifndef READSENSORPROTOCOL_H
#define READSENSORPROTOCOL_H

#include "IProtocol.h"
#include "ComPort.h"

#include <QTimer>

class RS232TerminalProtocol : public IProtocol
{
    Q_OBJECT
public:
    explicit RS232TerminalProtocol(ComPort *comPort, QObject *parent = 0);
    virtual void setDataToWrite(const QString &data);
    virtual QByteArray getReadedData() const;

private:
    ComPort *itsComPort;

    QString itsWriteData;
    QByteArray itsReadData;

    // преобразует word в byte
    int wordToInt(QByteArray ba);
    QByteArray intToByteArray(const int &value, const int &numBytes);

public slots:
    virtual void writeData();

private slots:
    void readData(bool isReaded);
};

#endif // READSENSORPROTOCOL_H
