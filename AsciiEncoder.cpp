#include "AsciiEncoder.h"

void AsciiEncoder::setData(const QString &data, const QString &sep)
{
    Q_UNUSED(sep);
    DataEncoder::setData(data, sep);

    int count = data.size();
    for(int i = 0; i < count; i++) {
        int n = data.at(i).toLatin1();
        byteData.append(static_cast<char>(n));
    }
    stringListData.append(data);
}

void AsciiEncoder::setData(const QByteArray &data)
{
    DataEncoder::setData(data);

    QString dataString;
    for(int i = 0; i < byteData.size(); ++i) {
        dataString.append(QString(byteData.at(i)));
    }
    stringListData.append(dataString);
}
