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
#include "ck_data_sync.h"

#include <QDebug>

namespace cherry_kit {
typedef std::function<void(sync_object &, const std::string &, bool)> FoundFunc;

class data_sync::Privatedata_sync {
public:
  Privatedata_sync() : m_engine(0) {}
  ~Privatedata_sync() {
    if (m_engine)
      delete m_engine;
  }

  sync_engine_interface *m_engine;
  std::string m_app_name;

  std::vector<FoundFunc> m_on_object_found_handler_list;
  std::vector<std::function<void(const sync_object &)> >
  m_on_saved_handler_list;
};

data_sync::data_sync(const std::string &a_app_name)
    : m_priv(new Privatedata_sync) {
  m_priv->m_app_name = a_app_name;
}

data_sync::~data_sync() { delete m_priv; }

void data_sync::set_sync_engine(sync_engine_interface *a_iface) {
  if (m_priv->m_engine)
    return;

  m_priv->m_engine = a_iface;
  m_priv->m_engine->set_app_name(m_priv->m_app_name);
}

void data_sync::add_object(const sync_object &a_obj) {
  if (m_priv->m_engine) {
    m_priv->m_engine->on_insert_complete([this](
        const sync_object &a_saved_obj) {
      std::for_each(m_priv->m_on_saved_handler_list.begin(),
                    m_priv->m_on_saved_handler_list.end(),
                    [&](std::function<void(const sync_object &)> a_func) {
        if (a_func)
          a_func(a_saved_obj);
      });
    });
    m_priv->m_engine->insert_request(a_obj);
  }
}

void data_sync::save_object(const sync_object &a_obj) {
  if (m_priv->m_engine) {
    m_priv->m_engine->update_request(a_obj);
  }
}

void data_sync::remove_object(const std::string &a_object_name,
                              const std::string &a_key,
                              const std::string &a_value) {
  if (m_priv->m_engine) {
    m_priv->m_engine->delete_request(a_object_name, a_key, a_value);
  }
}

void data_sync::find(const std::string &a_object_name,
                     const std::string &a_attrib, const std::string &a_value) {
  if (m_priv->m_engine) {

    m_priv->m_engine->on_search_complete([this](cherry_kit::sync_object &a_obj,
                                                const std::string &a_app_name,
                                                bool a_found) {
      a_obj.set_data_sync(this);
      std::for_each(m_priv->m_on_object_found_handler_list.begin(),
                    m_priv->m_on_object_found_handler_list.end(),
                    [&](FoundFunc a_func) {
        if (a_func) {
          a_func(a_obj, a_app_name, a_found);
        }
      });
    });

    m_priv->m_engine->find(a_object_name, a_attrib, a_value);
  }
}

void data_sync::on_object_found(
    std::function<void(sync_object &, const std::string &, bool)> a_handler) {
  m_priv->m_on_object_found_handler_list.push_back(a_handler);
}

void
data_sync::on_object_saved(std::function<void(const sync_object &)> a_handler) {
  m_priv->m_on_saved_handler_list.push_back(a_handler);
}
}
