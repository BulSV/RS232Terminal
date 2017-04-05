#ifndef SAVE_MACRO_H
#define SAVE_MACRO_H

#include <QString>
#include <QByteArray>

class SaveMacro
{
public:
    void setData(const QByteArray &data);
    bool save(const QString &fileName);
private:
    QString data;
};

#endif // SAVE_MACRO_H
