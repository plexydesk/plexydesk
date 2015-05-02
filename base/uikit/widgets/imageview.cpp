#include "imageview.h"

#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <themepackloader.h>

namespace UIKit {
class ImageView::PrivateImageView {
public:
  PrivateImageView() {}

  ~PrivateImageView() {
    // qDebug() << Q_FUNC_INFO;
  }
  QPixmap mPixmap;
};

ImageView::ImageView(QGraphicsObject *parent)
    : Widget(parent), d(new PrivateImageView) {
  setFlag(QGraphicsItem::ItemIsMovable, true);
}

ImageView::~ImageView() { delete d; }

QSizeF ImageView::sizeHint(Qt::SizeHint which,
                           const QSizeF &a_constraint) const {
  return d->mPixmap.size();
}

void ImageView::set_size(const QSizeF &size) {
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

void ImageView::set_pixmap(const QPixmap &a_pixmap) { d->mPixmap = a_pixmap; }

StylePtr ImageView::style() const { return Theme::style(); }

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
  int marginLeft = (a_exposeRect.width() - d->mPixmap.width()) / 2;

  QRect rect(marginLeft, a_exposeRect.y(), d->mPixmap.width(),
             d->mPixmap.height());

  a_painter_ptr->save();
  a_painter_ptr->drawPixmap(rect, d->mPixmap);
  a_painter_ptr->restore();
}
}
