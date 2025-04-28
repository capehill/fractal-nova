#include "GuiWindow.hpp"
#include "NovaContext.hpp"
#include "Timer.hpp"
#include "Logger.hpp"

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>

#include <workbench/startup.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <exception>

namespace fractalnova {

static constexpr double eventPeriod { 1.0 / 60.0 };
static constexpr int minIter { 100 };
static constexpr int maxIter { 1000 };

static const char* const version __attribute__((used)) { "$VER: Fractal-Nova 1.1 (13.3.2025)" };

static Params params;

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

static void ParseScreenMode(const char* const str)
{
    params.screenSize = ParseResolution(str);

    logging::Debug("SCREENMODE tooltype %lu x %lu", params.screenSize.width, params.screenSize.height);
}

static void ParseWindowSize(const char* const str)
{
    params.windowSize = ParseResolution(str);

    logging::Debug("WINDOWSIZE tooltype %lu x %lu", params.windowSize.width, params.windowSize.height);
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

static void ReadToolTypes(const char* const filename)
{
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

            ParseScreenMode(IIcon->FindToolType(object->do_ToolTypes, "SCREENMODE"));
            ParseWindowSize(IIcon->FindToolType(object->do_ToolTypes, "WINDOWSIZE"));

            IIcon->FreeDiskObject(object);
        } else {
            logging::Error("Failed to open disk object");
        }
    } else {
        logging::Error("Filename is a nullptr");
    }
}

static void CheckStack()
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

static void HandleShell(char** argv)
{
    ReadToolTypes(argv[0]);
}

static void HandleWorkbench(WBStartup* startup)
{
    WBArg* args = startup->sm_ArgList;

    ReadToolTypes(args->wa_Name);
}

} // fractalnova

int main(int argc, char* argv[])
{
    uint64 frames { 0 };
    uint64 events { 0 };
    //double duration { 0.0 };

    using namespace fractalnova;

    if (argc > 0) {
        HandleShell(argv);
    } else {
        HandleWorkbench(reinterpret_cast<WBStartup*>(argv));
    }

    try {
        GuiWindow window { params };
        NovaContext context { window, params.iterations };
        Timer timer;

        const uint64 start = timer.GetTicks();
        uint64 eventTicks = start;
        uint64 fpsTicks = start;
        uint64 lastFrames = 0;

        while (true) {
            const uint64 now = timer.GetTicks();

            if (timer.TicksToSeconds(now - eventTicks) >= eventPeriod) {
                if (!window.Run()) {
                    break;
                }

                eventTicks = now;
                events++;

                if (window.Flagged(EFlag::Resize)) {
                    context.Resize();
                }

                if (window.Flagged(EFlag::Reset)) {
                    context.Reset();
                }

                context.UseProgram(window.GetFractal());
                context.UsePalette(window.GetPalette());
                context.SetZoom(window.GetZoom());
                context.SetPosition(window.GetPosition());
                context.SetIterations(window.GetIterations());
            }

            const double passed = timer.TicksToSeconds(now - fpsTicks);

            if (!params.lazyClear || passed >= 1.0) {
                context.Clear();
            }

            context.Draw();
            context.SwapBuffers();
            context.SetPosition({0.0f, 0.0f});

            frames++;

            if (passed >= 1.0) {
                static char buffer[64];
                snprintf(buffer, sizeof(buffer), "FPS %.2f, zoom %.1f", static_cast<double>(frames - lastFrames) / passed, window.GetZoom());
                window.SetTitle(buffer);
                fpsTicks = now;
                lastFrames = frames;
            }
        }

        //const uint64 finish = timer.GetTicks();
        //duration = timer.TicksToSeconds(finish - start);

    } catch (const std::exception& e) {
        logging::Error("Exception: %s", e.what());
    }

    //logging::Log("Frames %llu in %.1f second. FPS %.1f", frames, duration, frames / duration);
    //logging::Log("Events checked %llu. EPS %.1f", events, events / duration);

    CheckStack();

    return 0;
}
