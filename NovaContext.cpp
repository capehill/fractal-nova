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

NovaObject::NovaObject(W3DN_Context* context): context(context)
{
}

std::string NovaObject::ErrorToString(W3DN_ErrorCode errCode) const
{
    return IW3DNova->W3DN_GetErrorString(errCode);
}

void NovaObject::ThrowOnError(W3DN_ErrorCode errCode, const std::string& message) const
{
    if (errCode != W3DNEC_SUCCESS) {
        throw std::runtime_error(message + ": " + ErrorToString(errCode));
    }
}

Shader::Shader(W3DN_Context* context, W3DN_ShaderType shaderType, const bool verbose): NovaObject(context), verbose(verbose)
{
    if (shaderType == W3DNST_VERTEX) {
        dbo = std::make_unique<DataBuffer>(context, W3DNST_VERTEX, sizeof(VertexShaderData), shader);
    } else if (shaderType == W3DNST_FRAGMENT) {
        dbo = std::make_unique<DataBuffer>(context, W3DNST_FRAGMENT, sizeof(FragmentShaderData), shader);
    } else {
        throw std::runtime_error("Unknown shader type");
    }
}

Shader::~Shader()
{
    if (shader) {
        context->DestroyShader(shader);
        shader = nullptr;
    }

    dbo.reset();
}

void Shader::Compile(const std::string& fileName)
{
    W3DN_ErrorCode errCode;
    const char* shaderLog = nullptr;
    const std::string shaderPath = "shaders/" + fileName;

    shader = context->CompileShaderTags(&errCode,
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
}

void Shader::UpdateVertexDBO(Program* program) const // -> DataBuffer? Or Program?
{
    W3DN_ErrorCode errCode;

    static float angle = 0.0f;

    W3DN_BufferLock* lock = context->DBOLock(&errCode, dbo->dbo, 0 /* readOffset */, 0 /* readSize */);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object (vertex)");
    }

    VertexShaderData* data = reinterpret_cast<VertexShaderData *>(lock->buffer);

    data->angle = angle * toRadians;
    data->zoom = program->zoom;
    data->point = { program->position.x + program->oldPosition.x, program->position.y + program->oldPosition.y };

#if 0
    if (oldPosition.x != data->point.x || oldPosition.y != data->point.y) {
        printf("%f, %f\n", oldPosition.x, oldPosition.y);
    }
#endif

    program->oldPosition = data->point;

    errCode = context->BufferUnlock(lock, 0 /* writeOffset */, sizeof(VertexShaderData));

    ThrowOnError(errCode, "Failed to unlock data buffer object (vertex)");

    //angle += 1.0f;

    if (angle >= 360.0f) {
        angle = 0.0f;
    }
}

void Shader::UpdateFragmentDBO(Program* program) const // -> DataBuffer?
{
    W3DN_ErrorCode errCode;

#if 0
    static int iter = 0;

    if (++iter > iterations) {
        iter = 20;
    }
#endif

    W3DN_BufferLock* lock = context->DBOLock(&errCode, dbo->dbo, 0 /* readOffset */, 0 /* readSize */);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object (fragment)");
    }

    FragmentShaderData* data = reinterpret_cast<FragmentShaderData *>(lock->buffer);

    data->iterations = program->iterations;

    errCode = context->BufferUnlock(lock, 0 /* writeOffset */, sizeof(FragmentShaderData));

    ThrowOnError(errCode, "Failed to unlock data buffer object (fragment)");
}

VertexBuffer::~VertexBuffer()
{
    if (vbo) {
        context->BindVertexAttribArray(nullptr, 0 /* attribNum*/, nullptr, 0 /* arrayIdx */);
        context->BindVertexAttribArray(nullptr, 1 /* attribNum*/, nullptr, 0 /* arrayIdx */);
        context->DestroyVertexBufferObject(vbo);
        vbo = nullptr;
    }
}

VertexBuffer::VertexBuffer(W3DN_Context* context): NovaObject(context)
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

    constexpr uint32 posAttributeIndex { 0 };
    constexpr uint32 texCoordAttributeIndex { 0 };

    errCode = context->BindVertexAttribArray(nullptr, posArrayIndex, vbo, posAttributeIndex);

    ThrowOnError(errCode, "Failed to bind vertex attribute array (pos)");

    errCode = context->BindVertexAttribArray(nullptr, texCoordArrayIndex, vbo, texCoordAttributeIndex);

    ThrowOnError(errCode, "Failed to bind vertex attribute array (texCoord)");
}

