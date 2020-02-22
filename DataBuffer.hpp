#pragma once

#include "NovaObject.hpp"

namespace fractalnova {

struct DataBuffer: public NovaObject
{
    DataBuffer(W3DN_Context* context, W3DN_ShaderType shaderType, std::size_t size, W3DN_Shader* shader);
    ~DataBuffer();

    W3DN_ShaderType shaderType { W3DNST_END };
    W3DN_DataBuffer* dbo { nullptr };
};

} // fractalnova

