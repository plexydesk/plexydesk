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
#include "clockwidget.h"

#include <qmatrix.h>

#include <QPixmap>
#include <QPaintEvent>
#include <QBitmap>
#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDateTime>

#include <plexy.h>
#include <plexyconfig.h>
#include <resource_manager.h>

#include <math.h>
#include <cmath>

namespace UIKit {
bool double_equals(double a, double b, double epsilon = 0.001) {
  return std::abs(a - b) < epsilon;
}
typedef std::function<void()> completion_callback_func;
typedef std::function<void(const ClockWidget *)> timeout_callback_func;

class ClockWidget::PrivateClockWidget {
public:
  PrivateClockWidget() : m_timezone(0) {}
  ~PrivateClockWidget() {}

  void on_timout_slot_func();
  void on_range_timout_slot_func(const ClockWidget *a_widget);
  void update();
  void notify_completion();
  void notify_timeout(const ClockWidget *a_widget);

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
  int m_range_timer_initial_duration;
  QString m_completion_time_label;

  QByteArray m_timezone_id;
  QTimeZone *m_timezone;

  std::vector<completion_callback_func> m_completed_callback_list;
  std::vector<timeout_callback_func> m_timeout_callback_list;
};

ClockWidget::ClockWidget(QGraphicsObject *parent)
    : UIKit::Widget(parent), d(new PrivateClockWidget) {
  d->m_clock_timer = new QTimer(this);
  d->m_range_timer = new QTimer(this);
  d->m_mark_hour_value = 0.0;
  d->m_mark_minutes_value = 0.0;
  d->m_mark_start = 0.1;
  d->m_mark_end = 0.0;
  d->m_range_timer_duration = 1000;
  d->m_clock_timer->setTimerType(Qt::VeryCoarseTimer);
  d->m_range_timer->setTimerType(Qt::VeryCoarseTimer);

  connect(d->m_clock_timer, &QTimer::timeout, [this]() {
    d->on_timout_slot_func();
    update();
  });

  connect(d->m_range_timer, &QTimer::timeout, [this]() {
    d->on_range_timout_slot_func(this);
    update();
  });

  d->m_clock_timer->start(1000);
  d->m_range_timer->stop();

  set_widget_flag(Widget::kRenderDropShadow, false);
  setFlag(QGraphicsItem::ItemIsMovable, false);

  set_widget_name("Clock");
}

void ClockWidget::set_timezone_id(const QByteArray &a_timezone_id) {
  d->m_timezone_id = a_timezone_id;

  d->m_clock_timer->stop();

  if (d->m_timezone) {
    delete d->m_timezone;
  }

  d->m_timezone = new QTimeZone(a_timezone_id);

  d->m_clock_timer->start();
}

void ClockWidget::add_marker(double a_hour, double a_min) {
  d->m_mark_hour_value = a_hour;
  d->m_mark_minutes_value = a_min;
  update();
}

void ClockWidget::add_range_marker(double a_start, double a_end) {
  d->m_mark_start = a_start;
  int duration = a_end - a_start;
  d->m_range_timer_initial_duration = duration;
  int hours = duration / 3600;
  int min = (duration / 60) - (hours * 60);
  int sec = duration - ((hours * 3600) + (min * 60));
  d->m_mark_end = sec;
  d->m_range_timer_duration = duration;

  QDateTime current_date_time = QDateTime::currentDateTime();

  if (d->m_timezone)
    current_date_time = current_date_time.toTimeZone(*d->m_timezone);

  QTime current_time = current_date_time.time();

  current_time = current_time.addSecs(duration);
  d->m_mark_minutes_value = current_time.minute();
  d->m_mark_hour_value = current_time.hour();
  d->m_completion_time_label = current_time.toString("hh:mm:ss ap");
}

int ClockWidget::duration() const { return d->m_range_timer_initial_duration; }

int ClockWidget::elapsed_time_in_seconds() const {
    return d->m_range_timer_duration;
}

QString ClockWidget::completion_time_as_string() const {
   return d->m_completion_time_label;
}

void ClockWidget::run_timer(Direction a_direction) {
  d->m_range_timer->start(1000);
}

void ClockWidget::on_timer_ended(std::function<void()> a_callback) {
  d->m_completed_callback_list.push_back(a_callback);
}

void
ClockWidget::on_timeout(std::function<void(const ClockWidget *)> a_callback) {
  d->m_timeout_callback_list.push_back(a_callback);
}

ClockWidget::~ClockWidget() {
  if (d->m_clock_timer) {
    d->m_clock_timer->stop();
  }

  if (d->m_timezone)
    delete d->m_timezone;

  delete d;
}

void ClockWidget::PrivateClockWidget::on_timout_slot_func() { update(); }

void ClockWidget::PrivateClockWidget::on_range_timout_slot_func(
    const ClockWidget *a_widget) {
  m_range_timer_duration -= 1.0;
  int duration = m_range_timer_duration;
  int hours = duration / 3600;
  int min = (duration / 60) - (hours * 60);
  int sec = duration - ((hours * 3600) + (min * 60));
  m_mark_end = sec;
  notify_timeout(a_widget);

  if (m_range_timer_duration <= 0) {
    m_range_timer->stop();
    notify_completion();
  }
}

void ClockWidget::PrivateClockWidget::update() {
  QDateTime _date_time = QDateTime::currentDateTime();

  if (m_timezone)
    _date_time = _date_time.toTimeZone(*m_timezone);

  m_second_value = 6.0 * _date_time.time().second();
  m_minutes_value = 6.0 * _date_time.time().minute();
  m_hour_value =
      (60 * _date_time.time().hour() + _date_time.time().minute()) / 2;
}

void ClockWidget::PrivateClockWidget::notify_completion() {
  std::for_each(std::begin(m_completed_callback_list),
                std::end(m_completed_callback_list),
                [=](completion_callback_func a_func) {
    if (a_func)
      a_func();
  });
}

void ClockWidget::PrivateClockWidget::notify_timeout(const ClockWidget *a_widget) {
  std::for_each(std::begin(m_timeout_callback_list),
                std::end(m_timeout_callback_list),
                [=](timeout_callback_func a_func) {
    if (a_func)
      a_func(a_widget);
  });
}

void ClockWidget::paint_view(QPainter *p, const QRectF &r) {
  QRectF rect(r.x() + 16, r.y() + 16, r.width() - 32, r.height() - 32);

  StyleFeatures feature;

  feature.text_data =
      QString("%1:%2:%3").arg(d->m_hour_value).arg(d->m_minutes_value).arg(
          d->m_second_value);

  feature.geometry = rect;
  feature.render_state = StyleFeatures::kRenderElement;
  feature.attributes["hour"] = d->m_hour_value;
  feature.attributes["minutes"] = d->m_minutes_value;
  feature.attributes["seconds"] = d->m_second_value;

  feature.attributes["mark_hour"] = d->m_mark_hour_value;
  feature.attributes["mark_minutes"] = d->m_mark_minutes_value;

  feature.attributes["mark_start"] = d->m_mark_start;
  feature.attributes["mark_end"] = d->m_mark_end;
  feature.text_data = d->m_completion_time_label;

  if (UIKit::ResourceManager::style()) {
    UIKit::ResourceManager::style()->draw("clock", feature, p);
  }
}
}
