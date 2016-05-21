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
#ifndef DATASYNC_H
#define DATASYNC_H

#include <ck_sync_engine_interface.h>
#include <ck_sync_object.h>
#include <string>

namespace cherry_kit {
class QuetzalDataKitQt_EXPORT data_sync {
public:
  data_sync(const cherry_kit::string &a_app_name);
  virtual ~data_sync();

  virtual void set_sync_engine(sync_engine_interface *a_iface);

  virtual void add_object(const sync_object &a_obj);
  virtual void save_object(const sync_object &a_obj);
  virtual void remove_object(const cherry_kit::string &a_object_name,
                             const cherry_kit::string &a_key,
                             const cherry_kit::string &a_value);

  virtual void find(const cherry_kit::string &a_object_name,
                    const cherry_kit::string &a_attrib, const cherry_kit::string &a_value);

  virtual void on_object_found(
      std::function<void(sync_object &, const cherry_kit::string &, bool)> a_handler);
  virtual void
  on_object_saved(std::function<void(const sync_object &a_object)> a_handler);

private:
  class Privatedata_sync;
  Privatedata_sync *const m_priv;
};
}

#endif // DATASYNC_H
