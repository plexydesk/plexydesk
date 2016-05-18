#ifndef DEVICE_WINDOW_H
#define DEVICE_WINDOW_H

#include <config.h>
#include <plexydesk_ui_exports.h>

#ifdef __WINDOWS_PLATFORM__
#include <Windows.h>
#endif

namespace cherry_kit {

#ifdef __WINDOWS_PLATFORM__
typedef HWND window_handle_t;
#else
typedef void* window_handle_t;
#endif

class DECL_UI_KIT_EXPORT device_window
{
public:
    device_window();
    virtual ~device_window();

    window_handle_t handle();
    void set_handle(window_handle_t a_id);

private:
   class window_context;
   window_context *const ctx;
};
}

#endif // DEVICE_WINDOW_H
