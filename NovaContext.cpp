#include "NovaContext.hpp"
#include "GuiWindow.hpp"
#include "Palette.hpp"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/warp3dnova.h>

#include <stdexcept>
#include <string>
#include <cmath>

// TODO: display coordinate/zoom on window title?
// TODO: shader, texture, vbo, dbo classes?

namespace fractalnova {

struct VertexShaderData {
    float angle;
    float zoom;
    Vertex point;
};

struct FragmentShaderData {
    int32 iterations;
};

namespace {
    static constexpr uint32 posArrayIndex { 0 };
    static constexpr uint32 texCoordArrayIndex { 1 };
    static constexpr uint32 vertexCount { 4 };
    static constexpr bool textureFiltering { false };
    static constexpr float toRadians { M_PI/ 180.0f };
}

struct Warp3DNovaIFace* IW3DNova;
static struct Library* NovaBase;

NovaContext::NovaContext(const GuiWindow& window, const bool verbose, const bool vsync, const int iterations)
    : window(window), verbose(verbose), vsync(vsync), iterations(iterations)
{
    NovaBase = IExec->OpenLibrary("Warp3DNova.library", 1);

    if (NovaBase) {
        IW3DNova = (struct Warp3DNovaIFace *)IExec->GetInterface(NovaBase, "main", 1, nullptr);
    }

    if (!IW3DNova) {
        CloseLib();
        throw std::runtime_error("Failed to open Warp3DNova.library");
    }

    W3DN_ErrorCode errCode;
    context = IW3DNova->W3DN_CreateContextTags(&errCode, W3DNTag_Screen, nullptr, TAG_DONE);

    if (!context) {
        ThrowOnError(errCode, "Failed to create Nova context");
    }

    Resize();
}

NovaContext::~NovaContext()
{
    if (texture) {
        context->BindTexture(nullptr, 0, nullptr, nullptr);
        context->DestroyTexture(texture);
        texture = nullptr;
    }

    if (vertexDbo) {
        context->BindShaderDataBuffer(nullptr, W3DNST_VERTEX, nullptr, 0);
        context->DestroyDataBufferObject(vertexDbo);
        vertexDbo = nullptr;
    }

    if (fragmentDbo) {
        context->BindShaderDataBuffer(nullptr, W3DNST_FRAGMENT, nullptr, 0);
        context->DestroyDataBufferObject(fragmentDbo);
        fragmentDbo = nullptr;
    }

    if (vbo) {
        context->BindVertexAttribArray(nullptr, 0 /* attribNum*/, nullptr, 0 /* arrayIdx */);
        context->BindVertexAttribArray(nullptr, 1 /* attribNum*/, nullptr, 0 /* arrayIdx */);
        context->DestroyVertexBufferObject(vbo);
        vbo = nullptr;
    }

    if (shaderPipeline) {
        context->SetShaderPipeline(nullptr, nullptr);
        context->DestroyShaderPipeline(shaderPipeline);
        shaderPipeline = nullptr;
    }

    if (fragmentShader) {
        context->DestroyShader(fragmentShader);
        fragmentShader = nullptr;
    }

    if (vertexShader) {
        context->DestroyShader(vertexShader);
        vertexShader = nullptr;
    }

    if (context) {
        context->Destroy();
        context = nullptr;
    }

    if (IW3DNova) {
        IExec->DropInterface((struct Interface *)IW3DNova);
        IW3DNova = nullptr;
    }

    CloseLib();

    if (backBuffer) {
        IGraphics->FreeBitMap(backBuffer);
        backBuffer = nullptr;
    }
}

void NovaContext::CloseLib()
{
    if (NovaBase) {
        IExec->CloseLibrary(NovaBase);
        NovaBase = nullptr;
    }
}

W3DN_Shader* NovaContext::CompileShader(const std::string& fileName)
{
    W3DN_ErrorCode errCode;
    const char* shaderLog = nullptr;
    const std::string shaderPath = "shaders/" + fileName;

    W3DN_Shader* shader = context->CompileShaderTags(&errCode,
        W3DNTag_FileName, shaderPath.c_str(),
        W3DNTag_Log, &shaderLog,
        W3DNTag_LogLevel, verbose ? W3DNLL_DEBUG : W3DNLL_INFO,
        TAG_DONE);

    if (!shader) {
        if (shaderLog) {
            printf("Compiling %s failed (%u) with error: %s\n",
               fileName.c_str(), errCode, ErrorToString(errCode).c_str());

            context->DestroyShaderLog(shaderLog);
        }

        ThrowOnError(errCode, "Failed to compile shader " + fileName);
    }

    if (shaderLog) {
        printf("%s compilation log:\n%s\n", fileName.c_str(), shaderLog);
        context->DestroyShaderLog(shaderLog);
    }

    return shader;
}

void NovaContext::LoadShaders()
{
#if 0
    vertexShader = CompileShader("simple.vert.spv");
    fragmentShader = CompileShader("simple.frag.spv");
#else
    vertexShader = CompileShader("mandelbrot.vert.spv");
    fragmentShader = CompileShader("mandelbrot.frag.spv");
#endif

    W3DN_ErrorCode errCode;
    shaderPipeline = context->CreateShaderPipelineTags(&errCode,
        W3DNTag_Shader, vertexShader,
        W3DNTag_Shader, fragmentShader,
        TAG_DONE);

    if (!shaderPipeline) {
        ThrowOnError(errCode, "Failed to create shader pipeline");
    }

    errCode = context->SetShaderPipeline(nullptr, shaderPipeline);

    ThrowOnError(errCode, "Failed to set shader pipeline");
}

void NovaContext::CreateVBO()
{
    constexpr uint32 arrayCount { 2 };

    W3DN_ErrorCode errCode;

    vbo = context->CreateVertexBufferObjectTags(&errCode,
        vertexCount * sizeof(Vertex4), W3DN_STATIC_DRAW, arrayCount, TAG_DONE);

    ThrowOnError(errCode, "Failed create vertex buffer object");

    constexpr uint32 stride { sizeof(Vertex4) };
    constexpr uint32 posElementCount { 2 };
    constexpr uint32 texCoordElementCount { 2 };

    errCode = context->VBOSetArray(vbo, posArrayIndex, W3DNEF_FLOAT, FALSE, posElementCount, stride, 0, vertexCount);

    ThrowOnError(errCode, "Failed to set VBO array");

    errCode = context->VBOSetArray(vbo, texCoordArrayIndex, W3DNEF_FLOAT, FALSE, texCoordElementCount, stride, 0, vertexCount);

    ThrowOnError(errCode, "Failed to set VBO array (texCoord)");

    W3DN_BufferLock* lock = context->VBOLock(&errCode, vbo, 0, 0);

    ThrowOnError(errCode, "Failed to lock vertex buffer object");

    Vertex4* vertices = reinterpret_cast<Vertex4 *>(lock->buffer);

    // Make a quad
    vertices[0].x = -1.0f;
    vertices[0].y = -1.0f;
    vertices[0].s = 0.0f;
    vertices[0].t = 0.0f;

    vertices[1].x = -1.0f;
    vertices[1].y =  1.0f;
    vertices[1].s =  0.0f;
    vertices[1].t =  1.0f;

    vertices[2].x =  1.0f;
    vertices[2].y = -1.0f;
    vertices[2].s =  1.0f;
    vertices[2].t =  1.0f;

    vertices[3].x = 1.0f;
    vertices[3].y = 1.0f;
    vertices[3].s = 1.0f;
    vertices[3].t = 0.0f;

    errCode = context->BufferUnlock(lock, 0, lock->size);

    ThrowOnError(errCode, "Failed to unlock vertex buffer object");
}

void NovaContext::CreateDBO()
{
    CreateVertexDBO();
    CreateFragmentDBO();
}

void NovaContext::CreateVertexDBO()
{
    W3DN_ErrorCode errCode;

    vertexDbo = context->CreateDataBufferObjectTags(&errCode, sizeof(VertexShaderData), W3DN_STREAM_DRAW, 1,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to create data buffer object (vertex)");

    context->DBOSetBufferTags(vertexDbo, 0 /* bufferIdx */, 0 /* offset */, sizeof(VertexShaderData), vertexShader,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to set data buffer object (vertex)");

    context->BindShaderDataBuffer(nullptr, W3DNST_VERTEX, vertexDbo, 0 /* bufferIdx */);

    ThrowOnError(errCode, "Failed to bind data buffer object (vertex)");
}

void NovaContext::CreateFragmentDBO()
{
    W3DN_ErrorCode errCode;

    fragmentDbo = context->CreateDataBufferObjectTags(&errCode, sizeof(FragmentShaderData), W3DN_STREAM_DRAW, 1,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to create data buffer object (fragment)");

    context->DBOSetBufferTags(fragmentDbo, 0 /* bufferIdx */, 0 /* offset */, sizeof(FragmentShaderData), fragmentShader,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to set data buffer object (fragment)");

    context->BindShaderDataBuffer(nullptr, W3DNST_FRAGMENT, fragmentDbo, 0 /* bufferIdx */);

    ThrowOnError(errCode, "Failed to bind data buffer object (fragment)");
}

void NovaContext::UpdateVertexDBO() const
{
    W3DN_ErrorCode errCode;

    static float angle = 0.0f;

    W3DN_BufferLock* lock = context->DBOLock(&errCode, vertexDbo, 0 /* readOffset */, 0 /* readSize */);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object (vertex)");
    }

    VertexShaderData* data = reinterpret_cast<VertexShaderData *>(lock->buffer);

    data->angle = angle * toRadians;
    data->zoom = zoom;
    data->point = { position.x + oldPosition.x, position.y + oldPosition.y };

    if (oldPosition.x != data->point.x || oldPosition.y != data->point.y) {
        printf("%f, %f\n", oldPosition.x, oldPosition.y);
    }

    oldPosition = data->point;

    errCode = context->BufferUnlock(lock, 0 /* writeOffset */, sizeof(VertexShaderData));

    ThrowOnError(errCode, "Failed to unlock data buffer object (vertex)");

    // angle += 1.0f;

    if (angle >= 360.0f) {
        angle = 0.0f;
    }
}

void NovaContext::UpdateFragmentDBO() const
{
    W3DN_ErrorCode errCode;

    W3DN_BufferLock* lock = context->DBOLock(&errCode, fragmentDbo, 0 /* readOffset */, 0 /* readSize */);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object (fragment)");
    }

    FragmentShaderData* data = reinterpret_cast<FragmentShaderData *>(lock->buffer);

    data->iterations = iterations;

    errCode = context->BufferUnlock(lock, 0 /* writeOffset */, sizeof(FragmentShaderData));

    ThrowOnError(errCode, "Failed to unlock data buffer object (fragment)");
}

void NovaContext::CreateTexture()
{
    Palette palette { 4 * 256 };

    palette.Add( {   0,   0,   0, 255 }, 1.0f );
    palette.Add( {   0,   0, 255, 255 }, 4.0f );
    palette.Add( { 255, 255, 255, 255 }, 8.0f );

    auto colors = palette.Create();

    W3DN_ErrorCode errCode;

    texture = context->CreateTexture(&errCode, W3DN_TEXTURE_2D, W3DNPF_RGBA, W3DNEF_UINT8,
        colors.size(), 1, 1 /* depth */, FALSE /* mipmapped*/, W3DN_STATIC_DRAW);

    ThrowOnError(errCode, "Failed to create texture");

    errCode = context->TexUpdateImage(texture, colors.data(), 0 /* level */, 0 /* arrayIdx */, sizeof(Color) * colors.size(), 0 /* srcRowsPerLayer */);

    ThrowOnError(errCode, "Failed to update texture");

    sampler = context->CreateTexSampler(&errCode);

    ThrowOnError(errCode, "Failed to create texture sampler");

    errCode = context->TSSetParametersTags(sampler,
        W3DN_TEXTURE_MIN_FILTER, textureFiltering ? W3DN_LINEAR : W3DN_NEAREST,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to set texture sampler parameters");

    context->BindTexture(nullptr, 0 /* texture unit */, texture, sampler);

    ThrowOnError(errCode, "Failed to bind texture");
}

void NovaContext::Resize()
{
    if ((IIntuition->GetWindowAttrs(window.window,
        WA_InnerWidth, &width,
        WA_InnerHeight, &height,
        TAG_DONE)) != 0)
    {
        throw std::runtime_error("Failed to get window dimensions");
    }

    if (!backBuffer ||
        IGraphics->GetBitMapAttr(backBuffer, BMA_ACTUALWIDTH) < width ||
        IGraphics->GetBitMapAttr(backBuffer, BMA_HEIGHT) < height)
    {
        IGraphics->FreeBitMap(backBuffer);
        backBuffer = IGraphics->AllocBitMapTags(width, height, 0,
            BMATags_Friend, window.window->RPort->BitMap,
            BMATags_Displayable, TRUE,
            TAG_DONE);

        printf("Bitmap size %lu * %lu\n", width, height);
    }

    if (!backBuffer) {
        throw std::runtime_error("Failed to allocate bitmap");
    }

    W3DN_ErrorCode errCode = context->FBBindBufferTags(nullptr, W3DN_FB_COLOUR_BUFFER_0,
        W3DNTag_BitMap, backBuffer,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to bind buffer to frame buffer object");

    //errCode = context->SetViewport(nullptr, 0.0, height, width, -height, 0.0, 1.0);
    errCode = context->SetViewport(nullptr, 0.0, 0.0, width, height, 0.0, 1.0);

    ThrowOnError(errCode, "Failed to set viewport");

    printf("Viewport %lu * %lu\n", width, height);

    //position = { width / 2.0f, height / 2.0f };
}

// Not used at the moment
void NovaContext::Clear() const
{
    constexpr float opaqueBlack[4] { 0.0f, 0.0f, 0.0f, 1.0f };

    const W3DN_ErrorCode errCode = context->Clear(nullptr /* renderState */, opaqueBlack, nullptr /* depth */, nullptr /* stencil*/);

    ThrowOnError(errCode, "Failed to clear");
}

void NovaContext::Draw() const
{
    constexpr uint32 posAttributeIndex { 0 };
    constexpr uint32 texCoordAttributeIndex { 0 };

    W3DN_ErrorCode errCode = context->BindVertexAttribArray(nullptr, posArrayIndex, vbo, posAttributeIndex);

    ThrowOnError(errCode, "Failed to bind vertex attribute array (pos)");

    errCode = context->BindVertexAttribArray(nullptr, texCoordArrayIndex, vbo, texCoordAttributeIndex);

    ThrowOnError(errCode, "Failed to bind vertex attribute array (texCoord)");

    UpdateVertexDBO();
    UpdateFragmentDBO();

    errCode = context->DrawArrays(nullptr, W3DN_PRIM_TRISTRIP, 0 /* base */, vertexCount);

    ThrowOnError(errCode, "Failed to draw arrays");
}

void NovaContext::SwapBuffers()
{
    W3DN_ErrorCode errCode;

    const uint32 submitID = context->Submit(&errCode);

    if (!submitID) {
        ThrowOnError(errCode, "Submit failed");
    }

    const uint32 winw = window.window->Width - (window.window->BorderLeft + window.window->BorderRight);
    const uint32 winh = window.window->Height - (window.window->BorderTop + window.window->BorderBottom);

    width = std::min(winw, width);
    height = std::min(winh, height);

    errCode = context->WaitDone(submitID, 0);

    ThrowOnError(errCode, "WaitDone failed");

    if (vsync) {
        IGraphics->WaitTOF();
    }

    IGraphics->BltBitMapRastPort(backBuffer, 0, 0, window.window->RPort, window.window->BorderLeft,
        window.window->BorderTop, width, height, 0xC0);
}

std::string NovaContext::ErrorToString(const W3DN_ErrorCode errCode) const
{
    return IW3DNova->W3DN_GetErrorString(errCode);
}

void NovaContext::ThrowOnError(W3DN_ErrorCode errCode, const std::string& message) const
{
    if (errCode != W3DNEC_SUCCESS) {
        throw std::runtime_error(message + ": " + ErrorToString(errCode));
    }
}

void NovaContext::SetPosition(const Vertex& pos)
{
    position = pos;
}

void NovaContext::SetZoom(const float z)
{
    zoom = z;
}

void NovaContext::Reset()
{
    oldPosition = { 0.0f, 0.0f };
}

} // fractal-nova
