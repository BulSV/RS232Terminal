#include "DataEncoder.h"

const QByteArray &DataEncoder::encodedByteArray() const
{
    return byteData;
}

const QStringList &DataEncoder::encodedStringList() const
{
    return stringData;
}
