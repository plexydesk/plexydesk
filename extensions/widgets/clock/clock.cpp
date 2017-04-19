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
#include "ck_clock_view.h"

// uikit
#include <ck_ToolBar.h>
#include <ck_desktop_controller_interface.h>
#include <ck_dial_view.h>
#include <ck_extension_manager.h>
#include <ck_range_view.h>
#include <ck_resource_manager.h>
#include <ck_session_sync.h>
#include <ck_space.h>
#include <ck_screen.h>

// datakit
#include <ck_clock_view.h>
#include <ck_data_sync.h>
#include <ck_disk_engine.h>
#include <ck_window.h>

// Qt
#include <ck_fixed_layout.h>
#include <ck_icon_button.h>
#include <ck_label.h>
#include <ck_timer.h>

// c++
#include <chrono>

class time_controller::PrivateClockController {
public:
  PrivateClockController() {}
  ~PrivateClockController() {}

  void setup_create_clock_ui(time_controller *a_controller,
                             cherry_kit::session_sync *a_session);
  cherry_kit::icon_button *add_action_button(cherry_kit::fixed_layout *ui,
                                             int a_row, int a_col,
                                             const std::string &a_label,
                                             const std::string &a_icon);
  void setup_create_timer_ui(time_controller *a_controller,
                             cherry_kit::session_sync *a_session);

  cherry_kit::timer *m_timer;
};

time_controller::time_controller(QObject *parent)
    : cherry_kit::desktop_controller_interface(parent),
      priv(new PrivateClockController) {}

time_controller::~time_controller() {
  qDebug() << Q_FUNC_INFO << "Deleted";
  delete priv;
}

void time_controller::init() {}

void time_controller::set_view_rect(const QRectF &rect) {}

void time_controller::session_data_ready(
    const cherry_kit::sync_object &a_session_root) {
  revoke_previous_session(
      "Clock", [this](cherry_kit::desktop_controller_interface *a_controller,
                      cherry_kit::session_sync *a_session) {
        priv->setup_create_clock_ui((time_controller *)a_controller, a_session);
      });

  revoke_previous_session(
      "Timer", [this](cherry_kit::desktop_controller_interface *a_controller,
                      cherry_kit::session_sync *a_session) {
        priv->setup_create_timer_ui((time_controller *)a_controller, a_session);
      });
}

void time_controller::submit_session_data(cherry_kit::sync_object *a_obj) {
  write_session_data("Clock");
  write_session_data("Timer");
}

bool time_controller::remove_widget(cherry_kit::widget *widget) { return 1; }

/*
cherry_kit::ActionList time_controller::actions() const {
  return priv->m_supported_action_list;
}

void time_controller::request_action(const QString &actionName,
                                     const QVariantMap &args) {
  QPointF window_location;

  if (viewport()) {
    window_location = viewport()->center(QRectF(0, 0, 240, 240 + 48));
  }

  if (actionName == tr("Track")) {
    QVariantMap session_args;

    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["clock_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_store_name("clock"));

    start_session("Clock", session_args, false,
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
      // d->_create_clock_ui((Clock *)a_controller, a_session);
      priv->setup_create_clock_ui((time_controller *)a_controller,
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
        QString::fromStdString(session_store_name("timer"));

    start_session("Timer", session_args, false,
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
      priv->setup_create_timer_ui((time_controller *)a_controller,
                                               a_session);
    });
    return;
  }
}
*/

