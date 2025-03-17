#include "DataBuffer.hpp"

namespace fractalnova {

static constexpr uint32 bufferIdx = 0;

DataBuffer::DataBuffer(W3DN_Context* context, const W3DN_ShaderType shaderType, const std::size_t size, W3DN_Shader* shader): NovaObject(context), shaderType(shaderType)
{
    W3DN_ErrorCode errCode;

    dbo = context->CreateDataBufferObjectTags(&errCode, size, W3DN_STREAM_DRAW, 1,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to create data buffer object");

    constexpr uint64 offset = 0;

    context->DBOSetBufferTags(dbo, bufferIdx, offset, size, shader,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to set data buffer object");

    context->BindShaderDataBuffer(defaultRSO, shaderType, dbo, bufferIdx);

    ThrowOnError(errCode, "Failed to bind data buffer object");
}

DataBuffer::~DataBuffer()
{
    if (dbo) {
        context->BindShaderDataBuffer(defaultRSO, shaderType, nullptr, bufferIdx);
        context->DestroyDataBufferObject(dbo);
        dbo = nullptr;
    }
}

W3DN_DataBuffer* DataBuffer::Ptr() const
{
    return dbo;
}

} // fractalnova

