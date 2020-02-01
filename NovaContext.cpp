#include "NovaContext.hpp"
#include "GuiWindow.hpp"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <stdexcept>
#include <string>

namespace fractalnova {

struct Vertex {
    float x, y;
};

namespace {
    constexpr uint32 posArrayIndex { 0 };
    constexpr uint32 vertexCount { 3 };
}

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
           ErrorToString(errCode));
    }

    Resize();
}

NovaContext::~NovaContext()
{
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

    W3DN_Shader* shader = context->CompileShaderTags(&errCode,
        W3DNTag_FileName, fileName.c_str(),
        W3DNTag_Log, &shaderLog,
        W3DNTag_LogLevel, W3DNLL_DEBUG,
        TAG_DONE);

    if (!shader) {
        if (shaderLog) {
            printf("Compiling %s failed (%u) with error: %s\n",
               fileName.c_str(), errCode, ErrorToString(errCode).c_str());
            context->DestroyShaderLog(shaderLog);
        }

        throw std::runtime_error(std::string("Failed to compiler shader ") + fileName + " " +
            ErrorToString(errCode));
    }

    if (shaderLog) {
        printf("%s compilation log:\n%s\n", fileName.c_str(), shaderLog);
        context->DestroyShaderLog(shaderLog);
    }

    return shader;
}

void NovaContext::LoadShaders()
{
    vertexShader = CompileShader("simple.vert.spv");
    fragmentShader = CompileShader("simple.frag.spv");

    W3DN_ErrorCode errCode;
    shaderPipeline = context->CreateShaderPipelineTags(&errCode,
        W3DNTag_Shader, vertexShader,
        W3DNTag_Shader, fragmentShader,
        TAG_DONE);

    if (!shaderPipeline) {
        throw std::runtime_error(std::string("Failed to create shader pipeline: ") + ErrorToString(errCode));
    }

    errCode = context->SetShaderPipeline(nullptr, shaderPipeline);

    if (errCode != W3DNEC_SUCCESS) {
        throw std::runtime_error(std::string("Failed to set shader pipeline: ") + ErrorToString(errCode));
    }
}

void NovaContext::CreateVBO()
{
    constexpr uint32 arrayCount { 1 };

    W3DN_ErrorCode errCode;

    vbo = context->CreateVertexBufferObjectTags(&errCode,
        vertexCount * sizeof(Vertex), W3DN_STATIC_DRAW, arrayCount, TAG_DONE);

    if (errCode != W3DNEC_SUCCESS) {
        throw std::runtime_error(std::string("Failed create vertex buffer object: ") + ErrorToString(errCode));
    }

    constexpr uint32 stride { sizeof(Vertex) };
    constexpr uint32 posElementCount { 2 };

    errCode = context->VBOSetArray(vbo, posArrayIndex, W3DNEF_FLOAT, FALSE, posElementCount, stride, 0, vertexCount);

    W3DN_BufferLock* lock = context->VBOLock(&errCode, vbo, 0, 0);
    if (errCode != W3DNEC_SUCCESS) {
        throw std::runtime_error(std::string("Failed to lock vertex buffer object: ") + ErrorToString(errCode));
    }

    Vertex* vertices = reinterpret_cast<Vertex *>(lock->buffer);

    vertices[0].x = 100.0f;
    vertices[0].y = 50.0f;

    vertices[1].x = 320.0f;
    vertices[1].y = 420.0f;

    vertices[2].x = 540.0f;
    vertices[2].y = 50.0f;

    errCode = context->BufferUnlock(lock, 0, lock->size);

    if (errCode != W3DNEC_SUCCESS) {
        throw std::runtime_error(std::string("Failed to unlock vertex buffer object: ") + ErrorToString(errCode));
    }
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
    }

    if (!backBuffer) {
        throw std::runtime_error("Failed to allocate bitmap");
    }

    context->FBBindBufferTags(nullptr, W3DN_FB_COLOUR_BUFFER_0,
        W3DNTag_BitMap, backBuffer,
        TAG_DONE);
}

void NovaContext::Clear() const
{
    constexpr float opaqueBlack[4] { 0.0f, 0.0f, 0.0f, 1.0f };

    context->Clear(nullptr, opaqueBlack, nullptr, nullptr);
}

void NovaContext::Draw() const
{
    constexpr uint32 posAttributeIndex { 0 };

    const W3DN_ErrorCode errCode = context->BindVertexAttribArray(nullptr, posArrayIndex, vbo, posAttributeIndex);

    if (errCode != W3DNEC_SUCCESS) {
        throw std::runtime_error("Failed to bind vertex attribute array");
    }

    context->DrawArrays(nullptr, W3DN_PRIM_TRIANGLES, 0, vertexCount);
}

void NovaContext::SwapBuffers()
{
    W3DN_ErrorCode errCode;

    const uint32 submitID = context->Submit(&errCode);

    if (!submitID) {
        throw std::runtime_error(std::string("Submit failed: ") + ErrorToString(errCode));
    }

    const uint32 winw = window.window->Width - (window.window->BorderLeft + window.window->BorderRight);
    const uint32 winh = window.window->Height - (window.window->BorderTop + window.window->BorderBottom);

    width = std::min(winw, width);
    height = std::min(winh, height);

    IGraphics->BltBitMapRastPort(backBuffer, 0, 0, window.window->RPort, window.window->BorderLeft,
        window.window->BorderTop, width, height, 0xC0);
}

std::string NovaContext::ErrorToString(const W3DN_ErrorCode errCode) const
{
    return IW3DNova->W3DN_GetErrorString(errCode);
}

} // fractal-nova
