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

  virtual void setModel(TableModel *model);

  virtual TableModel *model();

  virtual void clearSelection();

  virtual QRectF boundingRect() const;

  virtual void setGeometry(const QRectF &rect);

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

  virtual TableViewItem *itemAt(uint i);

  virtual StylePtr style() const;

  virtual uint count() const;

  virtual void setItemActivationCallback(std::function<void (TableViewItem *item)> aCallback);

private Q_SLOTS:
  virtual void onItemClick(TableViewItem *component);
  virtual void onAddViewItem(UIKit::TableViewItem *item);
  virtual void onClear();

Q_SIGNALS:
  void activated(TableViewItem *component);

protected:
  virtual bool event(QEvent *event);
  virtual bool sceneEvent(QEvent *event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void wheelEvent(QGraphicsSceneWheelEvent *event);
  virtual void scrollBy(int x, int y);
  virtual void paintView(QPainter *painter, const QRectF &exposeRect);

private:
  class PrivateTableView;
  PrivateTableView *const d;
};
}

#endif // PLEXYDESKTABLEVIEW_H
