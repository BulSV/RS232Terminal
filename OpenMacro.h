#ifndef OPEN_MACRO_H
#define OPEN_MACRO_H

#include <QString>
#include <QByteArray>

class OpenMacro
{
public:
    bool open(const QString &fileName);
    const QByteArray &getData() const;
private:
    QByteArray data;
};

#endif // OPEN_MACRO_H
