#ifndef DIAL_WIDGET_H
#define DIAL_WIDGET_H

#include <plexy.h>
#include <widget.h>
#include <QGraphicsSceneMouseEvent>
#include <plexydesk_ui_exports.h>

namespace UI
{
class DECL_UI_KIT_EXPORT DialWidget : public UI::Widget
{
  Q_OBJECT
public:
  DialWidget(QGraphicsObject *parent = 0);

  virtual ~DialWidget();

  virtual void setMaxValue(float maxValue);

  virtual float maxValue() const;

  virtual float currentValue() const;

Q_SIGNALS:
  void value(float value);

protected:
  virtual void paintView(QPainter *painter, const QRectF &rect);
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
