#include "GuiWindow.hpp"

#include <proto/exec.h>
#include <proto/intuition.h>

#include <cstdio>

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
        WA_MaxHeight, 2048,
        WA_MaxWidth, 2048,
        WA_MinHeight, 200,
        WA_MinWidth, 200,
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

bool GuiWindow::Run()
{
    IntuiMessage* msg;

    bool running { true };

    while ((msg = (struct IntuiMessage *)IExec->GetMsg(window->UserPort))) {
        switch (msg->Class) {
            case IDCMP_CLOSEWINDOW:
                running = false;
                break;
            case IDCMP_NEWSIZE:
                resize = true;
                break;
            case IDCMP_REFRESHWINDOW:
                refresh = true;
                break;
            default:
                printf("Unknown event %lu\n", msg->Class);
                break;
        }

        IExec->ReplyMsg((struct Message *)msg);
    }

    return running;
}

void GuiWindow::SetTitle(const char* title)
{
    IIntuition->SetWindowTitles(window, title, title);
}

} // fractalnova

