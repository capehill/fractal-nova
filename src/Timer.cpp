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

#include "Timer.hpp"
#include "Logger.hpp"

#include <proto/exec.h>
#include <proto/timer.h>

#include <stdexcept>

namespace fractalnova {

Timer::Timer()
{
    logging::Debug("Create Timer");

    port = static_cast<struct MsgPort *>(IExec->AllocSysObjectTags(ASOT_PORT,
        ASOPORT_Name, "timer_port",
        TAG_DONE));

    if (!port) {
        throw std::runtime_error("Failed to create timer port");
    }

    request = static_cast<struct TimeRequest *>(IExec->AllocSysObjectTags(ASOT_IOREQUEST,
        ASOIOR_Size, sizeof(struct TimeRequest),
        ASOIOR_ReplyPort, port,
        TAG_DONE));

    if (!request) {
        FreeMsgPort();

        throw std::runtime_error("Failed to create timer IO request");
    }

    device = static_cast<BYTE>(IExec->OpenDevice(TIMERNAME, UNIT_WAITUNTIL,
        reinterpret_cast<struct IORequest *>(request), 0));

    if (device) {
        FreeMsgPort();
        FreeIoRequest();

        throw std::runtime_error("Failed to open timer.device");
    }

    if (!ITimer) {
        ITimer = reinterpret_cast<struct TimerIFace *>(IExec->GetInterface(
            reinterpret_cast<struct Library *>(request->Request.io_Device), "main", 1, nullptr));

        if (!ITimer) {
            FreeMsgPort();
            FreeIoRequest();
            CloseDevice();

            throw std::runtime_error("Failed to get timer interface");
        }
    }

    EClockVal clockVal;
    frequency = ITimer->ReadEClock(&clockVal);
}

Timer::~Timer()
{
    if (ITimer) {
        IExec->DropInterface(reinterpret_cast<struct Interface *>(ITimer));
        ITimer = nullptr;
    }

    CloseDevice();
    FreeIoRequest();
    FreeMsgPort();
}

void Timer::CloseDevice()
{
    if (device == 0 && request) {
        IExec->CloseDevice(reinterpret_cast<struct IORequest *>(request));
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
        EClockVal clockVal;
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
    return static_cast<double>(ticks) / frequency;
}

} // fractalnova
