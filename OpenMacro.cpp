#include <QTextStream>
#include <QFile>
#include <QList>

#include "OpenMacro.h"

bool OpenMacro::open(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)) {
        return false;
    }
    data.clear();

    QTextStream in(&file);
    QList<QString> dataStringList = in.readLine().split(" ");
    int dataSize = dataStringList.size();
    bool ok;
    for(int i = 0; i < dataSize; ++i) {
        data.append(static_cast<char>(dataStringList.at(i).toInt(&ok, 16)));
    }

    return true;
}

const QByteArray &OpenMacro::getData() const
{
    return data;
}
