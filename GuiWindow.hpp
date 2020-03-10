#pragma once

#include "Vertex.hpp"
#include "EFractal.hpp"
#include "EPalette.hpp"

#include <proto/intuition.h>

namespace fractalnova {

struct GuiWindow
{
    GuiWindow();
    ~GuiWindow();

    bool Run();
    void HandleExtendedMouse(struct IntuiWheelData* data);
    bool HandleMenuPick();
    void HandleMouseButtons(UWORD code);
    void HandleMouseMove(int mouseX, int mouseY);
    void HandleNewSize();
    bool HandleRawKey(UWORD code);

    void SetTitle(const char* title);

    Vertex GetPosition() const;
    void ClearPosition();
    float GetZoom() const;

    void ResetView();

    float GetZoomStep() const;
    void ZoomIn();
    void ZoomOut();

    struct Window* window { nullptr };

    uint32 Width() const;
    uint32 Height() const;

    EFractal GetFractal() const;
    EPalette GetPalette() const;

    mutable bool resize { false };
    mutable bool refresh { true };
    mutable bool reset { false };

    bool panning { false };
    bool fastZoom { false };

    Vertex position { };
    float zoom { 1.0f };
    uint32 width { 800 };
    uint32 height { 600 };

    EFractal fractal { EFractal::Mandelbrot };
    EPalette palette { EPalette::Rainbow };
};

} // fractalnova
