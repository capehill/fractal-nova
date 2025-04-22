#pragma once

#include <exec/types.h>

namespace fractalnova {

class Buffer
{
public:
    explicit Buffer(unsigned size);
    ~Buffer();

    char* Data() const;
    unsigned Size() const;

protected:
    char* data { nullptr };
    char* start { nullptr };

    unsigned size { 0 };
};

} // fractalnova
