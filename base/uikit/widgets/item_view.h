/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#ifndef MODELVIEW_H
#define MODELVIEW_H

#include <QGraphicsObject>

#include <widget.h>
#include <model_view_item.h>

#include <functional>

namespace CherryKit {
class DECL_UI_KIT_EXPORT ItemView : public Widget {
  Q_OBJECT
public:
  typedef enum {
    kGridModel,
    kListModel,
    kTableModel
  } ModelType;

  ItemView(Widget *a_parent_ptr = 0, ModelType a_model_type = kListModel);
  virtual ~ItemView();

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
  virtual void
  on_item_removed(std::function<void(ModelViewItem *item)> a_handler);

protected:
  virtual void insert(Widget *a_widget_ptr);
  virtual void remove(Widget *a_widget_ptr);

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
