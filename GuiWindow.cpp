#include "GuiWindow.hpp"

#include <proto/exec.h>
#include <proto/intuition.h>

#include <libraries/keymap.h>
#include <libraries/gadtools.h>

#include <intuition/menuclass.h>

#include <cstdio>
#include <stdexcept>

namespace fractalnova {

namespace {
    constexpr float zoomStep { 1.0f };
    constexpr const char* const name { "Fractal Nova" };
}

enum EMenu {
    MID_Iconify = 1,
    MID_About,
    MID_Quit,
    MID_ResetView,
    //
    MID_Mandelbrot,
    MID_Julia,
    //
    MID_Rainbow,
    MID_RainbowRev
};

Object* menu;

GuiWindow::GuiWindow()
{
    menu = IIntuition->NewObject(nullptr, "menuclass",
        MA_Type, T_ROOT,
        // Main
        MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
            MA_Type, T_MENU,
            MA_Label, "Main",
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Reset view",
                MA_ID, MID_ResetView,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Quit",
                MA_ID, MID_Quit,
                TAG_DONE),
            TAG_DONE),
        // Fractal
        MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
            MA_Type, T_MENU,
            MA_Label, "Fractal",
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Manderbrot",
                MA_ID, MID_Mandelbrot,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Julia",
                MA_ID, MID_Julia,
                TAG_DONE),
            TAG_DONE),
        // Colours
        MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
            MA_Type, T_MENU,
            MA_Label, "Colours",
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Rainbow",
                MA_ID, MID_Rainbow,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Rainbow rev.",
                MA_ID, MID_RainbowRev,
                TAG_DONE),
            TAG_DONE),
        // The end
        TAG_DONE);

    if (!menu) {
        printf("Failed to create menus\n");
    }

    window = IIntuition->OpenWindowTags(nullptr,
        WA_Title, name,
        WA_ScreenTitle, name,
        WA_Activate, TRUE,
        WA_BackFill, LAYERS_NOBACKFILL,
        WA_CloseGadget, TRUE,
        WA_DragBar, TRUE,
        WA_DepthGadget, TRUE,
        WA_Flags, WFLG_REPORTMOUSE,
        WA_IDCMP, IDCMP_REFRESHWINDOW | IDCMP_NEWSIZE | IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE |
                  IDCMP_DELTAMOVE | IDCMP_EXTENDEDMOUSE | IDCMP_RAWKEY | IDCMP_MENUPICK,
        WA_InnerWidth, width,
        WA_InnerHeight, height,
        WA_MaxHeight, 2048,
        WA_MaxWidth, 2048,
        WA_MenuStrip, menu,
        WA_MinHeight, 200,
        WA_MinWidth, 200,
        WA_SimpleRefresh, TRUE,
        WA_SizeGadget, TRUE,
        TAG_DONE);

    if (!window) {
        throw std::runtime_error("Failed to open window");
    }
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

    position = { 0.0f, 0.0f };

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
            case IDCMP_MENUPICK: {
                uint32 id = NO_MENU_ID;
                while (((id = IIntuition->IDoMethod(reinterpret_cast<Object *>(window->MenuStrip), MM_NEXTSELECT, 0, id))) != NO_MENU_ID) {
                    printf("ID %d\n", id);
                }

                } break;
            case IDCMP_MOUSEBUTTONS:
                switch (msg->Code & ~IECODE_UP_PREFIX) {
                    case IECODE_LBUTTON:
                        if (!(msg->Code & IECODE_UP_PREFIX)) {
                            //printf("%u, %u\n", msg->MouseX, msg->MouseY);
                            refresh = true;
                            panning = true;
                        } else {
                            panning = false;
                        }
                        break;
                }
                break;
            case IDCMP_MOUSEMOVE:
                if (panning) {
                    position.x += msg->MouseX / static_cast<float>(width/2) / zoom;
                    position.y += msg->MouseY / static_cast<float>(height/2) / zoom;
                    refresh = true;
                }
                break;
            case IDCMP_NEWSIZE:
                if ((IIntuition->GetWindowAttrs(window,
                    WA_InnerWidth, &width,
                    WA_InnerHeight, &height,
                    TAG_DONE)) != 0)
                {
                    throw std::runtime_error("Failed to get window attributes");
                }
                resize = true;
                break;
            case IDCMP_RAWKEY:
                // TODO: cursor panning
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
                        case RAWKEY_LSHIFT:
                        case RAWKEY_RSHIFT:
                            fastZoom = true;
                            break;
                    }
                } else {
                    switch (msg->Code & 0x7F) {
                        case RAWKEY_LSHIFT:
                        case RAWKEY_RSHIFT:
                            fastZoom = false;
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

void GuiWindow::ClearPosition()
{
    position = { 0.0f, 0.0f };
}

float GuiWindow::GetZoom() const
{
    return zoom;
}

void GuiWindow::ZoomIn()
{
    zoom += fastZoom ? 10 * zoomStep : zoomStep;
    refresh = true;
}

void GuiWindow::ZoomOut()
{
    zoom -= fastZoom ? 10 * zoomStep : zoomStep;

    if (zoom <= 0.0f) {
        puts("Cannot zoom further");
        zoom = 1.0f;
    }

    refresh = true;
}

uint32 GuiWindow::Width() const
{
    return width;
}

uint32 GuiWindow::Height() const
{
    return height;
}

} // fractalnova

