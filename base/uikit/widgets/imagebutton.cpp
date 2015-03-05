#include "imagebutton.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <themepackloader.h>

#include <QDebug>

namespace UIKit
{

class ImageButton::PrivateImageButton
{
public:
  PrivateImageButton() {}
  ~PrivateImageButton()
  {
    if (mZoomAnimation) {
      delete mZoomAnimation;
    }
  }

  QPixmap mPixmap;

  QPropertyAnimation *mZoomAnimation;
  QString mLabel;
  QColor mBgColor;
};

void ImageButton::createZoomAnimation()
{
  if (!d->mZoomAnimation) {
    return;
  }

  d->mZoomAnimation->setDuration(100);
  d->mZoomAnimation->setStartValue(1.0);
  d->mZoomAnimation->setEndValue(1.1);
}

void ImageButton::setBackgroundColor(const QColor &color)
{
  d->mBgColor = color;
  update();
}

StylePtr ImageButton::style() const
{
  return Theme::instance()->defaultDesktopStyle();
}

ImageButton::ImageButton(QGraphicsObject *parent)
  : Widget(parent), d(new PrivateImageButton)
{
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setAcceptHoverEvents(true);

  d->mBgColor = Qt::transparent;

  /// setSize(QSize(32.0, 32.0));

  d->mZoomAnimation = new QPropertyAnimation(this, "scale");
  d->mZoomAnimation->setDuration(100);
  d->mZoomAnimation->setStartValue(1.0);
  d->mZoomAnimation->setEndValue(1.1);

  connect(d->mZoomAnimation, SIGNAL(finished()), this, SLOT(onZoomDone()));
}

ImageButton::~ImageButton() { delete d; }

void ImageButton::setSize(const QSize &size)
{
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

QSizeF ImageButton::sizeHint(Qt::SizeHint which,
                             const QSizeF &constraint) const
{
  return geometry().size();
}

void ImageButton::setPixmap(const QPixmap &pixmap)
{
  d->mPixmap = pixmap;
  // QGraphicsItem::setTransformOriginPoint(boundingRect().center());
}

void ImageButton::setLable(const QString &text) { d->mLabel = text; }

QString ImageButton::label() const { return d->mLabel; }

void ImageButton::onZoomDone() {}

void ImageButton::onZoomOutDone() {}

void ImageButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  if (!(d->mZoomAnimation->state() == QAbstractAnimation::Running)) {
    d->mZoomAnimation->setDirection(QAbstractAnimation::Backward);
    d->mZoomAnimation->start();
  }

  qDebug() << Q_FUNC_INFO;
  Widget::mouseReleaseEvent(event);
}

void ImageButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  Q_EMIT selected(true);
  Widget::mousePressEvent(event);
}

void ImageButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  if (!(d->mZoomAnimation->state() == QAbstractAnimation::Running)) {
    d->mZoomAnimation->setDirection(QAbstractAnimation::Forward);
    d->mZoomAnimation->start();
  }

  event->accept();

  Widget::hoverEnterEvent(event);
}

void ImageButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  if (scale() > 1.0) {
    d->mZoomAnimation->setDirection(QAbstractAnimation::Backward);
    d->mZoomAnimation->start();
  }

  event->accept();

  Q_EMIT selected(false);

  Widget::hoverLeaveEvent(event);
}

void ImageButton::paintView(QPainter *painter, const QRectF &rect)
{
  painter->save();

  painter->setRenderHints(QPainter::SmoothPixmapTransform |
                          QPainter::Antialiasing |
                          QPainter::HighQualityAntialiasing);

  QPainterPath bgPath;

  bgPath.addEllipse(rect);
  painter->fillPath(bgPath, d->mBgColor);

  painter->drawPixmap(rect.toRect(), d->mPixmap);
  painter->restore();
}
}
