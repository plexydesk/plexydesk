/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#include "model_view_item.h"

#include <QDebug>

namespace CherryKit {

class ModelViewItem::PrivateModelViewItem {
public:
  PrivateModelViewItem() : m_view_ptr(0), m_index(0) {}
  ~PrivateModelViewItem() {}

  QVariantMap m_data;
  Widget *m_view_ptr;

  int m_index;

  std::function<void(ModelViewItem *)> m_item_handler;
  std::function<void(ModelViewItem *)> m_item_remove_handler;
  std::function<bool(const Widget *, const QString &)> m_filter_handler;
};

ModelViewItem::ModelViewItem() : o_model_view_item(new PrivateModelViewItem) {}

ModelViewItem::~ModelViewItem() {
  qDebug() << Q_FUNC_INFO;
  if (o_model_view_item->m_view_ptr)
    remove_view();

  delete o_model_view_item;
}

void ModelViewItem::set_view(Widget *a_widget) {
  o_model_view_item->m_view_ptr = a_widget;

  a_widget->on_input_event([&](Widget::InputEvent a_event,
                               const Widget *a_widget) {
    qDebug() << Q_FUNC_INFO << "Model view register";
    if (a_event != Widget::kMouseReleaseEvent)
      return;

    if (o_model_view_item->m_item_handler) {
      o_model_view_item->m_item_handler(this);
    }
  });
}

void ModelViewItem::remove_view() {
  if (o_model_view_item->m_item_remove_handler)
    o_model_view_item->m_item_remove_handler(this);

  o_model_view_item->m_view_ptr = 0;
}

Widget *ModelViewItem::view() const { return o_model_view_item->m_view_ptr; }

void ModelViewItem::set_data(const QString &a_key, const QVariant &a_value) {
  o_model_view_item->m_data[a_key] = a_value;
}

QVariant ModelViewItem::data(const QString &a_key) const {
  return o_model_view_item->m_data[a_key];
}

void ModelViewItem::set_index(int a_index) { o_model_view_item->m_index = a_index; }

int ModelViewItem::index() const { return o_model_view_item->m_index; }

bool ModelViewItem::is_a_match(const QString &a_keyword) {
  if (!o_model_view_item->m_filter_handler)
    return 0;
  if (!o_model_view_item->m_view_ptr)
    return 0;

  return o_model_view_item->m_filter_handler(o_model_view_item->m_view_ptr, a_keyword);
}

void
ModelViewItem::on_activated(std::function<void(ModelViewItem *)> a_handler) {
  o_model_view_item->m_item_handler = a_handler;
}

void
ModelViewItem::on_view_removed(std::function<void(ModelViewItem *)> a_handler) {
  o_model_view_item->m_item_remove_handler = a_handler;
}

void ModelViewItem::on_filter(
    std::function<bool(const Widget *, const QString &)> a_handler) {
  o_model_view_item->m_filter_handler = a_handler;
}
}
