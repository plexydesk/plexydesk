#ifndef MODELVIEWITEM_H
#define MODELVIEWITEM_H

#include <functional>

#include <widget.h>

namespace UIKit {
class ModelViewItem
{
public:
  explicit ModelViewItem();
  virtual ~ModelViewItem();

  virtual void set_view(Widget *a_widget);
  virtual Widget *view() const;

  virtual void set_data(const QString &a_key, const QVariant &a_value);
  virtual QVariant data(const QString &a_key) const;

  virtual void set_index(int a_index);
  virtual int index() const;

  virtual void on_activated(std::function<void(ModelViewItem *item)> a_handler);

private:
  class PrivateModelViewItem;
  PrivateModelViewItem * const m_priv_ptr;
};
}

#endif // MODELVIEWITEM_H
