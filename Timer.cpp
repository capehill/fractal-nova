#include "Timer.hpp"

#include <proto/exec.h>
#include <proto/timer.h>

#include <stdexcept>

namespace fractalnova {

Timer::Timer()
{
    port = (struct MsgPort *) IExec->AllocSysObjectTags(ASOT_PORT,
        ASOPORT_Name, "timer_port",
        TAG_DONE);

    if (!port) {
        throw std::runtime_error("Failed to create timer port");
    }

    request = (struct TimeRequest *) IExec->AllocSysObjectTags(ASOT_IOREQUEST,
        ASOIOR_Size, sizeof(struct TimeRequest),
        ASOIOR_ReplyPort, port,
        TAG_DONE);

    if (!request) {
        FreeMsgPort();

        throw std::runtime_error("Failed to create timer IO request");
    }

    device = IExec->OpenDevice(TIMERNAME, UNIT_WAITUNTIL,
        (struct IORequest *) request, 0);

    if (device) {
        FreeMsgPort();
        FreeIoRequest();

        throw std::runtime_error("Failed to open timer.device");
    }

    if (!ITimer) {
        ITimer = (struct TimerIFace *) IExec->GetInterface(
            (struct Library *) request->Request.io_Device, "main", 1, nullptr);

        if (!ITimer) {
            FreeMsgPort();
            FreeIoRequest();
            CloseDevice();

            throw std::runtime_error("Failed to get timer interface");
        }
    }

    struct EClockVal clockVal;
    frequency = ITimer->ReadEClock(&clockVal);
}

Timer::~Timer()
{
    if (ITimer) {
        IExec->DropInterface((struct Interface *)ITimer);
        ITimer = nullptr;
    }

    CloseDevice();
    FreeIoRequest();
    FreeMsgPort();
}

void Timer::CloseDevice()
{
    if (device == 0 && request) {
        IExec->CloseDevice((struct IORequest *) request);
        device = -1;
    }
}

void Timer::FreeMsgPort()
{
    if (port) {
        IExec->FreeSysObject(ASOT_PORT, port);
        port = nullptr;
    }
}

void Timer::FreeIoRequest()
{
    if (request) {
        IExec->FreeSysObject(ASOT_IOREQUEST, request);
        request = nullptr;
    }
}

struct MyClock {
    union {
        uint64 ticks;
        struct EClockVal clockVal;
    };
};

uint64 Timer::GetTicks() const
{
    MyClock now;
    ITimer->ReadEClock(&now.clockVal);
    return now.ticks;
}

double Timer::TicksToSeconds(uint64 ticks) const
{
    return ticks / frequency;
}

} // fractalnova
