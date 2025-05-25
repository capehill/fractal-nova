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

#include "Texture.hpp"
#include "Logger.hpp"

namespace fractalnova {

static constexpr uint32 textureUnit { 0 };

Texture::Texture(W3DN_Context* context, const std::vector<Color>& colors): NovaObject(context)
{
    W3DN_ErrorCode errCode;

    constexpr uint32 height = 1;
    constexpr uint32 depth = 1;
    constexpr BOOL mipmapped = FALSE;

    logging::Debug("Create texture");

    texture = context->CreateTexture(&errCode, W3DN_TEXTURE_2D, W3DNPF_RGBA, W3DNEF_UINT8,
        colors.size(), height, depth, mipmapped, W3DN_STATIC_DRAW);

    ThrowOnError(errCode, "Failed to create texture");

    constexpr uint32 level = 0;
    constexpr uint32 arrayIdx = 0;
    constexpr uint32 srcRowsPerLayer = 0;

    errCode = context->TexUpdateImage(texture, const_cast<Color*>(colors.data()), level, arrayIdx, sizeof(Color) * colors.size(), srcRowsPerLayer);

    ThrowOnError(errCode, "Failed to update texture");

    sampler = context->CreateTexSampler(&errCode);

    ThrowOnError(errCode, "Failed to create texture sampler");

    constexpr bool textureFiltering { true };

    SetFiltering(textureFiltering);

    context->BindTexture(defaultRSO, textureUnit, texture, sampler);

    ThrowOnError(errCode, "Failed to bind texture");
}

Texture::~Texture()
{
    if (texture) {
        constexpr W3DN_Texture* defaultTexture = nullptr;
        constexpr W3DN_TextureSampler* defaultSampler = nullptr;

        context->BindTexture(defaultRSO, textureUnit, defaultTexture, defaultSampler);
        context->DestroyTexture(texture);
        texture = nullptr;
    }

    if (sampler) {
        context->DestroyTexSampler(sampler);
        sampler = nullptr;
    }
}

void Texture::SetFiltering(const bool textureFiltering)
{
    auto errCode = context->TSSetParametersTags(sampler,
        W3DN_TEXTURE_MIN_FILTER, textureFiltering ? W3DN_LINEAR : W3DN_NEAREST,
        W3DN_TEXTURE_MAG_FILTER, textureFiltering ? W3DN_LINEAR : W3DN_NEAREST,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to set texture sampler parameters");
}

} // fractalnova
