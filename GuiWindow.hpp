#pragma once

#include <proto/intuition.h>

namespace fractalnova {

struct GuiWindow
{
    GuiWindow();
    ~GuiWindow();

    bool Run();
    void SetTitle(const char* title);

    struct Window* window { nullptr };

    mutable bool resize { false };
    mutable bool refresh { true };
};

} // fractalnova
