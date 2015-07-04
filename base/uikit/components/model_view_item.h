#ifndef MODELVIEWITEM_H
#define MODELVIEWITEM_H

#include <functional>

#include <widget.h>

namespace CherryKit {
class DECL_UI_KIT_EXPORT ModelViewItem {
public:
  explicit ModelViewItem();
  virtual ~ModelViewItem();

  virtual void set_view(Widget *a_widget);
  virtual void remove_view();
  virtual Widget *view() const;

  virtual void set_data(const QString &a_key, const QVariant &a_value);
  virtual QVariant data(const QString &a_key) const;

  virtual void set_index(int a_index);
  virtual int index() const;

  virtual bool is_a_match(const QString &a_keyword);

  virtual void on_activated(std::function<void(ModelViewItem *)> a_handler);
  virtual void on_view_removed(std::function<void(ModelViewItem *)> a_handler);
  virtual void
  on_filter(std::function<bool(const Widget *, const QString &)> a_handler);

private:
  class PrivateModelViewItem;
  PrivateModelViewItem *const o_model_view_item;
};
}

#endif // MODELVIEWITEM_H
