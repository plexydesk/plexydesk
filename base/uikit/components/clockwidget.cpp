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

namespace CherryKit {
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

ClockWidget::ClockWidget(Widget *parent)
    : CherryKit::Widget(parent), o_clock_widget(new PrivateClockWidget) {
  o_clock_widget->m_clock_timer = new QTimer(this);
  o_clock_widget->m_range_timer = new QTimer(this);
  o_clock_widget->m_mark_hour_value = 0.0;
  o_clock_widget->m_mark_minutes_value = 0.0;
  o_clock_widget->m_mark_start = 0.1;
  o_clock_widget->m_mark_end = 0.0;
  o_clock_widget->m_range_timer_duration = 1000;
  o_clock_widget->m_clock_timer->setTimerType(Qt::VeryCoarseTimer);
  o_clock_widget->m_range_timer->setTimerType(Qt::VeryCoarseTimer);

  connect(o_clock_widget->m_clock_timer, &QTimer::timeout, [this]() {
    o_clock_widget->on_timout_slot_func();
    update();
  });

  connect(o_clock_widget->m_range_timer, &QTimer::timeout, [this]() {
    o_clock_widget->on_range_timout_slot_func(this);
    update();
  });

  o_clock_widget->m_clock_timer->start(1000);
  o_clock_widget->m_range_timer->stop();

  set_widget_flag(Widget::kRenderDropShadow, false);
  setFlag(QGraphicsItem::ItemIsMovable, false);

  set_widget_name("Clock");
}

void ClockWidget::set_timezone_id(const QByteArray &a_timezone_id) {
  o_clock_widget->m_timezone_id = a_timezone_id;

  o_clock_widget->m_clock_timer->stop();

  if (o_clock_widget->m_timezone) {
    delete o_clock_widget->m_timezone;
  }

  o_clock_widget->m_timezone = new QTimeZone(a_timezone_id);

  o_clock_widget->m_clock_timer->start();
}

void ClockWidget::add_marker(double a_hour, double a_min) {
  o_clock_widget->m_mark_hour_value = a_hour;
  o_clock_widget->m_mark_minutes_value = a_min;
  update();
}

void ClockWidget::add_range_marker(double a_start, double a_end) {
  o_clock_widget->m_mark_start = a_start;
  int duration = a_end - a_start;
  o_clock_widget->m_range_timer_initial_duration = duration;
  int hours = duration / 3600;
  int min = (duration / 60) - (hours * 60);
  int sec = duration - ((hours * 3600) + (min * 60));
  o_clock_widget->m_mark_end = sec;
  o_clock_widget->m_range_timer_duration = duration;

  QDateTime current_date_time = QDateTime::currentDateTime();

  if (o_clock_widget->m_timezone)
    current_date_time = current_date_time.toTimeZone(*o_clock_widget->m_timezone);

  QTime current_time = current_date_time.time();

  current_time = current_time.addSecs(duration);
  o_clock_widget->m_mark_minutes_value = current_time.minute();
  o_clock_widget->m_mark_hour_value = current_time.hour();
  o_clock_widget->m_completion_time_label = current_time.toString("hh:mm:ss ap");
}

int ClockWidget::duration() const { return o_clock_widget->m_range_timer_initial_duration; }

int ClockWidget::elapsed_time_in_seconds() const {
  return o_clock_widget->m_range_timer_duration;
}

QString ClockWidget::completion_time_as_string() const {
  return o_clock_widget->m_completion_time_label;
}

void ClockWidget::run_timer(Direction a_direction) {
  o_clock_widget->m_range_timer->start(1000);
}

void ClockWidget::on_timer_ended(std::function<void()> a_callback) {
  o_clock_widget->m_completed_callback_list.push_back(a_callback);
}

void
ClockWidget::on_timeout(std::function<void(const ClockWidget *)> a_callback) {
  o_clock_widget->m_timeout_callback_list.push_back(a_callback);
}

ClockWidget::~ClockWidget() {
  if (o_clock_widget->m_clock_timer) {
    o_clock_widget->m_clock_timer->stop();
  }

  if (o_clock_widget->m_timezone)
    delete o_clock_widget->m_timezone;

  delete o_clock_widget;
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

void
ClockWidget::PrivateClockWidget::notify_timeout(const ClockWidget *a_widget) {
  std::for_each(std::begin(m_timeout_callback_list),
                std::end(m_timeout_callback_list),
                [=](timeout_callback_func a_func) {
    if (a_func)
      a_func(a_widget);
  });
}

void ClockWidget::paint_view(QPainter *p, const QRectF &r) {
  // done intentionally to make the clock look squre.
  QRectF rect(r.x() + 16, r.y(), r.width() - 32, r.width() - 32);

  StyleFeatures feature;

  feature.text_data =
      QString("%1:%2:%3").arg(o_clock_widget->m_hour_value).arg(o_clock_widget->m_minutes_value).arg(
          o_clock_widget->m_second_value);

  feature.geometry = rect;
  feature.render_state = StyleFeatures::kRenderElement;
  feature.attributes["hour"] = o_clock_widget->m_hour_value;
  feature.attributes["minutes"] = o_clock_widget->m_minutes_value;
  feature.attributes["seconds"] = o_clock_widget->m_second_value;

  feature.attributes["mark_hour"] = o_clock_widget->m_mark_hour_value;
  feature.attributes["mark_minutes"] = o_clock_widget->m_mark_minutes_value;

  feature.attributes["mark_start"] = o_clock_widget->m_mark_start;
  feature.attributes["mark_end"] = o_clock_widget->m_mark_end;
  feature.text_data = o_clock_widget->m_completion_time_label;

  if (CherryKit::ResourceManager::style()) {
    CherryKit::ResourceManager::style()->draw("clock", feature, p);
  }
}
}
