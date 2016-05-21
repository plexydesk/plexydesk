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
#ifndef CONTROLLER_ACTION_H
#define CONTROLLER_ACTION_H

#include <cstring>
#include <algorithm>
#include <functional>
#include <map>
#include <vector>

#include <config.h>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {
class ui_action;
typedef std::vector<ui_action> ui_action_list;
typedef std::map<cherry_kit::string, cherry_kit::string> ui_task_data_t;
typedef std::function<void(const ui_action *, const ui_task_data_t &)>
ui_task_callback_t;

class DECL_UI_KIT_EXPORT ui_action {
public:
  ui_action();
  ui_action(const ui_action &copy);

  virtual ~ui_action();

  virtual cherry_kit::string name() const;
  virtual void set_name(const cherry_kit::string &a_name);

  virtual cherry_kit::string controller() const;
  virtual void set_controller(const cherry_kit::string &a_name);

  virtual unsigned int id() const;
  virtual void set_id(unsigned int a_id);

  virtual void set_visible(bool a_visibility = true);
  virtual bool is_visibile() const;

  virtual void set_icon(const cherry_kit::string a_icon);
  virtual cherry_kit::string icon() const;

  virtual void add_action(const ui_action &action);
  virtual ui_action_list sub_actions() const;

  virtual void set_task(ui_task_callback_t callback);
  virtual void execute(const ui_task_data_t &a_data = ui_task_data_t()) const;
  virtual void execute(const cherry_kit::string &a_task_name,
                       const ui_task_data_t &a_data = ui_task_data_t());

private:
  class PrivateControllerAction;
  PrivateControllerAction *const priv;
};
}

#endif // CONTROLLER_ACTION_H
