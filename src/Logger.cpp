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
