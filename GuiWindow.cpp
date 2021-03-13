#include "GuiWindow.hpp"
#include "Logger.hpp"
#include "BackBuffer.hpp"

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include <libraries/keymap.h>
#include <libraries/gadtools.h>

#include <intuition/menuclass.h>

#include <cstdio>
#include <stdexcept>

namespace fractalnova {

static const char* const name { "Fractal Nova" };
static const char* const menuClass { "menuclass" };

enum class EMenu {
    Iconify = 1,
    About,
    Quit,
    ResetView,
    // Fractals
    Mandelbrot,
    Julia1,
    Julia2,
    Julia3,
    Julia4,
    Julia5,
    Julia6,
    Julia7,
    Julia8,
    Julia9,
    Julia10,
    // Palettes
    Rainbow,
    RainbowRev,
    Red,
    Green,
    Blue,
    BlackAndWhite,
    BlackAndWhiteRev
};

Object* GuiWindow::CreateMenu()
{
    Object* menu = IIntuition->NewObject(nullptr, menuClass,
        MA_Type, T_ROOT,
        // Main
        MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
            MA_Type, T_MENU,
            MA_Label, "Main",
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Reset view",
                MA_ID, EMenu::ResetView,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Quit",
                MA_ID, EMenu::Quit,
                TAG_DONE),
            TAG_DONE),
        // Fractal
        MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
            MA_Type, T_MENU,
            MA_Label, "Fractal",
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Manderbrot",
                MA_ID, EMenu::Mandelbrot,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 1",
                MA_ID, EMenu::Julia1,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 2",
                MA_ID, EMenu::Julia2,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 3",
                MA_ID, EMenu::Julia3,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 4",
                MA_ID, EMenu::Julia4,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 5",
                MA_ID, EMenu::Julia5,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 6",
                MA_ID, EMenu::Julia6,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 7",
                MA_ID, EMenu::Julia7,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 8",
                MA_ID, EMenu::Julia8,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 9",
                MA_ID, EMenu::Julia9,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 10",
                MA_ID, EMenu::Julia10,
                TAG_DONE),

            TAG_DONE),
        // Colours
        MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
            MA_Type, T_MENU,
            MA_Label, "Colours",
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Rainbow",
                MA_ID, EMenu::Rainbow,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Rainbow rev.",
                MA_ID, EMenu::RainbowRev,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Red",
                MA_ID, EMenu::Red,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Green",
                MA_ID, EMenu::Green,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Blue",
                MA_ID, EMenu::Blue,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Black and white",
                MA_ID, EMenu::BlackAndWhite,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Black and white rev.",
                MA_ID, EMenu::BlackAndWhiteRev,
                TAG_DONE),
            TAG_DONE),
        // The end
        TAG_DONE);

    if (!menu) {
        logging::Error("Failed to create menus");
    }

    return menu;
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
        WA_Flags, WFLG_REPORTMOUSE | WFLG_NEWLOOKMENUS,
        WA_IDCMP, IDCMP_REFRESHWINDOW | IDCMP_NEWSIZE | IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE |
                  IDCMP_DELTAMOVE | IDCMP_EXTENDEDMOUSE | IDCMP_RAWKEY | IDCMP_MENUPICK,
        WA_InnerWidth, width,
        WA_InnerHeight, height,
        WA_MaxHeight, 2048,
        WA_MaxWidth, 2048,
        WA_MenuStrip, CreateMenu(),
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
    flags = 0;

    while ((msg = (struct IntuiMessage *)IExec->GetMsg(window->UserPort))) {
        switch (msg->Class) {
            case IDCMP_CLOSEWINDOW:
                running = false;
                break;
            case IDCMP_EXTENDEDMOUSE:
                HandleExtendedMouse((struct IntuiWheelData *)msg->IAddress);
                break;
            case IDCMP_MENUPICK:
                running = HandleMenuPick();
                break;
            case IDCMP_MOUSEBUTTONS:
                HandleMouseButtons(msg->Code);
                break;
            case IDCMP_MOUSEMOVE:
                HandleMouseMove(msg->MouseX, msg->MouseY);
                break;
            case IDCMP_NEWSIZE:
                HandleNewSize();
                break;
            case IDCMP_RAWKEY:
                running = HandleRawKey(msg->Code);
                break;
            case IDCMP_REFRESHWINDOW:
                break;
            default:
                logging::Error("Unknown event %lu", msg->Class);
                break;
        }

        IExec->ReplyMsg((struct Message *)msg);
    }

    return running;
}

void GuiWindow::HandleExtendedMouse(struct IntuiWheelData * data)
{
    if (data->WheelY < 0) {
        ZoomIn();
    } else {
        ZoomOut();
    }
}

