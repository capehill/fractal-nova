#pragma once

#include <exec/types.h>
#include <devices/timer.h>

namespace fractalnova {

class Timer
{
public:

    Timer();
    ~Timer();

    uint64 GetTicks() const;
    double TicksToSeconds(uint64 ticks) const;

private:
    void FreeIoRequest();
    void FreeMsgPort();
    void CloseDevice();

    struct MsgPort* port { nullptr };
    struct TimeRequest* request { nullptr };
    BYTE device { -1 };
    double frequency { 0.0 };
};

} // fractalnova

