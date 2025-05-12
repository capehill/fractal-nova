#include "VertexShader.hpp"
#include "DataBuffer.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"

#include <cmath> // M_PI

namespace {
    static constexpr float toRadians { static_cast<float>(M_PI) / 180.0f };
    static constexpr float angle = 0.0f;
    static constexpr float angleInRadians = angle * toRadians;
}

namespace fractalnova {

struct VertexShaderData {
    float angle;
    float zoom;
    //Vertex zoom64;
    Vertex point;
};

VertexShader::VertexShader(W3DN_Context* context, const std::string& fileName): Shader(context, fileName + ".vert.spv")
{
    logging::Debug("Create VertexShader %s", fileName.c_str());
    dbo = std::make_unique<DataBuffer>(context, W3DNST_VERTEX, sizeof(VertexShaderData), shader);
}

Vertex VertexShader::UpdateDBO(const float zoom, const Vertex& oldPosition, const Vertex& position) const
{
    W3DN_ErrorCode errCode;

    constexpr uint64 readOffset = 0;
    constexpr uint64 readSize = 0;

    logging::Detail("Update vertex shader DBO: angle %f, zoom %f, point { %f, %f }",
                    angleInRadians,
                    zoom,
                    position.x + oldPosition.x,
                    position.y + oldPosition.y);

    W3DN_BufferLock* lock = context->DBOLock(&errCode, dbo->Ptr(), readOffset, readSize);

    if (!lock) {
        ThrowOnError(errCode, "Failed to lock data buffer object (vertex)");
    }

    auto data = reinterpret_cast<VertexShaderData *>(lock->buffer);

    data->angle = angleInRadians;
    data->zoom = zoom;
    //data->zoom64 = { std::floor(zoom), 1000000000.0f * (zoom - std::floor(zoom)) };
    auto oldPos = data->point = { position.x + oldPosition.x, position.y + oldPosition.y };

    constexpr uint64 writeOffset = 0;

    errCode = context->BufferUnlock(lock, writeOffset, sizeof(VertexShaderData));

    ThrowOnError(errCode, "Failed to unlock data buffer object (vertex)");

    return oldPos;
}

} // fractalnova

