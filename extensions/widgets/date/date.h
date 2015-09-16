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
#ifndef DATE_DATA_H
#define DATE_DATA_H

#include <ck_desktop_controller_interface.h>
#include <ck_fixed_layout.h>
#include <ck_item_view.h>

class date_controller : public cherry_kit::desktop_controller_interface {
public:
  explicit date_controller(QObject *object = 0);
  virtual ~date_controller();

  void init();

  void session_data_ready(const cherry_kit::sync_object &a_session_root);
  void submit_session_data(cherry_kit::sync_object *a_obj);

  void set_view_rect(const QRectF &a_rect);

  bool remove_widget(cherry_kit::widget *a_widget_ptr);

  cherry_kit::ui_action task();

  void add_action_button(cherry_kit::fixed_layout *ui, int a_row, int a_col,
                         const std::string &a_label, const std::string &a_icon);

  void insert_time_element(cherry_kit::item_view *ck_model_view, int i);
private:
  void create_ui_calendar_ui(cherry_kit::session_sync *a_session);

  class PrivateDate;
  PrivateDate *const priv;
};
#endif
