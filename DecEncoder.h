#ifndef DEC_ENCODER_H
#define DEC_ENCODER_H

#include <DataEncoder.h>

class DecEncoder : public DataEncoder
{
public:
    virtual~DecEncoder() {}
    virtual void setData(const QString &data, const QString &sep = "");
    virtual void setData(const QByteArray &data);
};

#endif // DEC_ENCODER_H
