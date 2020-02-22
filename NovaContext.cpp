#include "NovaContext.hpp"
#include "GuiWindow.hpp"
#include "Palette.hpp"
#include "Texture.hpp"
#include "DataBuffer.hpp"
#include "Shader.hpp"
#include "VertexBuffer.hpp"
#include "Program.hpp"
#include "BackBuffer.hpp"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/warp3dnova.h>

#include <stdexcept>

// TODO: display coordinate/zoom on window title?

namespace fractalnova {

struct Warp3DNovaIFace* IW3DNova;
static struct Library* NovaBase;

NovaContext::NovaContext(const GuiWindow& window, const bool verbose, const bool vsync, const int iterations)
    : NovaObject(nullptr), window(window), verbose(verbose), vsync(vsync)
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

    program = std::make_unique<Program>(context, verbose, iterations);

    CreateTexture();
}

NovaContext::~NovaContext()
{
    texture.reset();
    program.reset();

    if (context) {
        context->Destroy();
        context = nullptr;
    }

    if (IW3DNova) {
        IExec->DropInterface((struct Interface *)IW3DNova);
        IW3DNova = nullptr;
    }

    CloseLib();

    backBuffer.reset();
}

void NovaContext::CloseLib()
{
    if (NovaBase) {
        IExec->CloseLibrary(NovaBase);
        NovaBase = nullptr;
    }
}

void NovaContext::CreateTexture()
{
    Palette palette { 4 * 256 };

#if 1
    palette.Add( {   0,   0,   0 }, 1.0f );
    palette.Add( { 255,   0,   0 }, 1.0f );
    palette.Add( { 255, 127,   0 }, 1.0f );
    palette.Add( { 255, 255,   0 }, 1.0f );
    palette.Add( {   0, 255,   0 }, 1.0f );
    palette.Add( {   0,   0, 255 }, 1.0f );
    palette.Add( {  75,   0, 130 }, 1.0f );
    palette.Add( { 148,   0, 211 }, 1.0f );
#else
    palette.Add( {   0,   0,   0, 255 }, 1.0f );
    palette.Add( {   0,   0, 255, 255 }, 8.0f );
    palette.Add( { 255,   0, 255, 255 }, 8.0f );
    palette.Add( { 255, 255, 255, 255 }, 8.0f );
    palette.Add( {   0, 255, 0,   255 }, 8.0f );
    palette.Add( {   0,   0, 255, 255 }, 8.0f );
#endif

    auto colors = palette.Create();

    texture = std::make_unique<Texture>(context, colors);
}

void NovaContext::Resize()
{
    width = window.Width();
    height = window.Height();

    if (!backBuffer ||
        IGraphics->GetBitMapAttr(backBuffer->bitMap, BMA_ACTUALWIDTH) < width ||
        IGraphics->GetBitMapAttr(backBuffer->bitMap, BMA_HEIGHT) < height)
    {
        backBuffer = std::make_unique<BackBuffer>(width, height, window.window->RPort->BitMap);
    }

    W3DN_ErrorCode errCode = context->FBBindBufferTags(nullptr, W3DN_FB_COLOUR_BUFFER_0,
        W3DNTag_BitMap, backBuffer->bitMap,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to bind buffer to frame buffer object");

    //errCode = context->SetViewport(nullptr, 0.0, height, width, -height, 0.0, 1.0);
    errCode = context->SetViewport(nullptr, 0.0, 0.0, width, height, 0.0, 1.0);

    ThrowOnError(errCode, "Failed to set viewport");

    printf("Viewport %lu * %lu\n", width, height);
}

void NovaContext::Clear() const
{
    constexpr float opaqueBlack[4] { 0.0f, 0.0f, 0.0f, 1.0f };

    const W3DN_ErrorCode errCode = context->Clear(nullptr /* renderState */, opaqueBlack, nullptr /* depth */, nullptr /* stencil*/);

    ThrowOnError(errCode, "Failed to clear");
}

void NovaContext::Draw() const
{
    program->UpdateVertexDBO();
    program->UpdateFragmentDBO();

    const W3DN_ErrorCode errCode = context->DrawArrays(nullptr, W3DN_PRIM_TRISTRIP, 0 /* base */, program->vbo->vertexCount);

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

    IGraphics->BltBitMapRastPort(backBuffer->bitMap, 0, 0, window.window->RPort, window.window->BorderLeft,
        window.window->BorderTop, width, height, 0xC0);
}

void NovaContext::SetPosition(const Vertex& pos)
{
    program->SetPosition(pos);
}

void NovaContext::SetZoom(const float z)
{
    program->SetZoom(z);
}

void NovaContext::Reset()
{
    program->Reset();
}

} // fractal-nova
