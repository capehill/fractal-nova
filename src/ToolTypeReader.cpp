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

#include "ToolTypeReader.hpp"
#include "Logger.hpp"

#include <proto/icon.h>

#include <algorithm>
#include <array>
#include <string>
#include <cstring>

namespace fractalnova {

static constexpr int minIter { 100 };
static constexpr int maxIter { 1000 };

static Resolution ParseResolution(const char* const str)
{
    Resolution r;

    if (str) {
        const std::string temp{ str };
        const std::size_t pos = temp.find('x');
        if (pos == std::string::npos) {
            return r;
        }

        const std::string w = temp.substr(0, pos);
        const std::string h = temp.substr(pos + 1);

        r.width = atoi(w.c_str());
        r.height = atoi(h.c_str());
    }

    return r;
}

static Resolution ParseScreenMode(const char* const str)
{
    auto screenSize = ParseResolution(str);

    logging::Debug("SCREENMODE tooltype %u x %u", screenSize.width, screenSize.height);

    return screenSize;
}

static Resolution ParseWindowSize(const char* const str)
{
    auto windowSize = ParseResolution(str);

    logging::Debug("WINDOWSIZE tooltype %u x %u", windowSize.width, windowSize.height);

    return windowSize;
}

static logging::ELevel ConvertToLogLevel(const char* const str)
{
    struct LogLevelItem {
        const char* const name;
        logging::ELevel level;
    };

    constexpr std::array<LogLevelItem, 5> items {{
        { "DETAIL", logging::ELevel::Detail },
        { "DEBUG", logging::ELevel::Debug },
        { "INFO", logging::ELevel::Info },
        { "WARNING", logging::ELevel::Warning },
        { "ERROR", logging::ELevel::Error }
    }};

    for (const auto& i: items) {
        if (strcmp(i.name, str) == 0) {
            return i.level;
        }
    }

    logging::Info("Unknown log level '%s'", str);

    return logging::ELevel::Info;
}

Params ToolTypeReader::ReadToolTypes(const char* const filename)
{
    Params params {};

    if (filename) {
        auto object = IIcon->GetDiskObject(filename);

        if (object) {
            params.vsync = IIcon->FindToolType(object->do_ToolTypes, "VSYNC");
            params.fullscreen = IIcon->FindToolType(object->do_ToolTypes, "FULLSCREEN");
            params.lazyClear = IIcon->FindToolType(object->do_ToolTypes, "LAZYCLEAR");

            const char* const iterationsStr = IIcon->FindToolType(object->do_ToolTypes, "ITERATIONS");
            if (iterationsStr) {
                const int iterations = atoi(iterationsStr);
                params.iterations = std::clamp(iterations, minIter, maxIter);
            }

            const char* const logLevelStr = IIcon->FindToolType(object->do_ToolTypes, "LOGLEVEL");
            if (logLevelStr) {
                logging::SetLevel(ConvertToLogLevel(logLevelStr));
            }

            params.screenSize = ParseScreenMode(IIcon->FindToolType(object->do_ToolTypes, "SCREENMODE"));
            params.windowSize = ParseWindowSize(IIcon->FindToolType(object->do_ToolTypes, "WINDOWSIZE"));

            IIcon->FreeDiskObject(object);
        } else {
            logging::Error("Failed to open disk object");
        }
    } else {
        logging::Error("Filename is a nullptr");
    }

    return params;
}

} // fractalnova
