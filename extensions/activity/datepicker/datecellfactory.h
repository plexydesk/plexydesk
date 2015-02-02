#ifndef DATE_CELL_FACTORY_H
#define DATE_CELL_FACTORY_H

#include <tableviewcellinterface.h>

using namespace UI;

class DateCellFactory : public TableModel {
  Q_OBJECT

public:
  DateCellFactory(QGraphicsObject *parent = 0);
  virtual ~DateCellFactory();

  QList<UI::TableViewItem *> componentList();

  float margin() const;

  float padding() const;

  virtual float leftMargin() const;

  virtual float rightMargin() const;

  virtual bool init();

  virtual TableRenderMode renderType() const;

  void addDataItem(const QString &label, const QPixmap pixmap,
                   bool selected = false);

  void setLabelVisibility(bool visibility);

  void setHeaderMode(bool mode);

  void setCellSize(const QSize &size);

  UI::TableViewItem *itemAt(int i);

private:
  class PrivateDateCellFactory;
  PrivateDateCellFactory *const d;
};

#endif // DATE_CELL_FACTORY_H
