#include "NovaContext.hpp"
#include "GuiWindow.hpp"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <stdexcept>
#include <string>

namespace fractalnova {

struct Warp3DNovaIFace* IW3DNova;
static struct Library* NovaBase;

NovaContext::NovaContext(const GuiWindow& window)
    : window(window)
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
        throw std::runtime_error(std::string("Failed to create Nova context: ") +
           IW3DNova->W3DN_GetErrorString(errCode));
    }

    Resize();
}

NovaContext::~NovaContext()
{
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

void NovaContext::Resize()
{
    //uint32 width, height;

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
    }

    if (!backBuffer) {
        throw std::runtime_error("Failed to allocate bitmap");
    }

    context->FBBindBufferTags(nullptr, W3DN_FB_COLOUR_BUFFER_0,
        W3DNTag_BitMap, backBuffer,
        TAG_DONE);
}

void NovaContext::Clear()
{
    constexpr float opaqueBlack[4] { 0.0f, 0.0f, 0.0f, 1.0f };

    context->Clear(nullptr, opaqueBlack, nullptr, nullptr);
}

void NovaContext::SwapBuffers()
{
    W3DN_ErrorCode errCode;

    const uint32 submitID = context->Submit(&errCode);

    if (!submitID) {
        throw std::runtime_error(std::string("Submit failed: ") + IW3DNova->W3DN_GetErrorString(errCode));
    }

    const uint32 winw = window.window->Width - (window.window->BorderLeft + window.window->BorderRight);
    const uint32 winh = window.window->Height - (window.window->BorderTop + window.window->BorderBottom);

    width = std::min(winw, width);
    height = std::min(winh, height);

    IGraphics->BltBitMapRastPort(backBuffer, 0, 0, window.window->RPort, window.window->BorderLeft,
        window.window->BorderTop, width, height, 0xC0);
}

} // fractal-nova
