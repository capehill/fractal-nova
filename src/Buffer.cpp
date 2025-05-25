/*
Copyright (C) 2020-2025 Juha Niemimäki

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

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
    logging::Debug("Create Buffer of %zu bytes", size);

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
