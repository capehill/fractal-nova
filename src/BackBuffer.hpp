#pragma once

#include <exec/types.h>

struct BitMap;

namespace fractalnova {

class BackBuffer
{
public:
    BackBuffer(uint32 width, uint32 height, BitMap* friendBitMap);
    ~BackBuffer();

    BitMap* Data() const;

private:
    BitMap* bitMap { nullptr };
};

} // fractalnova
