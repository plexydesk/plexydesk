#ifndef SIMPLE_CELL_CREATOR_H
#define SIMPLE_CELL_CREATOR_H

#include <tableviewcellinterface.h>
#include <plexydesk_ui_exports.h>

using namespace PlexyDesk;

class DECL_UI_KIT_EXPORT DefaultTableModel : public TableModel {
  Q_OBJECT

public:
  DefaultTableModel(QGraphicsObject *parent = 0);

  virtual ~DefaultTableModel();

  float margin() const;

  float padding() const;

  virtual float leftMargin() const;

  virtual float rightMargin() const;

  virtual bool init();

  virtual TableRenderMode renderType() const;

  void insertItem(const QString &label, const QPixmap pixmap,
                  bool selected = false);

  void setLabelVisibility(bool visibility);

  void setCellSize(const QSize &size);

  void clear();

private:
  class PrivateTableDelegate;
  PrivateTableDelegate *const d;
};

#endif // SIMPLE_CELL_CREATOR_H
