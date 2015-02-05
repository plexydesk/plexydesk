#ifndef CELLITEM_H
#define CELLITEM_H

#include <plexy.h>

#include <QGraphicsItem>
#include <QGraphicsObject>
#include <abstractcellcomponent.h>

class CellItem : public PlexyDesk::TableViewItem
{
  Q_OBJECT
public:
  CellItem(const QRectF &rect, QGraphicsItem *parent = 0);

  QRectF boundingRect() const;

  virtual QSizeF sizeHint(Qt::SizeHint hint, const QSizeF &size) const;

  void setSelected();

  void clearSelection();

protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);

private:
  class PrivateCellItem;
  PrivateCellItem *const d;
};

#endif // CELLITEM_H
