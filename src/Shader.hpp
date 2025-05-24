#pragma once

#include "NovaObject.hpp"

#include <string>
#include <memory>

namespace fractalnova {

class DataBuffer;

class Shader: public NovaObject
{
public:
    Shader(W3DN_Context* context, const std::string& fileName);
    ~Shader();

    void Compile(const std::string& fileName);
    DataBuffer* DboPtr() const;
    W3DN_Shader* Ptr() const;

protected:
    std::unique_ptr<DataBuffer> dbo;

    W3DN_Shader* shader { nullptr };
};

} // fractalnova

