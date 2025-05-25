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

#include "Logger.hpp"
#include "Buffer.hpp"

#include <proto/exec.h>

#include <cstdarg>
#include <cstdio>

namespace logging {

static constexpr unsigned bufferSize { 4 * 1024 };

static ELevel logLevel { ELevel::Info };

ELevel Level()
{
    return logLevel;
}

void SetLevel(const enum ELevel level)
{
    logLevel = level;
}

bool IsVerbose()
{
    return logLevel < ELevel::Info;
}

static void LogImpl(const enum ELevel level, const char * fmt, va_list ap)
{
    if (level < logLevel) {
        return;
    }

    va_list copy;
    va_copy(copy, ap);
    const unsigned len = vsnprintf(nullptr, 0, fmt, copy) + 1;
    va_end(copy);

    if (len < bufferSize) {
        char buffer[bufferSize];
        vsnprintf(buffer, sizeof(buffer), fmt, ap);
        puts(buffer);
    } else {
        fractalnova::Buffer temp { len };
        vsnprintf(temp.Data(), len, fmt, ap);
        puts(temp.Data());
    }

    fflush(stdout);
}

void Detail(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    LogImpl(ELevel::Detail, fmt, ap);
    va_end(ap);
}

void Debug(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    LogImpl(ELevel::Debug, fmt, ap);
    va_end(ap);
}

void Info(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    LogImpl(ELevel::Info, fmt, ap);
    va_end(ap);
}

void Warning(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    LogImpl(ELevel::Warning, fmt, ap);
    va_end(ap);
}

void Error(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    LogImpl(ELevel::Error, fmt, ap);
    va_end(ap);
}

} // logging
