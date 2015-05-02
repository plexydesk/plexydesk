#ifndef WEBKITWEBVIEW_H
#define WEBKITWEBVIEW_H

#include <plexy.h>
#include <widget.h>
#include <QGraphicsWebView>
#include <plexydesk_ui_exports.h>

namespace UIKit {
class DECL_UI_KIT_EXPORT WebKitWebView : public UIKit::Widget {
  Q_OBJECT
public:
  WebKitWebView(QGraphicsObject *a_parent_ptr = 0);
  virtual ~WebKitWebView();

  virtual void set_url(const QUrl &a_url);
  virtual void inject_q_object(const QString &a_object_name,
                               QObject *a_object_ptr);

public
Q_SLOTS:
  void add_java_script_object();

private:
  class PrivateWebKitWebView;
  PrivateWebKitWebView *const d;
};
}

#endif // WEBKITWEBVIEW_H
