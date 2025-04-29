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


