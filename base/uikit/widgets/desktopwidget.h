#ifndef DESKTOPWIDGET_H
#define DESKTOPWIDGET_H

#include "plexydeskuicore_global.h"
#include <plexydesk_ui_exports.h>

#include <widget.h>
#include <QGraphicsLayoutItem>

#include <style.h>

namespace UI {
class DECL_UI_KIT_EXPORT UIWidget : public Widget {
  Q_OBJECT

public:
  typedef enum {
    kRenderBackground = 1ul << 0,
    kRenderDropShadow = 1ul << 1,
    kDockWindowType = 1ul << 2,
    kTopLevelWindow = 1ul << 3,
    kConvertToWindowType = 1ul << 4,
    kRenderWindowTitle = 1ul << 5
  } WidgetFlags;

  enum WindowState { kRenderAsWindow, kRenderAsMinimizedWindow };

  UIWidget(QGraphicsObject *parent = 0);

  virtual ~UIWidget();

  void setWindowTitle(const QString &title);

  virtual QString windowTitle() const;

  virtual void windowButtonAction();

  virtual void setWindowFlag(int flags, bool enable = true);

  virtual void updateWindowButton(int zValue);

  virtual WindowState state();

  virtual void setState(WindowState s);

Q_SIGNALS:
  void clicked();

protected:
  virtual void paintView(QPainter *painter, const QRectF &rect);
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
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
