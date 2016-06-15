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

#include <ck_calendar_view.h>
#include <ck_item_view.h>
#include <ck_ToolBar.h>
#include <ck_label.h>
#include <ck_config.h>
#include <ck_session_sync.h>
#include <ck_icon_button.h>
#include <ck_resource_manager.h>
#include <ck_window.h>
#include <ck_fixed_layout.h>
#include <ck_model_view_item.h>
#include <ck_timer.h>

#include <ctime>
#include <chrono>
#include <ck_button.h>
#include <ck_line_edit.h>
#include <ck_text_editor.h>

#include "time_event.h"
#include "time_segment.h"

class date_controller::date_controller_context {
public:
  date_controller_context() {}
  ~date_controller_context() {}
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

cherry_kit::widget *
date_controller::add_action_button(cherry_kit::fixed_layout *ui, int a_row,
                                   int a_col, const std::string &a_label,
                                   const std::string &a_icon) {
  cherry_kit::widget_properties_t ui_data;
  ui_data["label"] = a_label;
  ui_data["icon"] = "toolbar/" + a_icon + ".png";
  return ui->add_widget(a_row, a_col, "image_button", ui_data, [=]() {});
}

time_segment *date_controller::insert_time_element(
    cherry_kit::session_sync *a_session, cherry_kit::item_view *a_view,
    int a_value, int a_type, cherry_kit::window *a_window_ref,
    const date_controller *a_controller_ref) {
  cherry_kit::model_view_item *ck_model_itm = new cherry_kit::model_view_item();
  time_segment *ck_base_view = new time_segment(this, a_session, a_view);
  ck_base_view->set_time_value(a_value);
  ck_base_view->set_time_type((time_segment::segment_t)a_type);

  cherry_kit::label *ck_item_lbl = new cherry_kit::label(ck_base_view);
  cherry_kit::icon_button *ck_button =
      new cherry_kit::icon_button(ck_base_view);

  QString time_str = "AM";
  switch (a_type) {
  case time_segment::kAMTime:
    time_str = QString("%1 AM").arg(a_value);
    break;
  case time_segment::kPMTime:
    time_str = QString("%1 PM").arg(a_value);
    break;
  case time_segment::kNoonTime:
    time_str = "Noon";
    break;
  };

  ck_base_view->set_contents_geometry(0, 0, a_view->contents_geometry().width(),
                                      32);
  ck_item_lbl->set_alignment(Qt::AlignHCenter | Qt::AlignVCenter);
  ck_item_lbl->set_text(time_str);
  ck_item_lbl->set_size(QSize(64, 32));

  ck_model_itm->on_activated([=](cherry_kit::model_view_item *a_item) {
    ck_base_view->create_new([=](cherry_kit::window *ck_app_window) {

      ck_app_window->setPos(a_window_ref->pos());
      QPointF window_pos(a_window_ref->mapToScene(QPointF()));
      QRectF window_geometry(window_pos.x(), window_pos.y(),
                             a_window_ref->geometry().width(),
                             a_window_ref->geometry().height());

      QPointF sub_window_pos(a_window_ref->mapToScene(QPointF()));
      QRectF sub_window_geometry(sub_window_pos.x(), sub_window_pos.y(),
                                 ck_app_window->geometry().width(),
                                 ck_app_window->geometry().height());

      ck_app_window->setPos(
          viewport()->center(sub_window_geometry, window_geometry,
                             cherry_kit::space::kCenterOnWindow));

      insert(ck_app_window);
    });
  });

  ck_model_itm->set_view(ck_base_view);
  a_view->insert(ck_model_itm);

  // event handlers.
  ck_model_itm->on_view_removed([=](cherry_kit::model_view_item *a_item) {
    if (a_item && a_item->view()) {
      cherry_kit::widget *view = a_item->view();
      if (view)
        delete view;
    }
  });

  return ck_base_view;
}

void date_controller::new_event_store(
    const std::string &a_value, const std::string &a_key, int a_id,
    std::function<void(cherry_kit::desktop_controller_interface *,
                       cherry_kit::session_sync *)> a_callback) {
  QVariantMap session_args;

  session_args[a_key.c_str()] = a_value.c_str();

  session_args["event_id"] = std::to_string(a_id).c_str();
  session_args["database_name"] = session_store_name("event").c_str();

  start_session("Event", session_args, false, a_callback);
}

void date_controller::save_to_store(cherry_kit::session_sync *a_session,
                                    const std::string &a_key,
                                    const std::string &a_value, int a_id) {
  a_session->save_session_attribute(session_store_name("event"), "Event",
                                    "event_id",
                                    std::to_string(a_id), a_key,
                                    a_value);
}

void
date_controller::create_ui_calendar_ui(cherry_kit::session_sync *a_session) {
  cherry_kit::window *window = new cherry_kit::window();
  cherry_kit::fixed_layout *ui = new cherry_kit::fixed_layout(window);
  cherry_kit::item_view *ck_model_view = 0;
  cherry_kit::timer *ck_timer = new cherry_kit::timer(1000 * 60);
  cherry_kit::icon_button *ck_add_button = 0;

  ui->set_content_margin(5, 5, 5, 5);
  ui->set_geometry(0, 0, 320, 480);
  ui->add_rows(3);
  ui->add_segments(0, 1);
  ui->add_segments(1, 1);
  ui->add_segments(2, 1);

  ui->set_row_height(0, "50%");
  ui->set_row_height(1, "45%");
  ui->set_row_height(2, "5%");

  cherry_kit::widget_properties_t ui_data;
  ui_data["text"] + "";

  ui->add_widget(0, 0, "calendar", ui_data, [=]() {});
  ck_model_view = dynamic_cast<cherry_kit::item_view *>(
      ui->add_widget(1, 0, "model_view", ui_data, [=]() {}));
  ck_model_view->set_content_size(320, 64);

  ck_model_view->on_item_removed([=](cherry_kit::model_view_item *a_item) {
    delete a_item;
  });

  time_segment_list_t time_segment_list;

  time_segment_list.push_back(insert_time_element(
      a_session, ck_model_view, 12, time_segment::kAMTime, window, this));

  for (int i = 1; i <= 11; i++) {
    time_segment_list.push_back(insert_time_element(
        a_session, ck_model_view, i, time_segment::kAMTime, window, this));
  }

  time_segment_list.push_back(insert_time_element(
      a_session, ck_model_view, 12, time_segment::kNoonTime, window, this));

  for (int i = 1; i <= 11; i++) {
    time_segment_list.push_back(insert_time_element(
        a_session, ck_model_view, i, time_segment::kPMTime, window, this));
  }

  ck_add_button = dynamic_cast<cherry_kit::icon_button *>(
      add_action_button(ui, 2, 0, "", "ck_plus"));

  window->set_window_title("Appointments");
  window->set_window_content(ui->viewport());

  ck_add_button->on_click([=]() {
    std::chrono::system_clock::time_point now =
        std::chrono::system_clock::now();

    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);

    time_segment::segment_t type;

    if (local_tm.tm_hour == 12) {
      type = time_segment::kNoonTime;
    } else if (local_tm.tm_hour > 12) {
      type = time_segment::kPMTime;
    } else {
      type = time_segment::kAMTime;
    }

    std::for_each(std::begin(time_segment_list), std::end(time_segment_list),
                  [=](time_segment *a_time_seg_ref) {
      if (!a_time_seg_ref)
        return;
      int current_time = local_tm.tm_hour;

      if (current_time == 0)
        current_time = 12;

      if (type == time_segment::kPMTime)
        current_time = (local_tm.tm_hour - 12);

      if (a_time_seg_ref->time_value() == current_time &&
          a_time_seg_ref->time_type() == type) {

        a_time_seg_ref->create_new([=](cherry_kit::window *ck_app_window) {
          ck_app_window->setPos(window->pos());
          QPointF window_pos(window->mapToScene(QPointF()));
          QRectF window_geometry(window_pos.x(), window_pos.y(),
                                 window->geometry().width(),
                                 window->geometry().height());

          QPointF sub_window_pos(window->mapToScene(QPointF()));
          QRectF sub_window_geometry(sub_window_pos.x(), sub_window_pos.y(),
                                     ck_app_window->geometry().width(),
                                     ck_app_window->geometry().height());

          ck_app_window->setPos(
              viewport()->center(sub_window_geometry, window_geometry,
                                 cherry_kit::space::kCenterOnWindow));

          insert(ck_app_window);
        });
      }
    });
  });
  a_session->bind_to_window(window);

