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

#include "Palette.hpp"
#include "Logger.hpp"

#include <cstdio>
#include <stdexcept>

namespace fractalnova {

Palette::Palette(const EPalette palette, const int size): size(size)
{
    logging::Debug("Create Palette %d, size %d", static_cast<int>(palette), size);
    Create(palette);
}

void Palette::Rainbow()
{
    Add( {   0,   0,   0 }, 1.0f );
    Add( { 255,   0,   0 }, 1.0f );
    Add( { 255, 127,   0 }, 1.0f );
    Add( { 255, 255,   0 }, 1.0f );
    Add( {   0, 255,   0 }, 1.0f );
    Add( {   0,   0, 255 }, 1.0f );
    Add( {  75,   0, 130 }, 1.0f );
    Add( { 148,   0, 211 }, 1.0f );
}

void Palette::RainbowRev()
{
    Add( {   0,   0,   0 }, 1.0f );
    Add( { 148,   0, 211 }, 1.0f );
    Add( {  75,   0, 130 }, 1.0f );
    Add( {   0,   0, 255 }, 1.0f );
    Add( {   0, 255,   0 }, 1.0f );
    Add( { 255, 255,   0 }, 1.0f );
    Add( { 255, 127,   0 }, 1.0f );
    Add( { 255,   0,   0 }, 1.0f );
}

void Palette::Red()
{
    Add( {   0,   0,   0 }, 1.0f );
    Add( { 255,   0,   0 }, 1.0f );
}

void Palette::Green()
{
    Add( {   0,   0,   0 }, 1.0f );
    Add( {   0, 255,   0 }, 1.0f );
}

void Palette::Blue()
{
    Add( {   0,   0,   0 }, 1.0f );
    Add( {   0,   0, 255 }, 1.0f );
}

void Palette::BlackAndWhite()
{
    Add( {   0,   0,   0 }, 1.0f );
    Add( { 255, 255, 255 }, 1.0f );
}

void Palette::BlackAndWhiteRev()
{
    Add( { 255, 255, 255 }, 1.0f );
    Add( {   0,   0,   0 }, 1.0f );
}

void Palette::Create(const EPalette palette)
{
    switch (palette) {
        case EPalette::Rainbow:
            return Rainbow();
        case EPalette::RainbowRev:
            return RainbowRev();
        case EPalette::Red:
            return Red();
        case EPalette::Green:
            return Green();
        case EPalette::Blue:
            return Blue();
        case EPalette::BlackAndWhite:
            return BlackAndWhite();
        case EPalette::BlackAndWhiteRev:
            return BlackAndWhiteRev();
        default:
            logging::Error("Unknown palette %d", static_cast<int>(palette));
            return Rainbow();
    }
}

void Palette::Add(const Color& c, const float weight)
{
    weightedColors.emplace_back(WeightedColor { c, weight });
}

std::vector<Color> Palette::GetColorArray()
{
    float weight = 0.0f;

    for (auto wc: weightedColors) {
        weight += wc.w;
    }

    logging::Debug("Palette size %d, total weight %f", size, weight);

    if (weight <= 0.0f || weightedColors.size() < 2) {
        throw std::runtime_error("Invalid weight");
    }

    // Make it cyclic
    weightedColors.emplace_back(weightedColors.front());

    for (std::size_t i = 0; i < weightedColors.size() - 1; i++) {

        const float flen = weightedColors[i].w / weight * static_cast<float>(size);
        const int len = static_cast<int>(flen);

        const Color& c1 = weightedColors[i].c;
        const Color& c2 = weightedColors[i + 1].c;

        const float r = (c2.r - c1.r) / flen;
        const float g = (c2.g - c1.g) / flen;
        const float b = (c2.b - c1.b) / flen;

        for (int j = 0; j < len; j++) {
            const auto jf = static_cast<float>(j);
            colors.emplace_back(Color {
                static_cast<uint8_t>(c1.r + jf * r),
                static_cast<uint8_t>(c1.g + jf * g),
                static_cast<uint8_t>(c1.b + jf * b)
                });
        }
    }

    return colors;
}

} // fractalnova
