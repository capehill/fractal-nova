#include "Palette.hpp"
#include "Logger.hpp"

#include <cstdio>
#include <stdexcept>

namespace fractalnova {

Palette::Palette(const EPalette palette, const int size): size(size)
{
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

void Palette::Create(const EPalette palette)
{
    switch (palette) {
        case EPalette::Rainbow:
            return Rainbow();
        case EPalette::RainbowRev:
            return RainbowRev();
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

    logging::Log("Palette size %d, total weight %f", size, weight);

    if (weight <= 0.0f || weightedColors.size() < 2) {
        throw std::runtime_error("Invalid weight");
    }

    // Make it cyclic
    weightedColors.emplace_back(weightedColors.front());

    for (size_t i = 0; i < weightedColors.size() - 1; i++) {

        const int len = weightedColors[i].w / weight * size;
        const float flen = len;

        const Color& c1 = weightedColors[i].c;
        const Color& c2 = weightedColors[i + 1].c;

        const float r = (c2.r - c1.r) / flen;
        const float g = (c2.g - c1.g) / flen;
        const float b = (c2.b - c1.b) / flen;

        for (int j = 0; j < len; j++) {
            colors.emplace_back(Color {
                static_cast<uint8_t>(c1.r + j * r),
                static_cast<uint8_t>(c1.g + j * g),
                static_cast<uint8_t>(c1.b + j * b)
                });
        }
    }

    return colors;
}

} // fractalnova
