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
  cherry_kit::button *m_done_btn;
  cherry_kit::calendar_view *m_cal_widget;
  cherry_kit::fixed_layout *m_layout;
};

date_dialog::date_dialog(QObject *object)
    : cherry_kit::desktop_dialog(object), priv(new PrivateDatePicker) {}

date_dialog::~date_dialog() { delete priv; }

void date_dialog::create_window() {
  QRectF window_geometry(0, 0, 320, 320);
  QString window_title = "Date";

  priv->m_activity_window = new cherry_kit::window();
  priv->m_activity_window->set_geometry(window_geometry);
  priv->m_activity_window->set_window_title(window_title);

  priv->m_layout =
      new cherry_kit::fixed_layout(priv->m_activity_window);

  priv->m_layout->set_content_margin(5, 5, 5, 5);
  priv->m_layout->set_geometry(0, 0, window_geometry.width(), window_geometry.height());

  priv->m_layout->add_rows(2);
  priv->m_layout->add_segments(0, 1);
  priv->m_layout->add_segments(1, 3);

  priv->m_layout->set_row_height(0, "90%");
  priv->m_layout->set_row_height(1, "10%");

  cherry_kit::widget_properties_t prop;
  prop["label"] = "Apply";

  priv->m_cal_widget = dynamic_cast<cherry_kit::calendar_view *>(
      priv->m_layout->add_widget(0, 0, "calendar", prop, [=]() {}));

  priv->m_done_btn = dynamic_cast<cherry_kit::button *>(
      priv->m_layout->add_widget(1, 1, "button", prop, [=]() {}));

  priv->m_activity_window->set_window_content(priv->m_layout->viewport());

  priv->m_done_btn->on_click([this]() {
    notify_calendar_value();
    notify_done();
  });
}

cherry_kit::window *date_dialog::dialog_window() const {
  return priv->m_activity_window;
}

bool date_dialog::purge() {
  if (priv->m_cal_widget)
    delete priv->m_cal_widget;
  if (priv->m_done_btn)
    delete priv->m_done_btn;
  if (priv->m_done_btn)
     delete priv->m_layout;
  if (priv->m_activity_window)
    delete priv->m_activity_window;

  priv->m_activity_window = 0;

  return true;
}

void date_dialog::notify_calendar_value() {
  notify_message("date",
                 priv->m_cal_widget->selected_date().toString().toStdString());
}
