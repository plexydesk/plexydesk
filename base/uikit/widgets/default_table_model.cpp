#include "default_table_model.h"
#include "default_table_component.h"
#include <themepackloader.h>
#include <QHash>

class DefaultTableModel::PrivateTableDelegate
{
public:
  PrivateTableDelegate() {}
  ~PrivateTableDelegate() { m_data_map.clear(); }

  QList<TableViewItem *> m_table_view_item_list;
  QMap<QString, QPixmap> m_data_map;
  QSize m_current_item_size;
  bool m_current_item_label_visibility;
};

DefaultTableModel::DefaultTableModel(QGraphicsObject *parent)
  : UI::TableModel(parent), d(new PrivateTableDelegate)
{
  d->m_current_item_label_visibility = false;
  setCellSize(QSize(96, 96));
}

DefaultTableModel::~DefaultTableModel() { delete d; }

float DefaultTableModel::margin() const { return 0.0; }

float DefaultTableModel::padding() const { return 0.0; }

float DefaultTableModel::leftMargin() const { return 0.0; }

float DefaultTableModel::rightMargin() const { return 0.0; }

bool DefaultTableModel::init() { return true; }

TableModel::TableRenderMode DefaultTableModel::renderType() const
{
  return TableModel::kRenderAsGridView;
}

void DefaultTableModel::insertItem(const QString &label, const QPixmap pixmap,
                                   bool selected)
{
  DefaultTableComponent *item =
    new DefaultTableComponent(QRectF(0.0, 0.0, d->m_current_item_size.width(),
                                     d->m_current_item_size.height()),
                              DefaultTableComponent::kGridLayout, 0);

  item->setLabelVisibility(d->m_current_item_label_visibility);
  item->setData(pixmap, label);

  Q_EMIT add(item);
}

void DefaultTableModel::setLabelVisibility(bool visibility)
{
  d->m_current_item_label_visibility = visibility;
}

void DefaultTableModel::setCellSize(const QSize &size)
{
  d->m_current_item_size = size;
}

void DefaultTableModel::clear() { Q_EMIT cleared(); }
