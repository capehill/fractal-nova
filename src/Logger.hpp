#pragma once

namespace logging {

enum class ELevel
{
    Detail,
    Debug,
    Info,
    Warning,
    Error
};

void SetLevel(ELevel level);
ELevel Level();
bool IsVerbose();

void Detail(const char * fmt, ...) __attribute__ ((format (printf, 1, 2)));
void Debug(const char * fmt, ...) __attribute__ ((format (printf, 1, 2)));
void Info(const char * fmt, ...) __attribute__ ((format (printf, 1, 2)));
void Warning(const char * fmt, ...) __attribute__ ((format (printf, 1, 2)));
void Error(const char * fmt, ...) __attribute__ ((format (printf, 1, 2)));

} // logging
