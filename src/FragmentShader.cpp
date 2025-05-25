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

#include "FragmentShader.hpp"
#include "DataBuffer.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"

namespace fractalnova {

struct FragmentShaderData {
    int32 iterations;
    Vertex complex;
};

FragmentShader::FragmentShader(W3DN_Context* context, const std::string& fileName): Shader(context, fileName + ".frag.spv")
{
    logging::Debug("Create FragmentShader %s", fileName.c_str());
    dbo = std::make_unique<DataBuffer>(context, W3DNST_FRAGMENT, sizeof(FragmentShaderData), shader);
}

void FragmentShader::UpdateDBO(const int iterations, const Vertex& complex) const
{
    W3DN_ErrorCode errCode;

    constexpr uint64 readOffset = 0;
    constexpr uint64 readSize = 0;

    logging::Detail("Update fragment shader DBO: iterations %d, complex {%f, %f}",
                    iterations,
                    complex.x,
                    complex.y);

    W3DN_BufferLock* lock = context->DBOLock(&errCode, dbo->Ptr(), readOffset, readSize);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock fragment shader DBO");
    }

    auto data = reinterpret_cast<FragmentShaderData *>(lock->buffer);

    data->iterations = iterations;
    data->complex = complex; // NOTE: only Julia uses this

    constexpr uint64 writeOffset = 0;

    errCode = context->BufferUnlock(lock, writeOffset, sizeof(FragmentShaderData));

    ThrowOnError(errCode, "Failed to unlock fragment shader DBO");
}

} // fractalnova
