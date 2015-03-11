#ifndef DATE_CELL_H
#define DATE_CELL_H

#include <abstractcellcomponent.h>

class DateCell : public UIKit::TableViewItem
{
  Q_OBJECT
public:
  typedef enum { List = 0, Grid } ItemLayout;

  DateCell(const QRectF &rect, ItemLayout = List, QGraphicsItem *a_parent_ptr = 0);
  virtual ~DateCell();

  QRectF boundingRect() const;

  virtual QSizeF sizeHint(Qt::SizeHint hint, const QSizeF &size) const;

  void set_selected();

  void setLabelVisibility(bool visible);

  void clear_selection();

  void setLabel(const QString &txt);

  void setIcon(const QPixmap &pixmap);

  QPixmap icon();

  void setDefault(bool selection);

  QString label() const;

  QString name();

  void setHeaderMode(bool header);

  void addDataItem(const QPixmap &pixmap, const QString &label);

protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private Q_SLOTS:
  void onClicked();

private:
  class PrivateDateCell;
  PrivateDateCell *const d;
};

#endif // DATE_CELL_H
