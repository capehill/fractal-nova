#include "GuiWindow.hpp"
#include "NovaContext.hpp"

//#include <proto/dos.h>
//#include <proto/exec.h>

#include <cstdio>
#include <exception>

int main(void)
{
    uint64 frames { 0 };
    bool running { true };

    try {
        fractalnova::GuiWindow window;
        fractalnova::NovaContext context { window };

        while (running) {
            running = window.Run();

            if (window.resize) {
                printf("resize\n");
                context.Resize();
                window.refresh = true;
                window.resize = false;
            }

            if (window.refresh) {
                printf("refresh\n");
                context.Clear();
                context.SwapBuffers();
                window.refresh = false;
                frames++;
            }
        }
    } catch (std::exception& e) {
        printf("Exception %s\n", e.what());
    }

    printf("Frames %llu\n", frames);

    //IDOS->Delay(50);
    return 0;
}
