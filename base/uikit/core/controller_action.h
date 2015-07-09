/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include <string>

namespace CherryKit {

class ControllerAction {
public:
  ControllerAction();

  ~ControllerAction();

  virtual std::string name() const;

  virtual void setName(const std::string &a_name);

  virtual unsigned int id() const;

  virtual void setId(unsigned int a_id);

  virtual void setVisibility(bool a_visibility = true);

  virtual bool isVisibile() const;

  virtual void setIcon(const std::string a_icon);

  virtual std::string icon() const;

private:
  class PrivateControllerAction;
  PrivateControllerAction *const o_controller_action;
};
}

#endif // CONTROLLER_ACTION_H
