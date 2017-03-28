#ifndef ASCII_ENCODER_H
#define ASCII_ENCODER_H

#include "DataEncoder.h"

class AsciiEncoder : public DataEncoder
{
public:
    virtual ~AsciiEncoder() {}
    virtual void setData(const QString &data, const QString &sep = "");
    virtual void setData(const QByteArray &data);
};

#endif // ASCII_ENCODER_H
