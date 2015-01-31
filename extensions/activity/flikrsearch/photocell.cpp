#include "photocell.h"

#include <QPainter>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

#include <button.h>
#include <imageview.h>
#include <label.h>
#include <themepackloader.h>

class PhotoCell::PrivatePhotoCell {

public:
  PrivatePhotoCell() {}
  ~PrivatePhotoCell() {}

  QRectF mBoundingRect;

  QString mLabel;
  // QPixmap mIconImage;
  bool mIsSelected;
  bool mVisibility;

  QImage mImage;
  ItemLayout mType;
};

PhotoCell::PhotoCell(const QRectF &rect, ItemLayout type, QGraphicsItem *parent)
    : PlexyDesk::TableViewItem(rect, parent), d(new PrivatePhotoCell) {
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

PhotoCell::~PhotoCell() {
#ifdef PD_MEM_LEAK
  qDebug() << Q_FUNC_INFO;
#endif
  delete d;
}

QRectF PhotoCell::boundingRect() const { return QRectF(0.0, 0.0, 138, 138); }

QSizeF PhotoCell::sizeHint(Qt::SizeHint hint, const QSizeF &size) const {
  return boundingRect().size();
}

void PhotoCell::setSelected() {
  d->mIsSelected = true;
  update();
}

void PhotoCell::setLabelVisibility(bool visible) { d->mVisibility = visible; }

void PhotoCell::clearSelection() { d->mIsSelected = false; }

void PhotoCell::setLabel(const QString &txt) {
  d->mLabel = txt;
  update();
}

void PhotoCell::setIcon(const QPixmap &pixmap) {}

void PhotoCell::setDefault(bool selection) { d->mIsSelected = selection; }

QString PhotoCell::label() const { return d->mLabel; }

void PhotoCell::addDataItem(const QImage &img, const QString &label,
                            const QVariantMap &metaData) {
  /*
  d->mImageView->setPixmap(img);
  d->mLabelView->setLabel(label);
  */
  d->mImage = img;
  d->mLabel = label;
  update();
}

void PhotoCell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
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

  QRectF source(0.0, 0.0, 150, 150);
  painter->fillRect(option->exposedRect, Qt::black);
  painter->drawImage(option->exposedRect, d->mImage, source,
                     Qt::ColorOnly | Qt::DiffuseDither |
                         Qt::DiffuseAlphaDither);

  painter->save();
  painter->setPen(QColor(0, 0, 0));
  painter->drawRect(option->exposedRect);
  painter->restore();
}

void PhotoCell::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  qDebug() << Q_FUNC_INFO << "Item Clicked";
  Q_EMIT clicked(this);
}

void PhotoCell::mousePressEvent(QGraphicsSceneMouseEvent *event) {}

void PhotoCell::onClicked() {

  qDebug() << Q_FUNC_INFO << "Item Clicked";
  Q_EMIT clicked(this);
}
