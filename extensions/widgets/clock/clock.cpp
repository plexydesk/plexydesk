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
#include <window.h>
#include <clockwidget.h>
#include <datasync.h>
#include <disksyncengine.h>

// Qt
#include <QAction>
#include <imagebutton.h>
#include <label.h>
#include <viewbuilder.h>

class Clock::PrivateClockController {
public:
  PrivateClockController() {}
  ~PrivateClockController() {}

  void setup_create_clock_ui(Clock *a_controller,
                             cherry_kit::session_sync *a_session);
  cherry_kit::ImageButton *add_action_button(cherry_kit::fixed_layout *ui,
                                             int a_row, int a_col,
                                             const std::string &a_label,
                                             const std::string &a_icon);
  cherry_kit::ActionList m_supported_action_list;
  void setup_create_timer_ui(Clock *a_controller,
                             cherry_kit::session_sync *a_session);
};

Clock::Clock(QObject *parent)
    : cherry_kit::desktop_controller_interface(parent),
      o_view_controller(new PrivateClockController) {}

Clock::~Clock() {
  qDebug() << Q_FUNC_INFO << "Deleted";
  delete o_view_controller;
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
  o_view_controller->m_supported_action_list
      << createAction(1, tr("Clock"), "pd_clock_icon.png");
  o_view_controller->m_supported_action_list
      << createAction(3, tr("Alarm"), "pd_alarm_icon.png");
  o_view_controller->m_supported_action_list
      << createAction(2, tr("Timer"), "pd_timer_icon.png");
}

void Clock::set_view_rect(const QRectF &rect) {}

void Clock::session_data_available(const cherry::sync_object &a_session_root) {
  revoke_previous_session(
      "Clock", [this](cherry_kit::desktop_controller_interface *a_controller,
                      cherry_kit::session_sync *a_session) {
        o_view_controller->setup_create_clock_ui((Clock *)a_controller,
                                                 a_session);
      });

  revoke_previous_session(
      "Timer", [this](cherry_kit::desktop_controller_interface *a_controller,
                      cherry_kit::session_sync *a_session) {
        o_view_controller->setup_create_timer_ui((Clock *)a_controller,
                                                 a_session);
      });
}

void Clock::submit_session_data(cherry::sync_object *a_obj) {
  write_session_data("Clock");
  write_session_data("Timer");
}

bool Clock::remove_widget(cherry_kit::widget *widget) {
  disconnect(dataSource(), SIGNAL(sourceUpdated(QVariantMap)));
  int index = 0;

  return 1;
}

cherry_kit::ActionList Clock::actions() const {
  return o_view_controller->m_supported_action_list;
}

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
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
      // d->_create_clock_ui((Clock *)a_controller, a_session);
      o_view_controller->setup_create_clock_ui((Clock *)a_controller,
                                               a_session);
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
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
      o_view_controller->setup_create_timer_ui((Clock *)a_controller,
                                               a_session);
    });
    return;
  }
}

QString Clock::icon() const { return QString("pd_clock_frame_icon.png"); }

void Clock::onDataUpdated(const QVariantMap &data) {}

cherry_kit::ImageButton *Clock::PrivateClockController::add_action_button(
    cherry_kit::fixed_layout *ui, int a_row, int a_col,
    const std::string &a_label, const std::string &a_icon) {
  cherry_kit::ImageButton *ck_rv = 0;
  cherry_kit::widget_properties_t ck_ui_data;
  ck_ui_data["label"] = a_label;
  ck_ui_data["icon"] = "actions/" + a_icon + ".png";
  ck_rv = dynamic_cast<cherry_kit::ImageButton *>(
      ui->add_widget(a_row, a_col, "image_button", ck_ui_data));
  return ck_rv;
}

