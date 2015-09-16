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
#include <QGraphicsLinearLayout>
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

class date_controller::PrivateDate {
public:
  PrivateDate() {}
  ~PrivateDate() {}
};

date_controller::date_controller(QObject *object)
    : cherry_kit::desktop_controller_interface(object), priv(new PrivateDate) {}

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
  task.set_icon("panel/ck_add.png");

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

void date_controller::add_action_button(cherry_kit::fixed_layout *ui, int a_row,
                                        int a_col, const std::string &a_label,
                                        const std::string &a_icon) {
  cherry_kit::widget_properties_t ui_data;
  ui_data["label"] = a_label;
  ui_data["icon"] = "toolbar/" + a_icon + ".png";
  ui->add_widget(a_row, a_col, "image_button", ui_data);
}

void date_controller::insert_time_element(cherry_kit::item_view *ck_model_view,
                                          int i)
{
  cherry_kit::model_view_item *ck_model_itm =
      new cherry_kit::model_view_item();
  cherry_kit::widget *ck_base_view = new cherry_kit::widget(ck_model_view);
  cherry_kit::label *ck_item_lbl = new cherry_kit::label(ck_base_view);
  cherry_kit::icon_button *ck_button =
      new cherry_kit::icon_button(ck_base_view);

  ck_base_view->setMinimumSize(ck_model_view->boundingRect().width(), 32);
  ck_item_lbl->set_alignment(Qt::AlignLeft);
  ck_item_lbl->set_text(QString("%1 AM").arg(i));
  ck_item_lbl->set_size(
        QSize(ck_model_view->boundingRect().width() - 32, 32));

  ck_model_itm->set_view(ck_base_view);
  ck_model_view->insert(ck_model_itm);
}

void
date_controller::create_ui_calendar_ui(cherry_kit::session_sync *a_session) {
  cherry_kit::window *window = new cherry_kit::window();
  cherry_kit::fixed_layout *ui = new cherry_kit::fixed_layout(window);
  cherry_kit::item_view *ck_model_view = 0;

  ui->set_content_margin(5, 5, 5, 5);
  ui->set_geometry(0, 0, 320, 480);
  ui->add_rows(3);
  ui->add_segments(0, 1);
  ui->add_segments(1, 1);
  ui->add_segments(2, 1);
  ui->set_row_height(0, "60%");
  ui->set_row_height(1, "35%");
  ui->set_row_height(2, "5%");

  cherry_kit::widget_properties_t ui_data;
  ui_data["text"] + "";

  ui->add_widget(0, 0, "calendar", ui_data);
  ck_model_view  = dynamic_cast<cherry_kit::item_view*>
      (ui->add_widget(1, 0, "model_view", ui_data));

  insert_time_element(ck_model_view, 12);

  for(int i = 1; i < 11; i++) {
    insert_time_element(ck_model_view, i);
  }

  insert_time_element(ck_model_view, 0);

  for(int i = 1; i < 11; i++) {
    insert_time_element(ck_model_view, i);
  }

  insert_time_element(ck_model_view, 12);

  add_action_button(ui, 2, 0, "", "ck_person_add");

  window->set_window_title("Appointments");
  window->set_window_content(ui->viewport());

  a_session->bind_to_window(window);

  window->on_window_discarded([=](cherry_kit::window *aWindow) {
    a_session->unbind_window(aWindow);
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
