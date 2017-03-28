#include "DataEncoder.h"

void DataEncoder::setData(const QString &data, const QString &sep)
{
    Q_UNUSED(data);
    Q_UNUSED(sep);

    clearData();
}

void DataEncoder::setData(const QByteArray &data)
{
    clearData();

    byteData = data;
}

const QByteArray &DataEncoder::encodedByteArray() const
{
    return byteData;
}

const QStringList &DataEncoder::encodedStringList() const
{
    return stringListData;
}

void DataEncoder::clearData()
{
    byteData.clear();
    stringListData.clear();
}
