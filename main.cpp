#include "GuiWindow.hpp"

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/warp3dnova.h>

#include <cstdio>

//struct ExecIFace* IExec;
struct Warp3DIFace* IWarp3DNova;

static struct Library* NovaBase;

int main(void)
{
    NovaBase = IExec->OpenLibrary("Warp3DNova.library", 1);

    if (NovaBase) {
        IWarp3DNova = (struct Warp3DIFace *)IExec->GetInterface(NovaBase, "main", 1, nullptr);
        if (IWarp3DNova) {
            puts("Hoplaa");
            IExec->DropInterface((struct Interface *)IWarp3DNova);
            IWarp3DNova = nullptr;
        }
        IExec->CloseLibrary(NovaBase);
        NovaBase = nullptr;
    }

    fractalnova::GuiWindow window;
    window.run();
    //IDOS->Delay(50);
    return 0;
}
