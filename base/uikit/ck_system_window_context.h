#ifndef SYSTEM_WINDOW_CONTEXT_H
#define SYSTEM_WINDOW_CONTEXT_H

#include <config.h>

#include <string>
#include <functional>

#include <ck_device_window.h>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {
class DECL_UI_KIT_EXPORT system_window_context
{
public:
    typedef enum {
      kWindowTopMost,
      kWindowBottom,
      kWindowNormal,
      kWindowPanel,
      kWindowPopup,
      kWindowTool
    } window_flag_t;

    typedef std::function<void(device_window *)> window_change_callback_t;

    system_window_context();
    virtual ~system_window_context();

    virtual device_window *find_window(const std::string &a_title) = 0;
    virtual bool reparent(device_window *a_child, device_window *a_parent) = 0;

    virtual device_window *desktop() = 0;
    virtual bool set_desktop(device_window *a_window) = 0;

    virtual void on_change(window_change_callback_t a_callback);

protected:
    virtual void notify_window_change();

private:
    class private_window_context;
    private_window_context *const ctx;
};
}

#endif // SYSTEM_WINDOW_CONTEXT_H