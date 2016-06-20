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

class time_segment;
class time_event;

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

  void save_to_store(cherry_kit::session_sync *a_session,
                     const std::string &a_key, const std::string &a_value,
                     int a_id);
  void
  new_event_store(const std::string &a_value, const std::string &a_key,
                  int a_id,
                  std::function<void(cherry_kit::desktop_controller_interface *,
                                     cherry_kit::session_sync *)>
                      a_callback);

  int event_count();
  void update_event_count();

private:
  void create_ui_calendar_ui(cherry_kit::session_sync *a_session);

  class date_controller_context;
  date_controller_context *const priv;
};
#endif
