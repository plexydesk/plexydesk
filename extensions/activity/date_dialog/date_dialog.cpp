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
#include "date_dialog.h"
#include <ck_widget.h>
#include <ck_config.h>

#include <ck_desktop_controller_interface.h>
#include <ck_TableView.h>
#include <ck_DefaultTableModel.h>
#include <ck_resource_manager.h>
#include <ck_icon_button.h>
#include <ck_button.h>
#include <ck_calendar_view.h>
#include <ck_fixed_layout.h>

class date_dialog::PrivateDatePicker {
public:
  PrivateDatePicker() {}
  ~PrivateDatePicker() {}

  cherry_kit::window *m_activity_window;
  cherry_kit::widget *m_window_content;
  cherry_kit::button *m_done_btn;
  cherry_kit::calendar_view *m_cal_widget;
};

date_dialog::date_dialog(QObject *object)
    : cherry_kit::desktop_dialog(object), priv(new PrivateDatePicker) {}

date_dialog::~date_dialog() { delete priv; }

void date_dialog::create_window(const QRectF &window_geometry,
                                const QString &window_title,
                                const QPointF &window_pos) {
  priv->m_activity_window = new cherry_kit::window();
  priv->m_activity_window->setGeometry(window_geometry);
  priv->m_activity_window->set_window_title(window_title);

  cherry_kit::fixed_layout *view =
      new cherry_kit::fixed_layout(priv->m_activity_window);

  view->set_content_margin(5, 5, 5, 5);
  view->set_geometry(0, 0, window_geometry.width(), window_geometry.height());

  view->add_rows(2);
  view->add_segments(0, 1);
  view->add_segments(1, 3);

  view->set_row_height(0, "90%");
  view->set_row_height(1, "10%");

  cherry_kit::widget_properties_t prop;
  prop["label"] = "Apply";

  priv->m_cal_widget = dynamic_cast<cherry_kit::calendar_view *>(
      view->add_widget(0, 0, "calendar", prop));

  priv->m_done_btn = dynamic_cast<cherry_kit::button *>(
      view->add_widget(1, 1, "button", prop));

  priv->m_activity_window->set_window_content(view->viewport());

  priv->m_done_btn->on_input_event([this](
      cherry_kit::widget::InputEvent a_event,
      const cherry_kit::widget *a_widget) {
    if (a_event == cherry_kit::widget::kMouseReleaseEvent) {
      end_calendar();
      notify_done();
    }
  });
}

cherry_kit::window *date_dialog::dialog_window() const {
  return priv->m_activity_window;
}

void date_dialog::cleanup() {
  if (priv->m_activity_window) {
    delete priv->m_activity_window;
  }

  priv->m_activity_window = 0;
}

void date_dialog::end_calendar() {
  notify_message("date",
                 priv->m_cal_widget->selected_date().toString().toStdString());
}
