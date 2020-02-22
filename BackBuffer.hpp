#pragma once

#include <exec/types.h>

struct BitMap;

namespace fractalnova {

struct BackBuffer
{
    BackBuffer(uint32 width, uint32 height, BitMap* friendBitMap);
    ~BackBuffer();

    BitMap* bitMap { nullptr };
};

} // fractalnova
