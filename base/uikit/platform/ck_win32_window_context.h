#ifndef WIN32_WINDOW_CONTEXT_H
#define WIN32_WINDOW_CONTEXT_H

#include <config.h>

#include <ck_system_window_context.h>

#include <ck_system_window_context.h>

namespace cherry_kit {

class win32_window_context : public cherry_kit::system_window_context
{
public:
    win32_window_context();
    ~win32_window_context();

    device_window *find_window(const std::string &a_title);
    bool reparent(device_window *a_child, device_window *a_parent) {return 0;}

    device_window *desktop() { return 0;}

    bool convert_to_popup_window(device_window *a_window) {return false;}
    bool convert_to_panel_window(device_window *a_window) {return false;}
    bool convert_to_desktop_window(device_window *a_window) {return false;}
    bool convert_to_frameless_window(device_window *a_window) {return false;}
    bool convert_to_transparent_window(device_window *a_window) {return false;}
    bool convert_to_notification_window(device_window *a_window) {return false;}
    void post_notifycation(const std::string &icon, const std::string &msg) {}

    bool hide_native_desktop();

private:
    int get_windows_major_version();

};

}
#endif // WIN32_WINDOW_CONTEXT_H
