#pragma once

#include <exec/types.h>
#include <devices/timer.h>

namespace fractalnova {

struct Timer
{
    Timer();
    ~Timer();

    void FreeIoRequest();
    void FreeMsgPort();
    void CloseDevice();

    uint64 GetTicks() const;
    double TicksToSeconds(uint64 ticks) const;

    struct MsgPort* port { nullptr };
    struct TimeRequest* request { nullptr };
    BYTE device { -1 };
    double frequency { 0.0 };
};

} // fractalnova

