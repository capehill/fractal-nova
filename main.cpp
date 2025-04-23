#include "GuiWindow.hpp"
#include "NovaContext.hpp"
#include "Timer.hpp"
#include "Logger.hpp"

#include <proto/dos.h>

#include <cstdio>
#include <exception>

namespace fractalnova {

static constexpr double eventPeriod { 1.0 / 60.0 };
static constexpr int minIter { 20 };
static constexpr int maxIter { 1000 };

static const char* const version __attribute__((used)) { "$VER: Fractal-Nova 1.0 (13.3.2021)" };

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

    struct RDArgs *result = IDOS->ReadArgs(pattern, reinterpret_cast<int32 *>(&params), nullptr);

    if (result) {
        if (params.verbose) {
            logging::SetLevel(logging::ELevel::Debug);
        }

        logging::Debug("VSYNC [%s]", ToString(params.vsync));
        logging::Debug("Lazy clear [%s]", ToString(params.lazyClear));
        logging::Debug("Verbose [%s]", ToString(params.verbose));

        if (params.iter) {
            iterations = *params.iter;
            if (iterations < minIter) {
                iterations = minIter;
            } else if (iterations > maxIter) {
                iterations = maxIter;
            }
        }
        logging::Debug("ITER [%ld]", iterations);

        IDOS->FreeArgs(result);
    } else {
        logging::Error("Error when reading command-line arguments. Known parameters are: %s", pattern);
    }
}

} // fractalnova

int main(void)
{
    uint64 frames { 0 };
    uint64 events { 0 };
    //double duration { 0.0 };

    using namespace fractalnova;

    ParseArgs();

    try {
        GuiWindow window { static_cast<bool>(params.vsync) };
        NovaContext context { window, iterations };
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
        logging::Error("Exception %s", e.what());
    }

    //logging::Log("Frames %llu in %.1f second. FPS %.1f", frames, duration, frames / duration);
    //logging::Log("Events checked %llu. EPS %.1f", events, events / duration);

    return 0;
}
