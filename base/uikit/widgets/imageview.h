#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <widget.h>

#include <QGraphicsObject>
#include <QGraphicsLayoutItem>
#include <plexydesk_ui_exports.h>

namespace UIKit {
class DECL_UI_KIT_EXPORT ImageView : public Widget {
  Q_OBJECT
public:
  explicit ImageView(QGraphicsObject *a_parent_ptr = 0);
  virtual ~ImageView();

  virtual void set_size(const QSizeF &a_size);
  virtual QSizeF sizeHint(Qt::SizeHint which,
                          const QSizeF &a_constraint = QSizeF()) const;

  virtual void set_pixmap(const QPixmap &a_pixmap);
  virtual StylePtr style() const;
Q_SIGNALS:
  void mouseOver();
  void mouseReleased();
  void mouseEnter();
  void mouseLeave();

private:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event_ptr);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *a_event_ptr);

  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_exposeRect);

private:
  class PrivateImageView;
  PrivateImageView *const d;
};
}
#endif // IMAGEVIEW_H
