#include "NovaContext.hpp"
#include "GuiWindow.hpp"
#include "Palette.hpp"
#include "Texture.hpp"
#include "DataBuffer.hpp"
#include "VertexBuffer.hpp"
#include "Program.hpp"
#include "BackBuffer.hpp"
#include "Logger.hpp"
#include "VertexBuffer.hpp"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/warp3dnova.h>

#include <stdexcept>

// TODO: display coordinate/zoom on window title?

namespace fractalnova {

struct Warp3DNovaIFace* IW3DNova;
static struct Library* NovaBase;

NovaContext::NovaContext(const GuiWindow& window, const int iterations)
    : NovaObject(nullptr), window(window), iterations(iterations)
{
    logging::Debug("Create NovaContext");

    NovaBase = IExec->OpenLibrary("Warp3DNova.library", 1);

    if (NovaBase) {
        IW3DNova = reinterpret_cast<struct Warp3DNovaIFace *>(IExec->GetInterface(NovaBase, "main", 1, nullptr));
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

    UseProgram(EFractal::Mandelbrot);
    UsePalette(EPalette::Rainbow);

    vbo = std::make_unique<VertexBuffer>(context);
}

NovaContext::~NovaContext()
{
    if (context) {
        texture.reset();
        program.reset();
        vbo.reset();

        context->Destroy();
        context = nullptr;
    }

    if (IW3DNova) {
        IExec->DropInterface(reinterpret_cast<struct Interface *>(IW3DNova));
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

void NovaContext::CreateTexture(Palette& palette)
{
    auto colors = palette.GetColorArray();

    texture.reset();
    texture = std::make_unique<Texture>(context, colors);
}

void NovaContext::Resize()
{
    width = window.Width();
    height = window.Height();

    if (!backBuffer ||
        IGraphics->GetBitMapAttr(backBuffer->Data(), BMA_ACTUALWIDTH) < width ||
        IGraphics->GetBitMapAttr(backBuffer->Data(), BMA_HEIGHT) < height)
    {
        backBuffer = std::make_unique<BackBuffer>(width, height, window.WindowPtr()->RPort->BitMap);
    }

    W3DN_FrameBuffer* defaultFBO = nullptr;
    W3DN_ErrorCode errCode = context->FBBindBufferTags(defaultFBO, W3DN_FB_COLOUR_BUFFER_0,
        W3DNTag_BitMap, backBuffer->Data(),
        TAG_DONE);

    ThrowOnError(errCode, "Failed to bind buffer to frame buffer object");

    //errCode = context->SetViewport(defaultRSO, 0.0, height, width, -height, 0.0, 1.0);
    errCode = context->SetViewport(defaultRSO, 0.0, 0.0, width, height, 0.0, 1.0);

    ThrowOnError(errCode, "Failed to set viewport");

    logging::Debug("Viewport %lu * %lu", width, height);
}

void NovaContext::Clear() const
{
    constexpr float opaqueBlack[4] { 0.0f, 0.0f, 0.0f, 1.0f };
    constexpr double* depth = nullptr;
    constexpr uint32* stencil = nullptr;

    const W3DN_ErrorCode errCode = context->Clear(defaultRSO, opaqueBlack, depth, stencil);

    ThrowOnError(errCode, "Failed to clear");
}

void NovaContext::Draw() const
{
    program->UpdateVertexDBO();
    program->UpdateFragmentDBO();

    constexpr uint32 base = 0;
    const W3DN_ErrorCode errCode = context->DrawArrays(defaultRSO, W3DN_PRIM_TRISTRIP, base, vbo->vertexCount);

    ThrowOnError(errCode, "Failed to draw arrays");
}

void NovaContext::SwapBuffers()
{
    W3DN_ErrorCode errCode;

    const uint32 submitID = context->Submit(&errCode);

    if (!submitID) {
        ThrowOnError(errCode, "Submit failed");
    }

    constexpr uint32 noTimeout = 0;

    errCode = context->WaitDone(submitID, noTimeout);

    ThrowOnError(errCode, "WaitDone failed");

    window.Draw(backBuffer.get());
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

void NovaContext::UseProgram(const EFractal fractal)
{
    static EFractal current { EFractal::Unknown };

    if (current == fractal) {
        return;
    }

    logging::Debug("Switch fractal %d", static_cast<int>(fractal));

    current = fractal;

    const char* name;
    Vertex complex {};

    switch (fractal) {
        case EFractal::Mandelbrot:
            name = "mandelbrot";
            break;
        case EFractal::Julia1:
            name = "julia";
            complex = { -0.618f, 0.0f };
            break;
        case EFractal::Julia2:
            name = "julia";
            complex = { -0.4f, 0.6f };
            break;
        case EFractal::Julia3:
            name = "julia";
            complex = { 0.285f, 0.0f };
            break;
        case EFractal::Julia4:
            name = "julia";
            complex = { 0.285f, 0.01f };
            break;
        case EFractal::Julia5:
            name = "julia";
            complex = { 0.45f, 0.1428f };
            break;
        case EFractal::Julia6:
            name = "julia";
            complex = { -0.70176f, 0.3842f };
            break;
        case EFractal::Julia7:
            name = "julia";
            complex = { -0.835f, 0.232f };
            break;
        case EFractal::Julia8:
            name = "julia";
            complex = { -0.8f, 0.156f };
            break;
        case EFractal::Julia9:
            name = "julia";
            complex = { -0.7269f, 0.1889f };
            break;
        case EFractal::Julia10:
            name = "julia";
            complex = { 0.0, -0.8f };
            break;
        default:
            logging::Error("Unknown fractal %d", static_cast<int>(fractal));
            break;
    }

    program.reset(); // Destroy old program first. Otherwise Program destructor removes ShaderPipeline afterwards!
    program = std::make_unique<Program>(context, iterations, name);
    program->SetComplex(complex);
}

void NovaContext::UsePalette(const EPalette palette)
{
    static EPalette current { EPalette::Unknown };

    if (current == palette) {
        return;
    }

    current = palette;

    logging::Debug("Switch palette %d", static_cast<int>(palette));

    Palette p { palette };
    CreateTexture(p);
}

} // fractal-nova
