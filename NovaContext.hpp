#pragma once

#include "NovaObject.hpp"
#include "EFractal.hpp"
#include "EPalette.hpp"

#include <Warp3DNova/Context.h>

#include <memory>

namespace fractalnova {

class GuiWindow;

class Texture;
class Program;
class BackBuffer;
class Vertex;
class Palette;
class VertexBuffer;

class NovaContext: public NovaObject
{
public:

    NovaContext(const GuiWindow& window, int iterations);
    ~NovaContext();

    void Resize();
    void Clear() const;
    void Draw() const;
    void SwapBuffers();

    void SetPosition(const Vertex& position);
    void SetZoom(float zoom);
    void Reset();

    void UseProgram(EFractal fractal);
    void UsePalette(EPalette palette);

private:
    void CloseLib();

    void CreateTexture(Palette& palette);
    void CreateRainbowPalette();
    void CreateRainbowRevPalette();

    std::unique_ptr<BackBuffer> backBuffer;
    std::unique_ptr<Program> program;
    std::unique_ptr<Texture> texture;
    std::unique_ptr<VertexBuffer> vbo;

    const GuiWindow& window;
    uint32 width { 0 };
    uint32 height { 0 };

    const int iterations;
};

} // fractalnova
