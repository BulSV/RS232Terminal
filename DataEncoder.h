#ifndef DATA_ENCODER_H
#define DATA_ENCODER_H

#include <QByteArray>
#include <QStringList>

class DataEncoder
{
public:
    virtual ~DataEncoder() {}
    virtual void setData(const QString &data, const QString &sep = "") = 0;
    const QByteArray &encodedByteArray() const;
    const QStringList &encodedStringList() const;
protected:
    QByteArray byteData;
    QStringList stringData;
};

#endif // DATA_ENCODER_H
