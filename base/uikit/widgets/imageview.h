#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <widget.h>

#include <QGraphicsObject>
#include <QGraphicsLayoutItem>
#include <plexydesk_ui_exports.h>

namespace UIKit
{
class DECL_UI_KIT_EXPORT ImageView : public Widget
{
  Q_OBJECT
public:
  explicit ImageView(QGraphicsObject *a_parent_ptr = 0);
  virtual ~ImageView();

  virtual void setSize(const QSizeF &size);
  virtual QSizeF sizeHint(Qt::SizeHint which,
                          const QSizeF &constraint = QSizeF()) const;

  virtual void setPixmap(const QPixmap &pixmap);
  virtual StylePtr style() const;
Q_SIGNALS:
  void mouseOver();
  void mouseReleased();
  void mouseEnter();
  void mouseLeave();
private:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

  virtual void paint_view(QPainter *painter, const QRectF &exposeRect);
private:
  class PrivateImageView;
  PrivateImageView *const d;
};
}
#endif // IMAGEVIEW_H
