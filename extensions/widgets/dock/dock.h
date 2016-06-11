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
#ifndef DOCK_DATA_H
#define DOCK_DATA_H

#include <QtCore>

#include <ck_data_source.h>
#include <ck_desktop_controller_interface.h>
#include <ck_widget.h>
#include <QtNetwork>
#include <ck_table_view_item.h>
#include <ck_fixed_layout.h>

using namespace cherry_kit;

class desktop_panel_controller_impl
    : public cherry_kit::desktop_controller_interface {
  Q_OBJECT

public:
  desktop_panel_controller_impl(QObject *object = 0);
  virtual ~desktop_panel_controller_impl();

  void init();
  void prepare_removal();

  void session_data_ready(const cherry_kit::sync_object &a_sesion_root);
  virtual void submit_session_data(cherry_kit::sync_object *a_obj);

  void set_view_rect(const QRectF &rect);

  cherry_kit::ui_action task();

protected:

  widget *create_task_action(ui_action &a_task);

  void insert_action(ui_action &a_task);
  void insert_sub_action(ui_action &a_task);

  void discover_actions_from_controller(const QString &name);

  void toggle_panel();
  void update_desktop_preview();

  void switch_to_previous_space();
  void switch_to_next_space();
  void toggle_seamless();

  void remove_space_request();
  void add_new_space();

  void exec_action(const QString &action,
                   cherry_kit::window *a_window);

private:
  class PrivateDock;
  PrivateDock *const priv;
  void create_desktop_navigation_panel();
};

#endif
