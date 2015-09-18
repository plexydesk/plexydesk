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
#include "ck_clock_view.h"

#include <qmatrix.h>

#include <QPixmap>
#include <QPaintEvent>
#include <QBitmap>
#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDateTime>

#include <ck_config.h>
#include <ck_resource_manager.h>

#include <math.h>
#include <cmath>

namespace cherry_kit {
bool double_equals(double a, double b, double epsilon = 0.001) {
  return std::abs(a - b) < epsilon;
}
typedef std::function<void()> completion_callback_func;
typedef std::function<void(const clock_view *)> timeout_callback_func;

class clock_view::PrivateClockWidget {
public:
  PrivateClockWidget() : m_timezone(0) {}
  ~PrivateClockWidget() {}

  void on_timout_slot_func();
  void on_range_timout_slot_func(const clock_view *a_widget);
  void update();
  void notify_completion();
  void notify_timeout(const clock_view *a_widget);

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

clock_view::clock_view(widget *parent)
    : cherry_kit::widget(parent), priv(new PrivateClockWidget) {
  priv->m_clock_timer = new QTimer(this);
  priv->m_range_timer = new QTimer(this);
  priv->m_mark_hour_value = 0.0;
  priv->m_mark_minutes_value = 0.0;
  priv->m_mark_start = 0.1;
  priv->m_mark_end = 0.0;
  priv->m_range_timer_duration = 1000;
  priv->m_clock_timer->setTimerType(Qt::VeryCoarseTimer);
  priv->m_range_timer->setTimerType(Qt::VeryCoarseTimer);

  connect(priv->m_clock_timer, &QTimer::timeout, [this]() {
    priv->on_timout_slot_func();
    update();
  });

  connect(priv->m_range_timer, &QTimer::timeout, [this]() {
    priv->on_range_timout_slot_func(this);
    update();
  });

  priv->m_clock_timer->start(1000);
  priv->m_range_timer->stop();

  set_widget_flag(widget::kRenderDropShadow, false);
  setFlag(QGraphicsItem::ItemIsMovable, false);

  set_widget_name("Clock");
  priv->update();
}

void clock_view::set_timezone_id(const QByteArray &a_timezone_id) {
  priv->m_timezone_id = a_timezone_id;

  priv->m_clock_timer->stop();

  if (priv->m_timezone) {
    delete priv->m_timezone;
  }

  priv->m_timezone = new QTimeZone(a_timezone_id);

  priv->m_clock_timer->start();
  update();
}

void clock_view::add_marker(double a_hour, double a_min) {
  priv->m_mark_hour_value = a_hour;
  priv->m_mark_minutes_value = a_min;
  update();
}

void clock_view::add_range_marker(double a_start, double a_end) {
  priv->m_mark_start = a_start;
  int duration = a_end - a_start;
  priv->m_range_timer_initial_duration = duration;
  int hours = duration / 3600;
  int min = (duration / 60) - (hours * 60);
  int sec = duration - ((hours * 3600) + (min * 60));
  priv->m_mark_end = sec;
  priv->m_range_timer_duration = duration;

  QDateTime current_date_time = QDateTime::currentDateTime();

  if (priv->m_timezone)
    current_date_time =
        current_date_time.toTimeZone(*priv->m_timezone);

  QTime current_time = current_date_time.time();

  current_time = current_time.addSecs(duration);
  priv->m_mark_minutes_value = current_time.minute();
  priv->m_mark_hour_value = current_time.hour();
  priv->m_completion_time_label =
      current_time.toString("hh:mm:ss ap");
}

int clock_view::duration() const {
  return priv->m_range_timer_initial_duration;
}

int clock_view::elapsed_time_in_seconds() const {
  return priv->m_range_timer_duration;
}

QString clock_view::completion_time_as_string() const {
  return priv->m_completion_time_label;
}

void clock_view::run_timer(Direction a_direction) {
  priv->m_range_timer->start(1000);
}

void clock_view::on_timer_ended(std::function<void()> a_callback) {
  priv->m_completed_callback_list.push_back(a_callback);
}

void
clock_view::on_timeout(std::function<void(const clock_view *)> a_callback) {
  priv->m_timeout_callback_list.push_back(a_callback);
}

clock_view::~clock_view() {
  if (priv->m_clock_timer) {
    priv->m_clock_timer->stop();
  }

  if (priv->m_timezone)
    delete priv->m_timezone;

  delete priv;
}

void clock_view::PrivateClockWidget::on_timout_slot_func() { update(); }

void clock_view::PrivateClockWidget::on_range_timout_slot_func(
    const clock_view *a_widget) {
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

void clock_view::PrivateClockWidget::update() {
  QDateTime _date_time = QDateTime::currentDateTime();

  if (m_timezone)
    _date_time = _date_time.toTimeZone(*m_timezone);

  m_second_value = 6.0 * _date_time.time().second();
  m_minutes_value = 6.0 * _date_time.time().minute();
  m_hour_value =
      (60 * _date_time.time().hour() + _date_time.time().minute()) / 2;
}

void clock_view::PrivateClockWidget::notify_completion() {
  std::for_each(std::begin(m_completed_callback_list),
                std::end(m_completed_callback_list),
                [=](completion_callback_func a_func) {
    if (a_func)
      a_func();
  });
}

void
clock_view::PrivateClockWidget::notify_timeout(const clock_view *a_widget) {
  std::for_each(std::begin(m_timeout_callback_list),
                std::end(m_timeout_callback_list),
                [=](timeout_callback_func a_func) {
    if (a_func)
      a_func(a_widget);
  });
}

void clock_view::paint_view(QPainter *p, const QRectF &r) {
  // done intentionally to make the clock look squre.
  QRectF rect(r.x() + 16, r.y(), r.width() - 32, r.width() - 32);

  style_data feature;

  feature.text_data = QString("%1:%2:%3")
                          .arg(priv->m_hour_value)
                          .arg(priv->m_minutes_value)
                          .arg(priv->m_second_value);

  feature.geometry = rect;
  feature.render_state = style_data::kRenderElement;
  feature.attributes["hour"] = priv->m_hour_value;
  feature.attributes["minutes"] = priv->m_minutes_value;
  feature.attributes["seconds"] = priv->m_second_value;

  feature.attributes["mark_hour"] = priv->m_mark_hour_value;
  feature.attributes["mark_minutes"] = priv->m_mark_minutes_value;

  feature.attributes["mark_start"] = priv->m_mark_start;
  feature.attributes["mark_end"] = priv->m_mark_end;
  feature.text_data = priv->m_completion_time_label;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("clock", feature, p);
  }
}
}
