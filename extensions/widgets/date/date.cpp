/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  : *
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
#include "date.h"

#include <ck_ToolBar.h>
#include <ck_calendar_view.h>
#include <ck_config.h>
#include <ck_fixed_layout.h>
#include <ck_icon_button.h>
#include <ck_item_view.h>
#include <ck_label.h>
#include <ck_model_view_item.h>
#include <ck_resource_manager.h>
#include <ck_session_sync.h>
#include <ck_timer.h>
#include <ck_window.h>

#include <chrono>
#include <ctime>
#include <memory>

#include <ck_button.h>
#include <ck_line_edit.h>
#include <ck_text_editor.h>

#include "event_browser_ui.h"
#include "time_event.h"
#include "time_segment.h"

class date_controller::date_controller_context {
public:
  date_controller_context() : m_event_count(0) {}
  ~date_controller_context() { m_ui_list.clear(); }

  int m_event_count;

  std::map<int, std::unique_ptr<event_browser_ui>> m_ui_list;
};

date_controller::date_controller(QObject *object)
    : cherry_kit::desktop_controller_interface(object),
      priv(new date_controller_context) {}

date_controller::~date_controller() { delete priv; }

void date_controller::init() {}

void date_controller::session_data_ready(
    const cherry_kit::sync_object &a_session_root) {
  revoke_previous_session(
      "Calendar", [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
        create_ui_calendar_ui(a_session);
      });

  revoke_previous_session(
      "Event", [this](cherry_kit::desktop_controller_interface *a_controller,
                      cherry_kit::session_sync *a_session) {});
}

void date_controller::submit_session_data(cherry_kit::sync_object *a_obj) {
  write_session_data("Calendar");
  write_session_data("Event");
}

void date_controller::set_view_rect(const QRectF &a_rect) {}

bool date_controller::remove_widget(cherry_kit::widget *a_widget_ptr) {
  return false;
}

cherry_kit::ui_action date_controller::task() {
  cherry_kit::ui_action task;
  task.set_name("Organize");
  task.set_visible(1);
  task.set_controller(controller_name().toStdString());
  task.set_icon("navigation/ck_organize.png");

  cherry_kit::ui_action cal_task;
  cal_task.set_name("Calendar");
  cal_task.set_visible(1);
  cal_task.set_icon("panel/ck_add.png");
  cal_task.set_controller(controller_name().toStdString());
  cal_task.set_task([this](const cherry_kit::ui_action *a_action_ref,
                           const cherry_kit::ui_task_data_t &a_data) {
    QPointF window_location;
    if (viewport()) {
      window_location = viewport()->center(QRectF(0, 0, 240, 240 + 48));
    }
    QVariantMap session_args;

    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["calendar_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_store_name("calendar"));

    start_session("Calendar", session_args, false,
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
                    create_ui_calendar_ui(a_session);
                  });
  });

  task.add_action(cal_task);

  return task;
}

void date_controller::new_event_store(
    const std::string &a_value, const std::string &a_key, int a_id,
    std::function<void(cherry_kit::desktop_controller_interface *,
                       cherry_kit::session_sync *)>
        a_callback) {
  QVariantMap session_args;

  session_args[a_key.c_str()] = a_value.c_str();

  session_args["event_id"] = std::to_string(a_id).c_str();
  session_args["database_name"] = session_store_name("event").c_str();

  start_session("Event", session_args, false, a_callback);
}

int date_controller::event_count() { return priv->m_event_count; }

void date_controller::update_event_count() { priv->m_event_count++; }

void date_controller::create_ui_calendar_ui(
    cherry_kit::session_sync *a_session) {
  std::unique_ptr<event_browser_ui> e_browser(
      new event_browser_ui(a_session, this));
  priv->m_ui_list[e_browser->event_id()] = std::move(e_browser);
}

void date_controller::save_to_store(cherry_kit::session_sync *a_session,
                                    const std::string &a_key,
                                    const std::string &a_value, int a_id) {
  a_session->save_session_attribute(session_store_name("event"), "Event",
                                    "event_id", std::to_string(a_id), a_key,
                                    a_value);
}
