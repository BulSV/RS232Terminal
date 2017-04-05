#include <QTextStream>
#include <QFile>

#include "SaveMacro.h"

void SaveMacro::setData(const QByteArray &data)
{
    if(data.isEmpty()) {
        return;
    }
    this->data.clear();
    int dataSize = data.size();
    int currentByte = 0;
    for(int i = 0; i < dataSize; ++i) {
        currentByte = static_cast<unsigned char>(data.at(i));
        if(currentByte < 0x10) {
            this->data.append("0");
        }
        this->data.append(QString::number(currentByte, 16).toUpper());
        this->data.append(" ");
    }
    this->data.chop(1);
}

bool SaveMacro::save(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return false;
    }
    QTextStream out(&file);
    out << data;
    file.close();

    return true;
}
