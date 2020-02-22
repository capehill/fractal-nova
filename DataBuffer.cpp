#include "DataBuffer.hpp"

namespace fractalnova {

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

DataBuffer::~DataBuffer()
{
    if (dbo) {
        context->BindShaderDataBuffer(nullptr, shaderType, nullptr, 0);
        context->DestroyDataBufferObject(dbo);
        dbo = nullptr;
    }
}

} // fractalnova

