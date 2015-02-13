#ifndef WEBKITWEBVIEW_H
#define WEBKITWEBVIEW_H

#include <plexy.h>
#include <widget.h>
#include <QGraphicsWebView>
#include <plexydesk_ui_exports.h>

namespace UIKit
{
class DECL_UI_KIT_EXPORT WebKitWebView : public UIKit::Widget
{
  Q_OBJECT
public:
  WebKitWebView(QGraphicsObject *parent = 0);
  virtual ~WebKitWebView();

  virtual void setUrl(const QUrl &url);
  virtual void injectQObject(const QString &objectName, QObject *object);

public Q_SLOTS:
  void addJavaScriptObject();

private:
  class PrivateWebKitWebView;
  PrivateWebKitWebView *const d;
};
}

#endif // WEBKITWEBVIEW_H
