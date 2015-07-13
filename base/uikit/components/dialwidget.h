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
#ifndef DIAL_WIDGET_H
#define DIAL_WIDGET_H

#include <widget.h>
#include <QGraphicsSceneMouseEvent>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {
class DECL_UI_KIT_EXPORT DialWidget : public cherry_kit::widget {
  Q_OBJECT
public:
  DialWidget(widget *a_parent_ptr = 0);

  virtual ~DialWidget();

  virtual void set_maximum_dial_value(float maximum_dial_value);
  virtual float maximum_dial_value() const;

  virtual float current_dial_value() const;
  virtual void reset();

  virtual void on_dialed(std::function<void(int)> a_callback);
Q_SIGNALS:
  void value(float value);

protected:
  virtual void paint_view(QPainter *painter, const QRectF &rect);
  virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
  class PrivateDialWidget;
  PrivateDialWidget *const o_dial_widget;
};
}

#endif // DIAL_WIDGET_H
