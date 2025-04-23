#pragma once

#include "Vertex.hpp"
#include "EFractal.hpp"
#include "EPalette.hpp"
#include "Logger.hpp"
#include "EMenu.hpp"

#include <proto/intuition.h>

namespace fractalnova {

enum class EFlag
{
    Resize = 1,
    Reset = 2
};

class BackBuffer;

class GuiWindow
{
public:
    GuiWindow();
    ~GuiWindow();

    bool Run();
    void Draw(const BackBuffer* backBuffer) const;

    void SetTitle(const char* title);

    Vertex GetPosition() const;
    void ClearPosition();
    float GetZoom() const;

    uint32 Width() const;
    uint32 Height() const;

    EFractal GetFractal() const;
    EPalette GetPalette() const;

    bool Flagged(EFlag flag) const;
    void Set(EFlag flag);
    void Clear(EFlag flag);

    Window* WindowPtr() const;

private:
    Object* CreateMenu();

    void HandleExtendedMouse(const struct IntuiWheelData* data);
    bool HandleMenuPick();
    void HandleMouseButtons(UWORD code);
    void HandleMouseMove(int mouseX, int mouseY);
    void HandleNewSize();
    bool HandleRawKey();
    void HandleIconify();
    void HandleUniconify();

    void ResetView();

    float GetZoomStep() const;
    void ZoomIn();
    void ZoomOut();

    void ToggleLogLevel(const EMenu id);

    static uint32 IdcmpHook(Hook* hook, APTR window, IntuiMessage* msg);

    Object* windowObject { nullptr };
    Window* window { nullptr };
    MsgPort* appPort { nullptr };

    bool panning { false };
    bool fastZoom { false };

    Vertex position { };
    float zoom { 1.0f };
    uint32 width { 800 };
    uint32 height { 600 };

    EFractal fractal { EFractal::Mandelbrot };
    EPalette palette { EPalette::Rainbow };

    logging::ELevel logLevel { logging::ELevel::Info };

    uint32 flags { 0 };
};

} // fractalnova
