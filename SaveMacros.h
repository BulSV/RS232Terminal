#ifndef SAVE_MACROS_H
#define SAVE_MACROS_H

#include <QString>
#include <QByteArray>

class SaveMacros
{
public:
    void setData(const QByteArray &data);
    bool save(const QString &fileName);
private:
    QString data;
};

#endif // SAVE_MACROS_H
