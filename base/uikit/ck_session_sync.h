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

#ifndef SESSIONSYNC_H
#define SESSIONSYNC_H

#include <QVariant>
#include <ck_desktop_controller_interface.h>

namespace cherry_kit {
class DECL_UI_KIT_EXPORT session_sync {
public:
  session_sync(const cherry_kit::string &a_session_name,
               const QVariantMap &a_data = QVariantMap());
  virtual ~session_sync() {}

  virtual void session_init();

  void set_session_data(const QString &a_key, const QVariant &a_data);
  QVariant session_data(const QString &a_key) const;
  QList<QString> session_keys() const;

  void purge();
  bool is_purged();

  void set_session_id(int a_id);
  int session_id();
  cherry_kit::string session_id_to_string() const;

  virtual void update_session();

  virtual void bind_to_window(cherry_kit::window *a_window);
  virtual void unbind_window(const window *a_window);

  virtual void delete_session_data(const cherry_kit::string &a_session_name,
                                   const cherry_kit::string &a_object_name,
                                   const cherry_kit::string &a_object_key,
                                   const cherry_kit::string &a_value);

  virtual void save_session_attribute(const cherry_kit::string &a_session_name,
                                      const cherry_kit::string &a_object_name,
                                      const cherry_kit::string &a_object_key,
                                      const cherry_kit::string &a_object_value,
                                      const cherry_kit::string &a_key,
                                      const cherry_kit::string &a_value);

  virtual void on_session_init(std::function<void()> a_handler);
  virtual void on_session_update(std::function<void()> a_handler);
  virtual void on_session_end(std::function<void()> a_handler);

  cherry_kit::string session_group_key() const;

private:
  class PrivSessionSync;
  PrivSessionSync *const d;
};
}
#endif // SESSIONSYNC_H
