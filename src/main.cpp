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

#include "GuiWindow.hpp"
#include "NovaContext.hpp"
#include "Timer.hpp"
#include "Logger.hpp"
#include "Version.hpp"
#include "StackChecker.hpp"
#include "ToolTypeReader.hpp"

#include <proto/dos.h>

#include <workbench/startup.h>

#include <cstdio>
#include <exception>

namespace fractalnova {

static constexpr double eventPeriod { 1.0 / 60.0 };

static const char* const version __attribute__((used)) { "$VER: "
    NAME_STRING " " VERSION_STRING " " DATE_STRING };

static Params HandleShell(char* argv[])
{
    ToolTypeReader reader;
    return reader.ReadToolTypes(argv[0]);
}

static Params HandleWorkbench(WBStartup* startup)
{
    WBArg* args = startup->sm_ArgList;

    ToolTypeReader reader;
    return reader.ReadToolTypes(args->wa_Name);
}

static Params ReadParams(int argc, char* argv[])
{
    if (argc > 0) {
        return HandleShell(argv);
    }

    return HandleWorkbench(reinterpret_cast<WBStartup*>(argv));
}

} // fractalnova

int main(int argc, char* argv[])
{
    uint64 frames { 0 };
    uint64 events { 0 };
    //double duration { 0.0 };

    using namespace fractalnova;

    const Params params = ReadParams(argc, argv);

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

    StackChecker sc;

    return 0;
}
