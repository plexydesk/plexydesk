#include "ck_screen.h"

#ifdef __X11_PLATFORM__
#include <core/qt/ck_screen_impl_qt.h>
#endif

#ifdef __APPLE_PLATFORM__
#include <core/mac/ck_screen_impl_mac.h>
#endif

#ifdef __WINDOWS__PLATFORM__
#error "Windows Platform Screen Support not Implemented"
#endif

namespace cherry_kit {

screen *screen::__self = 0;

screen *screen::get() {
   if (__self)
       return __self;

   __self = new screen();

   return __self;
}

screen::screen() : priv(new platform_screen) {}

screen::~screen() { delete priv; }

int screen::screen_count() const { return priv->screen_count(); }

float screen::scale_factor(int a_display_id) const {
  return priv->scale_factor(a_display_id);
}

int screen::x_resolution(int a_display_id) const {
  return priv->display_width(a_display_id);
}

int screen::y_resolution(int a_display_id) const {
  return priv->display_height(a_display_id);
}

float screen::desktop_height(int a_display_id) const {
  return priv->desktop_height(a_display_id);
}

void screen::change_notifications(display_change_notify_callback_t a_callback) {
  m_notify_chain.push_back(a_callback);
}

float screen::desktop_width(int a_display_id) const {
  return priv->desktop_width(a_display_id);
}
}
