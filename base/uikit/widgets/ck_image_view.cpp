#include "ck_image_view.h"

#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <ck_resource_manager.h>

namespace cherry_kit {
class image_view::PrivateImageView {
public:
  PrivateImageView() {}

  ~PrivateImageView() {
    // qDebug() << Q_FUNC_INFO;
  }
  QPixmap mPixmap;
  QImage m_image;
};

image_view::image_view(widget *parent)
    : widget(parent), o_image_view(new PrivateImageView) {
  setFlag(QGraphicsItem::ItemIsMovable, true);
}

image_view::~image_view() { delete o_image_view; }

QSizeF image_view::sizeHint(Qt::SizeHint which,
                           const QSizeF &a_constraint) const {
  //return o_image_view->mPixmap.size();
  return geometry().size();
}

void image_view::set_size(const QSizeF &size) {
  set_geometry(QRectF(0, 0, size.width(), size.height()));
}

void image_view::set_pixmap(const QPixmap &a_pixmap) {
    o_image_view->mPixmap = a_pixmap;
}

void image_view::set_image(const QImage &a_image) {
    o_image_view->m_image = a_image;
    update();
}

style_ref image_view::style() const { return resource_manager::style(); }

void image_view::mouseMoveEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  Q_EMIT mouseOver();
  // QGraphicsObject::mouseMoveEvent(event);
}

void image_view::hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  Q_EMIT mouseEnter();
  QGraphicsObject::hoverEnterEvent(a_event_ptr);
}

void image_view::hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  Q_EMIT mouseLeave();
  QGraphicsObject::hoverLeaveEvent(a_event_ptr);
}

void image_view::hoverMoveEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  Q_EMIT mouseOver();
  QGraphicsObject::hoverMoveEvent(a_event_ptr);
}

void image_view::paint_view(QPainter *a_painter_ptr,
                           const QRectF &a_exposeRect) {
  int marginLeft = (a_exposeRect.width() - o_image_view->mPixmap.width()) / 2;
  a_painter_ptr->save();

  a_painter_ptr->setRenderHints(QPainter::HighQualityAntialiasing |
                                QPainter::SmoothPixmapTransform |
                                QPainter::Antialiasing,
                                true);

  QRect rect(marginLeft, a_exposeRect.y(), o_image_view->mPixmap.width(),
             o_image_view->mPixmap.height());

  if (!o_image_view->mPixmap.isNull())
    a_painter_ptr->drawPixmap(a_exposeRect.toRect(), o_image_view->mPixmap);

  if (!o_image_view->m_image.isNull()) {
    a_painter_ptr->drawImage(a_exposeRect, o_image_view->m_image,
                             a_exposeRect);
  }
  a_painter_ptr->restore();
}
}
