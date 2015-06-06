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
// internal
#include "clock.h"
#include "clockwidget.h"

#include <QDebug>

// uikit
#include <view_controller.h>
#include <extensionmanager.h>
#include <session_sync.h>
#include <rangewidget.h>
#include <dialwidget.h>
#include <toolbar.h>
#include <space.h>

// datakit
#include <clockwidget.h>
#include <datasync.h>
#include <disksyncengine.h>

// Qt
#include <QAction>
#include <label.h>

class Clock::PrivateClockController {
public:
  PrivateClockController() {}
  ~PrivateClockController() {}

  void _create_clock_ui(Clock *a_controller, UIKit::SessionSync *a_session);
  void _create_timer_ui(Clock *a_controller, UIKit::SessionSync *a_session);

  UIKit::ActionList m_supported_action_list;
  void _create_countdown_ui(Clock *a_controller, UIKit::SessionSync *a_session);
};

Clock::Clock(QObject *parent)
    : UIKit::ViewController(parent), d(new PrivateClockController) {}

Clock::~Clock() {
  qDebug() << Q_FUNC_INFO << "Deleted";
  delete d;
}

QAction *Clock::createAction(int id, const QString &action_name,
                             const QString &icon_name) {
  QAction *_add_clock_action = new QAction(this);
  _add_clock_action->setText(action_name);
  _add_clock_action->setProperty("id", QVariant(id));
  _add_clock_action->setProperty("icon_name", icon_name);
  _add_clock_action->setProperty("hidden", 0);

  return _add_clock_action;
}

void Clock::init() {
  d->m_supported_action_list << createAction(1, tr("Clock"),
                                             "pd_clock_frame_icon.png");
  d->m_supported_action_list << createAction(2, tr("Timer"),
                                             "pd_clock_frame_icon.png");
  d->m_supported_action_list << createAction(3, tr("Alarm"),
                                             "pd_clock_frame_icon.png");
  d->m_supported_action_list << createAction(3, tr("Countdown"),
                                             "pd_clock_frame_icon.png");
}

void Clock::set_view_rect(const QRectF &rect) {}

void
Clock::session_data_available(const QuetzalKit::SyncObject &a_session_root) {
  revoke_previous_session("Clock", [this](UIKit::ViewController *a_controller,
                                          UIKit::SessionSync *a_session) {
    d->_create_clock_ui((Clock *)a_controller, a_session);
  });

  revoke_previous_session("Timer", [this](UIKit::ViewController *a_controller,
                                          UIKit::SessionSync *a_session) {
    d->_create_timer_ui((Clock *)a_controller, a_session);
  });

  revoke_previous_session("Countdown",
                          [this](UIKit::ViewController *a_controller,
                                 UIKit::SessionSync *a_session) {
    d->_create_countdown_ui((Clock *)a_controller, a_session);
  });
}

void Clock::submit_session_data(QuetzalKit::SyncObject *a_obj) {
  write_session_data("Clock");
  write_session_data("Timer");
  write_session_data("Countdown");
}

bool Clock::remove_widget(UIKit::Widget *widget) {
  disconnect(dataSource(), SIGNAL(sourceUpdated(QVariantMap)));
  int index = 0;

  return 1;
}

UIKit::ActionList Clock::actions() const { return d->m_supported_action_list; }

