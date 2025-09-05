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
#include "Logger.hpp"
#include "BackBuffer.hpp"
#include "EMenu.hpp"
#include "AboutWindow.hpp"
#include "Version.hpp"

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include <libraries/dos.h>
#include <libraries/keymap.h>
#include <libraries/gadtools.h>

#include <intuition/menuclass.h>
#include <classes/window.h>

#include <cstdio>
#include <stdexcept>

namespace fractalnova {

static const char* const name { NAME_STRING };
static const char* const menuClass { "menuclass" };

static constexpr uint32 Mx(const int index)
{
    return ~(1 << index);
}

Object* GuiWindow::CreateMenu()
{
    menuObject = IIntuition->NewObject(nullptr, menuClass,
        MA_Type, T_ROOT,
        // Main
        MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
            MA_Type, T_MENU,
            MA_Label, "Main",
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "?|About...",
                MA_ID, EMenu::About,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "I|Iconify",
                MA_ID, EMenu::Iconify,
                MA_Disabled, screen,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Q|Quit",
                MA_ID, EMenu::Quit,
                TAG_DONE),
            TAG_DONE),
        // Control
        MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
            MA_Type, T_MENU,
            MA_Label, "Control",
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Reset view",
                MA_ID, EMenu::ResetView,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "VSync",
                MA_ID, EMenu::VSync,
                MA_Toggle, TRUE,
                MA_Selected, vsync,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Fullscreen",
                MA_ID, EMenu::ToggleFullscreen,
                MA_Toggle, TRUE,
                MA_Selected, fullscreen,
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Iterations",
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "100",
                    MA_ID, EMenu::Iterations100,
                    MA_Selected, iterations == 100,
                    MA_MX, Mx(0),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "200",
                    MA_ID, EMenu::Iterations200,
                    MA_Selected, iterations == 200,
                    MA_MX, Mx(1),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "300",
                    MA_ID, EMenu::Iterations300,
                    MA_Selected, iterations == 300,
                    MA_MX, Mx(2),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "400",
                    MA_ID, EMenu::Iterations400,
                    MA_Selected, iterations == 400,
                    MA_MX, Mx(3),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "500",
                    MA_ID, EMenu::Iterations500,
                    MA_Selected, iterations == 500,
                    MA_MX, Mx(4),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "600",
                    MA_ID, EMenu::Iterations600,
                    MA_Selected, iterations == 600,
                    MA_MX, Mx(5),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "700",
                    MA_ID, EMenu::Iterations700,
                    MA_Selected, iterations == 700,
                    MA_MX, Mx(6),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "800",
                    MA_ID, EMenu::Iterations800,
                    MA_Selected, iterations == 800,
                    MA_MX, Mx(7),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "900",
                    MA_ID, EMenu::Iterations900,
                    MA_Selected, iterations == 900,
                    MA_MX, Mx(8),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "1000",
                    MA_ID, EMenu::Iterations1000,
                    MA_Selected, iterations == 1000,
                    MA_MX, Mx(9),
                    TAG_DONE),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                MA_Type, T_ITEM,
                MA_Label, "Log level",
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "Detail",
                    MA_ID, EMenu::LogDetail,
                    MA_Selected, logging::Level() == logging::ELevel::Detail,
                    MA_MX, Mx(0),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "Debug",
                    MA_ID, EMenu::LogDebug,
                    MA_Selected, logging::Level() == logging::ELevel::Debug,
                    MA_MX, Mx(1),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "Info",
                    MA_ID, EMenu::LogInfo,
                    MA_Selected, logging::Level() == logging::ELevel::Info,
                    MA_MX, Mx(2),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "Warning",
                    MA_ID, EMenu::LogWarning,
                    MA_Selected, logging::Level() == logging::ELevel::Warning,
                    MA_MX, Mx(3),
                    TAG_DONE),
                MA_AddChild, IIntuition->NewObject(nullptr, "menuclass",
                    MA_Type, T_ITEM,
                    MA_Label, "Error",
                    MA_ID, EMenu::LogError,
                    MA_Selected, logging::Level() == logging::ELevel::Error,
                    MA_MX, Mx(4),
                    TAG_DONE),
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
                MA_Selected, fractal == EFractal::Mandelbrot,
                MA_MX, Mx(0),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 1",
                MA_ID, EMenu::Julia1,
                MA_Selected, fractal == EFractal::Julia1,
                MA_MX, Mx(1),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 2",
                MA_ID, EMenu::Julia2,
                MA_Selected, fractal == EFractal::Julia2,
                MA_MX, Mx(2),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 3",
                MA_ID, EMenu::Julia3,
                MA_Selected, fractal == EFractal::Julia3,
                MA_MX, Mx(3),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 4",
                MA_ID, EMenu::Julia4,
                MA_Selected, fractal == EFractal::Julia4,
                MA_MX, Mx(4),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 5",
                MA_ID, EMenu::Julia5,
                MA_Selected, fractal == EFractal::Julia5,
                MA_MX, Mx(5),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 6",
                MA_ID, EMenu::Julia6,
                MA_Selected, fractal == EFractal::Julia6,
                MA_MX, Mx(6),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 7",
                MA_ID, EMenu::Julia7,
                MA_Selected, fractal == EFractal::Julia7,
                MA_MX, Mx(7),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 8",
                MA_ID, EMenu::Julia8,
                MA_Selected, fractal == EFractal::Julia8,
                MA_MX, Mx(8),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 9",
                MA_ID, EMenu::Julia9,
                MA_Selected, fractal == EFractal::Julia9,
                MA_MX, Mx(9),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Julia 10",
                MA_ID, EMenu::Julia10,
                MA_Selected, fractal == EFractal::Julia10,
                MA_MX, Mx(10),
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
                MA_Selected, palette == EPalette::Rainbow,
                MA_MX, Mx(0),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Rainbow rev.",
                MA_ID, EMenu::RainbowRev,
                MA_Selected, palette == EPalette::RainbowRev,
                MA_MX, Mx(1),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Red",
                MA_ID, EMenu::Red,
                MA_Selected, palette == EPalette::Red,
                MA_MX, Mx(2),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Green",
                MA_ID, EMenu::Green,
                MA_Selected, palette == EPalette::Green,
                MA_MX, Mx(3),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Blue",
                MA_ID, EMenu::Blue,
                MA_Selected, palette == EPalette::Blue,
                MA_MX, Mx(4),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Black and white",
                MA_ID, EMenu::BlackAndWhite,
                MA_Selected, palette == EPalette::BlackAndWhite,
                MA_MX, Mx(5),
                TAG_DONE),
            MA_AddChild, IIntuition->NewObject(nullptr, menuClass,
                MA_Type, T_ITEM,
                MA_Label, "Black and white rev.",
                MA_ID, EMenu::BlackAndWhiteRev,
                MA_Selected, palette == EPalette::BlackAndWhiteRev,
                MA_MX, Mx(6),
                TAG_DONE),
            TAG_DONE),
        // The end
        TAG_DONE);

    if (!menuObject) {
        logging::Error("Failed to create menus");
    }

    return menuObject;
}

