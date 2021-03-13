#pragma once

namespace logging {

void MakeVerbose();
bool IsVerbose();

void Log(const char * fmt, ...) __attribute__ ((format (printf, 1, 2)));
void Error(const char * fmt, ...) __attribute__ ((format (printf, 1, 2)));

} // logging
