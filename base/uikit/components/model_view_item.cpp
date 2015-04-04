#include "model_view_item.h"


#include <QDebug>

namespace UIKit {

class ModelViewItem::PrivateModelViewItem {
public:
  PrivateModelViewItem() : m_view_ptr(0), m_index(0) {}
  ~PrivateModelViewItem() {}

  QVariantMap m_data;
  Widget *m_view_ptr;

  int m_index;

  std::function<void (ModelViewItem *)> m_item_handler;
};

ModelViewItem::ModelViewItem() : m_priv_ptr(new PrivateModelViewItem)
{
}

ModelViewItem::~ModelViewItem()
{
  delete m_priv_ptr;
}

void ModelViewItem::set_view(Widget *a_widget)
{
  m_priv_ptr->m_view_ptr = a_widget;

  a_widget->on_input_event([&](Widget::InputEvent a_event,
                                         const Widget *a_widget) {
    qDebug() << Q_FUNC_INFO << "Model view register";
    if (a_event != Widget::kMouseReleaseEvent)
      return;

    if (m_priv_ptr->m_item_handler) {
      m_priv_ptr->m_item_handler(this);
    }
  });
}

Widget *ModelViewItem::view() const
{
  return m_priv_ptr->m_view_ptr;
}

void ModelViewItem::set_data(const QString &a_key, const QVariant &a_value)
{
 m_priv_ptr->m_data[a_key] = a_value;
}

QVariant ModelViewItem::data(const QString &a_key) const
{
  return m_priv_ptr->m_data[a_key];
}

void ModelViewItem::set_index(int a_index)
{
  m_priv_ptr->m_index = a_index;
}

int ModelViewItem::index() const
{
  return m_priv_ptr->m_index;
}

void ModelViewItem::on_activated(std::function<void (ModelViewItem *)> a_handler)
{
  m_priv_ptr->m_item_handler = a_handler;
}
}
