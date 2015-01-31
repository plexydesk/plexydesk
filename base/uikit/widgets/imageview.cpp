#include "imageview.h"

#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <themepackloader.h>

namespace PlexyDesk {
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

QSizeF ImageView::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
  return d->mPixmap.size();
}

void ImageView::setSize(const QSizeF &size) {
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

void ImageView::setPixmap(const QPixmap &pixmap) { d->mPixmap = pixmap; }

StylePtr ImageView::style() const { return Theme::style(); }

void ImageView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  Q_EMIT clicked();
  QGraphicsObject::mouseReleaseEvent(event);
}

void ImageView::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  Q_EMIT mouseOver();
  //QGraphicsObject::mouseMoveEvent(event);
}

void ImageView::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  Q_EMIT mouseEnter();
  QGraphicsObject::hoverEnterEvent(event);
}

void ImageView::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  Q_EMIT mouseLeave();
  QGraphicsObject::hoverLeaveEvent(event);
}

void ImageView::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  Q_EMIT mouseOver();
  QGraphicsObject::hoverMoveEvent(event);
}

void ImageView::paintView(QPainter *painter, const QRectF &exposeRect) {
  int marginLeft = (exposeRect.width() - d->mPixmap.width()) / 2;

  QRect rect(marginLeft, exposeRect.y(), d->mPixmap.width(),
             d->mPixmap.height());

  painter->save();
  painter->drawPixmap(rect, d->mPixmap);
  painter->restore();
}
}
