#include "imagebutton.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <themepackloader.h>

#include <QDebug>

namespace UIKit {

class ImageButton::PrivateImageButton {
public:
  PrivateImageButton() {
  }
  ~PrivateImageButton() {
  }

  QPixmap mPixmap;
  QString mLabel;
  QColor mBgColor;
};

void ImageButton::set_background_color(const QColor &a_color) {
  d->mBgColor = a_color;
  update();
}

StylePtr ImageButton::style() const { return Theme::style(); }

ImageButton::ImageButton(QGraphicsObject *a_parent_ptr)
    : Widget(a_parent_ptr), d(new PrivateImageButton) {
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setAcceptHoverEvents(true);

  d->mBgColor = Qt::transparent;
}

ImageButton::~ImageButton() { delete d; }

void ImageButton::set_size(const QSize &a_size) {
  setGeometry(QRectF(0, 0, a_size.width(), a_size.height()));
}

QSizeF ImageButton::sizeHint(Qt::SizeHint which,
                             const QSizeF &a_constraint) const {
  return geometry().size();
}

void ImageButton::set_pixmap(const QPixmap &a_pixmap) {
  d->mPixmap = a_pixmap;
  update();
}

void ImageButton::set_lable(const QString &a_text) {
    d->mLabel = a_text;
    update();
}

QString ImageButton::label() const { return d->mLabel; }

void ImageButton::onZoomDone() {}

void ImageButton::onZoomOutDone() {}

void ImageButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  Widget::mouseReleaseEvent(a_event_ptr);
}

void ImageButton::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  Q_EMIT selected(true);
  Widget::mousePressEvent(a_event_ptr);
}

void ImageButton::hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  a_event_ptr->accept();
  Widget::hoverEnterEvent(a_event_ptr);
}

void ImageButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  a_event_ptr->accept();
  Q_EMIT selected(false);
  Widget::hoverLeaveEvent(a_event_ptr);
}

void ImageButton::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  a_painter_ptr->save();

  a_painter_ptr->setRenderHints(QPainter::SmoothPixmapTransform |
                                QPainter::Antialiasing |
                                QPainter::HighQualityAntialiasing);

  QPainterPath bgPath;

  bgPath.addEllipse(a_rect);

  a_painter_ptr->fillPath(bgPath, d->mBgColor);

  QRect icon_rect = a_rect.toRect();
  icon_rect.setX(a_rect.center().x() - (icon_rect.width() / 2));
  icon_rect.setWidth(icon_rect.height());

  QRect text_rect = a_rect.toRect();
  text_rect.setX(icon_rect.width() + 5);

  //a_painter_ptr->drawRect(icon_rect);
  //a_painter_ptr->drawRect(text_rect);
  a_painter_ptr->drawPixmap(icon_rect, d->mPixmap);
  a_painter_ptr->drawText(text_rect, d->mLabel,
                          Qt::AlignLeft | Qt::AlignVCenter);
  a_painter_ptr->restore();
}
}
