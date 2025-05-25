/*
Copyright (C) 2020-2025 Juha Niemimäki

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
    void SetIterations(int iterations);
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

    int iterations { 100 };
};

} // fractalnova
