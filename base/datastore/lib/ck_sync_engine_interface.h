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
#ifndef SYNCENGINEINTERFACE_H
#define SYNCENGINEINTERFACE_H

#include <functional>

#include <QObject>
#include <ck_sync_object.h>
#include <QStringList>
#include <QuetzalDataKitQt_export.h>

namespace cherry_kit {

class QuetzalDataKitQt_EXPORT sync_engine_interface : public QObject {
  Q_OBJECT
public:
  explicit sync_engine_interface(QObject *a_parent_ptr = 0);
  virtual ~sync_engine_interface();

  virtual QString data(const QString &storeName) = 0;

  virtual void setEngineName(const QString &name);

  virtual void sync(const QString &storeName, const QString &data) = 0;

  virtual bool hasLock() = 0;

  virtual void set_app_name(const cherry_kit::string &a_app_name) {}

  virtual void insert_request(const sync_object &a_object) {}
  virtual void update_request(const sync_object &a_object) {}
  virtual void delete_request(const cherry_kit::string &a_object_name,
                              const cherry_kit::string &a_key,
                              const cherry_kit::string &value) {}

  virtual void find(const cherry_kit::string &a_object_name,
                    const cherry_kit::string &a_attrib, const cherry_kit::string &a_value) {}

  virtual void on_search_complete(std::function<void(
      sync_object &a_object, const cherry_kit::string &a_app_name, bool)> a_handler);

  virtual void
  on_insert_complete(std::function<void(const sync_object &)> a_handler);

protected:
  virtual void search_request_complete(sync_object &a_object,
                                       cherry_kit::string &a_app_name, bool a_found);
  virtual void insert_request_complete(const sync_object &a_object);

private:
  class PrivateSyncEngineIface;
  PrivateSyncEngineIface *const d;
// virtual QStringList storeList() = 0;

Q_SIGNALS:
  void syncDone();
  void dataReady();
  void modified();
};
}

#endif // SYNCENGINEINTERFACE_H
