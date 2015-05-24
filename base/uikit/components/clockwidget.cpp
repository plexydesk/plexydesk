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
#include <plexy.h>
#include "clockwidget.h"
#include <qmatrix.h>
// Added by qt3to4:
#include <QPixmap>
#include <QPaintEvent>
#include <math.h>
#include <QBitmap>
#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDateTime>
#include <QDir>
#include <plexyconfig.h>
#include <themepackloader.h>

#include <cmath>

namespace UIKit {
bool double_equals(double a, double b, double epsilon = 0.001) {
      return std::abs(a - b) < epsilon;
}

ClockWidget::ClockWidget(QGraphicsObject *parent)
    : UIKit::Widget(parent), m_timezone(0) {
  m_clock_timer = new QTimer(this);
  m_range_timer = new QTimer(this);

  m_mark_hour_value = 0.0;
  m_mark_minutes_value = 0.0;
  m_mark_start = 0.1;
  m_mark_end = 0.0;
  m_range_timer_duration = 1000;
  m_clock_timer->setTimerType(Qt::VeryCoarseTimer);
  m_range_timer->setTimerType(Qt::VeryCoarseTimer);

  connect(m_clock_timer, SIGNAL(timeout()), this, SLOT(on_timout_slot_func()));
  connect(m_range_timer, SIGNAL(timeout()), this,
          SLOT(on_range_timout_slot_func()));

  m_clock_timer->start(1000);
  m_range_timer->stop();
  set_widget_flag(Widget::kRenderDropShadow, false);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  set_widget_name("Clock");
  on_timout_slot_func();
}

void ClockWidget::update_time(const QVariantMap &a_data) {
  QDateTime _date_time = QDateTime::currentDateTime();

  if (m_timezone)
    _date_time = _date_time.toTimeZone(*m_timezone);

  m_second_value = 6.0 * _date_time.time().second();
  m_minutes_value = 6.0 * _date_time.time().minute();
  m_hour_value =
          (60 * _date_time.time().hour() + _date_time.time().minute()) / 2;

  update();
}

void ClockWidget::set_timezone_id(const QByteArray &a_timezone_id) {
  m_timezone_id = a_timezone_id;

  m_clock_timer->stop();

  if (m_timezone) {
    delete m_timezone;
  }

  m_timezone = new QTimeZone(a_timezone_id);

  m_clock_timer->start();
}

void ClockWidget::add_marker(double a_hour, double a_min)
{
  m_mark_hour_value = a_hour;
  m_mark_minutes_value = a_min;
  update();
}

void ClockWidget::add_range_marker(double a_start, double a_end) {
    m_mark_start = a_start;
    int duration = a_end - a_start;
    int hours = duration / 3600;
    int min = (duration / 60) - (hours * 60);
    int sec = duration  - ((hours * 3600) + (min * 60));
    m_mark_end = sec;
    m_range_timer_duration = duration;

    QDateTime current_date_time = QDateTime::currentDateTime();

    if (m_timezone)
    current_date_time = current_date_time.toTimeZone(*m_timezone);

    QTime current_time = current_date_time.time();

    current_time = current_time.addSecs(duration);
    m_mark_minutes_value = current_time.minute();
    m_mark_hour_value = current_time.hour();

    qDebug() << Q_FUNC_INFO << m_range_timer_duration;
    qDebug() << Q_FUNC_INFO << "Hour" << hours;
    qDebug() << Q_FUNC_INFO << "Minute" << min;
    qDebug() << Q_FUNC_INFO << "Seconds" << sec;
    qDebug() << Q_FUNC_INFO << "Goes Of at Hour" << m_mark_hour_value;
    qDebug() << Q_FUNC_INFO << "Goes Of at Min" << m_mark_minutes_value;
}

void ClockWidget::run_timer(Direction a_direction) {
    m_range_timer->start(1000);
}

ClockWidget::~ClockWidget() {
  if (m_clock_timer) {
    m_clock_timer->stop();
    delete m_clock_timer;
  }

  if (m_timezone)
    delete m_timezone;
}

void ClockWidget::on_timout_slot_func() {
  update_time(QVariantMap());
  update();
}

void ClockWidget::on_range_timout_slot_func() {
   m_range_timer_duration -= 1.0;
   int duration = m_range_timer_duration;
   int hours = duration / 3600;
   int min = (duration / 60) - (hours * 60);
   int sec = duration  - ((hours * 3600) + (min * 60));
   m_mark_end = sec;

   if (m_range_timer_duration <= 0)
       m_range_timer->stop();

   update();
}

void ClockWidget::paint_view(QPainter *p, const QRectF &r) {
  QRectF rect(r.x() + 16, r.y() + 16, r.width() - 32, r.height() - 32);

  StyleFeatures feature;

  feature.text_data = QString("%1:%2:%3").arg(m_hour_value).
      arg(m_minutes_value).arg(m_second_value);

  feature.geometry = rect;
  feature.render_state = StyleFeatures::kRenderElement;
  feature.attributes["hour"] = m_hour_value;
  feature.attributes["minutes"] = m_minutes_value;
  feature.attributes["seconds"] = m_second_value;

  feature.attributes["mark_hour"] = m_mark_hour_value;
  feature.attributes["mark_minutes"] = m_mark_minutes_value;

  feature.attributes["mark_start"] = m_mark_start;
  feature.attributes["mark_end"] = m_mark_end;

  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("clock", feature, p);
  }
}

}
