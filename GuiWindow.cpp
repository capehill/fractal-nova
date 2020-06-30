#include "GuiWindow.hpp"
#include "Logger.hpp"

#include <proto/exec.h>
#include <proto/intuition.h>

#include <libraries/keymap.h>
#include <libraries/gadtools.h>

#include <intuition/menuclass.h>

#include <cstdio>
#include <stdexcept>

namespace fractalnova {

namespace {
    constexpr const char* const name { "Fractal Nova" };
}

enum EMenu {
    MID_Iconify = 1,
    MID_About,
    MID_Quit,
    MID_ResetView,
    // Fractals
    MID_Mandelbrot,
    MID_Julia1,
    MID_Julia2,
    MID_Julia3,
    MID_Julia4,
    MID_Julia5,
    MID_Julia6,
    MID_Julia7,
    MID_Julia8,
    // Palettes
    MID_Rainbow,
    MID_RainbowRev,
    MID_Red,
    MID_Green,
    MID_Blue,
    MID_BlackAndWhite,
    MID_BlackAndWhiteRev
};

GuiWindow::GuiWindow()
{
    Object* menu = IIntuition->NewObject(nullptr, "menuclass",
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
                MA_Label, "Julia 1",
                MA_ID, MID_Julia1,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Julia 2",
                MA_ID, MID_Julia2,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Julia 3",
                MA_ID, MID_Julia3,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Julia 4",
                MA_ID, MID_Julia4,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Julia 5",
                MA_ID, MID_Julia5,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Julia 6",
                MA_ID, MID_Julia6,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Julia 7",
                MA_ID, MID_Julia7,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Julia 8",
                MA_ID, MID_Julia8,
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
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Red",
                MA_ID, MID_Red,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Green",
                MA_ID, MID_Green,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Blue",
                MA_ID, MID_Blue,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Black and white",
                MA_ID, MID_BlackAndWhite,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Black and white rev.",
                MA_ID, MID_BlackAndWhiteRev,
                TAG_DONE),
            TAG_DONE),
        // The end
        TAG_DONE);

    if (!menu) {
        logging::Error("Failed to create menus");
    }

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
                Set(EFlag::Refresh);
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
        switch(id) {
            case MID_Quit:
                running = false;
                break;
            case MID_ResetView:
                ResetView();
                break;

            // Fractals

            case MID_Mandelbrot:
                fractal = EFractal::Mandelbrot;
                break;
            case MID_Julia1:
                fractal = EFractal::Julia1;
                break;
            case MID_Julia2:
                fractal = EFractal::Julia2;
                break;
            case MID_Julia3:
                fractal = EFractal::Julia3;
                break;
            case MID_Julia4:
                fractal = EFractal::Julia4;
                break;
            case MID_Julia5:
                fractal = EFractal::Julia5;
                break;
            case MID_Julia6:
                fractal = EFractal::Julia6;
                break;
            case MID_Julia7:
                fractal = EFractal::Julia7;
                break;
            case MID_Julia8:
                fractal = EFractal::Julia8;
                break;

            // Palettes

            case MID_Rainbow:
                palette = EPalette::Rainbow;
                break;
            case MID_RainbowRev:
                palette = EPalette::RainbowRev;
                break;
            case MID_Red:
                palette = EPalette::Red;
                break;
            case MID_Green:
                palette = EPalette::Green;
                break;
            case MID_Blue:
                palette = EPalette::Blue;
                break;
            case MID_BlackAndWhite:
                palette = EPalette::BlackAndWhite;
                break;
            case MID_BlackAndWhiteRev:
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
                Set(EFlag::Refresh);
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
        position.x += mouseX / static_cast<float>(width/2) / zoom;
        position.y += mouseY / static_cast<float>(height/2) / zoom;
        Set(EFlag::Refresh);
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

    Set(EFlag::Refresh);
}

void GuiWindow::ZoomOut()
{
    constexpr float minZoom { 1.0f };

    zoom /= GetZoomStep();

    if (zoom < minZoom) {
        logging::Log("Cannot zoom further");
        zoom = minZoom;
    }

    Set(EFlag::Refresh);
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

} // fractalnova