uint32 GuiWindow::IdcmpHook(Hook* hook, APTR window __attribute__((unused)), IntuiMessage* msg)
{
    auto thisPtr = reinterpret_cast<GuiWindow*>(hook->h_Data);

    switch (msg->Class) {
        case IDCMP_MOUSEBUTTONS:
            thisPtr->HandleMouseButtons(msg->Code);
            break;
        case IDCMP_MOUSEMOVE:
            thisPtr->HandleMouseMove(msg->MouseX, msg->MouseY);
            break;
        case IDCMP_EXTENDEDMOUSE:
            thisPtr->HandleExtendedMouse(reinterpret_cast<struct IntuiWheelData *>(msg->IAddress));
            break;
        default:
            logging::Error("Unknown message %lu", msg->Class);
            break;
    }

    return 0;
}

static char* GetApplicationName()
{
    static constexpr int maxPathLen { 1024 };
    static char pathBuffer[maxPathLen];

    if (!IDOS->GetCliProgramName(pathBuffer, maxPathLen - 1)) {
        //logging::Error("Failed to get CLI program name, checking task node");
        snprintf(pathBuffer, maxPathLen, "%s", reinterpret_cast<Node *>(IExec->FindTask(nullptr))->ln_Name);
    }

    return pathBuffer;
}

