#include "Program.hpp"
#include "Shader.hpp"
#include "VertexBuffer.hpp"
#include "DataBuffer.hpp"

#include <cmath> // M_PI

namespace fractalnova {

namespace {
    static constexpr float toRadians { M_PI/ 180.0f };
}

Program::Program(W3DN_Context* context, const bool verbose, const int iterations):
    NovaObject(context),
    vertexShader(std::make_unique<Shader>(context, W3DNST_VERTEX, verbose)),
    fragmentShader(std::make_unique<Shader>(context, W3DNST_FRAGMENT, verbose)),
    iterations(iterations)
{
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

void Program::UpdateVertexDBO() const
{
    W3DN_ErrorCode errCode;

    static float angle = 0.0f;

    W3DN_BufferLock* lock = context->DBOLock(&errCode, vertexShader->dbo->dbo, 0 /* readOffset */, 0 /* readSize */);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object (vertex)");
    }

    VertexShaderData* data = reinterpret_cast<VertexShaderData *>(lock->buffer);

    data->angle = angle * toRadians;
    data->zoom = zoom;
    data->point = { position.x + oldPosition.x, position.y + oldPosition.y };

#if 0
    if (oldPosition.x != data->point.x || oldPosition.y != data->point.y) {
        printf("%f, %f\n", oldPosition.x, oldPosition.y);
    }
#endif

    oldPosition = data->point;

    errCode = context->BufferUnlock(lock, 0 /* writeOffset */, sizeof(VertexShaderData));

    ThrowOnError(errCode, "Failed to unlock data buffer object (vertex)");

    //angle += 1.0f;

    if (angle >= 360.0f) {
        angle = 0.0f;
    }
}

void Program::UpdateFragmentDBO() const
{
    W3DN_ErrorCode errCode;

#if 0
    static int iter = 0;

    if (++iter > iterations) {
        iter = 20;
    }
#endif

    W3DN_BufferLock* lock = context->DBOLock(&errCode, fragmentShader->dbo->dbo, 0 /* readOffset */, 0 /* readSize */);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object (fragment)");
    }

    FragmentShaderData* data = reinterpret_cast<FragmentShaderData *>(lock->buffer);

    data->iterations = iterations;

    errCode = context->BufferUnlock(lock, 0 /* writeOffset */, sizeof(FragmentShaderData));

    ThrowOnError(errCode, "Failed to unlock data buffer object (fragment)");
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

} // fractalnova