cherry_kit::ui_action time_controller::task() {
  cherry_kit::ui_action task;
  task.set_name("Track");
  task.set_visible(1);
  task.set_controller(controller_name().toStdString());
  task.set_icon("navigation/ck_alarm.png");

  cherry_kit::ui_action time_task;
  time_task.set_name("Time");
  time_task.set_id(0);
  time_task.set_visible(1);
  time_task.set_controller(controller_name().toStdString());
  time_task.set_icon("panel/ck_add.png");

  time_task.set_task([this](const cherry_kit::ui_action *a_action_ref,
                            const cherry_kit::ui_task_data_t &a_data) {
    //
    QPointF window_location;
    if (viewport()) {
      window_location = viewport()->center(QRectF(0, 0, 320, 320 + 48));
    }

    QVariantMap session_args;

    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["clock_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_store_name("clock"));

    start_session("Clock", session_args, false,
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
      priv->setup_create_clock_ui((time_controller *)a_controller, a_session);
    });
  });

  cherry_kit::ui_action timer_task;
  timer_task.set_name("Duration");
  timer_task.set_id(1);
  timer_task.set_visible(1);
  timer_task.set_controller(controller_name().toStdString());
  timer_task.set_icon("panel/ck_add.png");

  timer_task.set_task([this](const cherry_kit::ui_action *a_action_ref,
                             const cherry_kit::ui_task_data_t &a_data) {
    //
    QPointF window_location;
    if (viewport()) {
      window_location = viewport()->center(QRectF(0, 0, 320, 320 + 48));
    }

    QVariantMap session_args;

    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["timer_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_store_name("timer"));

    start_session("Timer", session_args, false,
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
      priv->setup_create_timer_ui((time_controller *)a_controller, a_session);
    });
  });

  task.add_action(time_task);
  task.add_action(timer_task);
  return task;
}

cherry_kit::icon_button *
time_controller::PrivateClockController::add_action_button(
    cherry_kit::fixed_layout *ui, int a_row, int a_col,
    const std::string &a_label, const std::string &a_icon) {
  cherry_kit::icon_button *ck_rv = 0;
  cherry_kit::widget_properties_t ck_ui_data;
  ck_ui_data["label"] = a_label;
  ck_ui_data["icon"] = "toolbar/" + a_icon + ".png";
  ck_rv = dynamic_cast<cherry_kit::icon_button *>(
      ui->add_widget(a_row, a_col, "image_button", ck_ui_data, [=]() {}));
  return ck_rv;
}