void Clock::request_action(const QString &actionName, const QVariantMap &args) {

  QPointF window_location;

  if (viewport()) {
    window_location = viewport()->center(QRectF(0, 0, 240, 240 + 48));
  }

  if (actionName == tr("Clock")) {
    QVariantMap session_args;

    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["clock_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_database_name("clock"));

    start_session("Clock", session_args, false,
                  [this](UIKit::ViewController *a_controller,
                         UIKit::SessionSync *a_session) {
      d->_create_clock_ui((Clock *)a_controller, a_session);
    });
    return;
  }

  if (actionName == tr("Timer")) {
    QVariantMap session_args;

    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["timer_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_database_name("timer"));

    start_session("Timer", session_args, false,
                  [this](UIKit::ViewController *a_controller,
                         UIKit::SessionSync *a_session) {
      qDebug() << Q_FUNC_INFO << "Create Timer";
      d->_create_timer_ui((Clock *)a_controller, a_session);
    });
    return;
  }

  if (actionName == tr("Countdown")) {
    QVariantMap session_args;

    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["countdown_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_database_name("countdown"));

    start_session("Countdown", session_args, false,
                  [this](UIKit::ViewController *a_controller,
                         UIKit::SessionSync *a_session) {
      d->_create_countdown_ui((Clock *)a_controller, a_session);
    });
    return;
  }
}

QString Clock::icon() const { return QString("pd_clock_frame_icon.png"); }

void Clock::onDataUpdated(const QVariantMap &data) {}

void
Clock::PrivateClockController::_create_countdown_ui(Clock *a_controller,
                                                UIKit::SessionSync *a_session) {
  float window_width = 200;

  UIKit::Window *m_clock_session_window = new UIKit::Window();
  UIKit::Widget *m_content_view = new UIKit::Widget(m_clock_session_window);

  UIKit::Label *m_countdown_widget =
          new UIKit::Label(m_content_view);
  UIKit::ToolBar *m_toolbar = new UIKit::ToolBar(m_content_view);
  UIKit::Label *m_timezone_label = new UIKit::Label(m_toolbar);
  m_timezone_label->set_size(QSizeF(window_width - 72, 32));

  m_content_view->setGeometry(QRectF(0, 0, window_width, window_width));
  m_countdown_widget->set_size(QSize(window_width, window_width - 100));
  m_countdown_widget->set_label("+");
  m_countdown_widget->set_font_size(30);
  m_countdown_widget->show();
  m_clock_session_window->set_window_title("Countdown");

  // toolbar placement.
  m_toolbar->set_icon_resolution("hdpi");
  m_toolbar->set_icon_size(QSize(24, 24));

  m_toolbar->add_action("Date", "actions/pd_location", false);
  m_toolbar->insert_widget(m_timezone_label);

  m_toolbar->setGeometry(m_toolbar->frame_geometry());
  m_toolbar->show();
  m_toolbar->setPos(0, window_width - 32);

  m_clock_session_window->set_window_content(m_content_view);

  a_session->bind_to_window(m_clock_session_window);

  m_toolbar->on_item_activated([=](const QString &a_action) {
      //todo : invoke calendar activity to get the date ?
  });

  if (a_controller && a_controller->viewport()) {
    a_controller->insert(m_clock_session_window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    m_clock_session_window->setPos(window_location);
  }
}


void
Clock::PrivateClockController::_create_clock_ui(Clock *a_controller,
                                                UIKit::SessionSync *a_session) {
  float window_width = 248;

  UIKit::Window *m_clock_session_window = new UIKit::Window();
  UIKit::Widget *m_content_view = new UIKit::Widget(m_clock_session_window);
  UIKit::ClockWidget *m_clock_widget = new UIKit::ClockWidget(m_content_view);
  UIKit::ToolBar *m_toolbar = new UIKit::ToolBar(m_content_view);
  UIKit::Label *m_timezone_label = new UIKit::Label(m_toolbar);
  m_timezone_label->set_size(QSizeF(window_width - 72, 32));

  if (a_session->session_keys().contains("zone_id")) {
    m_clock_widget->set_timezone_id(
        a_session->session_data("zone_id").toByteArray());
    m_timezone_label->set_label(a_session->session_data("zone_id").toString());
  }

  m_content_view->setGeometry(QRectF(0, 0, window_width, window_width + 48));
  m_clock_widget->setGeometry(QRectF(0, 0, window_width, window_width));
  m_clock_session_window->set_window_title("Clock");

  // toolbar placement.
  m_toolbar->set_icon_resolution("hdpi");
  m_toolbar->set_icon_size(QSize(24, 24));

  m_toolbar->add_action("TimeZone", "actions/pd_location", false);
  m_toolbar->insert_widget(m_timezone_label);

  m_toolbar->setGeometry(m_toolbar->frame_geometry());
  m_toolbar->show();
  m_toolbar->setPos(0, window_width);

  m_clock_session_window->set_window_content(m_content_view);

  a_session->bind_to_window(m_clock_session_window);

  m_toolbar->on_item_activated([=](const QString &a_action) {
    if (a_action == "TimeZone") {
      if (a_controller && a_controller->viewport()) {
        UIKit::Space *viewport = a_controller->viewport();

        QPointF _activity_window_location = viewport->center(
            QRectF(0, 0, 240, 320),
            QRectF(m_clock_session_window->x(), m_clock_session_window->y(),
                   m_clock_session_window->geometry().width(),
                   m_clock_session_window->geometry().height()),
            UIKit::Space::kCenterOnWindow);
        UIKit::DesktopActivityPtr activity = viewport->create_activity(
            "timezone", "TimeZone", _activity_window_location,
            QRectF(0, 0, 240, 320.0), QVariantMap());

        activity->on_action_completed([=](const QVariantMap &a_data) {
          m_clock_widget->set_timezone_id(a_data["zone_id"].toByteArray());
          m_timezone_label->set_label(a_data["zone_id"].toString());

          std::string zone_id(a_data["zone_id"].toByteArray().data());

          a_session->save_session_attribute(
              a_controller->session_database_name("clock"), "Clock", "clock_id",
              a_session->session_id_to_string(), "zone_id", zone_id);
        });
      }
    }
  });

  if (a_controller && a_controller->viewport()) {
    a_controller->insert(m_clock_session_window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    m_clock_session_window->setPos(window_location);
  }
}

void
Clock::PrivateClockController::_create_timer_ui(Clock *a_controller,
                                                UIKit::SessionSync *a_session) {
  float window_width = 248;

  UIKit::Window *m_clock_session_window = new UIKit::Window();
  UIKit::Widget *m_content_view = new UIKit::Widget(m_clock_session_window);
  UIKit::ClockWidget *m_clock_widget = new UIKit::ClockWidget(m_content_view);

  UIKit::DialWidget *m_dial_widget_seconds_ptr =
      new UIKit::DialWidget(m_content_view);
  UIKit::DialWidget *m_dial_widget_mintues_ptr =
      new UIKit::DialWidget(m_dial_widget_seconds_ptr);
  UIKit::DialWidget *m_dial_widget_hours_ptr =
      new UIKit::DialWidget(m_dial_widget_mintues_ptr);

  UIKit::ToolBar *m_toolbar = new UIKit::ToolBar(m_content_view);
  UIKit::Label *m_timezone_label = new UIKit::Label(m_toolbar);
  m_timezone_label->set_size(QSizeF(window_width / 2, 32));

  if (a_session->session_keys().contains("zone_id")) {
    m_clock_widget->set_timezone_id(
        a_session->session_data("zone_id").toByteArray());
    m_timezone_label->set_label(a_session->session_data("zone_id").toString());
  }

  m_content_view->setGeometry(QRectF(0, 0, window_width, window_width + 48));
  m_clock_widget->setGeometry(QRectF(0, 0, window_width, window_width));

  m_dial_widget_seconds_ptr->setGeometry(
      QRectF(0, 0, window_width, window_width));
  m_dial_widget_mintues_ptr->setGeometry(
      QRectF(0, 0, window_width - 75, window_width - 75));
  m_dial_widget_hours_ptr->setGeometry(
      QRectF(0, 0, window_width - 150, window_width - 150));

  m_dial_widget_mintues_ptr->setPos(
      (window_width / 2) - (m_dial_widget_mintues_ptr->geometry().width() / 2),
      (window_width / 2) -
          (m_dial_widget_mintues_ptr->geometry().height() / 2));

  m_dial_widget_hours_ptr->setPos(
      (m_dial_widget_mintues_ptr->geometry().width() / 2) -
          (m_dial_widget_hours_ptr->geometry().width() / 2),
      (m_dial_widget_mintues_ptr->geometry().width() / 2) -
          (m_dial_widget_hours_ptr->geometry().height() / 2));

  m_dial_widget_seconds_ptr->set_maximum_dial_value(60);
  m_dial_widget_mintues_ptr->set_maximum_dial_value(60);
  m_dial_widget_hours_ptr->set_maximum_dial_value(12);

  m_clock_widget->hide();
  m_dial_widget_seconds_ptr->show();
  m_dial_widget_mintues_ptr->show();

  m_clock_session_window->set_window_title("Timer");

  // toolbar placement.
  m_toolbar->set_icon_resolution("hdpi");
  m_toolbar->set_icon_size(QSize(24, 24));

  m_toolbar->add_action("Start", "actions/pd_play", false);
  m_toolbar->insert_widget(m_timezone_label);
  m_toolbar->add_action("Pause", "actions/pd_pause", false);
  m_toolbar->add_action("Stop", "actions/pd_stop", false);

  m_toolbar->setGeometry(QRectF(0, 0, window_width, 48));
  m_toolbar->show();
  m_toolbar->setPos(0, window_width);

  m_clock_session_window->set_window_content(m_content_view);

  a_session->bind_to_window(m_clock_session_window);

  // todo:
  // Fix duplication.
  m_dial_widget_seconds_ptr->on_dialed([=](int a_progress_value) {
    QString seconds_string =
        QString("%1").arg(m_dial_widget_seconds_ptr->current_dial_value());
    QString minutes_string =
        QString("%1").arg(m_dial_widget_mintues_ptr->current_dial_value());
    QString hours_string =
        QString("%1").arg(m_dial_widget_hours_ptr->current_dial_value());

    if (m_dial_widget_seconds_ptr->current_dial_value() < 9)
      seconds_string =
          QString("0%1").arg(m_dial_widget_seconds_ptr->current_dial_value());
    if (m_dial_widget_mintues_ptr->current_dial_value() < 9)
      minutes_string =
          QString("0%1").arg(m_dial_widget_mintues_ptr->current_dial_value());
    if (m_dial_widget_hours_ptr->current_dial_value() < 9)
      hours_string =
          QString("0%1").arg(m_dial_widget_hours_ptr->current_dial_value());

    m_clock_session_window->set_window_title(
        QString("%1:%2:%3").arg(hours_string).arg(minutes_string).arg(
            seconds_string));
  });

  m_dial_widget_mintues_ptr->on_dialed([=](int a_progress_value) {
    QString seconds_string =
        QString("%1").arg(m_dial_widget_seconds_ptr->current_dial_value());
    QString minutes_string =
        QString("%1").arg(m_dial_widget_mintues_ptr->current_dial_value());
    QString hours_string =
        QString("%1").arg(m_dial_widget_hours_ptr->current_dial_value());

    if (m_dial_widget_seconds_ptr->current_dial_value() < 9)
      seconds_string =
          QString("0%1").arg(m_dial_widget_seconds_ptr->current_dial_value());
    if (m_dial_widget_mintues_ptr->current_dial_value() < 9)
      minutes_string =
          QString("0%1").arg(m_dial_widget_mintues_ptr->current_dial_value());
    if (m_dial_widget_hours_ptr->current_dial_value() < 9)
      hours_string =
          QString("0%1").arg(m_dial_widget_hours_ptr->current_dial_value());

    m_clock_session_window->set_window_title(
        QString("%1:%2:%3").arg(hours_string).arg(minutes_string).arg(
            seconds_string));
  });

  m_dial_widget_hours_ptr->on_dialed([=](int a_progress_value) {
    QString seconds_string =
        QString("%1").arg(m_dial_widget_seconds_ptr->current_dial_value());
    QString minutes_string =
        QString("%1").arg(m_dial_widget_mintues_ptr->current_dial_value());
    QString hours_string =
        QString("%1").arg(m_dial_widget_hours_ptr->current_dial_value());

    if (m_dial_widget_seconds_ptr->current_dial_value() < 9)
      seconds_string =
          QString("0%1").arg(m_dial_widget_seconds_ptr->current_dial_value());
    if (m_dial_widget_mintues_ptr->current_dial_value() < 9)
      minutes_string =
          QString("0%1").arg(m_dial_widget_mintues_ptr->current_dial_value());
    if (m_dial_widget_hours_ptr->current_dial_value() < 9)
      hours_string =
          QString("0%1").arg(m_dial_widget_hours_ptr->current_dial_value());

    m_clock_session_window->set_window_title(
        QString("%1:%2:%3").arg(hours_string).arg(minutes_string).arg(
            seconds_string));
  });

  m_clock_widget->on_timer_ended([=]() {
    int duration = m_clock_widget->duration();
    int hours = duration / 3600;
    int min = (duration / 60) - (hours * 60);
    int sec = duration - ((hours * 3600) + (min * 60));

    QString hour_string = QString("%1").arg(hours);
    QString minutes_string = QString("%1").arg(min);
    QString secondss_string = QString("%1").arg(sec);

    if (hours < 10)
      hour_string = QString("0%1").arg(hours);
    if (min < 10)
      minutes_string = QString("0%1").arg(min);
    if (sec < 10)
      secondss_string = QString("0%1").arg(sec);

    m_clock_session_window->set_window_title(
        QString("%1:%2:%3 Done!").arg(hour_string).arg(minutes_string).arg(
            secondss_string));
    m_timezone_label->set_label("Ended@" +
                                m_clock_widget->completion_time_as_string());
  });

  m_clock_widget->on_timeout([=](const UIKit::ClockWidget *a_widget) {
    if (!a_widget)
      return;
    int duration = a_widget->elapsed_time_in_seconds();
    int hours = duration / 3600;
    int min = (duration / 60) - (hours * 60);
    int sec = duration - ((hours * 3600) + (min * 60));

    QString hour_string = QString("%1").arg(hours);
    QString minutes_string = QString("%1").arg(min);
    QString secondss_string = QString("%1").arg(sec);

    if (hours < 10)
      hour_string = QString("0%1").arg(hours);
    if (min < 10)
      minutes_string = QString("0%1").arg(min);
    if (sec < 10)
      secondss_string = QString("0%1").arg(sec);

    m_timezone_label->set_label("End@" + a_widget->completion_time_as_string());
    m_clock_session_window->set_window_title(
        QString("%1:%2:%3").arg(hour_string).arg(minutes_string).arg(
            secondss_string));
  });

  m_toolbar->on_item_activated([=](const QString &a_action) {
    if (a_action == "Start") {
      double duration =
          (m_dial_widget_hours_ptr->current_dial_value() * 60 * 60) +
          (m_dial_widget_mintues_ptr->current_dial_value() * 60) +
          m_dial_widget_seconds_ptr->current_dial_value();

      m_dial_widget_seconds_ptr->hide();
      m_clock_widget->add_range_marker(0.0, duration);
      m_clock_widget->show();
      m_clock_widget->run_timer();
    } else if (a_action == "Stop") {
      m_dial_widget_seconds_ptr->reset();
      m_dial_widget_seconds_ptr->set_maximum_dial_value(60);
      m_dial_widget_mintues_ptr->reset();
      m_dial_widget_mintues_ptr->set_maximum_dial_value(60);
      m_dial_widget_hours_ptr->reset();
      m_dial_widget_hours_ptr->set_maximum_dial_value(12);

      m_clock_widget->add_range_marker(
          0.0, m_dial_widget_hours_ptr->current_dial_value());
      m_clock_widget->hide();
      m_dial_widget_seconds_ptr->show();
      m_clock_session_window->set_window_title("00:00:00");
    }
  });

  if (a_controller && a_controller->viewport()) {
    a_controller->insert(m_clock_session_window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    m_clock_session_window->setPos(window_location);
  }
}
