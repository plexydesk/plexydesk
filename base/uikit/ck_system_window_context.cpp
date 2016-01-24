#include "ck_system_window_context.h"

namespace cherry_kit {
class system_window_context::private_window_context {
public:
    private_window_context() {}
    ~private_window_context() {}
};

system_window_context::system_window_context():
    ctx(new private_window_context){}

system_window_context::~system_window_context(){ delete ctx;}

void system_window_context::on_change(
        system_window_context::window_change_callback_t a_callback)
{
}

void system_window_context::notify_window_change()
{
}

}

