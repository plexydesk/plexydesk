#include "ck_screen_impl_mac.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QRectF>

#import <Cocoa/Cocoa.h>

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
  NSArray *screen_list = [NSScreen screens];

  if (a_display_id > screen_list.count || a_display_id < 0) {
    qDebug() << Q_FUNC_INFO << "Invalid display id";
    return 1.0f;
  }

  NSScreen *screen = [screen_list objectAtIndex:a_display_id];

  if (!screen)
    return 1.0f;

  float desktop_scale =
      display_width(a_display_id) / desktop_width(a_display_id);

  if (desktop_scale < 1) {
    return screen.backingScaleFactor / desktop_scale;
  }

  return screen.backingScaleFactor;
}

float cherry_kit::screen::platform_screen::desktop_width(
    int a_display_id) const {
  float rv = 1920;

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
  NSArray *screen_list = [NSScreen screens];

  if (a_display_id > screen_list.count || a_display_id < 0) {
    qDebug() << Q_FUNC_INFO << "Invalid display id";
    return 1920.0f;
  }

  NSScreen *screen = [screen_list objectAtIndex:a_display_id];

  if (!screen)
    return 1920.0f;

  return screen.frame.size.width;
}

float cherry_kit::screen::platform_screen::display_height(
    int a_display_id) const {
  NSArray *screen_list = [NSScreen screens];

  if (a_display_id > screen_list.count || a_display_id < 0) {
    qDebug() << Q_FUNC_INFO << "Invalid display id";
    return 1080.0f;
  }

  NSScreen *screen = [screen_list objectAtIndex:a_display_id];

  if (!screen)
    return 1080.0f;

  return screen.frame.size.height;
}