static DiskObject* MyGetDiskObject()
{
    BPTR oldDir = IDOS->SetCurrentDir(IDOS->GetProgramDir());
    DiskObject* diskObject = IIcon->GetDiskObject(GetApplicationName());

    if (diskObject) {
        diskObject->do_CurrentX = NO_ICON_POSITION;
        diskObject->do_CurrentY = NO_ICON_POSITION;
    }

    IDOS->SetCurrentDir(oldDir);

    return diskObject;
}

static ULONG FindScreenMode(const uint32 screenWidth, const uint32 screenHeight)
{
    ULONG found = INVALID_ID;

    if (screenWidth && screenHeight) {
        ULONG id = INVALID_ID;

        while ((id = IGraphics->NextDisplayInfo(id)) != INVALID_ID) {
            APTR handle = IGraphics->FindDisplayInfo(id);
            if (!handle) {
                logging::Error("Failed to find display info");
                break;
            }

            DimensionInfo dimInfo;

            if (!IGraphics->GetDisplayInfoData(handle, reinterpret_cast<UBYTE *>(&dimInfo), sizeof(dimInfo), DTAG_DIMS, 0)) {
                logging::Error("Failed to get dim info");
                break;
            }

            DisplayInfo dispInfo;

            if (!IGraphics->GetDisplayInfoData(handle, reinterpret_cast<UBYTE *>(&dispInfo), sizeof(dispInfo), DTAG_DISP, 0)) {
                logging::Error("Failed to get disp info");
                break;
            }

            if ((dispInfo.PropertyFlags & DIPF_IS_RTG) && dimInfo.MaxDepth >= 24) {
                const uint32 w = dimInfo.Nominal.MaxX - dimInfo.Nominal.MinX + 1;
                const uint32 h = dimInfo.Nominal.MaxY - dimInfo.Nominal.MinY + 1;

                logging::Debug("Found screen mode %lu: width %lu, height %lu, depth %u", id, w, h, dimInfo.MaxDepth);

                if (screenWidth == w && screenHeight == h) {
                    logging::Debug("Match!");
                    found = id;
                    break;
                }
            }
        }
    }

    return found;
}

void GuiWindow::CreateScreen()
{
    if (screen) {
        logging::Error("Screen exists already");
        return;
    }

    const ULONG id = FindScreenMode(screenSize.width, screenSize.height);

    if (id == INVALID_ID) {
        logging::Debug("Try to open Workbench-like screen");

        screen = IIntuition->OpenScreenTags(nullptr,
            SA_LikeWorkbench, TRUE,
            SA_ShowTitle, FALSE,
            TAG_DONE);
    } else {
        logging::Debug("Try to open requested screen");

        screen = IIntuition->OpenScreenTags(nullptr,
            SA_ShowTitle, FALSE,
            SA_DisplayID, id,
            SA_LikeWorkbench, TRUE,
            TAG_DONE);
    }

    if (!screen) {
        logging::Error("Failed to open screen");
        // Fullscreen is not essential for the application, hence no exception!
    }
}

void GuiWindow::SetLimits() const
{
    // No idea why, but WA_MinWidth & WA_MinHeight don't seem to work. Shaderjoy
    // had this same workaround.
    constexpr int minWidth = 200;
    constexpr int minHeight = 200;

    const auto minW = minWidth + window->BorderLeft + window->BorderRight;
    const auto minH = minHeight + window->BorderTop + window->BorderBottom;

    if (!IIntuition->WindowLimits(window, minW, minH, -1, -1)) {
        logging::Debug("Failed to set window limits (%d, %d) - (max, max)",
                       minWidth, minHeight);
    }
}

