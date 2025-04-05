#include "FragmentShader.hpp"
#include "DataBuffer.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"

#include <stdexcept>

namespace fractalnova {

struct FragmentShaderData {
    int32 iterations;
    Vertex complex;
};

FragmentShader::FragmentShader(W3DN_Context* context, const std::string& fileName): Shader(context, fileName + ".frag.spv")
{
    dbo = std::make_unique<DataBuffer>(context, W3DNST_FRAGMENT, sizeof(FragmentShaderData), shader);
}

FragmentShader::~FragmentShader()
{
}

void FragmentShader::UpdateDBO(const int iterations, const Vertex& complex) const
{
    W3DN_ErrorCode errCode;

    constexpr uint64 readOffset = 0;
    constexpr uint64 readSize = 0;

    W3DN_BufferLock* lock = context->DBOLock(&errCode, dbo->Ptr(), readOffset, readSize);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object (fragment)");
    }

    auto data = reinterpret_cast<FragmentShaderData *>(lock->buffer);

    data->iterations = iterations;
    data->complex = complex; // TODO: only Julia uses this

    constexpr uint64 writeOffset = 0;

    errCode = context->BufferUnlock(lock, writeOffset, sizeof(FragmentShaderData));

    ThrowOnError(errCode, "Failed to unlock data buffer object (fragment)");
}


} // fractalnova

