#pragma once

#include <proto/intuition.h>

namespace fractalnova {

struct GuiWindow
{
    GuiWindow();
    ~GuiWindow();

    void run();

    struct Window* window { nullptr };
};

} // fractalnova
