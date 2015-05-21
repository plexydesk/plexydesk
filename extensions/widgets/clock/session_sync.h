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

#ifndef SESSIONSYNC_H
#define SESSIONSYNC_H

#include <QVariant>
#include <view_controller.h>
#include <toolbar.h>
#include <label.h>
#include <clockwidget.h>

class SessionSync {
public:
  SessionSync(const QString &a_session_name, const QVariantMap &a_data = QVariantMap());
  virtual ~SessionSync() {}

  virtual void session_init();

  void set_session_data(const QString &a_key, const QVariant &a_data);
  QVariant session_data(const QString &a_key) const;
  QList<QString> session_keys() const;

  void purge();
  bool is_purged();

  void set_session_id(int a_id);
  int session_id();
  QString session_id_to_string() const;

  virtual void update_session();

  virtual void bind_to_window(UIKit::Window *a_window);

  virtual void delete_session_data(const QString &a_session_name,
                                   const QString &a_object_name,
                                   const QString &a_object_key,
                                   const QString &a_value);

  virtual void save_session_attribute(const QString &a_session_name,
                                      const QString &a_object_name,
                                      const QString &a_object_key, const QString &a_object_value,
                                      const QString &a_key,
                                      const QString &a_value);

  virtual void on_session_init(std::function<void ()> a_handler);
  virtual void on_session_update(std::function<void ()> a_handler);
  virtual void on_session_end(std::function<void ()> a_handler);

private:
  class PrivSessionSync;
  PrivSessionSync *const d;
};

#endif // SESSIONSYNC_H
