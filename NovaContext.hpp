#pragma once

#include "NovaObject.hpp"
#include "EFractal.hpp"
#include "EPalette.hpp"

#include <Warp3DNova/Context.h>

#include <memory>

namespace fractalnova {

struct GuiWindow;

struct Texture;
struct Program;
struct BackBuffer;
struct Vertex;
struct Palette;

struct NovaContext: public NovaObject
{
    NovaContext(const GuiWindow& window, bool vsync, int iterations);
    ~NovaContext();

    void CloseLib();

    void CreateTexture(Palette& palette);
    void CreateRainbowPalette();
    void CreateRainbowRevPalette();

    void Resize();
    void Clear() const;
    void Draw() const;
    void SwapBuffers();

    void SetPosition(const Vertex& position);
    void SetZoom(float zoom);
    void Reset();

    void UseProgram(EFractal fractal);
    void UsePalette(EPalette palette);

    std::unique_ptr<BackBuffer> backBuffer;
    std::unique_ptr<Program> program;
    std::unique_ptr<Texture> texture;

    const GuiWindow& window;
    uint32 width { 0 };
    uint32 height { 0 };

    const int iterations;

    bool vsync { false };
};

} // fractalnova
