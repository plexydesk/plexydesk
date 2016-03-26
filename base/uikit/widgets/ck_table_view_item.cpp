#include "ck_table_view_item.h"
#include <QDebug>

namespace cherry_kit {

table_view_item::table_view_item(const QRectF &a_rect, QGraphicsItem *parent)
    : QGraphicsObject(parent), QGraphicsLayoutItem(0, false) {
  setGraphicsItem(this);
}

table_view_item::~table_view_item() {
#ifdef PD_MEM_LEAK
  qDebug() << Q_FUNC_INFO;
#endif
}

void table_view_item::set_geometry(const QRectF &a_rect) {
  setPos(a_rect.x(), a_rect.y());
}

QString table_view_item::name() { return QString(); }
}
