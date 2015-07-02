#include "imagecell.h"

#include <QPainter>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

#include <button.h>
#include <imageview.h>
#include <label.h>
#include <resource_manager.h>

class ImageCell::PrivateImageCell {

public:
  PrivateImageCell() {}
  ~PrivateImageCell() {}

  QRectF mBoundingRect;

  QString mLabel;
  // QPixmap mIconImage;
  bool mIsSelected;
  bool mVisibility;

  QImage mImage;
  ItemLayout mType;
};

ImageCell::ImageCell(const QRectF &rect, ItemLayout type, QGraphicsItem *parent)
    : CherryKit::TableViewItem(rect, parent), d(new PrivateImageCell) {
  d->mBoundingRect = rect;
  d->mIsSelected = false;
  d->mVisibility = true;
  d->mType = type;

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
  setFiltersChildEvents(true);
  setAcceptTouchEvents(true);
  setAcceptHoverEvents(true);
}

ImageCell::~ImageCell() {
#ifdef PD_MEM_LEAK
  qDebug() << Q_FUNC_INFO;
#endif
  delete d;
}

QRectF ImageCell::boundingRect() const { return QRectF(0.0, 0.0, 150, 150); }

QSizeF ImageCell::sizeHint(Qt::SizeHint hint, const QSizeF &size) const {
  return boundingRect().size();
}

void ImageCell::set_selected() {
  d->mIsSelected = true;
  update();
}

void ImageCell::setLabelVisibility(bool visible) { d->mVisibility = visible; }

void ImageCell::clear_selection() { d->mIsSelected = false; }

void ImageCell::setLabel(const QString &txt) {
  d->mLabel = txt;
  update();
}

void ImageCell::setIcon(const QPixmap &pixmap) {}

void ImageCell::setDefault(bool selection) { d->mIsSelected = selection; }

QString ImageCell::label() const { return d->mLabel; }

void ImageCell::addDataItem(const QImage &img, const QString &label,
                            const QVariantMap &metaData) {
  /*
  d->mImageView->setPixmap(img);
  d->mLabelView->setLabel(label);
  */
  d->mImage = img;
  d->mLabel = label;
  update();
}

void ImageCell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *widget) {
  /* Painter settings */
  if (d->mType == List || d->mIsSelected) {
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

    QPainterPath path;
    path.addRoundedRect(boundingRect(), 4.0, 4.0);
    painter->fillPath(path, QColor(254, 254, 254));
  }

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  // QRectF source(0.0, 0.0, 150, 150);
  painter->fillRect(option->exposedRect, Qt::black);
  painter->drawImage(option->exposedRect, d->mImage, option->exposedRect,
                     Qt::ColorOnly | Qt::DiffuseDither |
                         Qt::DiffuseAlphaDither);

  painter->save();
  painter->setPen(QColor(0, 0, 0));
  painter->drawRect(option->exposedRect);
  painter->restore();
}

void ImageCell::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  qDebug() << Q_FUNC_INFO << "Item Clicked";
  Q_EMIT clicked(this);
}

void ImageCell::mousePressEvent(QGraphicsSceneMouseEvent *event) {}

void ImageCell::onClicked() {

  qDebug() << Q_FUNC_INFO << "Item Clicked";
  Q_EMIT clicked(this);
}
