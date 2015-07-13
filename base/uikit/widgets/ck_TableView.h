/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  : Siraj Razick <siraj@plexydesk.com>
*                 PhobosK <phobosk@kbfx.net>
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

#ifndef PLEXYDESKTABLEVIEW_H
#define PLEXYDESKTABLEVIEW_H

#include <QObject>

#include <ck_widget.h>
#include <ck_table_model.h>

#include <plexydesk_ui_exports.h>

namespace cherry_kit {
class table_model;

class DECL_UI_KIT_EXPORT TableView : public widget {
  Q_OBJECT
public:
  TableView(QGraphicsObject *a_parent_ptr = 0);
  virtual ~TableView();

  virtual void set_model(table_model *a_model_ptr);
  virtual table_model *model();

  virtual void clear_selection();

  virtual QRectF boundingRect() const;
  virtual void setGeometry(const QRectF &a_rect);
  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const;

  virtual table_view_item *itemAt(uint i);
  virtual uint count() const;

  virtual StylePtr style() const;

  virtual void set_item_activation_callback(
      std::function<void(table_view_item *a_item_ptr)> a_callback);
private
Q_SLOTS:
  virtual void on_item_click(table_view_item *a_component_ptr);
  virtual void on_add_viewItem(cherry_kit::table_view_item *a_item_ptr);
  virtual void on_clear();
Q_SIGNALS:
  void activated(table_view_item *a_component_ptr);

protected:
  virtual bool event(QEvent *a_event_ptr);
  virtual bool sceneEvent(QEvent *a_event_ptr);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void wheelEvent(QGraphicsSceneWheelEvent *a_event_ptr);
  virtual void scrollBy(int a_x, int a_y);
  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_exposeRect);

private:
  class PrivateTableView;
  PrivateTableView *const d;
};
}

#endif // PLEXYDESKTABLEVIEW_H
