/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  : Siraj Razick <siraj@plexydesk.org>
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

#include <plexy.h>

#include <QObject>

#include <scrollwidget.h>
#include <tableviewcellinterface.h>
#include <plexydesk_ui_exports.h>

namespace UIKit
{

class TableModel;

class DECL_UI_KIT_EXPORT TableView : public Widget
{
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)

public:
  TableView(QGraphicsObject *a_parent_ptr = 0);

  virtual ~TableView();

  virtual void set_model(TableModel *a_model_ptr);

  virtual TableModel *model();

  virtual void clear_selection();

  virtual QRectF boundingRect() const;

  virtual void setGeometry(const QRectF &a_rect);

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const;

  virtual TableViewItem *itemAt(uint i);

  virtual StylePtr style() const;

  virtual uint count() const;

  virtual void set_item_activation_callback(std::function<void (TableViewItem *a_item_ptr)> a_callback);

private Q_SLOTS:
  virtual void on_item_click(TableViewItem *a_component_ptr);
  virtual void on_add_viewItem(UIKit::TableViewItem *a_item_ptr);
  virtual void on_clear();

Q_SIGNALS:
  void activated(TableViewItem *a_component_ptr);

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
