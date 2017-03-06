#include "DecEncoder.h"

void DecEncoder::setData(const QString &data, const QString &sep)
{
    QStringList byteList = data.split(sep, QString::SkipEmptyParts);
    int count = byteList.size();
    for(int i = 0; i < count; i++) {
        bool ok;
        int n = byteList.at(i).toInt(&ok, 10);
        byteData.append(static_cast<char>(n));
        stringData.append(QString::number(n));
    }
}
