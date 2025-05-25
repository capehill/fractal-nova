/*
Copyright (C) 2020-2025 Juha Niemimäki

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "DataBuffer.hpp"
#include "Logger.hpp"

namespace fractalnova {

static constexpr uint32 bufferIdx = 0;

DataBuffer::DataBuffer(W3DN_Context* context, const W3DN_ShaderType shaderType, const std::size_t size, W3DN_Shader* shader): NovaObject(context), shaderType(shaderType)
{
    W3DN_ErrorCode errCode;

    logging::Debug("Create DataBuffer for shader type %d, size %zu", shaderType, size);

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
