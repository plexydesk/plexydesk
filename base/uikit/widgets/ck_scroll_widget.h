/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2012  <copyright holder> <email>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PLEXYDESK_SCROLL_WIDGET_H
#define PLEXYDESK_SCROLL_WIDGET_H

#include <config.h>
#include <ck_widget.h>

#include <QGraphicsItem>
#include <QGraphicsObject>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {

class DECL_UI_KIT_EXPORT scroll_widget : public widget {
  Q_OBJECT
public:
  scroll_widget(const QRectF &a_rect, widget *a_parent_ptr = 0);
  virtual ~scroll_widget();

  void set_viewport(QGraphicsObject *a_widget_ptr);
  void scroll_by(int x, int y);

  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_rect);

private:
  virtual void wheelEvent(QGraphicsSceneWheelEvent *a_event_ptr);
  virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *a_event_ptr);
  virtual bool event(QEvent *a_event_ptr);
  virtual bool sceneEvent(QEvent *a_e_ptr);

  class Private;
  Private *const o_scroll_widget;
};
}

#endif
