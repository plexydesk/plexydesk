#ifndef MODELVIEW_H
#define MODELVIEW_H

#include <QGraphicsObject>

#include <widget.h>
#include <model_view_item.h>

#include <functional>

namespace UIKit {
class DECL_UI_KIT_EXPORT ItemView : public Widget {
  Q_OBJECT
public:
  typedef enum {
    kGridModel,
    kListModel,
    kTableModel
  } ModelType;

  ItemView(QGraphicsObject *a_parent_ptr = 0,
           ModelType a_model_type = kListModel);
  virtual ~ItemView();

  virtual void insert(Widget *a_widget_ptr);
  virtual void remove(Widget *a_widget_ptr);

  virtual void insert(ModelViewItem *a_item_ptr);
  virtual void remove(ModelViewItem *a_item_ptr);

  virtual ModelViewItem *at(int index);

  virtual void set_filter(const QString &a_keyword);

  virtual void clear();

  virtual void set_view_geometry(const QRectF &a_rect);
  virtual QRectF boundingRect() const;
  virtual void setGeometry(const QRectF &a_rect);
  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const;

  virtual void on_activated(std::function<void(int index)> a_callback);

protected:
  bool sceneEvent(QEvent *e);
  bool event(QEvent *e);

  void insert_to_list_view(Widget *a_widget_ptr);
  void remove_from_list_view(Widget *a_widget_ptr);

  void insert_to_grid_view(Widget *a_widget_ptr);
  void insert_to_table_view(Widget *a_widget_ptr);

private:
  class PrivateModelView;
  PrivateModelView *const d;
};
}
#endif // MODELVIEW_H
