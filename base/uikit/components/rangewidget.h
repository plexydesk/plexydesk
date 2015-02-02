#ifndef RANGE_WIDGET_H
#define RANGE_WIDGET_H

#include <plexy.h>
#include <desktopwidget.h>
#include <QGraphicsSceneMouseEvent>
#include <plexydesk_ui_exports.h>

namespace UI {

class DECL_UI_KIT_EXPORT RangeWidget : public UI::UIWidget {
  Q_OBJECT
public:
  RangeWidget(QGraphicsObject *parent = 0);

  virtual ~RangeWidget();

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
  class PrivateRangeWidget;
  PrivateRangeWidget *const d;
};
}

#endif // RANGE_WIDGET_H
