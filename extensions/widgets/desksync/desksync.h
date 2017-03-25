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
#ifndef DESKSYNC_DATA_H
#define DESKSYNC_DATA_H

#include <ck_desktop_controller_interface.h>

class desksync_controller : public cherry_kit::desktop_controller_interface {
public:
  explicit desksync_controller(QObject *object = 0);
  virtual ~desksync_controller();

  void init();

  void session_data_ready(const cherry_kit::sync_object &a_session_root);
  void submit_session_data(cherry_kit::sync_object *a_obj);

  void set_view_rect(const QRectF &a_rect);

  bool remove_widget(cherry_kit::widget *a_widget_ptr);

  void request_pixabay_key(const std::string &a_token);
private:
  class Privatedesksync;
  Privatedesksync *const priv;

  void show_auth_dialog();
};
#endif
