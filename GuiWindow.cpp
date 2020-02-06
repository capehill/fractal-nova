#include "GuiWindow.hpp"

#include <proto/exec.h>
#include <proto/intuition.h>
#include <libraries/keymap.h>

#include <cstdio>

namespace fractalnova {

namespace {
    constexpr int width { 800 };
    constexpr int height { 600 };
    constexpr float zoomStep { 1.0f };
    constexpr const char* const name { "Fractal Nova" };
}

GuiWindow::GuiWindow()
{
    window = IIntuition->OpenWindowTags(nullptr,
        WA_Title, name,
        WA_ScreenTitle, name,
        WA_Activate, TRUE,
        WA_BackFill, LAYERS_NOBACKFILL,
        WA_CloseGadget, TRUE,
        WA_DragBar, TRUE,
        WA_DepthGadget, TRUE,
        WA_Flags, WFLG_REPORTMOUSE,
        WA_IDCMP, IDCMP_REFRESHWINDOW | IDCMP_NEWSIZE | IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_DELTAMOVE | IDCMP_EXTENDEDMOUSE | IDCMP_RAWKEY,
        WA_InnerWidth, width,
        WA_InnerHeight, height,
        WA_MaxHeight, 2048,
        WA_MaxWidth, 2048,
        WA_MinHeight, 200,
        WA_MinWidth, 200,
        WA_SimpleRefresh, TRUE,
        WA_SizeGadget, TRUE,
        TAG_DONE);

    //position = { width / 2.0f, height / 2.0f }; // TODO: resize
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
            case IDCMP_EXTENDEDMOUSE: {
                struct IntuiWheelData *data = (struct IntuiWheelData *)msg->IAddress;
                if (data->WheelY < 0) {
                    ZoomIn();
                } else {
                    ZoomOut();
                }
                } break;
            case IDCMP_MOUSEBUTTONS:
                switch (msg->Code & ~IECODE_UP_PREFIX) {
                    case IECODE_LBUTTON:
                        if (!(msg->Code & IECODE_UP_PREFIX)) {
                            //printf("%u, %u\n", msg->MouseX, msg->MouseY);
                            //position.x = msg->MouseX - window->BorderLeft;
                            //position.y = msg->MouseY - window->BorderTop;
                            //puts("panning");
                            refresh = true;
                            panning = true;
                        } else {
                            //puts("no pan");
                            panning = false;
                        }
                        break;
                }
                break;
            case IDCMP_MOUSEMOVE:
                if (panning) {
                    position.x = msg->MouseX / static_cast<float>(width / 2);
                    position.y = msg->MouseY / static_cast<float>(height / 2);
                    //printf("move %f, %f\n", position.x, position.y);
                    refresh = true;
                } else {
                    position = { 0.0f, 0.0f };
                }
                break;
            case IDCMP_NEWSIZE:
                resize = true;
                break;
            case IDCMP_RAWKEY:
                // TODO: cursor panning
                // TODO: faster zooming
                if (msg->Code <= 127) {
                    switch (msg->Code) {
                        case RAWKEY_ESC:
                            running = false;
                            break;
                        case RAWKEY_SPACE:
                            zoom = 1.0f;
                            reset = true;
                            break;
                        case RAWKEY_CRSRLEFT:
                            break;
                        case RAWKEY_CRSRRIGHT:
                            break;
                        case RAWKEY_CRSRUP:
                            ZoomIn();
                            break;
                        case RAWKEY_CRSRDOWN:
                            ZoomOut();
                            break;
                    }
                }
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
    IIntuition->SetWindowTitles(window, title, "Fractal Nova - Mandelbrot");
}

Vertex GuiWindow::GetPosition() const
{
    return position;
}

float GuiWindow::GetZoom() const
{
    return zoom;
}

void GuiWindow::ZoomIn()
{
    zoom += zoomStep;
    refresh = true;
}

void GuiWindow::ZoomOut()
{
    zoom -= zoomStep;

    if (zoom <= 0.0f) {
        puts("Cannot zoom further");
        zoom = zoomStep;
    }

    refresh = true;
}

} // fractalnova

