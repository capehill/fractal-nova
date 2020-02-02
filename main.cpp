#include "GuiWindow.hpp"
#include "NovaContext.hpp"
#include "Timer.hpp"

//#include <proto/dos.h>
//#include <proto/exec.h>

#include <cstdio>
#include <exception>

namespace {
    constexpr bool verboseMode { false };
}

int main(void)
{
    uint64 frames { 0 };
    bool running { true };
    double duration { 0.0 };

    try {
        fractalnova::GuiWindow window;
        fractalnova::NovaContext context { window, verboseMode };
        fractalnova::Timer timer;

        context.LoadShaders();
        context.CreateVBO();
        context.CreateDBO();

        const uint64 start = timer.GetTicks();

        while (running) {
            running = window.Run();

            if (window.resize) {
                printf("resize\n");
                context.Resize();
                window.refresh = true;
                window.resize = false;
            }

            if (window.refresh) {
                //printf("refresh\n");
                //context.Clear();
                context.Draw();
                context.SwapBuffers();
                //window.refresh = false;
                frames++;
            }
        }

        const uint64 finish = timer.GetTicks();
        duration = timer.TicksToSeconds(finish - start);

    } catch (std::exception& e) {
        printf("Exception %s\n", e.what());
    }

    printf("Duration %f. Frames %llu. FPS %f\n", duration, frames, frames / duration);

    //IDOS->Delay(50);
    return 0;
}
