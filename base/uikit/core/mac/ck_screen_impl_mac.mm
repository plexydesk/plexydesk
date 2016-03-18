#include "ck_screen_impl_mac.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QRectF>

#include <ApplicationServices/ApplicationServices.h>

cherry_kit::screen::platform_screen::platform_screen() {}

cherry_kit::screen::platform_screen::~platform_screen() {}

int cherry_kit::screen::platform_screen::screen_count() const {
  CGDisplayCount online_display_count = 0;

  CGGetOnlineDisplayList(0, NULL, &online_display_count);
  return online_display_count;
}

float cherry_kit::screen::platform_screen::scale_factor(
    int a_display_id) const {
  return 2.0f;
}

float cherry_kit::screen::platform_screen::desktop_width(
    int a_display_id) const {
  float rv = 1920.0f;

  return rv;
}

float cherry_kit::screen::platform_screen::desktop_height(
    int a_display_id) const {
  float rv = 1080.0f;

  rv = (display_height(a_display_id) / display_width(a_display_id)) *
       desktop_width(a_display_id);

  return rv;
}

float cherry_kit::screen::platform_screen::display_width(
    int a_display_id) const {
  return 1440.0f;
}

float cherry_kit::screen::platform_screen::display_height(
    int a_display_id) const {
  return 900.0f;
}
