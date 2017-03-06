#ifndef HEX_ENCODER_H
#define HEX_ENCODER_H

#include "DataEncoder.h"

class HexEncoder : public DataEncoder
{
public:
    virtual ~HexEncoder() {}
    virtual void setData(const QString &data, const QString &sep = "");
};

#endif // HEX_ENCODER_H
