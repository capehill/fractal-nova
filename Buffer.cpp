#include "Buffer.hpp"
#include "Logger.hpp"

#include <proto/exec.h>

#include <stdexcept>
#include <cstring>

namespace fractalnova {
namespace {
    constexpr uint32 head { 0x13579bdf };
    constexpr uint32 tail { 0x02468ace };
    constexpr char freeChar { 0xcc };
}

Buffer::Buffer(const size_t size): size(size)
{
    logging::Debug("Creating buffer of %zu bytes", size);

    data = static_cast<char *>(IExec->AllocVecTags(size + 8, AVT_ClearWithValue, 0, TAG_DONE));

    if (!data) {
        throw std::runtime_error("Failed to allocate memory");
    }

    start = data + 4;

    *reinterpret_cast<uint32 *>(data) = head;
    *reinterpret_cast<uint32 *>(start + size) = tail;
}

Buffer::~Buffer()
{
    if (data) {
        if (*reinterpret_cast<uint32*>(data) != head) {
            logging::Error("Buffer (size %u) head mashed", size);
        }

        if (*reinterpret_cast<uint32*>(start + size) != tail) {
            logging::Error("Buffer (size %u) tail mashed", size);
        }

        memset(data, freeChar, size + 8);

        IExec->FreeVec(data);
        data = nullptr;
    }
}

char* Buffer::Data() const
{
    return start;
}

unsigned Buffer::Size() const
{
    return size;
}

} // fractalnova

