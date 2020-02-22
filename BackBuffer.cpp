#include "BackBuffer.hpp"

#include <proto/graphics.h>

#include <stdexcept>

namespace fractalnova {

BackBuffer::BackBuffer(uint32 width, uint32 height, BitMap* friendBitMap)
{
    bitMap = IGraphics->AllocBitMapTags(width, height, 0,
        BMATags_Friend, friendBitMap,
        BMATags_Displayable, TRUE,
        TAG_DONE);

    printf("Bitmap size %lu * %lu\n", width, height);

    if (!bitMap) {
        throw std::runtime_error("Failed to allocate bitmap");
    }
}

BackBuffer::~BackBuffer()
{
    if (bitMap) {
        IGraphics->FreeBitMap(bitMap);
        bitMap = nullptr;
    }
}

} // fractalnova

