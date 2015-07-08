/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
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
#include <calendarwidget.h>
#include <item_view.h>
#include <toolbar.h>
#include <label.h>
#include <plexyconfig.h>
#include <session_sync.h>
#include <imagebutton.h>
#include <resource_manager.h>
#include <window.h>
#include <viewbuilder.h>

class DateControllerImpl::PrivateDate {
public:
  PrivateDate() {}
  ~PrivateDate() {}

  CherryKit::ActionList m_supported_action_list;
};

DateControllerImpl::DateControllerImpl(QObject *object)
    : CherryKit::ViewController(object), o_view_controller(new PrivateDate) {}

DateControllerImpl::~DateControllerImpl() { delete o_view_controller; }

void DateControllerImpl::init() {
  QAction *_add_note_action = new QAction(this);
  _add_note_action->setText(tr("Calendar"));

  _add_note_action->setProperty("id", QVariant(1));
  _add_note_action->setProperty("icon_name", "pd_calendar_icon.png");

  o_view_controller->m_supported_action_list << _add_note_action;
}

void DateControllerImpl::session_data_available(
    const QuetzalKit::SyncObject &a_session_root) {
  revoke_previous_session("Calendar",
                          [this](CherryKit::ViewController *a_controller,
                                 CherryKit::SessionSync *a_session) {
    create_ui_calendar_ui(a_session);
  });
}

void DateControllerImpl::submit_session_data(QuetzalKit::SyncObject *a_obj) {
  write_session_data("Calendar");
}

void DateControllerImpl::set_view_rect(const QRectF &a_rect) {}

bool DateControllerImpl::remove_widget(CherryKit::Widget *a_widget_ptr) {
  return false;
}

CherryKit::ActionList DateControllerImpl::actions() const {
  return o_view_controller->m_supported_action_list;
}

void DateControllerImpl::request_action(const QString &a_name,
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
                  [this](CherryKit::ViewController *a_controller,
                         CherryKit::SessionSync *a_session) {
      create_ui_calendar_ui(a_session);
    });
  }
}

QString DateControllerImpl::icon() const { return QString(); }

void DateControllerImpl::add_action_button(CherryKit::HybridLayout *ui,
                                           int a_row, int a_col,
                                           const std::string &a_label,
                                           const std::string &a_icon) {
  CherryKit::WidgetProperties ui_data;
  ui_data["label"] = a_label;
  ui_data["icon"] = "actions/" + a_icon + ".png";
  ui->add_widget(a_row, a_col, "image_button", ui_data);
}

void
DateControllerImpl::create_ui_calendar_ui(CherryKit::SessionSync *a_session) {
  CherryKit::Window *window = new CherryKit::Window();
  CherryKit::HybridLayout *ui = new CherryKit::HybridLayout(window);

  ui->set_content_margin(10, 10, 10, 10);
  ui->set_geometry(0, 0, 360, 340);
  ui->add_rows(2);
  ui->add_segments(0, 1);
  ui->add_segments(1, 3);
  ui->set_row_height(0, "95%");
  ui->set_row_height(1, "5%");

  CherryKit::WidgetProperties ui_data;
  ui_data["text"] + "";

  ui->add_widget(0, 0, "calendar", ui_data);

  add_action_button(ui, 1, 0, "Zoom In", "pd_zoom_in");
  add_action_button(ui, 1, 1, "Zoom Out", "pd_zoom_out");
  add_action_button(ui, 1, 2, "Tasks", "pd_view_list");

  window->set_window_content(ui->viewport());

  a_session->bind_to_window(window);

  window->on_window_discarded([this](CherryKit::Window *aWindow) {
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
