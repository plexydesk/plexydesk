#ifndef THEMEPACKLOADER_H
#define THEMEPACKLOADER_H

#include <plexydesk_ui_exports.h>

#include <QObject>
#include <QSettings>
#include <QPoint>
#include <widget.h>
#include <style.h>
#include <datastore.h>
#include <webservice.h>

namespace UIKit
{
class DECL_UI_KIT_EXPORT Theme : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString themeName READ QString WRITE set_theme_name)

public:
  Theme(const QString &a_theme_name, QObject *a_parent_ptr = 0);
  virtual ~Theme();

  static Theme *instance();

  virtual void set_theme_name(const QString &a_name);

  static StylePtr style();
  static QPixmap icon(const QString &a_name, const QString &a_resolution);

  virtual QPixmap drawable(const QString &a_fileName, const QString &a_resoution);
private:
  void scane_resources();
  StylePtr default_desktop_style();

  class ThemepackLoaderPrivate;
  ThemepackLoaderPrivate *const d;

  static Theme *s_theme_instance;
};

} // namespace PlexyDesk

#endif // THEMEPACKLOADER_H
