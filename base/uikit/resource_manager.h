/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <plexydesk_ui_exports.h>

#include <style.h>

namespace CherryKit {
class DECL_UI_KIT_EXPORT ResourceManager {
public:
  ResourceManager(const QString &a_theme_name);
  virtual ~ResourceManager();

  static ResourceManager *instance();

  virtual void set_theme_name(const QString &a_name);

  static StylePtr style();

  static QPixmap icon(const QString &a_name, const QString &a_resolution);
  virtual QPixmap drawable(const QString &a_fileName, const QString &a_dpi);

  typedef enum {
    kDarkPrimaryColor = 1,
    kPrimaryColor,
    kLightPrimaryColor,
    kTextBackground,
    kTextColor,
    kSecondryTextColor,
    kDividerColor,
    kAccentColor
  } ColorName;

  static const char *color(ColorName a_name);
  virtual void set_color_scheme(const std::string &a_name);
  virtual std::string color_scheme() const;
  virtual const char *color_code(ColorName a_name);

private:
  void scane_resources();
  StylePtr default_desktop_style();
  void load_default_color_values();

  class ThemepackLoaderPrivate;
  ThemepackLoaderPrivate *const o_resource_manager;

  static ResourceManager *s_theme_instance;
};
}
#endif
