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
#include "ck_sync_engine_interface.h"

#include <QDebug>

namespace cherry_kit {

class sync_engine_interface::PrivateSyncEngineIface {
public:
  PrivateSyncEngineIface() {}
  ~PrivateSyncEngineIface() {}

  std::vector<std::function<void(sync_object &, const cherry_kit::string &, bool)> >
  m_search_handler_list;
  std::vector<std::function<void(const sync_object &)> > m_insert_handler_list;
};

sync_engine_interface::sync_engine_interface(QObject *parent)
    : QObject(parent), d(new PrivateSyncEngineIface) {}

sync_engine_interface::~sync_engine_interface() { delete d; }

void sync_engine_interface::setEngineName(const QString &name) {
  // engine name;
}

void sync_engine_interface::on_search_complete(
    std::function<void(sync_object &, const cherry_kit::string &, bool)> a_handler) {
  d->m_search_handler_list.push_back(a_handler);
}

void sync_engine_interface::on_insert_complete(
    std::function<void(const sync_object &)> a_handler) {
  d->m_insert_handler_list.push_back(a_handler);
}

void sync_engine_interface::search_request_complete(sync_object &a_object,
                                                    cherry_kit::string &a_app_name,
                                                    bool a_found) {
  std::for_each(
      d->m_search_handler_list.begin(), d->m_search_handler_list.end(),
      [&](std::function<void(sync_object &, const cherry_kit::string &, bool)>
              a_func) {
        if (a_func)
          a_func(a_object, a_app_name, a_found);
      });
}

void
sync_engine_interface::insert_request_complete(const sync_object &a_object) {
  std::for_each(d->m_insert_handler_list.begin(),
                d->m_insert_handler_list.end(),
                [&](std::function<void(const sync_object &)> a_func) {
    if (a_func)
      a_func(a_object);
  });
}
}
