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

#include "AboutWindow.hpp"
#include "Version.hpp"

#include <proto/intuition.h>

#include <classes/requester.h>

#include <stdexcept>

namespace fractalnova {

AboutWindow::AboutWindow(Window* window)
{
    static char buf[128];

    snprintf(buf, sizeof(buf),
         NAME_STRING " "
         VERSION_STRING " "
         DATE_STRING " - written by Juha Niemimaki");

    auto object = IIntuition->NewObject(nullptr, "requester.class",
        REQ_TitleText, "About " NAME_STRING,
        REQ_BodyText, buf,
        REQ_GadgetText, "_Ok",
        REQ_Image, REQIMAGE_INFO,
        REQ_TimeOutSecs, 5,
        TAG_DONE);

    if (!object) {
        throw std::runtime_error("Failed to open About window");
    }

    IIntuition->IDoMethod(object, RM_OPENREQ, nullptr, window, nullptr);
}

} // fractalnova
