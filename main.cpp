#include "GuiWindow.hpp"
#include "NovaContext.hpp"
#include "Timer.hpp"
#include "Logger.hpp"

#include <proto/dos.h>
//#include <proto/exec.h>

#include <cstdio>
#include <exception>

namespace fractalnova {

namespace {
    constexpr double eventPeriod { 1.0 / 60 };

    static const char* const version __attribute__((used)) { "$VER: Fractal-Nova 0.2 (13.02.2020)" };
}

struct Params {
    LONG vsync;
    LONG* iter;
    LONG lazyClear;
    LONG verbose;
};

static Params params { 0, nullptr, 0, 0 };

static int32 iterations { 100 };

static const char* ToString(const bool b)
{
    return b ? "on" : "off";
}

void ParseArgs()
{
    const char* const pattern = "VSYNC/S,ITER/N,LAZYCLEAR/S,VERBOSE/S";

    struct RDArgs *result = IDOS->ReadArgs(pattern, (int32 *)&params, NULL);

    if (result) {
        logging::Log("VSYNC [%s]\n", ToString(params.vsync));
        logging::Log("Lazy clear [%s]\n", ToString(params.lazyClear));
        logging::Log("Verbose [%s]\n", ToString(params.verbose));

        if (params.iter) {
            iterations = *params.iter;
            if (iterations < 20) {
                iterations = 20;
            } else if (iterations > 1000) {
                iterations = 1000;
            }
        }
        logging::Log("ITER [%ld]\n", iterations);

        if (params.verbose) {
            logging::MakeVerbose();
        }

        IDOS->FreeArgs(result);
    } else {
        logging::Error("Error when reading command-line arguments. Known parameters are: %s\n", pattern);
    }
}

} // fractalnova

int main(void)
{
    uint64 frames { 0 };
    uint64 events { 0 };
    double duration { 0.0 };

    fractalnova::ParseArgs();

    try {
        fractalnova::GuiWindow window;
        fractalnova::NovaContext context { window, fractalnova::params.vsync, fractalnova::iterations };
        fractalnova::Timer timer;

        const uint64 start = timer.GetTicks();
        uint64 eventTicks = start;
        uint64 fpsTicks = start;
        uint64 lastFrames = 0;

        bool running { true };

        while (running) {
            const uint64 now = timer.GetTicks();

            if (timer.TicksToSeconds(now - eventTicks) >= fractalnova::eventPeriod) {
                running = window.Run();
                eventTicks = now;
                events++;

                if (window.resize) {
                    context.Resize();
                    window.refresh = true;
                    window.resize = false;
                }

                if (window.reset) {
                    context.Reset();
                    window.refresh = true;
                    window.reset = false;
                }
            }

            if (window.refresh) {
                const float zoom = window.GetZoom();

                context.SetZoom(zoom);
                context.SetPosition(window.GetPosition());
                window.ClearPosition();
                if (!fractalnova::params.lazyClear) {
                    context.Clear();
                }
                context.Draw();
                context.SwapBuffers();
                //window.refresh = false;
                frames++;

                const double passed = timer.TicksToSeconds(now - fpsTicks);
                if (passed >= 1.0) {
                    static char buffer[64];
                    snprintf(buffer, sizeof(buffer), "FPS %.1f, zoom %.1f", (frames - lastFrames) / passed, zoom);
                    if (fractalnova::params.lazyClear) {
                        context.Clear();
                    }
                    window.SetTitle(buffer);
                    fpsTicks = now;
                    lastFrames = frames;
                }
            }

        }

        const uint64 finish = timer.GetTicks();
        duration = timer.TicksToSeconds(finish - start);

    } catch (std::exception& e) {
        logging::Error("Exception %s\n", e.what());
    }

    logging::Log("Frames %llu in %.1f second. FPS %.1f\n", frames, duration, frames / duration);
    logging::Log("Events checked %llu. EPS %.1f\n", events, events / duration);

    return 0;
}
