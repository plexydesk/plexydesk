#include "ck_device_window.h"

namespace cherry_kit {
class device_window::window_context {
public:
    window_context() {}
    ~window_context() {}

    window_handle_t m_handle;
};

device_window::device_window() : ctx(new window_context){}

device_window::~device_window() { delete ctx;}

window_handle_t device_window::handle() {
    return ctx->m_handle;
}

void device_window::set_handle(window_handle_t a_id) {
  ctx->m_handle = a_id;
}
}
