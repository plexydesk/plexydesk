#ifndef ABSTRACTCELLCOMPONENT_H
#define ABSTRACTCELLCOMPONENT_H

#include <QGraphicsObject>
#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
#include <plexydesk_ui_exports.h>

namespace UIKit
{

class DECL_UI_KIT_EXPORT TableViewItem : public QGraphicsObject,
  public QGraphicsLayoutItem
{
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)

public:
  TableViewItem(const QRectF &rect, QGraphicsItem *a_parent_ptr = 0);

  virtual ~TableViewItem();

  virtual void setGeometry(const QRectF &rect);

  virtual void setSelected() = 0;

  virtual void clearSelection() = 0;

  virtual QString name();

Q_SIGNALS:
  void clicked(TableViewItem *component);
};
}
#endif // ABSTRACTCELLCOMPONENT_H
