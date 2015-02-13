#ifndef THEMEPACKLOADER_H
#define THEMEPACKLOADER_H

#include "plexydeskuicore_global.h"
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
  Q_PROPERTY(QString themeName READ QString WRITE setThemeName)

public:
  Theme(const QString &themeName, QObject *parent = 0);

  virtual ~Theme();

  void setThemeName(const QString &name);

  QStringList desktopWidgets() const;

  QString desktopBackgroundController() const;

  QRectF widgetPos(const QString &name, const QRectF &screen_rect);

  QRectF backgroundPos(const QString &name, const QRectF &screen_rect);

  QString desktopSessionData() const;

  void commitSessionData(const QString &data);

  QPixmap drawable(const QString &fileName, const QString &resoution);

  StylePtr defaultDesktopStyle();

  static StylePtr style();

  static Theme *instance();

  static QPixmap icon(const QString &name, const QString &resolution);

  void requestNoteSideImageFromWebService(const QString &key);

  void requestPhotoSizes(const QString &photoID);

public Q_SLOTS:
  void onImageReadyJson(const QString &fileName);
  void onImageSaveReadyJson();
  void onImageReady();
  void onDownloadCompleteJson(QuetzalSocialKit::WebService *service);
  void onSizeServiceCompleteJson(QuetzalSocialKit::WebService *service);
  void onServiceCompleteJson(QuetzalSocialKit::WebService *service);

Q_SIGNALS:
  void ready();
  void imageSearchDone(const QImage &pixmap);

private:
  void scanThemepackPrefix();
  int toScreenValue(const QString &val, int max_distance);

  class ThemepackLoaderPrivate;
  ThemepackLoaderPrivate *const d;
  static Theme *s_theme_instance;
};

} // namespace PlexyDesk

#endif // THEMEPACKLOADER_H
