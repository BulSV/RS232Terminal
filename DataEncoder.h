#ifndef DATA_ENCODER_H
#define DATA_ENCODER_H

#include <QByteArray>
#include <QStringList>

class DataEncoder
{
public:
    virtual ~DataEncoder() {}
    virtual void setData(const QString &data, const QString &sep = "");
    virtual void setData(const QByteArray &data);
    const QByteArray &encodedByteArray() const;
    const QStringList &encodedStringList() const;
protected:
    QByteArray byteData;
    QStringList stringListData;
private:
    void clearData();
};

#endif // DATA_ENCODER_H
