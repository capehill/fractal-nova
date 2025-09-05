/*
Copyright (C) 2020-2025 Juha Niemimaki

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

#include "StackChecker.hpp"
#include "Logger.hpp"

#include <proto/exec.h>

namespace fractalnova {

StackChecker::StackChecker()
{
    auto task = IExec->FindTask(nullptr);
    auto upper = static_cast<uint32 *>(task->tc_SPUpper);
    auto lower = static_cast<uint32 *>(task->tc_SPLower);

    for (auto ptr = lower; ptr <= upper; ptr++) {
        if (*ptr != 0 && *ptr != 0xbad1bad3) {
            logging::Debug("%u bytes left on stack, used %u", (ptr - lower) * 4, (upper - ptr) * 4);
            return;
        }
    }
}

} // fractalnova

