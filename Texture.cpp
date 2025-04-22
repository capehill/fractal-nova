#include "Texture.hpp"
#include "Logger.hpp"

namespace fractalnova {

static constexpr bool textureFiltering { false };
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

    errCode = context->TSSetParametersTags(sampler,
        W3DN_TEXTURE_MIN_FILTER, textureFiltering ? W3DN_LINEAR : W3DN_NEAREST,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to set texture sampler parameters");

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

} // fractalnova
