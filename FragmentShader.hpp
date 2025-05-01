#pragma once

#include "Shader.hpp"

namespace fractalnova {

struct Vertex;

class FragmentShader: public Shader
{
public:
    FragmentShader(W3DN_Context* context, const std::string& fileName);
    ~FragmentShader() = default;

    void UpdateDBO(int iterations, const Vertex& complex) const;

private:
};

} // fractalnova


