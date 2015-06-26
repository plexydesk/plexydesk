#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <plexydesk_ui_exports.h>

#include <style.h>

namespace UIKit {
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
  ThemepackLoaderPrivate *const d;

  static ResourceManager *s_theme_instance;
};
}
#endif
