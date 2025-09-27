/*
Copyright (C) 2020-2025 Juha Niemimaki

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

#include "Exception.hpp"

#include <proto/Warp3DNova.h>

#include <string>
#include <stdexcept>

namespace tools {

void ThrowException(std::string_view description, std::source_location location)
{
    std::string message = description.data();
    message += ". File ";
    message += location.file_name();
    message += ", line ";
    message += std::to_string(location.line());

    throw std::runtime_error(message);
}

void ThrowException(bool expression, std::string_view description, std::source_location location)
{
    if (!expression) {
        ThrowException(description, location);
    }
}

void ThrowOnNovaError(W3DN_ErrorCode errCode, std::string_view description, std::source_location location)
{
    if (errCode != W3DNEC_SUCCESS) {
        std::string message = description.data();
        message += ". W3DN error: ";
        message += IW3DNova->W3DN_GetErrorString(errCode);
        ThrowException(message, location);
    }
}

} // tools