void Clock::PrivateClockController::setup_create_clock_ui(
    Clock *a_controller, cherry_kit::session_sync *a_session) {
  cherry_kit::window *ck_window = new cherry_kit::window();
  cherry_kit::fixed_layout *ck_ui = new cherry_kit::fixed_layout(ck_window);

  cherry_kit::ImageButton *ck_location_btn = 0;
  cherry_kit::ClockWidget *ck_clock = 0;

  ck_ui->set_content_margin(10, 10, 10, 10);
  ck_ui->set_geometry(0, 0, 240, 260);

  ck_ui->add_rows(2);
  ck_ui->add_segments(0, 1);
  ck_ui->add_segments(1, 3);

  ck_ui->set_row_height(0, "90%");
  ck_ui->set_row_height(1, "10%");
  ck_ui->set_segment_width(1, 0, "30%");
  ck_ui->set_segment_width(1, 1, "60%");
  ck_ui->set_segment_width(1, 2, "10%");

  cherry_kit::widget_properties_t ui_data;
  ui_data["text"] + "";

  ck_clock = dynamic_cast<cherry_kit::ClockWidget *>(
      ck_ui->add_widget(0, 0, "clock", ui_data));

  ck_location_btn = add_action_button(ck_ui, 1, 1, "Location", "pd_location");
  // add_action_button(ck_ui, 1, 1, "Alarm", "pd_notification");
  // add_action_button(ck_ui, 1, 2, "Timer", "pd_history");

  ck_window->set_window_content(ck_ui->viewport());
  ck_window->set_window_title("Clock");

  a_session->bind_to_window(ck_window);
  ck_window->on_window_discarded([this](cherry_kit::window *aWindow) {
    delete aWindow;
  });

  if (a_session->session_keys().contains("zone_id")) {
    ck_clock->set_timezone_id(a_session->session_data("zone_id").toByteArray());
    ck_window->set_window_title(a_session->session_data("zone_id").toString());
  }

  if (a_controller->viewport()) {
    a_controller->insert(ck_window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    ck_window->setPos(window_location);
  }

  ck_location_btn->on_input_event([=](cherry_kit::widget::InputEvent a_event,
                                      const cherry_kit::widget *a_widget) {
    if (a_event == cherry_kit::widget::kMouseReleaseEvent) {
      if (a_controller && a_controller->viewport()) {
        cherry_kit::space *ck_space = a_controller->viewport();

        QPointF _activity_window_location = ck_space->center(
            QRectF(0, 0, 240, 320), QRectF(ck_window->x(), ck_window->y(),
                                           ck_window->geometry().width(),
                                           ck_window->geometry().height()),
            cherry_kit::space::kCenterOnWindow);
        cherry_kit::desktop_dialog_ref activity = ck_space->create_activity(
            "timezone", "TimeZone", _activity_window_location,
            QRectF(0, 0, 240, 320.0), QVariantMap());

        activity->on_action_completed([=](const QVariantMap &a_data) {
          ck_clock->set_timezone_id(a_data["zone_id"].toByteArray());
          ck_window->set_window_title(a_data["zone_id"].toString());

          std::string zone_id(a_data["zone_id"].toByteArray().data());

          a_session->save_session_attribute(
              a_controller->session_database_name("clock"), "Clock", "clock_id",
              a_session->session_id_to_string(), "zone_id", zone_id);
        });
      }
    }
  });
}

void Clock::PrivateClockController::setup_create_timer_ui(
    Clock *a_controller, cherry_kit::session_sync *a_session) {
  cherry_kit::window *ck_window = new cherry_kit::window();
  cherry_kit::fixed_layout *ck_ui = new cherry_kit::fixed_layout(ck_window);

  cherry_kit::ImageButton *ck_location_btn = 0;
  cherry_kit::DialWidget *ck_clock = 0;

  ck_ui->set_content_margin(10, 10, 10, 10);
  ck_ui->set_geometry(0, 0, 240, 260);

  ck_ui->add_rows(4);

  ck_ui->add_segments(0, 1);
  ck_ui->add_segments(1, 1);
  ck_ui->add_segments(2, 3);
  ck_ui->add_segments(3, 3);

  ck_ui->set_row_height(0, "10%");
  ck_ui->set_row_height(1, "70%");
  ck_ui->set_row_height(2, "10%");
  ck_ui->set_row_height(3, "10%");

  cherry_kit::widget_properties_t ui_data;

  ck_ui->add_widget(1, 0, "dial", ui_data);

  ui_data["label"] = "00";
  ck_ui->add_widget(2, 0, "label", ui_data);
  ck_ui->add_widget(2, 1, "label", ui_data);
  ck_ui->add_widget(2, 2, "label", ui_data);

  ck_location_btn = add_action_button(ck_ui, 3, 0, "Start", "pd_play");
  add_action_button(ck_ui, 3, 1, "Pause", "pd_pause");
  add_action_button(ck_ui, 3, 2, "Stop", "pd_stop");

  ck_window->set_window_content(ck_ui->viewport());
  ck_window->set_window_title("Timer");

  a_session->bind_to_window(ck_window);
  ck_window->on_window_discarded([this](cherry_kit::window *aWindow) {
    delete aWindow;
  });

  if (a_session->session_keys().contains("zone_id")) {
    // ck_clock->set_timezone_id(a_session->session_data("zone_id").toByteArray());
    ck_window->set_window_title(a_session->session_data("zone_id").toString());
  }

  if (a_controller->viewport()) {
    a_controller->insert(ck_window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    ck_window->setPos(window_location);
  }

  ck_location_btn->on_input_event([=](cherry_kit::widget::InputEvent a_event,
                                      const cherry_kit::widget *a_widget) {
    if (a_event == cherry_kit::widget::kMouseReleaseEvent) {
      if (a_controller && a_controller->viewport()) {
        cherry_kit::space *ck_space = a_controller->viewport();

        QPointF _activity_window_location = ck_space->center(
            QRectF(0, 0, 240, 320), QRectF(ck_window->x(), ck_window->y(),
                                           ck_window->geometry().width(),
                                           ck_window->geometry().height()),
            cherry_kit::space::kCenterOnWindow);
        cherry_kit::desktop_dialog_ref activity = ck_space->create_activity(
            "timezone", "TimeZone", _activity_window_location,
            QRectF(0, 0, 240, 320.0), QVariantMap());

        activity->on_action_completed([=](const QVariantMap &a_data) {
          // ck_clock->set_timezone_id(a_data["zone_id"].toByteArray());
          ck_window->set_window_title(a_data["zone_id"].toString());

          std::string zone_id(a_data["zone_id"].toByteArray().data());

          a_session->save_session_attribute(
              a_controller->session_database_name("clock"), "Clock", "clock_id",
              a_session->session_id_to_string(), "zone_id", zone_id);
        });
      }
    }
  });
}
