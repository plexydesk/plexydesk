#ifndef DEVICE_WINDOW_H
#define DEVICE_WINDOW_H

#include <config.h>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {
class DECL_UI_KIT_EXPORT device_window
{
public:
    device_window();
    virtual ~device_window();

private:
   class window_context;
   window_context *const ctx;
};
}

#endif // DEVICE_WINDOW_H
