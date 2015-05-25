/***************************************************************************
*   Copyright (C) 2005,2006,2007 by Siraj Razick                          *
*   siraj@kdemail.net                                                     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
***************************************************************************/

#ifndef CLOCK_WIDGET_H
#define CLOCK_WIDGET_H

#include <widget.h>

#include <QByteArray>
#include <QTimeZone>
#include <QTimer>

namespace UIKit {
class ClockWidget : public Widget {
public:
  typedef enum {
       kRunForwards,
       kRunBackwards
  } Direction;

  ClockWidget(QGraphicsObject *a_parent_ptr = 0);
  virtual ~ClockWidget();

  void set_timezone_id(const QByteArray &a_timezone_id);
  QByteArray timezone_id() const;

  virtual void add_marker(double a_hour, double a_min);
  virtual void add_range_marker(double a_start, double a_end);
  virtual void run_timer(Direction a_direction = kRunForwards);
protected:
  void paint_view(QPainter *painter, const QRectF &r);
private:
  class PrivateClockWidget;
  PrivateClockWidget *const d;
 };
}
#endif
