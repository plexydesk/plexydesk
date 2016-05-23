#include "ck_system_window_context.h"

#if defined(_WIN32) || defined(_WIN64)
#include <platform/ck_win32_window_context.h>
#endif

namespace cherry_kit {
class null_windows_context : public system_window_context {
public:
    null_windows_context() {}

    device_window *find_window(const std::string &a_title,
                               const std::string &a_window_class)  { return 0;}
    bool reparent(device_window *a_child, device_window *a_parent) {return 0;}

    device_window *desktop() { return 0;}

    bool convert_to_popup_window(device_window *a_window) {return false;}
    bool convert_to_panel_window(device_window *a_window) {return false;}
    bool convert_to_desktop_window(device_window *a_window) {return false;}
    bool convert_to_frameless_window(device_window *a_window) {return false;}
    bool convert_to_transparent_window(device_window *a_window) {return false;}
    bool convert_to_notification_window(device_window *a_window) {return false;}
    void post_notifycation(const std::string &icon, const std::string &msg) {}
    bool hide_native_desktop() {}

};

system_window_context *system_window_context::_self = 0;

class system_window_context::private_window_context {
public:
    private_window_context() {}
    ~private_window_context() {}
};

system_window_context::system_window_context():
    ctx(new private_window_context){}

system_window_context *system_window_context::get()
{
    if (_self)
        return _self;

#if defined(_WIN32) || defined(_WIN64)
    _self = new win32_window_context();
#else
    _self = new null_windows_context();
#endif
    return _self;
}

system_window_context::~system_window_context() {
    delete ctx;
    if (_self) {
      delete _self;
      _self = 0;
    }
}

void system_window_context::on_change(
        system_window_context::window_change_callback_t a_callback)
{
}

void system_window_context::notify_window_change()
{
}

}

