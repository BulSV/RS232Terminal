#ifndef OPEN_MACROS_H
#define OPEN_MACROS_H

#include <QString>
#include <QByteArray>

class OpenMacros
{
public:
    bool open(const QString &fileName);
    const QByteArray &getData() const;
private:
    QByteArray data;
};

#endif // OPEN_MACROS_H
