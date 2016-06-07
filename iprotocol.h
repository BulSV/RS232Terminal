#ifndef IPROTOCOL_H
#define IPROTOCOL_H

#include <QObject>
#include <QMultiMap>
#include <QString>

class IProtocol : public QObject
{
    Q_OBJECT
public:
    explicit IProtocol(QObject *parent = 0) : QObject(parent) {}
    virtual ~IProtocol() {}
    virtual void setDataToWrite(const QString &data) = 0;
    virtual QByteArray getReadedData() const = 0;
public slots:
    virtual void writeDataNow(QByteArray ba) = 0;
    virtual void writeData() = 0;
signals:
    void DataIsReaded(bool);
    void DataIsWrited(bool);
};

#endif // IPROTOCOL_H
