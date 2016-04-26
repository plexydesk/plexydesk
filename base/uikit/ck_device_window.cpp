#include "ck_device_window.h"

namespace cherry_kit {
class device_window::window_context {
public:
    window_context() {}
    ~window_context() {}
};

device_window::device_window() : ctx(new window_context){}

device_window::~device_window() { delete ctx;}
}