  std::function<void()> ck_timeout_func = ([=]() {
    std::chrono::system_clock::time_point now =
        std::chrono::system_clock::now();

    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);

    time_segment::segment_t type;

    if (local_tm.tm_hour == 12) {
      type = time_segment::kNoonTime;
    } else if (local_tm.tm_hour > 12) {
      type = time_segment::kPMTime;
    } else {
      type = time_segment::kAMTime;
    }

    std::for_each(std::begin(time_segment_list), std::end(time_segment_list),
                  [=](time_segment *a_time_seg_ref) {
      if (!a_time_seg_ref)
        return;
      int current_time = local_tm.tm_hour;

      if (current_time == 0)
        current_time = 12;

      if (type == time_segment::kPMTime)
        current_time = (local_tm.tm_hour - 12);

      if (a_time_seg_ref->time_value() == current_time &&
          a_time_seg_ref->time_type() == type) {
        a_time_seg_ref->set_heighlight(1);
      } else {
        a_time_seg_ref->set_heighlight(0);
      }
    });
  });

  ck_timer->start_once(1000, ck_timeout_func);
  ck_timer->on_timeout(ck_timeout_func);
  ck_timer->start();

  window->on_window_discarded([=](cherry_kit::window *aWindow) {
    a_session->unbind_window(aWindow);
    ck_model_view->clear();
    ck_timer->stop();
    delete ui;
    delete ck_timer;
    delete aWindow;
  });

  if (viewport()) {
    insert(window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    window->setPos(window_location);
  }
}