bool GuiWindow::HandleMenuPick()
{
    bool running { true };

    uint32 id = NO_MENU_ID;

    const EFractal currentFractal = fractal;

    while (((id = IIntuition->IDoMethod(reinterpret_cast<Object *>(window->MenuStrip), MM_NEXTSELECT, 0, id))) != NO_MENU_ID) {
        switch (static_cast<EMenu>(id)) {
            case EMenu::Quit:
                running = false;
                break;
            case EMenu::ResetView:
                ResetView();
                break;

            // Fractals

            case EMenu::Mandelbrot:
                fractal = EFractal::Mandelbrot;
                break;
            case EMenu::Julia1:
                fractal = EFractal::Julia1;
                break;
            case EMenu::Julia2:
                fractal = EFractal::Julia2;
                break;
            case EMenu::Julia3:
                fractal = EFractal::Julia3;
                break;
            case EMenu::Julia4:
                fractal = EFractal::Julia4;
                break;
            case EMenu::Julia5:
                fractal = EFractal::Julia5;
                break;
            case EMenu::Julia6:
                fractal = EFractal::Julia6;
                break;
            case EMenu::Julia7:
                fractal = EFractal::Julia7;
                break;
            case EMenu::Julia8:
                fractal = EFractal::Julia8;
                break;
            case EMenu::Julia9:
                fractal = EFractal::Julia9;
                break;
            case EMenu::Julia10:
                fractal = EFractal::Julia10;
                break;

            // Palettes

            case EMenu::Rainbow:
                palette = EPalette::Rainbow;
                break;
            case EMenu::RainbowRev:
                palette = EPalette::RainbowRev;
                break;
            case EMenu::Red:
                palette = EPalette::Red;
                break;
            case EMenu::Green:
                palette = EPalette::Green;
                break;
            case EMenu::Blue:
                palette = EPalette::Blue;
                break;
            case EMenu::BlackAndWhite:
                palette = EPalette::BlackAndWhite;
                break;
            case EMenu::BlackAndWhiteRev:
                palette = EPalette::BlackAndWhiteRev;
                break;
            default:
                logging::Error("Unhandled menu ID %lu", id);
                break;
        }
    }

    if (fractal != currentFractal) {
        ResetView();
    }

    return running;
}

void GuiWindow::HandleMouseButtons(UWORD code)
{
    switch (code & ~IECODE_UP_PREFIX) {
        case IECODE_LBUTTON:
            if (!(code & IECODE_UP_PREFIX)) {
                //logging::Log("%u, %u", msg->MouseX, msg->MouseY);
                panning = true;
            } else {
                panning = false;
            }
            break;
    }
}

void GuiWindow::HandleMouseMove(int mouseX, int mouseY)
{
    if (panning) {
        position.x += mouseX / static_cast<float>(width / 2) / zoom;
        position.y += mouseY / static_cast<float>(height / 2) / zoom;
    }
}

void GuiWindow::HandleNewSize()
{
    if ((IIntuition->GetWindowAttrs(window,
        WA_InnerWidth, &width,
        WA_InnerHeight, &height,
        TAG_DONE)) != 0)
    {
        throw std::runtime_error("Failed to get window attributes");
    }

    Set(EFlag::Resize);
}

bool GuiWindow::HandleRawKey(UWORD code)
{
    bool running { true };

    // TODO: cursor panning
    if (code <= 127) {
        switch (code) {
            case RAWKEY_ESC:
                running = false;
                break;
            case RAWKEY_SPACE:
                ResetView();
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
        switch (code & 0x7F) {
            case RAWKEY_LSHIFT:
            case RAWKEY_RSHIFT:
                fastZoom = false;
                break;
        }
    }

    return running;
}

void GuiWindow::SetTitle(const char* title)
{
    IIntuition->SetWindowTitles(window, title, name);
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

float GuiWindow::GetZoomStep() const
{
    constexpr float zoomStep { 1.01f };

    return fastZoom ? 2.0f * zoomStep : zoomStep;
}

void GuiWindow::ZoomIn()
{
    constexpr float maxZoom { 20000.0f };

    zoom *= GetZoomStep();

    if (zoom > maxZoom) {
        logging::Log("Cannot zoom closer");
        zoom = maxZoom;
    }
}

void GuiWindow::ZoomOut()
{
    constexpr float minZoom { 1.0f };

    zoom /= GetZoomStep();

    if (zoom < minZoom) {
        logging::Log("Cannot zoom further");
        zoom = minZoom;
    }
}

void GuiWindow::ResetView()
{
    zoom = 1.0f;
    Set(EFlag::Reset);
}

uint32 GuiWindow::Width() const
{
    return width;
}

uint32 GuiWindow::Height() const
{
    return height;
}

EFractal GuiWindow::GetFractal() const
{
    return fractal;
}

EPalette GuiWindow::GetPalette() const
{
    return palette;
}

bool GuiWindow::Flagged(const EFlag flag) const
{
    return flags & static_cast<uint32>(flag);
}

void GuiWindow::Set(const EFlag flag)
{
    flags |= static_cast<uint32>(flag);
}

void GuiWindow::Clear(const EFlag flag)
{
    flags &= ~static_cast<uint32>(flag);
}

Window* GuiWindow::WindowPtr() const
{
    return window;
}

void GuiWindow::Draw(BackBuffer* backBuffer) const
{
    const uint32 winw = window->Width - (window->BorderLeft + window->BorderRight);
    const uint32 winh = window->Height - (window->BorderTop + window->BorderBottom);

    IGraphics->BltBitMapRastPort(backBuffer->Data(), 0, 0, window->RPort,
        window->BorderLeft,
        window->BorderTop,
        std::min(winw, width),
        std::min(winh, height),
        0xC0);

}

} // fractalnova