void GuiWindow::CreateWindow()
{
    static Hook idcmpHook {
        { 0, 0 },
        reinterpret_cast<HOOKFUNC>(reinterpret_cast<void *>(IdcmpHook)),
        nullptr,
        this
    };

    Screen* pubScreen = nullptr;

    // TODO: FPS counter in fullscreen?
    if (fullscreen) {
        CreateScreen();
    }

    if (screen) {
        windowSize.width = screen->Width;
        windowSize.height = screen->Height;
    } else {
        pubScreen = IIntuition->LockPubScreen(nullptr);
    }

    //logging::Info("public screen %p", static_cast<void*>(pubScreen));

    windowObject = IIntuition->NewObject(nullptr, "window.class",
        WA_Activate, TRUE,
        WA_Title, !fullscreen ? name : nullptr,
        WA_ScreenTitle, name,
        WA_PubScreen, pubScreen ? pubScreen : screen,
        WA_BackFill, LAYERS_NOBACKFILL,
        WA_InnerWidth, windowSize.width,
        WA_InnerHeight, windowSize.height,
        WA_Flags, WFLG_REPORTMOUSE | WFLG_NEWLOOKMENUS,
        WA_IDCMP, /*IDCMP_REFRESHWINDOW |*/ IDCMP_NEWSIZE | IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE |
                  IDCMP_DELTAMOVE | IDCMP_EXTENDEDMOUSE | IDCMP_RAWKEY | IDCMP_MENUPICK,
        WA_CloseGadget, !fullscreen,
        WA_DragBar, !fullscreen,
        WA_DepthGadget, !fullscreen,
        WA_SizeGadget, !fullscreen,
        WA_MenuStrip, CreateMenu(),
        WA_SimpleRefresh, TRUE,
        WA_Borderless, fullscreen,
        WINDOW_IconifyGadget, !fullscreen,
        WINDOW_Icon, MyGetDiskObject(),
        WINDOW_AppPort, appPort, // For Iconification
        WINDOW_IDCMPHook, &idcmpHook,
        WINDOW_IDCMPHookBits, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_EXTENDEDMOUSE,
        WINDOW_Position, WPOS_CENTERSCREEN,
        TAG_DONE);

    if (!windowObject) {
        throw std::runtime_error("Failed to create window");
    }

    window = reinterpret_cast<Window *>(IIntuition->IDoMethod(windowObject, WM_OPEN));

    if (pubScreen) {
        IIntuition->UnlockPubScreen(nullptr, pubScreen);
    }

    if (!window) {
        throw std::runtime_error("Failed to open window");
    }

    SetLimits();
}

void GuiWindow::DestroyWindow()
{
    if (windowObject) {
        IIntuition->DisposeObject(windowObject);
        windowObject = nullptr;
        window = nullptr;
    }

    if (menuObject) {
        IIntuition->DisposeObject(menuObject);
        menuObject = nullptr;
    }

    if (screen) {
        IIntuition->CloseScreen(screen);
        screen = nullptr;
    }
}

GuiWindow::GuiWindow(const Params& params):
    vsync(params.vsync),
    fullscreen(params.fullscreen),
    screenSize(params.screenSize),
    windowSize(params.windowSize),
    iterations(params.iterations)
{
    logging::Debug("Create GuiWindow");

    appPort = static_cast<MsgPort *>(IExec->AllocSysObjectTags(ASOT_PORT, TAG_DONE));

    if (!appPort) {
        throw std::runtime_error("Failed to create app port");
    }

    CreateWindow();
}

GuiWindow::~GuiWindow()
{
    DestroyWindow();

    if (appPort) {
        IExec->FreeSysObject(ASOT_PORT, appPort);
        appPort = nullptr;
    }
}

