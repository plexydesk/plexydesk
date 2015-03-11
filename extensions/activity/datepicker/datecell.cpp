#include "datecell.h"

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

class DateCell::PrivateDateCell
{

public:
  PrivateDateCell() {}
  ~PrivateDateCell() {}

  QRectF mBoundingRect;

  QString mLabel;
  // QPixmap mIconImage;
  bool mIsSelected;
  bool mHeaderMode;

  bool mVisibility;
  QPixmap mPixmap;

  ItemLayout mType;
};

DateCell::DateCell(const QRectF &rect, ItemLayout type, QGraphicsItem *parent)
  : UIKit::TableViewItem(rect, parent), d(new PrivateDateCell)
{
  d->mBoundingRect = rect;
  d->mIsSelected = false;
  d->mVisibility = true;
  d->mHeaderMode = false;
  d->mType = type;

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
  setFiltersChildEvents(true);
  setAcceptTouchEvents(true);
  setAcceptHoverEvents(true);
}

DateCell::~DateCell()
{
  delete d;
}

QRectF DateCell::boundingRect() const
{
  QRectF rect;
  if (d->mType == List) {
    rect = QRectF(0.0, 0.0, d->mBoundingRect.width(), 64);
  } else {
    rect = d->mBoundingRect;
  }
  return rect;
}

QSizeF DateCell::sizeHint(Qt::SizeHint hint, const QSizeF &size) const
{
  return boundingRect().size();
}

void DateCell::set_selected()
{
  d->mIsSelected = true;
  update();
}

void DateCell::setLabelVisibility(bool visible) { d->mVisibility = visible; }

void DateCell::clear_selection() { d->mIsSelected = false; }

void DateCell::setLabel(const QString &txt)
{
  d->mLabel = txt;
  update();
}

void DateCell::setIcon(const QPixmap &pixmap) { d->mPixmap = pixmap; }

QPixmap DateCell::icon() { return d->mPixmap; }

void DateCell::setDefault(bool selection) { d->mIsSelected = selection; }

QString DateCell::label() const { return d->mLabel; }

QString DateCell::name() { return d->mLabel; }

void DateCell::setHeaderMode(bool header) { d->mHeaderMode = header; }

void DateCell::addDataItem(const QPixmap &pixmap, const QString &label)
{
  d->mLabel = label;
}

void DateCell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setRenderHint(QPainter::HighQualityAntialiasing);
  painter->setRenderHint(QPainter::SmoothPixmapTransform);

  QPen pen;
  if (d->mHeaderMode) {
    // painter->fillRect(option->exposedRect, QColor("#0AACF0"));
    painter->fillRect(option->exposedRect, QColor(Qt::transparent));
    pen.setColor(QColor("#FFFFFF"));
    painter->setPen(pen);
    QFont font = painter->font();
    font.setBold(true);
    font.setPixelSize(font.pixelSize() + 15);
    painter->setFont(font);
    // painter->drawRect(option->exposedRect);
    painter->drawText(option->exposedRect, Qt::AlignCenter, d->mLabel);
  } else {
    if (!d->mIsSelected) {
      painter->fillRect(option->exposedRect, QColor(Qt::transparent));
      pen.setColor(QColor("#FFFFFF"));
      painter->setPen(pen);
      QFont font = painter->font();
      font.setBold(true);
      font.setPixelSize(font.pixelSize() + 14);
      painter->setFont(font);
      // painter->drawRect(option->exposedRect);
      painter->drawText(option->exposedRect, Qt::AlignCenter, d->mLabel);
    } else {
      painter->fillRect(option->exposedRect, QColor(Qt::white));
      pen.setColor(QColor("#F28585"));
      painter->setPen(pen);
      QFont font = painter->font();
      font.setBold(true);
      font.setPixelSize(font.pixelSize() + 14);
      painter->setFont(font);
      painter->drawRect(option->exposedRect);
      painter->drawText(option->exposedRect, Qt::AlignCenter, d->mLabel);
    }
  }

  painter->restore();
}

void DateCell::mousePressEvent(QGraphicsSceneMouseEvent *event) {}

void DateCell::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  Q_EMIT clicked(this);
}

void DateCell::onClicked() { Q_EMIT clicked(this); }
