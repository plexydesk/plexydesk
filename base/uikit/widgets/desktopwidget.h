#ifndef DESKTOPWIDGET_H
#define DESKTOPWIDGET_H

#include "plexydeskuicore_global.h"
#include <plexydesk_ui_exports.h>

#include <widget.h>
#include <QGraphicsLayoutItem>

#include <style.h>

namespace UI
{
class DECL_UI_KIT_EXPORT Window : public Window
{
  Q_OBJECT

public:
  explicit Window(QGraphicsObject *parent = 0);
  virtual ~Window();

Q_SIGNALS:
  void clicked();
  void closed(UI::Window *widget);

protected:
  StylePtr style() const;

private Q_SLOTS:
  void zoomDone();
  void propertyAnimationForZoomDone();
  void pressHoldTimeOut();
  void windowCloseButtonClicked();

private:
  class PrivateDesktopWidget;
  PrivateDesktopWidget *const d;
};
}
#endif // DESKTOPWIDGET_H
