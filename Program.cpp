#include "Program.hpp"
#include "Shader.hpp"
#include "VertexBuffer.hpp"
#include "DataBuffer.hpp"
#include "Logger.hpp"

#include <cmath> // M_PI

namespace fractalnova {

namespace {
    static constexpr float toRadians { static_cast<float>(M_PI) / 180.0f };
}

Program::Program(W3DN_Context* context, const int iterations, const char* name):
    NovaObject(context),
    vertexShader(std::make_unique<Shader>(context, W3DNST_VERTEX)),
    fragmentShader(std::make_unique<Shader>(context, W3DNST_FRAGMENT)),
    iterations(iterations)
{
    logging::Log("Creating shader program '%s'", name);

    vertexShader->Compile(std::string(name) + ".vert.spv");
    fragmentShader->Compile(std::string(name) + ".frag.spv");

    W3DN_ErrorCode errCode;
    shaderPipeline = context->CreateShaderPipelineTags(&errCode,
        W3DNTag_Shader, vertexShader->Ptr(),
        W3DNTag_Shader, fragmentShader->Ptr(),
        TAG_DONE);

    if (!shaderPipeline) {
        ThrowOnError(errCode, "Failed to create shader pipeline");
    }

    errCode = context->SetShaderPipeline(defaultRSO, shaderPipeline);

    ThrowOnError(errCode, "Failed to set shader pipeline");

    vbo = std::make_unique<VertexBuffer>(context);
}

Program::~Program()
{
    vbo.reset();

    if (shaderPipeline) {
        context->SetShaderPipeline(defaultRSO, nullptr);
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

    W3DN_BufferLock* lock = context->DBOLock(&errCode, vertexShader->DboPtr()->Ptr(), 0 /* readOffset */, 0 /* readSize */);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object (vertex)");
    }

    VertexShaderData* data = reinterpret_cast<VertexShaderData *>(lock->buffer);

    data->angle = angle * toRadians;
    data->zoom = zoom;
    //data->zoom64 = { std::floor(zoom), 1000000000.0f * (zoom - std::floor(zoom)) };
    data->point = { position.x + oldPosition.x, position.y + oldPosition.y };

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

    W3DN_BufferLock* lock = context->DBOLock(&errCode, fragmentShader->DboPtr()->Ptr(), 0 /* readOffset */, 0 /* readSize */);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object (fragment)");
    }

    FragmentShaderData* data = reinterpret_cast<FragmentShaderData *>(lock->buffer);

    data->iterations = iterations;
    data->complex = complex; // TODO: only Julia uses this

    errCode = context->BufferUnlock(lock, 0 /* writeOffset */, sizeof(FragmentShaderData));

    ThrowOnError(errCode, "Failed to unlock data buffer object (fragment)");
}


void Program::SetPosition(const Vertex& pos)
{
    position = pos;
}

void Program::SetComplex(const Vertex& c)
{
    complex = c;
}

void Program::SetZoom(const float z)
{
    zoom = z;
}

void Program::Reset()
{
    oldPosition = { 0.0f, 0.0f };
}

VertexBuffer* Program::VboPtr() const
{
    return vbo.get();
}

} // fractalnova