bool GuiWindow::Run()
{
    bool running { true };

    position = { 0.0f, 0.0f };
    flags.reset();

    if (!window) {
        // When iconified, wait for some event to save CPU
        uint32 winSig = 0;
        if (!IIntuition->GetAttr(WINDOW_SigMask, windowObject, &winSig)) {
		    logging::Error("GetAttr failed on line %d", __LINE__);
        }

        const ULONG signals = IExec->Wait(winSig | SIGBREAKF_CTRL_C);

        if (signals & SIGBREAKF_CTRL_C) {
            logging::Debug("Control-C while iconified");
            running = false;
        }
    }

    uint32 result;
    int16 code = 0;

    while ((result = IIntuition->IDoMethod(windowObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG) {
        switch (result & WMHI_CLASSMASK) {
            case WMHI_CLOSEWINDOW:
                running = false;
                break;
            case WMHI_MENUPICK:
                running = HandleMenuPick();
                break;
            case WMHI_NEWSIZE:
                HandleNewSize();
                break;
            case WMHI_ICONIFY:
                HandleIconify();
                break;
            case WMHI_UNICONIFY:
                HandleUniconify();
                break;
            case WMHI_RAWKEY:
                running = HandleRawKey();
                break;
            case WMHI_MOUSEMOVE:
            case WMHI_MOUSEBUTTONS:
                // Handled in the hook
                break;
            default:
                logging::Debug("Unknown event %lX, code %d", result,  code);
                break;
        }
    }

    if (Flagged(EFlag::ToggleFullscreen)) {
        DestroyWindow();
        CreateWindow();
    }

    if (IExec->SetSignal(0, SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) {
        logging::Debug("Control-C");
        running = false;
    }

    return running;
}

void GuiWindow::HandleExtendedMouse(const struct IntuiWheelData * data)
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

    EMenu id = EMenu::NoMenuId;

    const EFractal currentFractal = fractal;

    auto menu = reinterpret_cast<Object *>(window->MenuStrip);

    while ((id = static_cast<EMenu>(IIntuition->IDoMethod(menu, MM_NEXTSELECT, 0, id))) != EMenu::NoMenuId) {
        switch (id) {
            case EMenu::About:
                ShowAboutWindow();
                break;

            case EMenu::Iconify:
                HandleIconify();
                break;

            case EMenu::Quit:
                running = false;
                break;

            // Control
            case EMenu::ResetView:
                ResetView();
                break;

            case EMenu::VSync:
                ToggleVSync();
                break;

            case EMenu::ToggleFullscreen:
                ToggleFullscreen();
                break;

            case EMenu::Iterations100:
                iterations = 100;
                break;
            case EMenu::Iterations200:
                iterations = 200;
                break;
            case EMenu::Iterations300:
                iterations = 300;
                break;
            case EMenu::Iterations400:
                iterations = 400;
                break;
            case EMenu::Iterations500:
                iterations = 500;
                break;
            case EMenu::Iterations600:
                iterations = 600;
                break;
            case EMenu::Iterations700:
                iterations = 700;
                break;
            case EMenu::Iterations800:
                iterations = 800;
                break;
            case EMenu::Iterations900:
                iterations = 900;
                break;
            case EMenu::Iterations1000:
                iterations = 1000;
                break;

            case EMenu::LogDetail:
            case EMenu::LogDebug:
            case EMenu::LogInfo:
            case EMenu::LogWarning:
            case EMenu::LogError:
                ToggleLogLevel(id);
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
                logging::Error("Unhandled menu ID %lu", static_cast<uint32>(id));
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
        position.x += static_cast<float>(mouseX) / static_cast<float>(windowSize.width) / zoom;
        position.y += static_cast<float>(mouseY) / static_cast<float>(windowSize.height) / zoom;
    }
}

void GuiWindow::HandleNewSize()
{
    if ((IIntuition->GetWindowAttrs(window,
        WA_InnerWidth, &windowSize.width,
        WA_InnerHeight, &windowSize.height,
        TAG_DONE)) != 0)
    {
        throw std::runtime_error("Failed to get window attributes");
    }

    Set(EFlag::Resize);
}

bool GuiWindow::HandleRawKey()
{
	InputEvent *ie;

	if (!IIntuition->GetAttr(WINDOW_InputEvent, windowObject, reinterpret_cast<ULONG *>(&ie))) {
		logging::Error("GetAttr failed on line %d", __LINE__);
		return 0;
	}

    const auto code = ie->ie_Code;

    bool running { true };

    // TODO: cursor panning? rotation (is problematic with panning)
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

void GuiWindow::HandleIconify()
{
    //if (!screen) {
        window = nullptr;
        IIntuition->IDoMethod(windowObject, WM_ICONIFY);
    //}
}

void GuiWindow::HandleUniconify()
{
    window = reinterpret_cast<Window *>(IIntuition->IDoMethod(windowObject, WM_OPEN));

    if (!window) {
        throw std::runtime_error("Failed to reopen window");
    }
}

void GuiWindow::SetTitle(const char* title)
{
    if (window) {
        IIntuition->SetWindowTitles(window, title, name);
    }
}

void GuiWindow::ClearPosition()
{
    position = { 0.0f, 0.0f };
}

float GuiWindow::GetZoomStep() const
{
    constexpr float zoomStep { 1.01f };

    return fastZoom ? 2.0f * zoomStep : zoomStep;
}

void GuiWindow::ZoomIn()
{
    constexpr float maxZoom { 100000.0f };

    zoom *= GetZoomStep();

    if (zoom > maxZoom) {
        logging::Debug("Cannot zoom closer");
        zoom = maxZoom;
    }
}

void GuiWindow::ZoomOut()
{
    constexpr float minZoom { 1.0f };

    zoom /= GetZoomStep();

    if (zoom < minZoom) {
        logging::Debug("Cannot zoom further");
        zoom = minZoom;
    }
}

void GuiWindow::ResetView()
{
    zoom = 1.0f;
    Set(EFlag::Reset);
}

bool GuiWindow::Flagged(const EFlag flag) const
{
    return flags.test(static_cast<std::size_t>(flag));
}

void GuiWindow::Set(const EFlag flag)
{
    flags.set(static_cast<std::size_t>(flag));
}

void GuiWindow::Clear(const EFlag flag)
{
    flags.reset(static_cast<std::size_t>(flag));
}

void GuiWindow::Draw(const BackBuffer* backBuffer) const
{
    if (window) {
        if (vsync) {
            IGraphics->WaitTOF();
        }

        const std::uint32_t winw = window->Width - (window->BorderLeft + window->BorderRight);
        const std::uint32_t winh = window->Height - (window->BorderTop + window->BorderBottom);

        IGraphics->BltBitMapRastPort(backBuffer->Data(), 0, 0, window->RPort,
            window->BorderLeft,
            window->BorderTop,
            static_cast<WORD>(std::min(winw, windowSize.width)),
            static_cast<WORD>(std::min(winh, windowSize.height)),
            0xC0);
    }
}

void GuiWindow::ToggleMenuItem(const EMenu id, const bool state)
{
    auto menu = reinterpret_cast<Object *>(window->MenuStrip);

    if (!IIntuition->IDoMethod(menu, MM_SETSTATE, 0, id, MS_CHECKED, state ? MS_CHECKED : 0)) {
        logging::Error("Invalid menu id %d", static_cast<int>(id));
    }
}

void GuiWindow::ToggleVSync()
{
    vsync = !vsync;

    ToggleMenuItem(EMenu::VSync, vsync);
}

void GuiWindow::ToggleFullscreen()
{
    fullscreen = !fullscreen;

    if (fullscreen) {
        oldWindowSize = windowSize;
    } else {
        windowSize = oldWindowSize;
    }

    ToggleMenuItem(EMenu::ToggleFullscreen, fullscreen);

    Set(EFlag::Resize);
    Set(EFlag::ToggleFullscreen);
}

void GuiWindow::ToggleLogLevel(const EMenu id)
{
    auto logLevel = logging::ELevel::Info;

    switch (id) {
        case EMenu::LogDetail:
            logLevel = logging::ELevel::Detail;
            break;
        case EMenu::LogDebug:
            logLevel = logging::ELevel::Debug;
            break;
        case EMenu::LogInfo:
            logLevel = logging::ELevel::Info;
            break;
        case EMenu::LogWarning:
            logLevel = logging::ELevel::Warning;
            break;
        case EMenu::LogError:
            logLevel = logging::ELevel::Error;
            break;
        default:
            logging::Error("Unknown menu item %d", static_cast<int>(id));
            break;
    }

    logging::SetLevel(logLevel);
}

void GuiWindow::ShowAboutWindow()
{
    AboutWindow aw { window };
}

} // fractalnova
