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

class date_controller::PrivateDate {
public:
  PrivateDate() {}
  ~PrivateDate() {}

  cherry_kit::ActionList m_supported_action_list;
};

date_controller::date_controller(QObject *object)
    : cherry_kit::desktop_controller_interface(object),
      o_view_controller(new PrivateDate) {}

date_controller::~date_controller() { delete o_view_controller; }

void date_controller::init() {
  QAction *_add_note_action = new QAction(this);
  _add_note_action->setText(tr("Calendar"));

  _add_note_action->setProperty("id", QVariant(1));
  _add_note_action->setProperty("icon_name", "pd_calendar_icon.png");

  o_view_controller->m_supported_action_list << _add_note_action;
}

void date_controller::session_data_available(
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

cherry_kit::ActionList date_controller::actions() const {
  return o_view_controller->m_supported_action_list;
}

void date_controller::request_action(const QString &a_name,
                                        const QVariantMap &a_args) {
  QPointF window_location;

  if (viewport()) {
    window_location = viewport()->center(QRectF(0, 0, 240, 240 + 48));
  }

  QVariantMap session_args;

  if (a_name == tr("Calendar")) {
    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["calendar_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_database_name("calendar"));

    start_session("Calendar", session_args, false,
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
      create_ui_calendar_ui(a_session);
    });
  }
}

QString date_controller::icon() const { return QString(); }

void date_controller::add_action_button(cherry_kit::fixed_layout *ui,
                                           int a_row, int a_col,
                                           const std::string &a_label,
                                           const std::string &a_icon) {
  cherry_kit::widget_properties_t ui_data;
  ui_data["label"] = a_label;
  ui_data["icon"] = "actions/" + a_icon + ".png";
  ui->add_widget(a_row, a_col, "image_button", ui_data);
}

void
date_controller::create_ui_calendar_ui(cherry_kit::session_sync *a_session) {
  cherry_kit::window *window = new cherry_kit::window();
  cherry_kit::fixed_layout *ui = new cherry_kit::fixed_layout(window);

  ui->set_content_margin(10, 10, 10, 10);
  ui->set_geometry(0, 0, 360, 340);
  ui->add_rows(2);
  ui->add_segments(0, 1);
  ui->add_segments(1, 3);
  ui->set_row_height(0, "95%");
  ui->set_row_height(1, "5%");

  cherry_kit::widget_properties_t ui_data;
  ui_data["text"] + "";

  ui->add_widget(0, 0, "calendar", ui_data);

  add_action_button(ui, 1, 0, "Zoom In", "pd_zoom_in");
  add_action_button(ui, 1, 1, "Zoom Out", "pd_zoom_out");
  add_action_button(ui, 1, 2, "Tasks", "pd_view_list");

  window->set_window_content(ui->viewport());

  a_session->bind_to_window(window);

  window->on_window_discarded([this](cherry_kit::window *aWindow) {
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
