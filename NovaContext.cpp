#include "NovaContext.hpp"
#include "GuiWindow.hpp"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/warp3dnova.h>

#include <stdexcept>
#include <string>

// TODO: display coordinate on window title?

namespace fractalnova {

struct FragmentShaderData {
    Vertex dimensions;
    Vertex point;
};

namespace {
    constexpr uint32 posArrayIndex { 0 };
    constexpr uint32 vertexCount { 4 };
}

struct Warp3DNovaIFace* IW3DNova;
static struct Library* NovaBase;

NovaContext::NovaContext(const GuiWindow& window, const bool verbose, const bool vsync)
    : window(window), verbose(verbose), vsync(vsync)
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
    if (dbo) {
        context->DestroyDataBufferObject(dbo);
        dbo = nullptr;
    }

    if (vbo) {
        context->DestroyVertexBufferObject(vbo);
        vbo = nullptr;
    }

    if (shaderPipeline) {
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

        ThrowOnError(errCode, "Failed to compiler shader " + fileName);
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
    constexpr uint32 arrayCount { 1 };

    W3DN_ErrorCode errCode;

    vbo = context->CreateVertexBufferObjectTags(&errCode,
        vertexCount * sizeof(Vertex), W3DN_STATIC_DRAW, arrayCount, TAG_DONE);

    ThrowOnError(errCode, "Failed create vertex buffer object");

    constexpr uint32 stride { sizeof(Vertex) };
    constexpr uint32 posElementCount { 2 };

    errCode = context->VBOSetArray(vbo, posArrayIndex, W3DNEF_FLOAT, FALSE, posElementCount, stride, 0, vertexCount);

    W3DN_BufferLock* lock = context->VBOLock(&errCode, vbo, 0, 0);

    ThrowOnError(errCode, "Failed to lock vertex buffer object");

    Vertex* vertices = reinterpret_cast<Vertex *>(lock->buffer);

    // Make a quad
    vertices[0].x = -1.0f;
    vertices[0].y = -1.0f;

    vertices[1].x = -1.0f;
    vertices[1].y =  1.0f;

    vertices[2].x =  1.0f;
    vertices[2].y = -1.0f;

    vertices[3].x = 1.0f;
    vertices[3].y = 1.0f;

    errCode = context->BufferUnlock(lock, 0, lock->size);

    ThrowOnError(errCode, "Failed to unlock vertex buffer object");
}

void NovaContext::CreateDBO()
{
    W3DN_ErrorCode errCode;

    dbo = context->CreateDataBufferObjectTags(&errCode, sizeof(FragmentShaderData), W3DN_STREAM_DRAW, 1,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to create data buffer object");

    context->DBOSetBufferTags(dbo, 0 /* bufferIdx */, 0 /* offset */, sizeof(FragmentShaderData), fragmentShader,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to set data buffer object");

    context->BindShaderDataBuffer(nullptr, W3DNST_FRAGMENT, dbo, 0 /* bufferIdx */);

    ThrowOnError(errCode, "Failed to bind data buffer object");
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

    W3DN_ErrorCode errCode = context->BindVertexAttribArray(nullptr, posArrayIndex, vbo, posAttributeIndex);

    ThrowOnError(errCode, "Failed to bind vertex attribute array");

    W3DN_BufferLock* lock = context->DBOLock(&errCode, dbo, 0 /* readOffset */, 0 /* readSize */);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object");
    }

    FragmentShaderData* data = reinterpret_cast<FragmentShaderData *>(lock->buffer);

    //const float fw = 3.5f / zoom;
    //const float fh = 2.0f / zoom;

    const float fx = 3.5f / zoom / width;
    const float fy = 2.0f / zoom / height;

    static Vertex lastPoint { -2.5f, -1.0f };
    static Vertex lastMouse { 0.0f, 0.0f };

    data->dimensions.x = fx; // 1.0f / width * fw;
    data->dimensions.y = fy; // 1.0f / height * fh;

    data->point.x = (position.x - width / 2.0f) * fx + lastPoint.x / zoom; //- 2.5f / zoom;
    data->point.y = (position.y - height / 2.0f) * fy + lastPoint.y / zoom; //- 1.0f / zoom;

    errCode = context->BufferUnlock(lock, 0 /* writeOffset */, sizeof(FragmentShaderData));

    if (position.x != lastMouse.x || position.y != lastMouse.y) {
        lastPoint = data->point;
    }

    lastMouse = position;

    //printf("%f, %f\n", lastPoint.x, lastPoint.y);

    ThrowOnError(errCode, "Failed to unlock data buffer object");

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

} // fractal-nova
