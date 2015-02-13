#include "abstractcellcomponent.h"
#include <QDebug>

namespace UIKit
{

TableViewItem::TableViewItem(const QRectF &rect, QGraphicsItem *parent)
  : QGraphicsObject(parent), QGraphicsLayoutItem(0, false)
{
  setGraphicsItem(this);
}

TableViewItem::~TableViewItem()
{
#ifdef PD_MEM_LEAK
  qDebug() << Q_FUNC_INFO;
#endif
}

void TableViewItem::setGeometry(const QRectF &rect)
{
  setPos(rect.x(), rect.y());
}

QString TableViewItem::name() { return QString(); }
}
