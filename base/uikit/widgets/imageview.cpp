#include "imageview.h"

#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <resource_manager.h>

namespace cherry_kit {
class ImageView::PrivateImageView {
public:
  PrivateImageView() {}

  ~PrivateImageView() {
    // qDebug() << Q_FUNC_INFO;
  }
  QPixmap mPixmap;
};

ImageView::ImageView(widget *parent)
    : widget(parent), o_image_view(new PrivateImageView) {
  setFlag(QGraphicsItem::ItemIsMovable, true);
}

ImageView::~ImageView() { delete o_image_view; }

QSizeF ImageView::sizeHint(Qt::SizeHint which,
                           const QSizeF &a_constraint) const {
  return o_image_view->mPixmap.size();
}

void ImageView::set_size(const QSizeF &size) {
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

void ImageView::set_pixmap(const QPixmap &a_pixmap) {
  o_image_view->mPixmap = a_pixmap;
}

StylePtr ImageView::style() const { return resource_manager::style(); }

void ImageView::mouseMoveEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  Q_EMIT mouseOver();
  // QGraphicsObject::mouseMoveEvent(event);
}

void ImageView::hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  Q_EMIT mouseEnter();
  QGraphicsObject::hoverEnterEvent(a_event_ptr);
}

void ImageView::hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  Q_EMIT mouseLeave();
  QGraphicsObject::hoverLeaveEvent(a_event_ptr);
}

void ImageView::hoverMoveEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  Q_EMIT mouseOver();
  QGraphicsObject::hoverMoveEvent(a_event_ptr);
}

void ImageView::paint_view(QPainter *a_painter_ptr,
                           const QRectF &a_exposeRect) {
  int marginLeft = (a_exposeRect.width() - o_image_view->mPixmap.width()) / 2;

  QRect rect(marginLeft, a_exposeRect.y(), o_image_view->mPixmap.width(),
             o_image_view->mPixmap.height());

  a_painter_ptr->save();
  a_painter_ptr->setRenderHint(QPainter::SmoothPixmapTransform, false);
  a_painter_ptr->setRenderHint(QPainter::HighQualityAntialiasing, false);
  a_painter_ptr->drawPixmap(rect, o_image_view->mPixmap);
  a_painter_ptr->restore();
}
}