DataBuffer::~DataBuffer()
{
    if (dbo) {
        context->BindShaderDataBuffer(nullptr, shaderType, nullptr, 0);
        context->DestroyDataBufferObject(dbo);
        dbo = nullptr;
    }
}

DataBuffer::DataBuffer(W3DN_Context* context, const W3DN_ShaderType shaderType, const std::size_t size, W3DN_Shader* shader): NovaObject(context), shaderType(shaderType)
{
    W3DN_ErrorCode errCode;

    dbo = context->CreateDataBufferObjectTags(&errCode, size, W3DN_STREAM_DRAW, 1,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to create data buffer object");

    context->DBOSetBufferTags(dbo, 0 /* bufferIdx */, 0 /* offset */, size, shader,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to set data buffer object");

    context->BindShaderDataBuffer(nullptr, shaderType, dbo, 0 /* bufferIdx */);

    ThrowOnError(errCode, "Failed to bind data buffer object");
}

Texture::Texture(W3DN_Context* context, const std::vector<Color>& colors): NovaObject(context)
{
    W3DN_ErrorCode errCode;

    texture = context->CreateTexture(&errCode, W3DN_TEXTURE_2D, W3DNPF_RGBA, W3DNEF_UINT8,
        colors.size(), 1, 1 /* depth */, FALSE /* mipmapped*/, W3DN_STATIC_DRAW);

    ThrowOnError(errCode, "Failed to create texture");

    errCode = context->TexUpdateImage(texture, const_cast<Color*>(colors.data()), 0 /* level */, 0 /* arrayIdx */, sizeof(Color) * colors.size(), 0 /* srcRowsPerLayer */);

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

Texture::~Texture()
{
    if (texture) {
        context->BindTexture(nullptr, 0, nullptr, nullptr);
        context->DestroyTexture(texture);
        texture = nullptr;
    }
}

BackBuffer::BackBuffer(uint32 width, uint32 height, BitMap* friendBitMap)
{
    bitMap = IGraphics->AllocBitMapTags(width, height, 0,
        BMATags_Friend, friendBitMap,
        BMATags_Displayable, TRUE,
        TAG_DONE);

    printf("Bitmap size %lu * %lu\n", width, height);

    if (!bitMap) {
        throw std::runtime_error("Failed to allocate bitmap");
    }
}

BackBuffer::~BackBuffer()
{
    if (bitMap) {
        IGraphics->FreeBitMap(bitMap);
        bitMap = nullptr;
    }
}

Program::Program(W3DN_Context* context, const bool verbose, const int iterations): NovaObject(context), iterations(iterations)
{
    vertexShader = std::make_unique<Shader>(context, W3DNST_VERTEX, verbose);
    fragmentShader = std::make_unique<Shader>(context, W3DNST_FRAGMENT, verbose);

#if 0
    vertexShader = CompileShader("simple.vert.spv");
    fragmentShader = CompileShader("simple.frag.spv");
#else
#if 1
    vertexShader->Compile("julia.vert.spv");
    fragmentShader->Compile("julia.frag.spv");
#else
    vertexShader = CompileShader("mandelbrot.vert.spv");
    fragmentShader = CompileShader("mandelbrot.frag.spv");
#endif
#endif

    W3DN_ErrorCode errCode;
    shaderPipeline = context->CreateShaderPipelineTags(&errCode,
        W3DNTag_Shader, vertexShader->shader,
        W3DNTag_Shader, fragmentShader->shader,
        TAG_DONE);

    if (!shaderPipeline) {
        ThrowOnError(errCode, "Failed to create shader pipeline");
    }

    errCode = context->SetShaderPipeline(nullptr, shaderPipeline);

    ThrowOnError(errCode, "Failed to set shader pipeline");

    vbo = std::make_unique<VertexBuffer>(context);
}

Program::~Program()
{
    vbo.reset();

    if (shaderPipeline) {
        context->SetShaderPipeline(nullptr, nullptr);
        context->DestroyShaderPipeline(shaderPipeline);
        shaderPipeline = nullptr;
    }

    vertexShader.reset();
    fragmentShader.reset();
}

void Program::SetPosition(const Vertex& pos)
{
    position = pos;
}

void Program::SetZoom(const float z)
{
    zoom = z;
}

void Program::Reset()
{
    oldPosition = { 0.0f, 0.0f };
}

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
    program->vertexShader->UpdateVertexDBO(program.get() /* TODO: this is a hack to get those shader parameters */);
    program->fragmentShader->UpdateFragmentDBO(program.get());

    const W3DN_ErrorCode errCode = context->DrawArrays(nullptr, W3DN_PRIM_TRISTRIP, 0 /* base */, vertexCount);

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
