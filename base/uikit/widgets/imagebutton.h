#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <plexydesk_ui_exports.h>
#include <QGraphicsLayoutItem>
#include <QGraphicsObject>
#include <widget.h>

namespace UIKit
{

class DECL_UI_KIT_EXPORT ImageButton : public Widget
{
  Q_OBJECT
public:
  explicit ImageButton(QGraphicsObject *a_parent_ptr = 0);
  virtual ~ImageButton();

  virtual void set_size(const QSize &a_size);
  virtual QSizeF sizeHint(Qt::SizeHint which,
                          const QSizeF &a_constraint = QSizeF()) const;

  virtual void set_pixmap(const QPixmap &a_pixmap);

  virtual void set_lable(const QString &a_text);
  virtual QString label() const;

  void set_background_color(const QColor &a_color);
  virtual StylePtr style() const;

Q_SIGNALS:
  void selected(bool);
private Q_SLOTS:
  void onZoomDone();
  void onZoomOutDone();
  void createZoomAnimation();

protected:
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event_ptr);
  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_rect);

private:
  class PrivateImageButton;
  PrivateImageButton *const d;
};
}

#endif // IMAGEBUTTON_H
