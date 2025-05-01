#pragma once

#include "Vertex.hpp"
#include "EFractal.hpp"
#include "EPalette.hpp"
#include "Logger.hpp"

#include <proto/intuition.h>

namespace fractalnova {

enum class EFlag
{
    Resize = 1,
    Reset = 2
};

class BackBuffer;
enum class EMenu;

struct Resolution
{
    uint32 width { 640 };
    uint32 height { 480 };
};

struct Params
{
    bool vsync { false };
    bool fullscreen { false };
    bool lazyClear { false };
    int iterations { 100 };

    Resolution windowSize {};
    Resolution screenSize {};
};

class GuiWindow
{
public:
    GuiWindow(const Params& params);
    ~GuiWindow();

    bool Run();
    void Draw(const BackBuffer* backBuffer) const;

    void SetTitle(const char* title);

    Vertex GetPosition() const { return position; }
    void ClearPosition();

    float GetZoom() const { return zoom; }
    int GetIterations() const { return iterations; }

    uint32 Width() const { return windowSize.width; }
    uint32 Height() const { return windowSize.height; }

    EFractal GetFractal() const { return fractal; };
    EPalette GetPalette() const { return palette; }

    bool Flagged(EFlag flag) const;
    void Set(EFlag flag);
    void Clear(EFlag flag);

    Window* WindowPtr() const { return window; }

private:
    void CreateScreen();
    Object* CreateMenu();

    void HandleExtendedMouse(const struct IntuiWheelData* data);
    bool HandleMenuPick();
    void HandleMouseButtons(UWORD code);
    void HandleMouseMove(int mouseX, int mouseY);
    void HandleNewSize();
    bool HandleRawKey();
    void HandleIconify();
    void HandleUniconify();

    void ShowAboutWindow();

    void ResetView();

    float GetZoomStep() const;
    void ZoomIn();
    void ZoomOut();

    void ToggleMenuItem(const EMenu id, bool state);
    void ToggleVSync();
    void ToggleLogLevel(const EMenu id);

    static uint32 IdcmpHook(Hook* hook, APTR window, IntuiMessage* msg);

    Object* windowObject { nullptr };
    Object* menuObject { nullptr };
    Window* window { nullptr };
    MsgPort* appPort { nullptr };
    Screen* screen { nullptr };

    bool panning { false };
    bool fastZoom { false };
    bool vsync { false };
    bool fullscreen { false };

    Vertex position { };
    float zoom { 1.0f };

    Resolution screenSize {};
    Resolution windowSize {};

    EFractal fractal { EFractal::Mandelbrot };
    EPalette palette { EPalette::Rainbow };

    uint32 flags { 0 };
    int iterations { 100 };
};

} // fractalnova
