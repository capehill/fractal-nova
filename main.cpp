#include "GuiWindow.hpp"
#include "NovaContext.hpp"
#include "Timer.hpp"

#include <proto/dos.h>
//#include <proto/exec.h>

#include <cstdio>
#include <exception>

namespace fractalnova {

namespace {
    constexpr bool verboseMode { false };
    constexpr double eventPeriod { 1.0 / 60 };

    static const char* const version __attribute__((used)) { "$VER: Fractal-Nova 0.1 (02.02.2020)" };
}

struct Params {
    LONG vsync;
};

static Params params { 0 };

void ParseArgs()
{
    const char* const pattern = "VSYNC/S";

    struct RDArgs *result = IDOS->ReadArgs(pattern, (int32 *)&params, NULL);

    if (result) {
        printf("VSYNC [%s]\n", params.vsync ? "on" : "off");
        IDOS->FreeArgs(result);
    } else {
        printf("Error when reading command-line arguments. Known parameters are: %s\n", pattern);
    }
}

} // fractalnova

int main(void)
{
    uint64 frames { 0 };
    uint64 events { 0 };
    bool running { true };
    double duration { 0.0 };

    fractalnova::ParseArgs();

    try {
        fractalnova::GuiWindow window;
        fractalnova::NovaContext context { window, fractalnova::verboseMode, fractalnova::params.vsync };
        fractalnova::Timer timer;

        context.LoadShaders();
        context.CreateVBO();
        context.CreateDBO();

        const uint64 start = timer.GetTicks();
        uint64 eventTicks = start;
        uint64 fpsTicks = start;
        uint64 lastFrames = 0;

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
            }

            if (window.refresh) {
                context.Draw();
                context.SwapBuffers();
                //window.refresh = false;
                frames++;

                const double passed = timer.TicksToSeconds(now - fpsTicks);
                if (passed >= 1.0) {
                    char buffer[64];
                    snprintf(buffer, sizeof(buffer), "FPS %.3f", (frames - lastFrames) / passed);
                    window.SetTitle(buffer);
                    fpsTicks = now;
                    lastFrames = frames;
                }
            }

        }

        const uint64 finish = timer.GetTicks();
        duration = timer.TicksToSeconds(finish - start);

    } catch (std::exception& e) {
        printf("Exception %s\n", e.what());
    }

    printf("Frames %llu in %.3f second. FPS %.3f\n", frames, duration, frames / duration);
    printf("Events checked %llu. EPS %.3f\n", events, events / duration);

    return 0;
}
