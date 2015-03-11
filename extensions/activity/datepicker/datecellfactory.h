#ifndef DATE_CELL_FACTORY_H
#define DATE_CELL_FACTORY_H

#include <tableviewcellinterface.h>

using namespace UIKit;

class DateCellFactory : public TableModel
{
  Q_OBJECT

public:
  DateCellFactory(QGraphicsObject *a_parent_ptr = 0);
  virtual ~DateCellFactory();

  QList<UIKit::TableViewItem *> componentList();

  float margin() const;

  float padding() const;

  virtual float left_margin() const;

  virtual float right_margin() const;

  virtual bool init();

  virtual TableRenderMode render_type() const;

  void addDataItem(const QString &label, const QPixmap pixmap,
                   bool selected = false);

  void setLabelVisibility(bool visibility);

  void setHeaderMode(bool mode);

  void setCellSize(const QSize &size);

  UIKit::TableViewItem *itemAt(int i);

private:
  class PrivateDateCellFactory;
  PrivateDateCellFactory *const d;
};

#endif // DATE_CELL_FACTORY_H
