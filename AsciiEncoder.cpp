#include "AsciiEncoder.h"

void AsciiEncoder::setData(const QString &data, const QString &sep)
{
    Q_UNUSED(sep);

    int count = data.size();
    for(int i = 0; i < count; i++) {
        int n = data.at(i).toLatin1();
        byteData.append(static_cast<char>(n));
        stringData.append(QString::number(n, 10));
    }
}
