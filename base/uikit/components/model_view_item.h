#ifndef MODELVIEWITEM_H
#define MODELVIEWITEM_H

#include <functional>

#include <widget.h>

namespace UIKit {
class ModelViewItem {
public:
  explicit ModelViewItem();
  virtual ~ModelViewItem();

  virtual void set_view(Widget *a_widget);
  virtual Widget *view() const;

  virtual void set_data(const QString &a_key, const QVariant &a_value);
  virtual QVariant data(const QString &a_key) const;

  virtual void set_index(int a_index);
  virtual int index() const;

  virtual bool is_a_match(const QString &a_keyword);

  virtual void on_activated(std::function<void(ModelViewItem *)> a_handler);
  virtual void on_filter(
      std::function<bool(const Widget *, const QString &)> a_handler);

private:
  class PrivateModelViewItem;
  PrivateModelViewItem *const m_priv_ptr;
};
}

#endif // MODELVIEWITEM_H
