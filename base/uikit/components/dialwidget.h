#ifndef DIAL_WIDGET_H
#define DIAL_WIDGET_H

#include <plexy.h>
#include <widget.h>
#include <QGraphicsSceneMouseEvent>
#include <plexydesk_ui_exports.h>

namespace UIKit {
class DECL_UI_KIT_EXPORT DialWidget : public UIKit::Widget {
  Q_OBJECT
public:
  DialWidget(QGraphicsObject *a_parent_ptr = 0);

  virtual ~DialWidget();

  virtual void set_maximum_dial_value(float maximum_dial_value);
  virtual float maximum_dial_value() const;

  virtual float current_dial_value() const;
  virtual void reset();

  virtual void on_dialed(std::function<void (int)> a_callback);
Q_SIGNALS:
  void value(float value);

protected:
  virtual void paint_view(QPainter *painter, const QRectF &rect);
  virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
  class PrivateDialWidget;
  PrivateDialWidget *const d;
};
}

#endif // DIAL_WIDGET_H
