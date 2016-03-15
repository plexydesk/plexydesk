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

screen::screen() : priv(new platform_screen) {}

screen::~screen() { delete priv; }

int screen::screen_count() const { return priv->screen_count(); }

float screen::scale_factor() const { return priv->scale_factor(); }

int screen::pixel_count_x() const { return priv->desktop_width(); }

int screen::pixel_count_y() const { return priv->desktop_height(); }

void screen::change_notifications(display_change_notify_callback_t a_callback) {
  m_notify_chain.push_back(a_callback);
}

float screen::virtual_desktop_width() { return priv->desktop_width(); }

float screen::virtual_desktop_height() { return priv->desktop_height(); }
}
