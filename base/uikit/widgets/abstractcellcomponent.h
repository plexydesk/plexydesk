#ifndef ABSTRACTCELLCOMPONENT_H
#define ABSTRACTCELLCOMPONENT_H

#include <QGraphicsObject>
#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
#include <plexydesk_ui_exports.h>

namespace CherryKit {

class DECL_UI_KIT_EXPORT TableViewItem : public QGraphicsObject,
                                         public QGraphicsLayoutItem {
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)

public:
  TableViewItem(const QRectF &a_rect, QGraphicsItem *a_parent_ptr = 0);

  virtual ~TableViewItem();

  virtual void setGeometry(const QRectF &a_rect);

  virtual void set_selected() = 0;

  virtual void clear_selection() = 0;

  virtual QString name();

Q_SIGNALS:
  void clicked(TableViewItem *a_component_ptr);
};
}
#endif // ABSTRACTCELLCOMPONENT_H
