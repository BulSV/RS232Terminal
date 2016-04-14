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
    virtual QString getReadedData() const;
signals:

public slots:
    virtual void writeData();
    virtual void resetProtocol();
private slots:
    void readData(bool isReaded);
private:
    ComPort *itsComPort;

    QString itsWriteData;
    QString itsReadData;

    QTimer *m_resend;
    int m_numResends;
    int m_currentResend;

    // преобразует word в byte
    int wordToInt(QByteArray ba);
    QByteArray intToByteArray(const int &value, const int &numBytes);
};

#endif // READSENSORPROTOCOL_H
