#include "default_table_model.h"
#include "default_table_component.h"
#include <resource_manager.h>
#include <QHash>

class DefaultTableModel::PrivateTableDelegate {
public:
  PrivateTableDelegate() {}
  ~PrivateTableDelegate() { m_data_map.clear(); }

  QList<TableViewItem *> m_table_view_item_list;
  QMap<QString, QPixmap> m_data_map;
  QSize m_current_item_size;
  bool m_current_item_label_visibility;
};

DefaultTableModel::DefaultTableModel(QGraphicsObject *parent)
    : cherry_kit::TableModel(parent), d(new PrivateTableDelegate) {
  d->m_current_item_label_visibility = false;
  set_cell_size(QSize(96, 96));
}

DefaultTableModel::~DefaultTableModel() { delete d; }

float DefaultTableModel::margin() const { return 0.0; }

float DefaultTableModel::padding() const { return 0.0; }

float DefaultTableModel::left_margin() const { return 0.0; }

float DefaultTableModel::right_margin() const { return 0.0; }

bool DefaultTableModel::init() { return true; }

TableModel::TableRenderMode DefaultTableModel::render_type() const {
  return TableModel::kRenderAsGridView;
}

void DefaultTableModel::insert_item(const QString &a_label,
                                    const QPixmap a_pixmap, bool a_selected) {
  DefaultTableComponent *item =
      new DefaultTableComponent(QRectF(0.0, 0.0, d->m_current_item_size.width(),
                                       d->m_current_item_size.height()),
                                DefaultTableComponent::kGridLayout, 0);

  item->set_label_visibility(d->m_current_item_label_visibility);
  item->set_data(a_pixmap, a_label);

  Q_EMIT add(item);
}

void DefaultTableModel::set_label_visibility(bool a_visibility) {
  d->m_current_item_label_visibility = a_visibility;
}

void DefaultTableModel::set_cell_size(const QSize &a_size) {
  d->m_current_item_size = a_size;
}

void DefaultTableModel::clear() { Q_EMIT cleared(); }
