#include "Logger.hpp"

#include <cstdarg>
#include <cstdio>

namespace logging {

static bool verbose { false };

void MakeVerbose()
{
    verbose = true;
}

static void LogImpl(const char * fmt, va_list ap)
{
    char buffer[16 * 1024];
    const int len = vsnprintf(buffer, sizeof(buffer), fmt, ap);
    puts(buffer);

    if (len >= static_cast<int>(sizeof(buffer))) {
        printf("Insufficient log buffer: %d bytes needed", len);
    }
}

void Log(const char* fmt, ...)
{
    if (verbose) {
        va_list ap;
        va_start(ap, fmt);
        LogImpl(fmt, ap);
        va_end(ap);
    }
}

void Error(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    LogImpl(fmt, ap);
    va_end(ap);
}


} // logging
