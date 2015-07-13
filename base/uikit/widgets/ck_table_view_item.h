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
#ifndef ABSTRACTCELLCOMPONENT_H
#define ABSTRACTCELLCOMPONENT_H

#include <QGraphicsObject>
#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {

class DECL_UI_KIT_EXPORT table_view_item : public QGraphicsObject,
                                           public QGraphicsLayoutItem {
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)

public:
  table_view_item(const QRectF &a_rect, QGraphicsItem *a_parent_ptr = 0);

  virtual ~table_view_item();

  virtual void setGeometry(const QRectF &a_rect);

  virtual void set_selected() = 0;

  virtual void clear_selection() = 0;

  virtual QString name();

Q_SIGNALS:
  void clicked(table_view_item *a_component_ptr);
};
}
#endif // ABSTRACTCELLCOMPONENT_H
