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

#include "session_sync.h"

#include <datasync.h>
#include <disksyncengine.h>

#include <QDebug>

class SessionSync::PrivSessionSync {
public:
  PrivSessionSync() : m_purged(0) {}
  ~PrivSessionSync() {}

  int m_session_id;
  QVariantMap m_session_data;
  QString m_session_database;
  bool m_purged;

  std::function<void()> m_on_session_init_func;
  std::function<void()> m_on_session_update_func;
  std::function<void()> m_on_session_end_func;
};

SessionSync::SessionSync(const QVariantMap &a_data) : d(new PrivSessionSync) {

  d->m_session_id = -1;
  d->m_session_data = a_data;
}

void SessionSync::session_init() {}

void SessionSync::set_session_data(const QString &a_key,
                                   const QVariant &a_data) {
  d->m_session_data[a_key] = a_data;
}

QVariant SessionSync::session_data(const QString &a_key) const {
  if (!d->m_session_data.contains(a_key)) {
    return QVariant("");
  }
  return d->m_session_data[a_key];
}

QList<QString> SessionSync::session_keys() const {
  return d->m_session_data.keys();
}

void SessionSync::purge() { d->m_purged = true; }

bool SessionSync::is_purged() { return d->m_purged; }

void SessionSync::set_session_id(int a_id) { d->m_session_id = a_id; }

int SessionSync::session_id() { return d->m_session_id; }

void SessionSync::update_session() {
  if (d->m_on_session_update_func)
    d->m_on_session_update_func();
}

void SessionSync::delete_session_data(const QString &a_session_name,
                                      const QString &a_object_name,
                                      const QString &a_object_key) {
  QuetzalKit::DataSync *sync =
      new QuetzalKit::DataSync(a_session_name.toStdString());

  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();
  sync->set_sync_engine(engine);

  sync->remove_object(a_object_name.toStdString(), a_object_key.toStdString(),
                      QString("%1").arg(d->m_session_id).toStdString());
  delete sync;
}

void SessionSync::save_session_attribute(const QString &a_session_name,
                                         const QString &a_object_name,
                                         const QString &a_object_key,
                                         const QString &a_key,
                                         const QString &a_value) {
  qDebug() << Q_FUNC_INFO << "Request save" << d->m_session_id;
  QuetzalKit::DataSync *sync =
      new QuetzalKit::DataSync(a_session_name.toStdString());
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](QuetzalKit::SyncObject &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (a_found) {
      a_object.setObjectAttribute(a_key, a_value);
      sync->save_object(a_object);
      qDebug() << Q_FUNC_INFO << "Save data";
    } else {
      qDebug() << Q_FUNC_INFO << "Object Not Found";
    }
  });

  qDebug() << Q_FUNC_INFO << "Look for : " << a_object_key
           << " In : " << a_object_name;

  sync->find(a_object_name.toStdString(), a_object_key.toStdString(),
             QString("%1").arg(d->m_session_id).toStdString());

  delete sync;
}

void SessionSync::on_session_init(std::function<void ()> a_handler) {
  d->m_on_session_init_func = a_handler;
}

void SessionSync::on_session_update(std::function<void()> a_handler) {
  d->m_on_session_update_func = a_handler;
}

void SessionSync::on_session_end(std::function<void ()> a_handler) {
  d->m_on_session_end_func = a_handler;
}
