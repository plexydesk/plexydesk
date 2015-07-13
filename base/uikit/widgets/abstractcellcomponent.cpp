#include "abstractcellcomponent.h"
#include <QDebug>

namespace cherry_kit {

TableViewItem::TableViewItem(const QRectF &a_rect, QGraphicsItem *parent)
    : QGraphicsObject(parent), QGraphicsLayoutItem(0, false) {
  setGraphicsItem(this);
}

TableViewItem::~TableViewItem() {
#ifdef PD_MEM_LEAK
  qDebug() << Q_FUNC_INFO;
#endif
}

void TableViewItem::setGeometry(const QRectF &a_rect) {
  setPos(a_rect.x(), a_rect.y());
}

QString TableViewItem::name() { return QString(); }
}
