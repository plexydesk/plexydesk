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

float cherry_kit::screen::platform_screen::scale_factor() const
{
  return 2.0f;
}

float cherry_kit::screen::platform_screen::desktop_width() const
{
  return 1920.0f;
}

float cherry_kit::screen::platform_screen::desktop_height() const
{
  return 1200.0f;
}

float cherry_kit::screen::platform_screen::display_width() const
{
  return 1920.0f;
}

float cherry_kit::screen::platform_screen::display_height() const
{
  return 1200.0f;
}
