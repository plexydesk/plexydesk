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
  Q_OBJECT
public:
  typedef enum {
       kRunForwards,
       kRunBackwards
  } Direction;

  ClockWidget(QGraphicsObject *a_parent_ptr = 0);
  virtual ~ClockWidget();

  void update_time(const QVariantMap &a_data);

  void set_timezone_id(const QByteArray &a_timezone_id);
  QByteArray timezone_id() const;

  virtual void add_marker(double a_hour, double a_min);
  virtual void add_range_marker(double a_start, double a_end);
  virtual void run_timer(Direction a_direction = kRunForwards);

public Q_SLOTS:
  void on_timout_slot_func();
  void on_range_timout_slot_func();

protected:
  void paint_view(QPainter *painter, const QRectF &r);

private:
  double m_second_value;
  double m_minutes_value;
  double m_hour_value;

  double m_mark_hour_value;
  double m_mark_minutes_value;

  double m_mark_start;
  double m_mark_end;

  QTimer *m_clock_timer;
  QTimer *m_range_timer;
  int m_range_timer_duration;

  QByteArray m_timezone_id;
  QTimeZone *m_timezone;
};
}
#endif
