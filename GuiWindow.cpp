#include "GuiWindow.hpp"

#include <proto/exec.h>
#include <proto/intuition.h>

namespace fractalnova {

namespace {
    constexpr int width { 800 };
    constexpr int height { 600 };
}

GuiWindow::GuiWindow()
{
    window = IIntuition->OpenWindowTags(nullptr,
        WA_Title, "Fractal-Noval",
        WA_Activate, TRUE,
        WA_BackFill, LAYERS_NOBACKFILL,
        WA_CloseGadget, TRUE,
        WA_DragBar, TRUE,
        WA_DepthGadget, TRUE,
        WA_IDCMP, IDCMP_REFRESHWINDOW | IDCMP_NEWSIZE | IDCMP_CLOSEWINDOW,
        WA_InnerWidth, width,
        WA_InnerHeight, height,
        WA_MaxWidth, 100,
        WA_MinWidth, 100,
        WA_SimpleRefresh, TRUE,
        WA_SizeGadget, TRUE,
        TAG_DONE);
}

GuiWindow::~GuiWindow()
{
    if (window) {
        IIntuition->CloseWindow(window);
        window = nullptr;
    }
}

void GuiWindow::run()
{
    IExec->WaitPort(window->UserPort);

    IntuiMessage* msg;

    bool running { true };

    while (running) {
        while ((msg = (struct IntuiMessage *)IExec->GetMsg(window->UserPort))) {
            switch (msg->Class) {
                case IDCMP_CLOSEWINDOW:
                    running = false;
                    break;
            }
        }
    }
}

} // fractalnova

