#pragma once

#include <proto/warp3dnova.h>

namespace fractalnova {

struct GuiWindow;

struct NovaContext
{
    NovaContext(const GuiWindow& window);
    ~NovaContext();

    void CloseLib();

    void Resize();
    void Clear();
    void SwapBuffers();

    struct BitMap* backBuffer { nullptr };
    W3DN_Context* context { nullptr };

    const GuiWindow& window;
    uint32 width { 0 };
    uint32 height { 0 };
};

} // fractalnova
