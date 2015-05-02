#ifndef SIMPLE_CELL_CREATOR_H
#define SIMPLE_CELL_CREATOR_H

#include <tableviewcellinterface.h>
#include <plexydesk_ui_exports.h>

using namespace UIKit;

class DECL_UI_KIT_EXPORT DefaultTableModel : public TableModel {
  Q_OBJECT

public:
  DefaultTableModel(QGraphicsObject *a_parent_ptr = 0);

  virtual ~DefaultTableModel();

  float margin() const;

  float padding() const;

  virtual float left_margin() const;

  virtual float right_margin() const;

  virtual bool init();

  virtual TableRenderMode render_type() const;

  void insert_item(const QString &a_label, const QPixmap a_pixmap,
                   bool a_selected = false);

  void set_label_visibility(bool a_visibility);

  void set_cell_size(const QSize &a_size);

  void clear();

private:
  class PrivateTableDelegate;
  PrivateTableDelegate *const d;
};

#endif // SIMPLE_CELL_CREATOR_H
