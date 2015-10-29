#include "cellitem.h"
#include <nativestyle.h>
#include <ck_button.h>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

class CellItem::PrivateCellItem {
public:
  PrivateCellItem() {}
  ~PrivateCellItem() {}
  QRectF mRect;
};

CellItem::CellItem(const QRectF &rect, QGraphicsItem *parent)
    : PlexyDesk::TableViewItem(rect, parent), d(new PrivateCellItem) {
  d->mRect = rect;
  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
  setAcceptTouchEvents(true);
  setAcceptHoverEvents(true);
  setGraphicsItem(this);
}

QRectF CellItem::boundingRect() const {
#ifdef Q_WS_QPA
  return QRectF(0.0, 0.0, d->mRect.width() - 80, 250);
#endif

  return QRectF(0, 0, d->mRect.width() - 20, 250);
}

QSizeF CellItem::sizeHint(Qt::SizeHint hint, const QSizeF &size) const {
  return boundingRect().size();
}

void CellItem::setSelected() {}

void CellItem::clearSelection() {}

void CellItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget) {
  if (!painter->isActive()) {
    return;
  }
  if (isObscured()) {
    return;
  }

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath path;
  path.addRoundedRect(boundingRect(), 4.0, 4.0);

  painter->fillPath(path, QColor(254, 254, 254));
}
