#include "ck_screen_impl_qt.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QRectF>

cherry_kit::screen::platform_screen::platform_screen()
{
}

cherry_kit::screen::platform_screen::~platform_screen()
{

}

int cherry_kit::screen::platform_screen::screen_count() const
{
  return qApp->desktop()->screenCount();
}

float cherry_kit::screen::platform_screen::scale_factor(int a_id) const
{
  float desktop_scale =
      display_width(a_id) / desktop_width(a_id);

  if (desktop_scale < 1) {
    return 1.0f / desktop_scale;
  }

  return desktop_scale;
}

float cherry_kit::screen::platform_screen::desktop_width(int a_id) const
{
  return 1920.0f;
}

float cherry_kit::screen::platform_screen::desktop_height(int a_id) const
{
  float rv = 1080.0f;

  rv = (display_height(a_id) / display_width(a_id)) *
       desktop_width(a_id);

  return rv;
}

float cherry_kit::screen::platform_screen::display_width(int a_id) const
{
  return qApp->desktop()->geometry().width();
}

float cherry_kit::screen::platform_screen::display_height(int a_id) const
{
  return qApp->desktop()->geometry().height();
}