void time_controller::PrivateClockController::setup_create_clock_ui(
    time_controller *a_controller, cherry_kit::session_sync *a_session) {
  cherry_kit::window *ck_window = new cherry_kit::window();
  cherry_kit::fixed_layout *ck_ui = new cherry_kit::fixed_layout(ck_window);

  cherry_kit::icon_button *ck_location_btn = 0;
  cherry_kit::clock_view *ck_clock = 0;

  ck_ui->set_content_margin(0, 0, 0, 0);
  ck_ui->set_geometry(0, 0, 320, 240);

  ck_ui->add_rows(2);

  ck_ui->add_segments(0, 1);
  ck_ui->add_segments(1, 1);

  ck_ui->set_row_height(0, "90%");
  ck_ui->set_row_height(1, "10%");

  /*
  ck_ui->set_segment_width(1, 0, "20%");
  ck_ui->set_segment_width(1, 1, "70%");
  ck_ui->set_segment_width(1, 2, "10%");
  */

  cherry_kit::widget_properties_t ui_data;
  ui_data["text"] + "";

  ck_clock = dynamic_cast<cherry_kit::clock_view *>(
      ck_ui->add_widget(0, 0, "clock", ui_data, [=]() {}));

  ck_location_btn = add_action_button(ck_ui, 1, 0, "", "ck_location");
  ck_location_btn->hide();

  ck_window->set_window_content(ck_ui->viewport());
  ck_window->set_window_title("Time");

  a_session->bind_to_window(ck_window);
  ck_window->on_window_discarded([=](cherry_kit::window *aWindow) {
    a_session->unbind_window(ck_window);
    delete ck_ui;
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

  ck_location_btn->on_click([=]() {
    if (a_controller && a_controller->viewport()) {
      cherry_kit::space *ck_space = a_controller->viewport();

      cherry_kit::desktop_dialog_ref activity =
              ck_space->create_child_activity("timezone_dialog", ck_window);

      activity->on_action_completed([=](const QVariantMap &a_data) {
        ck_clock->set_timezone_id(a_data["zone_id"].toByteArray());
        ck_window->set_window_title(a_data["zone_id"].toString());

        std::string zone_id(a_data["zone_id"].toByteArray().data());

        a_session->save_session_attribute(
            a_controller->session_store_name("clock"), "Clock", "clock_id",
            a_session->session_id_to_string(), "zone_id", zone_id);
      });
    }
  });
}

void time_controller::PrivateClockController::setup_create_timer_ui(
    time_controller *a_controller, cherry_kit::session_sync *a_session) {
  cherry_kit::window *ck_window = new cherry_kit::window();
  cherry_kit::fixed_layout *ck_ui = new cherry_kit::fixed_layout(ck_window);

  cherry_kit::icon_button *ck_start_btn = 0;
  cherry_kit::dial_view *ck_dial = 0;
  cherry_kit::label *ck_timer_label = 0;

  cherry_kit::timer *timer = new cherry_kit::timer(1000);
  int timeout_value = 0;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

  ck_ui->set_content_margin(10, 10, 10, 10);
  ck_ui->set_geometry(0, 0, 240, 240);

  ck_ui->add_rows(4);

  ck_ui->add_segments(0, 1);
  ck_ui->add_segments(1, 1);
  ck_ui->add_segments(2, 1);
  ck_ui->add_segments(3, 1);

  ck_ui->set_row_height(0, "10%");
  ck_ui->set_row_height(1, "70%");
  ck_ui->set_row_height(2, "10%");
  ck_ui->set_row_height(3, "10%");

  cherry_kit::widget_properties_t ui_data;

  ck_dial = dynamic_cast<cherry_kit::dial_view *>(
      ck_ui->add_widget(1, 0, "dial", ui_data, [=]() {}));

  ui_data["label"] = "00";
  ck_timer_label = dynamic_cast<cherry_kit::label *>(
      ck_ui->add_widget(2, 0, "label", ui_data, [=]() {}));

  ck_start_btn = add_action_button(ck_ui, 3, 0, "", "ck_play");

  ck_window->set_window_content(ck_ui->viewport());
  ck_window->set_window_title("Timer");

  a_session->bind_to_window(ck_window);
  ck_window->on_window_discarded([=](cherry_kit::window *aWindow) {
    a_session->unbind_window(ck_window);
    if (timer->is_active())
      timer->stop();
    delete timer;
    delete ck_ui;
    delete aWindow;
  });

  if (a_controller->viewport()) {
    a_controller->insert(ck_window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    ck_window->setPos(window_location);
  }

  timer->on_timeout([=]() {
    if (!ck_start_btn || !ck_dial || !ck_timer_label || !timer)
      return;

    int dial_value_in_seconds = ck_dial->current_dial_value() * 60;

    if (timer->elapsed_seconds() >= dial_value_in_seconds) {
      timer->stop();
      ck_window->set_window_title("Complete");
      QPixmap pixmap = cherry_kit::resource_manager::instance()->drawable(
          "toolbar/ck_play.png", "mdpi");
      ck_start_btn->set_pixmap(pixmap);

      return;
    }

    QString time_as_string =
        QString("%2 : %1")
            .arg((dial_value_in_seconds - timer->elapsed_seconds()) % 60)
            .arg(ck_dial->current_dial_value() -
                 (timer->elapsed_minutes() + 1));

    ck_window->set_window_title(time_as_string);
  });

  ck_start_btn->on_click([=]() {
    if (!timer->is_active()) {
      if (ck_dial) {
        timer->start();
      }
      if (ck_start_btn) {
        QPixmap pixmap = cherry_kit::resource_manager::instance()->drawable(
            "toolbar/ck_stop.png", "mdpi");
        ck_start_btn->set_pixmap(pixmap);
      }
    } else {
      timer->stop();
      if (ck_start_btn) {
        QPixmap pixmap = cherry_kit::resource_manager::instance()->drawable(
            "toolbar/ck_play.png", "mdpi");
        ck_start_btn->set_pixmap(pixmap);
      }
    }
  });

  // dial
  if (ck_dial) {
    ck_dial->set_maximum_dial_value(60);
    ck_dial->on_dialed([=](int a_value) {
      qDebug() << Q_FUNC_INFO << a_value;
      if (ck_timer_label) {
        ck_timer_label->set_text(QString("%1").arg(a_value));
      }
    });
  }
}
